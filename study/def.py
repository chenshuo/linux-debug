#!/usr/bin/python

import re, sys

P = re.compile('(.*) (.) DEF ([^ ]*)')
U = re.compile("undefined reference to `(.*)'")

symbols = {}
for line in open('objs'):
    m = P.match(line)
    if m:
        symbol = m.group(1)
        typ = m.group(2)
        src = m.group(3)
        symbols[symbol] = (typ, src)

for line in sys.stdin:
    m = U.search(line)
    if m:
        sym = m.group(1)
        d = ''
	t = ''
        if sym in symbols:
            t = symbols[sym][0]
            d = symbols[sym][1]
        print sym.ljust(30), t, d

