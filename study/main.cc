#include <stdio.h>
#include <netinet/in.h>
#include <string>

extern "C"
{
// fs/inode.c
// extern unsigned int get_next_ino(void);
// net/socket.c
extern int sock_init(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);
// net/ipv4/af_inet.c
extern int inet_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
// study/helper.c
extern int tcp_connect(struct socket *sock);
extern int tcp_bind(struct socket *sock);
struct sk_buff;
struct sk_buff *build_skbuff(const void* data, unsigned int len);
}

using std::string;
string build_syn()
{
  return string(40, '-');
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
  tcp_bind(sock);
  // tcp_listen();
  string syn = build_syn();
  sk_buff* skb = build_skbuff(syn.data(), syn.size());
  printf("skbuff %p\n", skb);
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_IP, &sock);
  printf("%d %p\n", err, sock);
  tcp_connect(sock);
}
