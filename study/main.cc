#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

extern "C"
{

// net/socket.c
int init_inodecache(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);

// net/core/skbuff.c
void skb_init(void);

// net/core/sock.c
struct proto;
int proto_register(struct proto *prot, int alloc_slab);

// net/ipv4/af_inet.c
int inet_init(void);

// net/ipv4/tcp.c
void tcp_init();

// net/ipv4/tcp_ipv4.c
// void tcp_v4_init();
struct inet_hashinfo;
void inet_hashinfo_init(struct inet_hashinfo *h);
extern struct inet_hashinfo tcp_hashinfo;
extern struct proto tcp_prot;
}

int main()
{
  // sock_init():
  skb_init();
  init_inodecache();

  // FIXME:
  // inet_init();
  // proto_register(&tcp_prot, 1);

  // tcp_v4_init():
  inet_hashinfo_init(&tcp_hashinfo);

  tcp_init();

  struct socket* sock = NULL;
  int err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("sock_create %d %s %p\n", err, strerror(-err), sock);
}
