#include <linux/printk.h>
#include <net/ip.h>
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
	fl4->saddr = htonl(0x7f000001);  // FIXME
	return &g_rt;
}

struct rtable *ip_route_output_flow(struct net *net, struct flowi4 *flp4,
				    const struct sock *sk)
{
	return &g_rt;
}

void __ip_select_ident(struct net *net, struct iphdr *iph, int segs)
{
	static u32 ip_idents_hashrnd __read_mostly;
	u32 hash, id;

	// net_get_random_once(&ip_idents_hashrnd, sizeof(ip_idents_hashrnd));

	hash = jhash_3words((__force u32)iph->daddr,
			    (__force u32)iph->saddr,
			    iph->protocol ^ net_hash_mix(net),
			    ip_idents_hashrnd);
	id = hash - segs;  // FIXME
	iph->id = htons(id);
}
EXPORT_SYMBOL(__ip_select_ident);

// temporary
void tcp_metrics_init(void)
{
	printk("tcp_metrics_init()\n");
}

void tcp_init_metrics(void)
{
	printk("tcp_init_metrics()\n");
}
