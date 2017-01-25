#!/bin/bash

set -x
g++ -m32 -g study/main.cc study/lib.cc -Wall -Wl,--gc-sections \
  fs/inode.o \
  fs/libfs.o \
  kernel/locking/rwsem-spinlock.o \
  kernel/rcu/tiny.o \
  kernel/sched/wait.o \
  lib/ctype.o \
  lib/kstrtox.o \
  net/socket.o \
  net/core/request_sock.o \
  net/core/skbuff.o \
  net/core/sock.o \
  net/ipv4/af_inet.o \
  net/ipv4/inet_hashtables.o \
  net/ipv4/tcp.o \
  net/ipv4/tcp_cong.o \
  net/ipv4/tcp_input.o \
  net/ipv4/tcp_ipv4.o \
  net/ipv4/tcp_minisocks.o \
  study/stub_arch.o \
  study/stub_fs.o \
  study/stub_kernel.o \
  study/stub_mm.o \
  study/stub_net.o
