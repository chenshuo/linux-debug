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

// net/core/dev.c
int schen_net_dev_init(void);
void net_rx_action(struct softirq_action *h);

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
//int sock_setsockopt(struct socket *sock, int level, int optname,
//		    void *optval, unsigned int optlen);

// net/ipv4/af_inet.c
void schen_inet_init(void);

// net/ipv4/tcp.c
void tcp_init();

// study/helper.c
extern void schen_dst_init(void);
extern void sock_reuseport(struct socket *sock);
extern int tcp_connect_lo_2222(struct socket *sock);
extern int sock_bind_any_2222(struct socket *sock);
extern int tcp_listen(struct socket *sock, int backlog);
extern int tcp_accept(struct socket *sock, struct socket **newsock);
extern int sock_read(struct socket *sock, void* data, int len);
extern int sock_write(struct socket *sock, const void* data, int len);
extern int sock_sendto(struct socket *sock, const void* data, int len,
		       const void *dest_addr, socklen_t addrlen);

extern unsigned long volatile jiffies;
}

void test_udp()
{
  struct socket* sock = NULL;
  int err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);
  printf("*** sock_create UDP server %d %s %p\n", err, strerror(-err), sock);
  sock_reuseport(sock);
  // int one = 1;
  // err = sock_setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &one, sizeof one);
  // printf("*** sock_setsockopt %d\n", err);
  err = sock_bind_any_2222(sock);
  printf("*** sock_bind %d\n", err);

  struct socket* sock2 = NULL;
  err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock2);
  printf("*** sock_create %d\n", err);
  sock_reuseport(sock2);
  // err = sock_setsockopt(sock2, SOL_SOCKET, SO_REUSEPORT, &one, sizeof one);
  // printf("*** sock_setsockopt %d\n", err);
  err = sock_bind_any_2222(sock2);
  printf("*** sock_bind %d\n", err);

  struct socket* client = NULL;
  err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &client);
  printf("*** sock_create UDP client %d %s %p\n", err, strerror(-err), client);
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(2222),
    .sin_addr = {
      .s_addr = htonl(INADDR_LOOPBACK)
    }
  };

  err = sock_sendto(client, "good", 4, &address, sizeof address);
  net_rx_action(NULL);

  char output[2048] = { 0 };
  err = sock_sendto(client, output, sizeof output, &address, sizeof address);
  net_rx_action(NULL);

  char buf[3000] = { 0 };
  err = sock_read(sock2, buf, sizeof buf);
  printf("*** sock_read %d '%s'\n", err, buf);

  err = sock_read(sock2, buf, sizeof buf);
  printf("*** sock_read %d '%s'\n", err, buf);
}

int main(int argc, char* argv[])
{
  setbuf(stdout, NULL);

  dcache_init();
  files_init();
  // sock_init():
  skb_init();
  init_inodecache();
  schen_net_dev_init();
  schen_inet_init();
  schen_dst_init();
  pcap_start("hello.pcap");

  if (argc > 1)
  {
    test_udp();
    pcap_stop();
    return 0;
  }

  int err = 0;
  struct socket* sock = NULL;
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("*** sock_create %d %s %p\n", err, strerror(-err), sock);

  // int fd = SyS_socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  // printf("*** SyS_socket %d\n", fd);

  // err = SyS_bind(fd, (struct sockaddr *)&address, sizeof address);

  err = sock_bind_any_2222(sock);
  printf("*** sock_bind %d\n", err);
  err = tcp_listen(sock, 5);
  printf("*** tcp_listen %d\n", err);

  struct socket* serversock = NULL;
  err = tcp_accept(sock, &serversock);
  printf("*** tcp_accept %d %s %p\n", err, strerror(-err), serversock);

  struct socket* clientsock = NULL;

  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &clientsock);
  printf("*** sock_create %d %s %p\n", err, strerror(-err), clientsock);
  jiffies += 10000;
  printf("*** tcp_connect\n");
  err = tcp_connect_lo_2222(clientsock);
  printf("    tcp_connect %d %s %p\n", err, strerror(-err), clientsock);
  jiffies += 10000;
  printf("*** server receive SYN\n");
  net_rx_action(NULL);

  jiffies += 10000;
  printf("*** client receive SYN+ACK\n");
  net_rx_action(NULL);
  jiffies += 10000;
  printf("*** server receive ACK\n");
  net_rx_action(NULL);

  err = tcp_accept(sock, &serversock);
  printf("*** tcp_accept %d %p\n", err, serversock);

  char buf[64] = { 0 };
  err = sock_read(serversock, buf, sizeof buf);
  printf("*** sock_read %d %s %p\n", err, strerror(-err), serversock);

  char output[2048] = { 0 };
  err = sock_write(clientsock, "hello", 5);
  printf("*** sock_write %d %p\n", err, clientsock);
  err = sock_write(clientsock, output, sizeof output);
  printf("*** sock_write %d %p\n", err, clientsock);
  net_rx_action(NULL);

  err = sock_read(serversock, buf, sizeof buf);
  printf("*** sock_read %d %p %s\n", err, serversock, buf);

  net_rx_action(NULL);
  pcap_stop();
}
