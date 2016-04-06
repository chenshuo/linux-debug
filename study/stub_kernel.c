#include <linux/rcupdate.h>

void call_rcu_sched(struct rcu_head *head, rcu_callback_t func)
{
	printk("call_rcu_sched %p %p\n", head, func);
}
