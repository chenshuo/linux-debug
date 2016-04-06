#!/usr/bin/python

import re, sys

stops = set(['panic'])
deps = {}
defs = {}

def build(calls, symbols):
    F = re.compile('<(.*)>:')
    C = re.compile('\t(.*)')
    curr = ''
    for line in open(calls):
        m = F.match(line)
        if m:
            curr = m.group(1)
            deps[curr] = set()
        else:
            m = C.match(line)
            if m:
                deps[curr].add(m.group(1))

    D = re.compile('^.:(.*) DEF ([^ ]*)')
    for line in open(symbols):
        m = D.search(line)
        if m:
            defs[m.group(1)] = m.group(2)


MAX_DEPTH = 8
def end(sym):
    if defs.get(sym, '').startswith('net'):
        return False
    return True


def dfs(stack, sym):
    if sym in stops:
        return
    if sym in deps:
        if len(stack) > MAX_DEPTH:
            print ' -> '.join(stack), ' -> ...'
            return
        for dep in sorted(deps[sym]):
            if dep in stack:
                print ' -> '.join(stack), ' <-> ', dep
                continue
            if end(dep):
                if dep not in stops:
                    print ' -> '.join(stack), ' |-> ', dep, defs.get(dep, '')  # out of scope
            else:
                stack.append(dep)
                dfs(stack, dep)
                stack.pop()
    else:
        print ' -> '.join(stack), "<>"  # leaf

build('CALLS', 'SYMBOLS')

for sym in sys.argv[1:]:
    dfs([sym], sym)

