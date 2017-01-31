#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "pcap.h"

extern "C"
{

int inet_add_protocol(const struct net_protocol *prot, unsigned char protocol);
// net/socket.c
struct net_proto_family;
int sock_register(const struct net_proto_family *ops);
int init_inodecache(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);

// net/core/skbuff.c
void skb_init(void);

// net/core/sock.c
struct proto;
int proto_register(struct proto *prot, int alloc_slab);

// net/ipv4/af_inet.c
extern struct net_proto_family inet_family_ops;
extern const struct net_protocol tcp_protocol;
int inet_init(void);
void schen_inet_init(void);

// net/ipv4/tcp.c
void tcp_init();

// net/ipv4/tcp_ipv4.c
// void tcp_v4_init();
struct inet_hashinfo;
void inet_hashinfo_init(struct inet_hashinfo *h);
extern struct inet_hashinfo tcp_hashinfo;
extern struct proto tcp_prot;

// study/helper.c
extern void schen_dst_init(void);

extern int tcp_connect_lo_2222(struct socket *sock);
extern int tcp_bind(struct socket *sock);
extern int tcp_listen(struct socket *sock, int backlog);
}

int main()
{
  // sock_init():
  skb_init();
  init_inodecache();

  // FIXME:
  // inet_init();
  schen_inet_init();
  schen_dst_init();

  struct socket* sock = NULL;
  int err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("sock_create %d %s %p\n", err, strerror(-err), sock);

  /*
  err = tcp_bind(sock);
  printf("tcp_bind %d\n", err);
  err = tcp_listen(sock, 5);
  printf("tcp_listen %d\n", err);
  */

  pcap_start("hello.pcap");
  tcp_connect_lo_2222(sock);
  pcap_stop();
}
