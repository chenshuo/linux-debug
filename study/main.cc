
extern "C"
{
// net/ipv4/tcp_ipv4.c
extern void tcp_v4_init(void);
}


int main(int argc, char* argv[])
{
  tcp_v4_init();
}
