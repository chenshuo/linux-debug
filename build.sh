#!/bin/bash

set -x
g++ -m32 -g study/main.cc study/lib.cc -Wall -Wl,--gc-sections \
  lib/ctype.o \
  lib/kstrtox.o \
  net/ipv4/inet_hashtables.o \
  net/ipv4/tcp.o \
  net/ipv4/tcp_cong.o \
  net/ipv4/tcp_input.o \
  net/ipv4/tcp_ipv4.o \
  net/ipv4/tcp_minisocks.o \
  net/core/request_sock.o \
  study/stub_net.o \
  study/stub_mm.o
