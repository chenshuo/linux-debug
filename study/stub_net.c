#include <linux/printk.h>
#include <uapi/linux/rtnetlink.h>

// net/core/dst.c
void dst_release(struct dst_entry *dst)
{
	printk("dst_release %p\n", dst);
}

// net/core/utils.c
int net_ratelimit(void)
{
	return 1;
}

// net/ipv4/fib_frontend.c
unsigned int inet_addr_type_table(struct net *net, __be32 addr, u32 tb_id)
{
	return RTN_LOCAL;
}

// net/ipv4/route.c
extern struct rtable g_rt;
struct rtable *__ip_route_output_key_hash(struct net *net, struct flowi4 *fl4,
					  int mp_hash)
{
	return &g_rt;
}

struct rtable *ip_route_output_flow(struct net *net, struct flowi4 *flp4,
				    const struct sock *sk)
{
	return &g_rt;
}

// temporary
void tcp_metrics_init(void)
{
	printk("tcp_metrics_init()\n");
}
