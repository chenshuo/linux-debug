#!/usr/bin/python

import re, sys

funcs = {}

for f in open('asms'):
# for f in ['net/ipv4/tcp.s']:
# for f in sys.argv[1:]:
  files = {}
  f = f.rstrip()
  print '%s:' % f.replace('.s', '.c')
  for l in open(f):
    m = re.search('\t.file (\\d+) "(.*)"', l)
    if m:
      files[m.group(1)] = m.group(2)

    m = re.search('\t.loc (\\d+) (\\d+)', l)
    if m:
      loc = m.group(1)
      file = files[m.group(1)]
      line = m.group(2)

    m = re.search('\t.type\t(.*), @function', l)
    if m:
      func = m.group(1)
      #if func not in funcs:
      #  funcs[func] = set()

    if re.search('\t.cfi_startproc', l):
      # define of function
      print '\t<%s>:%s:%s' % (func, file, line)
      start_file = file
      start_line = line

    if re.search('\t.cfi_endproc', l):
      continue
      if start_file == file:
        print 'L %d %s:%s:%s:%s' % (int(line) - int(start_line), func, file, start_line, line)
        pass
      else:
        print '====== different file'

    m = re.search('\tcall\t(\\S*)', l)
    if m:
      call = m.group(1)
      print '\t\tcall <%s>:%s:%s' % (call, file, line)
      #funcs[func].add(call)

    if 'debug_info' in l:
      break
