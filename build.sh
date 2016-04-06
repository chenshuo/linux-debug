#!/bin/sh

OBJS="study/stub_memory.o"
OBJS="$OBJS kernel/user.o"  # init_user_ns
OBJS="$OBJS kernel/locking/mutex.o"  # __mutex_init
OBJS="$OBJS fs/inode.o"  # new_inode_pseudo
OBJS="$OBJS net/socket.o"
OBJS="$OBJS net/core/skbuff.o"
OBJS="$OBJS net/ipv4/inet_hashtables.o"
OBJS="$OBJS net/ipv4/tcp_ipv4.o"
# OBJS="$OBJS net/socket.o"

set -x

make ARCH=um $OBJS
g++ -std=c++11 -Wall -g -m32 -O0 -c study/lib.cc -o study/lib.o
g++ -std=c++11 -Wall -g -m32 -O0 study/main.cc study/lib.o $OBJS
