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
