#!/usr/bin/env python3
# SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)
"""Convert directories of JSON events to C code."""
import argparse
import csv
import json
import os
import sys
from typing import (Callable, Dict, Optional, Sequence, Set, Tuple)
import collections

# Global command line arguments.
_args = None
# List of event tables generated from "/sys" directories.
_sys_event_tables = []
# Map from an event name to an architecture standard
# JsonEvent. Architecture standard events are in json files in the top
# f'{_args.starting_dir}/{_args.arch}' directory.
_arch_std_events = {}
# Track whether an events table is currently being defined and needs closing.
_close_table = False
# Events to write out when the table is closed
_pending_events = []
# Global BigCString shared by all structures.
_bcs = None
# Order specific JsonEvent attributes will be visited.
_json_event_attributes = [
    # cmp_sevent related attributes.
    'name', 'pmu', 'topic', 'desc', 'metric_name', 'metric_group',
    # Seems useful, put it early.
    'event',
    # Short things in alphabetical order.
    'aggr_mode', 'compat', 'deprecated', 'perpkg', 'unit',
    # Longer things (the last won't be iterated over during decompress).
    'metric_constraint', 'metric_expr', 'long_desc'
]


def removesuffix(s: str, suffix: str) -> str:
  """Remove the suffix from a string

  The removesuffix function is added to str in Python 3.9. We aim for 3.6
  compatibility and so provide our own function here.
  """
  return s[0:-len(suffix)] if s.endswith(suffix) else s


def file_name_to_table_name(parents: Sequence[str], dirname: str) -> str:
  """Generate a C table name from directory names."""
  tblname = 'pme'
  for p in parents:
    tblname += '_' + p
  tblname += '_' + dirname
  return tblname.replace('-', '_')

def c_len(s: str) -> int:
  """Return the length of s a C string

  This doesn't handle all escape characters properly. It first assumes
  all \ are for escaping, it then adjusts as it will have over counted
  \\. The code uses \000 rather than \0 as a terminator as an adjacent
  number would be folded into a string of \0 (ie. "\0" + "5" doesn't
  equal a terminator followed by the number 5 but the escape of
  \05). The code adjusts for \000 but not properly for all octal, hex
  or unicode values.
  """
  try:
    utf = s.encode(encoding='utf-8',errors='strict')
  except:
    print(f'broken string {s}')
    raise
  return len(utf) - utf.count(b'\\') + utf.count(b'\\\\') - (utf.count(b'\\000') * 2)

class BigCString:
  """A class to hold many strings concatenated together.

  Generating a large number of stand-alone C strings creates a large
  number of relocations in position independent code. The BigCString
  is a helper for this case. It builds a single string which within it
  are all the other C strings (to avoid memory issues the string
  itself is held as a list of strings). The offsets within the big
  string are recorded and when stored to disk these don't need
  relocation. To reduce the size of the string further, identical
  strings are merged. If a longer string ends-with the same value as a
  shorter string, these entries are also merged.
  """
  strings: Set[str]
  big_string: Sequence[str]
  offsets: Dict[str, int]

  def __init__(self):
    self.strings = set()

  def add(self, s: str) -> None:
    """Called to add to the big string."""
    self.strings.add(s)

  def compute(self) -> None:
    """Called once all strings are added to compute the string and offsets."""

    folded_strings = {}
    # Determine if two strings can be folded, ie. let 1 string use the
    # end of another. First reverse all strings and sort them.
    sorted_reversed_strings = sorted([x[::-1] for x in self.strings])

    # Strings 'xyz' and 'yz' will now be [ 'zy', 'zyx' ]. Scan forward
    # for each string to see if there is a better candidate to fold it
    # into, in the example rather than using 'yz' we can use'xyz' at
    # an offset of 1. We record which string can be folded into which
    # in folded_strings, we don't need to record the offset as it is
    # trivially computed from the string lengths.
    for pos,s in enumerate(sorted_reversed_strings):
      best_pos = pos
      for check_pos in range(pos + 1, len(sorted_reversed_strings)):
        if sorted_reversed_strings[check_pos].startswith(s):
          best_pos = check_pos
        else:
          break
      if pos != best_pos:
        folded_strings[s[::-1]] = sorted_reversed_strings[best_pos][::-1]

    # Compute reverse mappings for debugging.
    fold_into_strings = collections.defaultdict(set)
    for key, val in folded_strings.items():
      if key != val:
        fold_into_strings[val].add(key)

    # big_string_offset is the current location within the C string
    # being appended to - comments, etc. don't count. big_string is
    # the string contents represented as a list. Strings are immutable
    # in Python and so appending to one causes memory issues, while
    # lists are mutable.
    big_string_offset = 0
    self.big_string = []
    self.offsets = {}

    # Emit all strings that aren't folded in a sorted manner.
    for s in sorted(self.strings):
      if s not in folded_strings:
        self.offsets[s] = big_string_offset
        self.big_string.append(f'/* offset={big_string_offset} */ "')
        self.big_string.append(s)
        self.big_string.append('"')
        if s in fold_into_strings:
          self.big_string.append(' /* also: ' + ', '.join(fold_into_strings[s]) + ' */')
        self.big_string.append('\n')
        big_string_offset += c_len(s)
        continue

    # Compute the offsets of the folded strings.
    for s in folded_strings.keys():
      assert s not in self.offsets
      folded_s = folded_strings[s]
      self.offsets[s] = self.offsets[folded_s] + c_len(folded_s) - c_len(s)

_bcs = BigCString()

class JsonEvent:
  """Representation of an event loaded from a json file dictionary."""

  def __init__(self, jd: dict):
    """Constructor passed the dictionary of parsed json values."""

    def llx(x: int) -> str:
      """Convert an int to a string similar to a printf modifier of %#llx."""
      return '0' if x == 0 else hex(x)

    def fixdesc(s: str) -> str:
      """Fix formatting issue for the desc string."""
      if s is None:
        return None
      return removesuffix(removesuffix(removesuffix(s, '.  '),
                                       '. '), '.').replace('\n', '\\n').replace(
                                           '\"', '\\"').replace('\r', '\\r')

    def convert_aggr_mode(aggr_mode: str) -> Optional[str]:
      """Returns the aggr_mode_class enum value associated with the JSON string."""
      if not aggr_mode:
        return None
      aggr_mode_to_enum = {
          'PerChip': '1',
          'PerCore': '2',
      }
      return aggr_mode_to_enum[aggr_mode]

    def lookup_msr(num: str) -> Optional[str]:
      """Converts the msr number, or first in a list to the appropriate event field."""
      if not num:
        return None
      msrmap = {
          0x3F6: 'ldlat=',
          0x1A6: 'offcore_rsp=',
          0x1A7: 'offcore_rsp=',
          0x3F7: 'frontend=',
      }
      return msrmap[int(num.split(',', 1)[0], 0)]

    def real_event(name: str, event: str) -> Optional[str]:
      """Convert well known event names to an event string otherwise use the event argument."""
      fixed = {
          'inst_retired.any': 'event=0xc0,period=2000003',
          'inst_retired.any_p': 'event=0xc0,period=2000003',
          'cpu_clk_unhalted.ref': 'event=0x0,umask=0x03,period=2000003',
          'cpu_clk_unhalted.thread': 'event=0x3c,period=2000003',
          'cpu_clk_unhalted.core': 'event=0x3c,period=2000003',
          'cpu_clk_unhalted.thread_any': 'event=0x3c,any=1,period=2000003',
      }
      if not name:
        return None
      if name.lower() in fixed:
        return fixed[name.lower()]
      return event

    def unit_to_pmu(unit: str) -> Optional[str]:
      """Convert a JSON Unit to Linux PMU name."""
      if not unit:
        return None
      # Comment brought over from jevents.c:
      # it's not realistic to keep adding these, we need something more scalable ...
      table = {
          'CBO': 'uncore_cbox',
          'QPI LL': 'uncore_qpi',
          'SBO': 'uncore_sbox',
          'iMPH-U': 'uncore_arb',
          'CPU-M-CF': 'cpum_cf',
          'CPU-M-SF': 'cpum_sf',
          'PAI-CRYPTO' : 'pai_crypto',
          'UPI LL': 'uncore_upi',
          'hisi_sicl,cpa': 'hisi_sicl,cpa',
          'hisi_sccl,ddrc': 'hisi_sccl,ddrc',
          'hisi_sccl,hha': 'hisi_sccl,hha',
          'hisi_sccl,l3c': 'hisi_sccl,l3c',
          'imx8_ddr': 'imx8_ddr',
          'L3PMC': 'amd_l3',
          'DFPMC': 'amd_df',
          'cpu_core': 'cpu_core',
          'cpu_atom': 'cpu_atom',
      }
      return table[unit] if unit in table else f'uncore_{unit.lower()}'

    eventcode = 0
    if 'EventCode' in jd:
      eventcode = int(jd['EventCode'].split(',', 1)[0], 0)
    if 'ExtSel' in jd:
      eventcode |= int(jd['ExtSel']) << 8
    configcode = int(jd['ConfigCode'], 0) if 'ConfigCode' in jd else None
    self.name = jd['EventName'].lower() if 'EventName' in jd else None
    self.topic = ''
    self.compat = jd.get('Compat')
    self.desc = fixdesc(jd.get('BriefDescription'))
    self.long_desc = fixdesc(jd.get('PublicDescription'))
    precise = jd.get('PEBS')
    msr = lookup_msr(jd.get('MSRIndex'))
    msrval = jd.get('MSRValue')
    extra_desc = ''
    if 'Data_LA' in jd:
      extra_desc += '  Supports address when precise'
      if 'Errata' in jd:
        extra_desc += '.'
    if 'Errata' in jd:
      extra_desc += '  Spec update: ' + jd['Errata']
    self.pmu = unit_to_pmu(jd.get('Unit'))
    filter = jd.get('Filter')
    self.unit = jd.get('ScaleUnit')
    self.perpkg = jd.get('PerPkg')
    self.aggr_mode = convert_aggr_mode(jd.get('AggregationMode'))
    self.deprecated = jd.get('Deprecated')
    self.metric_name = jd.get('MetricName')
    self.metric_group = jd.get('MetricGroup')
    self.metric_constraint = jd.get('MetricConstraint')
    self.metric_expr = jd.get('MetricExpr')
    if self.metric_expr:
      self.metric_expr = self.metric_expr.replace('\\', '\\\\')
    arch_std = jd.get('ArchStdEvent')
    if precise and self.desc and '(Precise Event)' not in self.desc:
      extra_desc += ' (Must be precise)' if precise == '2' else (' (Precise '
                                                                 'event)')
    event = f'config={llx(configcode)}' if configcode is not None else f'event={llx(eventcode)}'
    event_fields = [
        ('AnyThread', 'any='),
        ('PortMask', 'ch_mask='),
        ('CounterMask', 'cmask='),
        ('EdgeDetect', 'edge='),
        ('FCMask', 'fc_mask='),
        ('Invert', 'inv='),
        ('SampleAfterValue', 'period='),
        ('UMask', 'umask='),
    ]
    for key, value in event_fields:
      if key in jd and jd[key] != '0':
        event += ',' + value + jd[key]
    if filter:
      event += f',{filter}'
    if msr:
      event += f',{msr}{msrval}'
    if self.desc and extra_desc:
      self.desc += extra_desc
    if self.long_desc and extra_desc:
      self.long_desc += extra_desc
    if self.pmu:
      if self.desc and not self.desc.endswith('. '):
        self.desc += '. '
      self.desc = (self.desc if self.desc else '') + ('Unit: ' + self.pmu + ' ')
    if arch_std and arch_std.lower() in _arch_std_events:
      event = _arch_std_events[arch_std.lower()].event
      # Copy from the architecture standard event to self for undefined fields.
      for attr, value in _arch_std_events[arch_std.lower()].__dict__.items():
        if hasattr(self, attr) and not getattr(self, attr):
          setattr(self, attr, value)

    self.event = real_event(self.name, event)

  def __repr__(self) -> str:
    """String representation primarily for debugging."""
    s = '{\n'
    for attr, value in self.__dict__.items():
      if value:
        s += f'\t{attr} = {value},\n'
    return s + '}'

  def build_c_string(self) -> str:
    s = ''
    for attr in _json_event_attributes:
      x = getattr(self, attr)
      s += f'{x}\\000' if x else '\\000'
    return s

  def to_c_string(self) -> str:
    """Representation of the event as a C struct initializer."""

    s = self.build_c_string()
    return f'{{ { _bcs.offsets[s] } }}, /* {s} */\n'


def read_json_events(path: str, topic: str) -> Sequence[JsonEvent]:
  """Read json events from the specified file."""

  try:
    result = json.load(open(path), object_hook=JsonEvent)
  except BaseException as err:
    print(f"Exception processing {path}")
    raise
  for event in result:
    event.topic = topic
  return result

def preprocess_arch_std_files(archpath: str) -> None:
  """Read in all architecture standard events."""
  global _arch_std_events
  for item in os.scandir(archpath):
    if item.is_file() and item.name.endswith('.json'):
      for event in read_json_events(item.path, topic=''):
        if event.name:
          _arch_std_events[event.name.lower()] = event


def print_events_table_prefix(tblname: str) -> None:
  """Called when a new events table is started."""
  global _close_table
  if _close_table:
    raise IOError('Printing table prefix but last table has no suffix')
  _args.output_file.write(f'static const struct compact_pmu_event {tblname}[] = {{\n')
  _close_table = True


def add_events_table_entries(item: os.DirEntry, topic: str) -> None:
  """Add contents of file to _pending_events table."""
  if not _close_table:
    raise IOError('Table entries missing prefix')
  for e in read_json_events(item.path, topic):
    _pending_events.append(e)


def print_events_table_suffix() -> None:
  """Optionally close events table."""

  def event_cmp_key(j: JsonEvent) -> Tuple[bool, str, str, str, str]:
    def fix_none(s: Optional[str]) -> str:
      if s is None:
        return ''
      return s

    return (j.desc is not None, fix_none(j.topic), fix_none(j.name), fix_none(j.pmu),
            fix_none(j.metric_name))

  global _close_table
  if not _close_table:
    return

  global _pending_events
  for event in sorted(_pending_events, key=event_cmp_key):
    _args.output_file.write(event.to_c_string())
    _pending_events = []

  _args.output_file.write('};\n\n')
  _close_table = False

def get_topic(topic: str) -> str:
  if topic.endswith('metrics.json'):
    return 'metrics'
  return removesuffix(topic, '.json').replace('-', ' ')

def preprocess_one_file(parents: Sequence[str], item: os.DirEntry) -> None:

  if item.is_dir():
    return

  # base dir or too deep
  level = len(parents)
  if level == 0 or level > 4:
    return

  # Ignore other directories. If the file name does not have a .json
  # extension, ignore it. It could be a readme.txt for instance.
  if not item.is_file() or not item.name.endswith('.json'):
    return

  topic = get_topic(item.name)
  for event in read_json_events(item.path, topic):
    _bcs.add(event.build_c_string())

def process_one_file(parents: Sequence[str], item: os.DirEntry) -> None:
  """Process a JSON file during the main walk."""
  global _sys_event_tables

  def is_leaf_dir(path: str) -> bool:
    for item in os.scandir(path):
      if item.is_dir():
        return False
    return True

  # model directory, reset topic
  if item.is_dir() and is_leaf_dir(item.path):
    print_events_table_suffix()

    tblname = file_name_to_table_name(parents, item.name)
    if item.name == 'sys':
      _sys_event_tables.append(tblname)
    print_events_table_prefix(tblname)
    return

  # base dir or too deep
  level = len(parents)
  if level == 0 or level > 4:
    return

  # Ignore other directories. If the file name does not have a .json
  # extension, ignore it. It could be a readme.txt for instance.
  if not item.is_file() or not item.name.endswith('.json'):
    return

  add_events_table_entries(item, get_topic(item.name))


def print_mapping_table(archs: Sequence[str]) -> None:
  """Read the mapfile and generate the struct from cpuid string to event table."""
  _args.output_file.write("""
/* Struct used to make the PMU event table implementation opaque to callers. */
struct pmu_events_table {
        const struct compact_pmu_event *entries;
        size_t length;
};

/*
 * Map a CPU to its table of PMU events. The CPU is identified by the
 * cpuid field, which is an arch-specific identifier for the CPU.
 * The identifier specified in tools/perf/pmu-events/arch/xxx/mapfile
 * must match the get_cpuid_str() in tools/perf/arch/xxx/util/header.c)
 *
 * The  cpuid can contain any character other than the comma.
 */
struct pmu_events_map {
        const char *arch;
        const char *cpuid;
        struct pmu_events_table table;
};

/*
 * Global table mapping each known CPU for the architecture to its
 * table of PMU events.
 */
const struct pmu_events_map pmu_events_map[] = {
""")
  for arch in archs:
    if arch == 'test':
      _args.output_file.write("""{
\t.arch = "testarch",
\t.cpuid = "testcpu",
\t.table = {
\t.entries = pme_test_soc_cpu,
\t.length = ARRAY_SIZE(pme_test_soc_cpu),
\t}
},
""")
    else:
      with open(f'{_args.starting_dir}/{arch}/mapfile.csv') as csvfile:
        table = csv.reader(csvfile)
        first = True
        for row in table:
          # Skip the first row or any row beginning with #.
          if not first and len(row) > 0 and not row[0].startswith('#'):
            tblname = file_name_to_table_name([], row[2].replace('/', '_'))
            cpuid = row[0].replace('\\', '\\\\')
            _args.output_file.write(f"""{{
\t.arch = "{arch}",
\t.cpuid = "{cpuid}",
\t.table = {{
\t\t.entries = {tblname},
\t\t.length = ARRAY_SIZE({tblname})
\t}}
}},
""")
          first = False

  _args.output_file.write("""{
\t.arch = 0,
\t.cpuid = 0,
\t.table = { 0, 0 },
}
};
""")


def print_system_mapping_table() -> None:
  """C struct mapping table array for tables from /sys directories."""
  _args.output_file.write("""
struct pmu_sys_events {
\tconst char *name;
\tstruct pmu_events_table table;
};

static const struct pmu_sys_events pmu_sys_event_tables[] = {
""")
  for tblname in _sys_event_tables:
    _args.output_file.write(f"""\t{{
\t\t.table = {{
\t\t\t.entries = {tblname},
\t\t\t.length = ARRAY_SIZE({tblname})
\t\t}},
\t\t.name = \"{tblname}\",
\t}},
""")
  _args.output_file.write("""\t{
\t\t.table = { 0, 0 }
\t},
};

static void decompress(int offset, struct pmu_event *pe)
{
\tconst char *p = &big_c_string[offset];
""")
  for attr in _json_event_attributes:
    _args.output_file.write(f"""
\tpe->{attr} = (*p == '\\0' ? NULL : p);
""")
    if attr == _json_event_attributes[-1]:
      continue
    _args.output_file.write('\twhile (*p++);')
  _args.output_file.write("""}

int pmu_events_table_for_each_event(const struct pmu_events_table *table,
                                    pmu_event_iter_fn fn,
                                    void *data)
{
        for (size_t i = 0; i < table->length; i++) {
                struct pmu_event pe;
                int ret;

                decompress(table->entries[i].offset, &pe);
                ret = fn(&pe, table, data);
                if (ret)
                        return ret;
        }
        return 0;
}

const struct pmu_events_table *perf_pmu__find_table(struct perf_pmu *pmu)
{
        const struct pmu_events_table *table = NULL;
        char *cpuid = perf_pmu__getcpuid(pmu);
        int i;

        /* on some platforms which uses cpus map, cpuid can be NULL for
         * PMUs other than CORE PMUs.
         */
        if (!cpuid)
                return NULL;

        i = 0;
        for (;;) {
                const struct pmu_events_map *map = &pmu_events_map[i++];
                if (!map->arch)
                        break;

                if (!strcmp_cpuid_str(map->cpuid, cpuid)) {
                        table = &map->table;
                        break;
                }
        }
        free(cpuid);
        return table;
}

const struct pmu_events_table *find_core_events_table(const char *arch, const char *cpuid)
{
        for (const struct pmu_events_map *tables = &pmu_events_map[0];
             tables->arch;
             tables++) {
                if (!strcmp(tables->arch, arch) && !strcmp_cpuid_str(tables->cpuid, cpuid))
                        return &tables->table;
        }
        return NULL;
}

int pmu_for_each_core_event(pmu_event_iter_fn fn, void *data)
{
        for (const struct pmu_events_map *tables = &pmu_events_map[0];
             tables->arch;
             tables++) {
                int ret = pmu_events_table_for_each_event(&tables->table, fn, data);

                if (ret)
                        return ret;
        }
        return 0;
}

const struct pmu_events_table *find_sys_events_table(const char *name)
{
        for (const struct pmu_sys_events *tables = &pmu_sys_event_tables[0];
             tables->name;
             tables++) {
                if (!strcmp(tables->name, name))
                        return &tables->table;
        }
        return NULL;
}

int pmu_for_each_sys_event(pmu_event_iter_fn fn, void *data)
{
        for (const struct pmu_sys_events *tables = &pmu_sys_event_tables[0];
             tables->name;
             tables++) {
                int ret = pmu_events_table_for_each_event(&tables->table, fn, data);

                if (ret)
                        return ret;
        }
        return 0;
}
""")


def main() -> None:
  global _args

  def dir_path(path: str) -> str:
    """Validate path is a directory for argparse."""
    if os.path.isdir(path):
      return path
    raise argparse.ArgumentTypeError(f'\'{path}\' is not a valid directory')

  def ftw(path: str, parents: Sequence[str],
          action: Callable[[Sequence[str], os.DirEntry], None]) -> None:
    """Replicate the directory/file walking behavior of C's file tree walk."""
    for item in os.scandir(path):
      action(parents, item)
      if item.is_dir():
        ftw(item.path, parents + [item.name], action)

  ap = argparse.ArgumentParser()
  ap.add_argument('arch', help='Architecture name like x86')
  ap.add_argument(
      'starting_dir',
      type=dir_path,
      help='Root of tree containing architecture directories containing json files'
  )
  ap.add_argument(
      'output_file', type=argparse.FileType('w', encoding='utf-8'), nargs='?', default=sys.stdout)
  _args = ap.parse_args()

  _args.output_file.write("""
#include "pmu-events/pmu-events.h"
#include "util/header.h"
#include "util/pmu.h"
#include <string.h>
#include <stddef.h>

struct compact_pmu_event {
  int offset;
};

""")
  archs = []
  for item in os.scandir(_args.starting_dir):
    if not item.is_dir():
      continue
    if item.name == _args.arch or _args.arch == 'all' or item.name == 'test':
      archs.append(item.name)

  if len(archs) < 2:
    raise IOError(f'Missing architecture directory \'{_args.arch}\'')

  archs.sort()
  for arch in archs:
    arch_path = f'{_args.starting_dir}/{arch}'
    preprocess_arch_std_files(arch_path)
    ftw(arch_path, [], preprocess_one_file)

  _bcs.compute()
  _args.output_file.write('static const char *const big_c_string =\n')
  for s in _bcs.big_string:
    _args.output_file.write(s)
  _args.output_file.write(';\n\n')
  for arch in archs:
    arch_path = f'{_args.starting_dir}/{arch}'
    ftw(arch_path, [], process_one_file)
    print_events_table_suffix()

  print_mapping_table(archs)
  print_system_mapping_table()


if __name__ == '__main__':
  main()
