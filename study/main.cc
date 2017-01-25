#include <stdio.h>

extern "C"
{

// net/ipv4/tcp.c
void tcp_init();

}

int main()
{
  tcp_init();
}
