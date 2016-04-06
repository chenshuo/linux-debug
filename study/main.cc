
extern "C"
{
// net/core/skbuff.c
extern void skb_init(void);
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
}


int main(int argc, char* argv[])
{
  skb_init();
  tcp_v4_init();
}
