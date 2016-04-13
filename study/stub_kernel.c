#include <linux/capability.h>
#include <linux/rcupdate.h>

// kernel/cred.c
void __put_cred(struct cred *cred)
{
}

// kernel/pid.c
void put_pid(struct pid *pid)
{
}

// kernel/rcu/tiny.c
void call_rcu_sched(struct rcu_head *head, rcu_callback_t func)
{
	printk("call_rcu_sched %p %p\n", head, func);
}

// kernel/softirq.c
void __local_bh_enable_ip(unsigned long ip, unsigned int cnt)
{
	// printk("__local_bh_enable_ip %lx %d\n", ip, cnt);
}

// kernel/locking/mutex.c
void __mutex_lock_slowpath(atomic_t *lock_count)
{
	printk("__mutex_lock_slowpath %p\n", lock_count);
}

void __mutex_unlock_slowpath(atomic_t *lock_count)
{
	printk("__mutex_unlock_slowpath %p\n", lock_count);
}

// kernel/sched/clock.c
u64 local_clock(void)
{
	// FIXME
	return 0;
}

unsigned long volatile jiffies;

// kernel/sched/wait.c
void __wake_up(wait_queue_head_t *q, unsigned int mode,
			int nr_exclusive, void *key)
{
	printk("__wake_up %p\n", q);
}

// kernel/capability.c
bool ns_capable(struct user_namespace *ns, int cap)
{
	return false;
}

// drivers/char/random.c
// void get_random_bytes(void *buf, int nbytes)
// {
// }

// kernel/time/timekeeping.c
ktime_t ktime_get_with_offset(enum tk_offsets offs)
{
	ktime_t kt = { 0 };
	return kt;
}

