#include <linux/capability.h>
#include <linux/kernel_stat.h>
#include <linux/percpu.h>
#include <linux/rcupdate.h>

unsigned long volatile jiffies;

// kernel/locking/mutex.c

void mutex_lock(struct mutex *lock)
{
}

void mutex_unlock(struct mutex *lock)
{
}

// kernel/sched/core.c
void resched_cpu(int cpu)
{
}


DEFINE_PER_CPU(struct kernel_stat, kstat);
DEFINE_PER_CPU(struct kernel_cpustat, kernel_cpustat);

EXPORT_PER_CPU_SYMBOL(kstat);
EXPORT_PER_CPU_SYMBOL(kernel_cpustat);

// kernel/time/timekeeping.c
ktime_t ktime_get_with_offset(enum tk_offsets offs)
{
	ktime_t kt = { 0 };
	return kt;
}

unsigned long get_seconds(void)
{
	panic("get_seconds");
	return 0;
}

void getnstimeofday64(struct timespec64 *ts)
{
	panic("getnstimeofday64");
	return 0;
}
