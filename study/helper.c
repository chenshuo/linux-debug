#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/file.h>
#include <linux/net.h>
#include <linux/interrupt.h>
#include <linux/thread_info.h>
#include <linux/rcupdate.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/if_bridge.h>
#include <linux/if_frad.h>
#include <linux/if_vlan.h>
#include <linux/ptp_classify.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/cache.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/mount.h>
#include <linux/security.h>
#include <linux/syscalls.h>
#include <linux/compat.h>
#include <linux/kmod.h>
#include <linux/audit.h>
#include <linux/wireless.h>
#include <linux/nsproxy.h>
#include <linux/magic.h>
#include <linux/slab.h>
#include <linux/xattr.h>

#include <asm/uaccess.h>
#include <asm/unistd.h>

#include <net/compat.h>
#include <net/wext.h>
#include <net/cls_cgroup.h>

#include <net/sock.h>
#include <linux/netfilter.h>

#include <linux/if_tun.h>
#include <linux/ipv6_route.h>
#include <linux/route.h>
#include <linux/sockios.h>
#include <linux/atalk.h>
#include <net/busy_poll.h>
#include <linux/errqueue.h>

int pcap_write(const char *buf, int len, int origlen);
int tcp_connect_lo_2222(struct socket *sock)
{
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_port = htons(2222),
		.sin_addr.s_addr = htonl(INADDR_LOOPBACK),
	};
	// struct proto_ops inet_stream_ops.connect -> inet_stream_connect
	int err = sock->ops->connect(sock, (struct sockaddr *)&address, sizeof address, O_NONBLOCK);
				     //sock->file->f_flags);
	return err;
}

int tcp_bind(struct socket *sock)
{
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_port = htons(2222),
		.sin_addr.s_addr = INADDR_ANY
	};
	// struct proto_ops inet_stream_ops.bind -> inet_bind
	int err = sock->ops->bind(sock, (struct sockaddr *)&address, sizeof address);
	return err;
}

int tcp_listen(struct socket *sock, int backlog)
{
	int err = sock->ops->listen(sock, backlog);
	return err;
}

unsigned int ipv4_mtu(const struct dst_entry *dst)
{
	return 1500;
}

const struct sk_buff *output_skb;
int ip_output_fake(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	output_skb = skb;
	pcap_write(skb->data, skb->len, skb->len);
	// panic("ip_output");
	return 0;
}

const u32 dst_default_metrics[RTAX_MAX + 1] = {
	[RTAX_ADVMSS-1] = 1460,
	[RTAX_MAX] = 0xdeadbeef,
};

struct net_device g_dev = {
  // .nd_net = { &init_net }
};

// include/net/route.h
// struct rtable {
//	struct dst_entry	dst;
//	...

struct dst_ops ipv4_dst_ops = {
	.mtu = ipv4_mtu,
};
struct rtable g_rt = {
	.rt_iif = 123,
	.rt_type = RTN_LOCAL,
	.dst = {
		.dev = &g_dev,
		.ops = &ipv4_dst_ops,
	},
};

void schen_dst_init(void)
{
	dst_init_metrics(&g_rt.dst, dst_default_metrics, true);
	g_rt.dst.output = ip_output_fake;
}

