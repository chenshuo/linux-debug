#include <stdio.h>
#include <netinet/in.h>

extern "C"
{
// fs/inode.c
extern unsigned int get_next_ino(void);
// net/socket.c
int sock_init(void);
struct socket;
int sock_create(int family, int type, int protocol, struct socket **res);
// net/ipv4/af_inet.c
int inet_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
}


int main(int argc, char* argv[])
{
  if (sock_init())
  {
    printf("sock_init failed.\n"); return 1;
  }
  if (inet_init())
  {
    printf("inet_init failed.\n"); return 1;
  }
  struct socket* sock = NULL;
  int err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  printf("%d %p\n", err, sock);
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_IP, &sock);
  printf("%d %p\n", err, sock);
}
