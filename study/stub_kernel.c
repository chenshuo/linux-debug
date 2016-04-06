#include <linux/rcupdate.h>

// kernel/rcu/tiny.c
void call_rcu_sched(struct rcu_head *head, rcu_callback_t func)
{
	printk("call_rcu_sched %p %p\n", head, func);
}

// kernel/softirq.c
void __local_bh_enable_ip(unsigned long ip, unsigned int cnt)
{
	printk("__local_bh_enable_ip %lx %d\n", ip, cnt);
}
