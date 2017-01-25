#include <stdio.h>

extern "C"
{

// net/socket.c
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);

// net/core/skbuff.c
void skb_init(void);

// net/ipv4/tcp.c
void tcp_init();

// net/ipv4/tcp_ipv4.c
// void tcp_v4_init();
struct inet_hashinfo;
void inet_hashinfo_init(struct inet_hashinfo *h);
extern struct inet_hashinfo tcp_hashinfo;
}

int main()
{
  skb_init();
  // tcp_v4_init();
  inet_hashinfo_init(&tcp_hashinfo);
  tcp_init();
}
