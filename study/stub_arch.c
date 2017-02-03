#include <net/net_namespace.h>

unsigned long volatile jiffies;

struct tcp_mib g_tcp_mib;
struct linux_mib g_linux_mib;
struct net init_net = {
	.dev_base_head = LIST_HEAD_INIT(init_net.dev_base_head),
	.mib = {
		.tcp_statistics = &g_tcp_mib,
		.net_statistics = &g_linux_mib,
	},
	.ipv4 = {
		.ip_local_ports = {
			.range = {32768, 60999}
		}
	}
};
EXPORT_SYMBOL(init_net);

struct thread_info g_thread_info = {
	.task = &init_task,
	.addr_limit = KERNEL_DS,
};

// arch/um/include/asm/thread_info.h
struct thread_info *current_thread_info(void)
{
	return &g_thread_info;
}

// arch/um/kernel/um_arch.c

int get_signals(void)
{
	return 0;
}

int set_signals(int enable)
{
	return 0;
}

void block_signals(void)
{
}

void unblock_signals(void)
{
}

