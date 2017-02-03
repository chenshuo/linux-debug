#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "pcap.h"

extern "C"
{
// fs/dcache.c
void dcache_init(void);
// fs/file_table.c
void files_init(void);

// net/socket.c
struct net_proto_family;
int init_inodecache(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);
int SyS_socket(int domain, int type, int protocol);
int SyS_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

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
extern int tcp_accept(struct socket *sock, struct socket **newsock);
extern int sock_read(struct socket *sock, void* data, int len);
extern int sock_write(struct socket *sock, const void* data, int len);

extern unsigned long volatile jiffies;
}

int main()
{
  setbuf(stdout, NULL);

  dcache_init();
  files_init();
  // sock_init():
  skb_init();
  init_inodecache();

  schen_inet_init();
  schen_dst_init();

  int err = 0;
  struct socket* sock = NULL;
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("*** sock_create %d %s %p\n", err, strerror(-err), sock);

  int fd = SyS_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
  printf("*** SyS_socket %d\n", fd);
  /*
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(2222),
    .sin_addr = {
      .s_addr = INADDR_ANY
    }
  };
  err = SyS_bind(fd, (struct sockaddr *)&address, sizeof address);
  */

  err = tcp_bind(sock);
  printf("*** tcp_bind %d\n", err);
  err = tcp_listen(sock, 5);
  printf("*** tcp_listen %d\n", err);

  struct socket* serversock = NULL;
  err = tcp_accept(sock, &serversock);
  printf("*** tcp_accept %d %s %p\n", err, strerror(-err), serversock);

  pcap_start("hello.pcap");
  struct socket* clientsock = NULL;

  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &clientsock);
  printf("*** sock_create %d %s %p\n", err, strerror(-err), clientsock);
  jiffies += 10000;
  printf("*** tcp_connect\n");
  err = tcp_connect_lo_2222(clientsock);
  printf("    tcp_connect %d %s %p\n", err, strerror(-err), clientsock);
  jiffies += 10000;
  printf("*** server receive SYN\n");
  tcp_v4_rcv(output_skb);
  jiffies += 10000;
  printf("*** client receive SYN+ACK\n");
  tcp_v4_rcv(output_skb);
  jiffies += 10000;
  printf("*** server receive ACK\n");
  tcp_v4_rcv(output_skb);

  err = tcp_accept(sock, &serversock);
  printf("*** tcp_accept %d %p\n", err, serversock);

  char buf[64] = { 0 };
  err = sock_read(serversock, buf, sizeof buf);
  printf("*** sock_read %d %s %p\n", err, strerror(-err), serversock);

  err = sock_write(clientsock, "hello", 5);
  printf("*** sock_write %d %p\n", err, clientsock);
  tcp_v4_rcv(output_skb);

  err = sock_read(serversock, buf, sizeof buf);
  printf("*** sock_read %d %p %s\n", err, serversock, buf);

  pcap_stop();
}
