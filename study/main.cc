#include <stdio.h>
#include <netinet/in.h>
#include <string>

extern "C"
{
// fs/inode.c
// extern unsigned int get_next_ino(void);
struct sk_buff;
// net/socket.c
extern int sock_init(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);
// net/ipv4/af_inet.c
extern int inet_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
extern int tcp_v4_rcv(struct sk_buff *skb);
// study/helper.c
extern int tcp_connect(struct socket *sock);
extern int tcp_bind(struct socket *sock);
struct sk_buff *build_skbuff(const void* ippacket, unsigned int len);
}

using std::string;
string build_syn(bool ether);

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
  string syn = build_syn(false);
  sk_buff* skb = build_skbuff(syn.data(), syn.size());
  printf("skbuff %p %zd\n", skb, syn.size());
  printf("tcp_v4_rcv %d\n", tcp_v4_rcv(skb));
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_IP, &sock);
  printf("%d %p\n", err, sock);
  tcp_connect(sock);
}
