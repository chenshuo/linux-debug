#include <stdio.h>
#include <netinet/in.h>

extern "C"
{
// fs/inode.c
extern unsigned int get_next_ino(void);
// net/socket.c
struct socket;
int sock_create(int family, int type, int protocol, struct socket **res);
// net/core/skbuff.c
extern void skb_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
}


int main(int argc, char* argv[])
{
  skb_init();
  tcp_v4_init();
  struct socket* sock = NULL;
  int err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("%d %p\n", err, sock);
}
