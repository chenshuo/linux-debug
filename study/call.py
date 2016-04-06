#!/usr/bin/python

import re, sys

ends = []
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


def end(sym):
    if sym in ends:
        return True
    if defs.get(sym, '').startswith('net'):
        return False
    if len(ends) == 0:
        return True
    return False


FIND_PATH = False

def print_stack(stack, last='', recur=False, end=False):
    if FIND_PATH:
        if end:
            print ' -> '.join(stack), '|->', last, defs.get(last, '')  # found
    else:
        if end:
            print ' -> '.join(stack), '|->', last, defs.get(last, '')  # out of scope
        elif recur:
            print ' -> '.join(stack), '<->', last
        else:
            print ' -> '.join(stack), ('->' if last else '[]'), last
    pass


MAX_DEPTH = 8

def dfs(stack):
    sym = stack[-1]
    if sym in stops:
        return
    if sym in deps:
        if len(stack) > MAX_DEPTH:
            print_stack(stack, '...')
            return
        if len(deps[sym]) == 0:
            print_stack(stack, '')
        for dep in sorted(deps[sym]):
            if dep in stack:
                print_stack(stack, dep, recur=True)
                continue
            if end(dep):
                if dep not in stops:
                    print_stack(stack, dep, end=True)
            else:
                stack.append(dep)
                dfs(stack)
                stack.pop()
    else:
        print_stack(stack, 'XXX')

build('CALLS', 'SYMBOLS')

if len(sys.argv) == 3:
    ends.append(sys.argv[2])
    FIND_PATH = True
    sys.argv.pop()

if len(sys.argv) == 2:
    dfs([sys.argv[1]])

