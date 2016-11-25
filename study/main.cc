#include <stdio.h>
#include <netinet/in.h>
#include <string>

extern "C"
{
// fs/inode.c
// extern unsigned int get_next_ino(void);
struct sk_buff;
void get_output(//const struct sk_buff *skb,
		void** data,
		int* len);
// net/socket.c
extern int sock_init(void);
struct socket;
extern int sock_create(int family, int type, int protocol, struct socket **res);
// net/core/sock.c
extern int sock_setsockopt(struct socket *sock, int level, int optname,
                           void *optval, unsigned int optlen);
// net/ipv4/af_inet.c
extern int inet_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
extern int tcp_v4_rcv(struct sk_buff *skb);
// study/helper.c
extern int tcp_connect(struct socket *sock);
extern int tcp_bind(struct socket *sock);
extern int tcp_listen(struct socket *sock, int backlog);
struct sk_buff *build_skbuff(const void* ippacket, unsigned int len);
// helper.c
extern void schen_dst_init(void);
}

using std::string;
string build_syn(bool ether);
string build_ack(const string& in);

int main(int argc, char* argv[])
{
  schen_dst_init();
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
  printf("sock_create %d %p\n", err, sock);
  {
    int one = 1;
    err = sock_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    printf("sock_setsockopt %d\n", err);
  }
  tcp_bind(sock);
  tcp_listen(sock, 5);
  string syn = build_syn(false);
  sk_buff* skb = build_skbuff(syn.data(), syn.size());
  printf("skbuff %p %zd\n", skb, syn.size());
  printf("tcp_v4_rcv SYN %d\n", tcp_v4_rcv(skb));
  void* data = NULL;
  int len = 0;
  get_output(&data, &len);
  printf("len = %d\n", len);
  string synack(static_cast<char*>(data), len);
  string ack = build_ack(synack);
  skb = build_skbuff(ack.data(), ack.size());
  printf("tcp_v4_rcv ACK %d\n", tcp_v4_rcv(skb));
  err = sock_create(AF_INET, SOCK_STREAM, IPPROTO_IP, &sock);
  printf("%d %p\n", err, sock);
  tcp_connect(sock);
}
