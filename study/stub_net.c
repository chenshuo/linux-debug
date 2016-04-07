#include <linux/skbuff.h>

// net/core/utils.c

int net_ratelimit(void)
{
	return 1;
}

// net/core/skbuff.c
void __kfree_skb(struct sk_buff *skb)
{
	printk("__kfree_skb %p\n", skb);
}

// net/core/dst.c
void dst_release(struct dst_entry *dst)
{
	printk("dst_release %p\n", dst);
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
