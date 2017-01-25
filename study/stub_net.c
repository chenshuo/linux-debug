#include <linux/printk.h>

// net/core/utils.c

int net_ratelimit(void)
{
	return 1;
}

// temporary
void tcp_metrics_init(void)
{
	printk("tcp_metrics_init()\n");
}

// temporary
void tcp_tasklet_init(void)
{
	printk("tcp_tasklet_init()\n");
}
