#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "pcap.h"

extern "C"
{

// net/socket.c
struct net_proto_family;
int init_inodecache(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);

// net/core/skbuff.c
void skb_init(void);

// net/core/sock.c

// net/ipv4/af_inet.c
void schen_inet_init(void);

// net/ipv4/tcp.c
void tcp_init();

// net/ipv4/tcp_ipv4.c
extern int tcp_v4_rcv(struct sk_buff *skb);

// study/helper.c
extern struct sk_buff *output_skb;
extern void schen_dst_init(void);
extern int tcp_connect_lo_2222(struct socket *sock);
extern int tcp_bind(struct socket *sock);
extern int tcp_listen(struct socket *sock, int backlog);

unsigned long volatile jiffies;

}

int main()
{
  int err = 0;
  // sock_init():
  skb_init();
  init_inodecache();

  schen_inet_init();
  schen_dst_init();

  struct socket* sock = NULL;
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("sock_create %d %s %p\n", err, strerror(-err), sock);

  err = tcp_bind(sock);
  printf("tcp_bind %d\n", err);
  err = tcp_listen(sock, 5);
  printf("tcp_listen %d\n", err);

  pcap_start("hello.pcap");
  struct socket* clientsock = NULL;

  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &clientsock);
  printf("sock_create %d %s %p\n", err, strerror(-err), clientsock);
  jiffies += 10000;
  tcp_connect_lo_2222(clientsock);
  jiffies += 10000;
  tcp_v4_rcv(output_skb); // receive SYN
  jiffies += 10000;
  tcp_v4_rcv(output_skb); // receive SYN+ACK
  jiffies += 10000;
  tcp_v4_rcv(output_skb); // receive ACK

  pcap_stop();
}
