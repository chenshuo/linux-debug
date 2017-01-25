#!/usr/bin/python

# find net/ -name '*.o' |xargs ./study/obj.py > objs

import os, re, sys
from subprocess import check_output

P = re.compile(' ([A-Za-z]) (.*)')

symbols = {}

def process(src, output):
    # print src
    for line in output.splitlines():
        m = P.search(line)
        if not m:
            continue
        t = m.group(1)
        sym = m.group(2)
        if t not in 'BbDdRTtWU':
            continue
        if not sym in symbols:
            symbols[sym] = [set(), set(), '']
        if t == 'U':
            symbols[sym][1].add(src)
        else:  #t == 'T' or t == 'D':
            symbols[sym][0].add(src)
            symbols[sym][2] = t


def define(d, yes, no):
    if d:
        return yes + ','.join(d)
    else:
        return no

out = open('asms', 'w')
for obj in sys.argv[1:]:
    if 'scripts/' in obj:
        continue
    src = obj.replace('.o', '.c')
    if os.path.isfile(src):
        out.write(obj.replace('.o', '.s')+'\n')
        process(src, check_output(['nm', obj]))
    # else:
    #     print 'NO SOURCE', src

for sym in sorted(symbols):
    print sym, symbols[sym][2], define(symbols[sym][0], 'DEF ', 'NODEF'), define(symbols[sym][1], 'REF ', 'NO USE')
