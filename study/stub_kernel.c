#include <linux/kernel_stat.h>
#include <linux/percpu.h>

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
