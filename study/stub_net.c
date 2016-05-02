#include <linux/skbuff.h>
#include <net/tcp.h>
#include <uapi/linux/rtnetlink.h>

// net/core/utils.c

int net_ratelimit(void)
{
	return 1;
}

// net/core/dev.c
void netdev_rx_csum_fault(struct net_device *dev)
{
	printk("netdev_rx_csum_fault %p\n", dev);
}

void net_disable_timestamp(void)
{
}

// net/core/dst.c
void dst_release(struct dst_entry *dst)
{
	printk("dst_release %p\n", dst);
}

// net/core/skbuff.c
void __kfree_skb(struct sk_buff *skb)
{
	printk("__kfree_skb %p\n", skb);
}

unsigned char *__pskb_pull_tail(struct sk_buff *skb, int delta)
{
	panic("__pskb_pull_tail %p %d\n", skb, delta);
	return NULL;
}

// net/core/sock.c
void sk_common_release(struct sock *sk)
{
	printk("sk_common_release %p\n", sk);
}

void __sk_mem_reclaim(struct sock *sk, int amount)
{
	printk("__sk_mem_reclaim %p %d\n", sk, amount);
}

void __lock_sock(struct sock *sk)
{
	printk("__lock_sock %p\n", sk);
}

void __release_sock(struct sock *sk)
{
	printk("__release_sock%p\n", sk);
}

// net/ipv4/fib_frontend.c
unsigned int inet_addr_type_table(struct net *net, __be32 addr, u32 tb_id)
{
	return RTN_LOCAL;
}

// net/core/filter.c
int sk_filter(struct sock *sk, struct sk_buff *skb)
{
	return 0;
}

// net/ipv4/tcp_fastopen.c
int sysctl_tcp_fastopen __read_mostly = TFO_CLIENT_ENABLE;
struct sock *tcp_try_fastopen(struct sock *sk, struct sk_buff *skb,
			      struct request_sock *req,
			      struct tcp_fastopen_cookie *foc,
			      struct dst_entry *dst)
{
	return NULL;
}

// net/ipv4/ip_output.c
void ip_send_unicast_reply(struct sock *sk, struct sk_buff *skb,
			   const struct ip_options *sopt,
			   __be32 daddr, __be32 saddr,
			   const struct ip_reply_arg *arg,
			   unsigned int len)
{
	printk("ip_send_unicast_reply\n");
}

// net/ipv4/tcp_metrics.c
bool tcp_peer_is_proven(struct request_sock *req, struct dst_entry *dst,
			bool paws_check, bool timestamps)
{
	panic("tcp_peer_is_proven\n");
}

// net/ipv4/tcp_timer.c
int sysctl_tcp_synack_retries __read_mostly = TCP_SYNACK_RETRIES;

// net/ipv4/route.c
extern struct rtable g_rt;
struct rtable *ip_route_output_flow(struct net *net, struct flowi4 *flp4,
				    const struct sock *sk)
{
	return &g_rt;
	// panic("ip_route_output_flow\n");
}

