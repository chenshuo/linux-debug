/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Management Component Transport Protocol (MCTP)
 *
 * Copyright (c) 2021 Code Construct
 * Copyright (c) 2021 Google
 */

#ifndef __NET_MCTP_H
#define __NET_MCTP_H

#include <linux/bits.h>
#include <linux/mctp.h>
#include <net/net_namespace.h>
#include <net/sock.h>

/* MCTP packet definitions */
struct mctp_hdr {
	u8	ver;
	u8	dest;
	u8	src;
	u8	flags_seq_tag;
};

#define MCTP_VER_MIN	1
#define MCTP_VER_MAX	1

/* Definitions for flags_seq_tag field */
#define MCTP_HDR_FLAG_SOM	BIT(7)
#define MCTP_HDR_FLAG_EOM	BIT(6)
#define MCTP_HDR_FLAG_TO	BIT(3)
#define MCTP_HDR_FLAGS		GENMASK(5, 3)
#define MCTP_HDR_SEQ_SHIFT	4
#define MCTP_HDR_SEQ_MASK	GENMASK(1, 0)
#define MCTP_HDR_TAG_SHIFT	0
#define MCTP_HDR_TAG_MASK	GENMASK(2, 0)

#define MCTP_HEADER_MAXLEN	4

#define MCTP_INITIAL_DEFAULT_NET	1

static inline bool mctp_address_ok(mctp_eid_t eid)
{
	return eid >= 8 && eid < 255;
}

static inline struct mctp_hdr *mctp_hdr(struct sk_buff *skb)
{
	return (struct mctp_hdr *)skb_network_header(skb);
}

/* socket implementation */
struct mctp_sock {
	struct sock	sk;

	/* bind() params */
	unsigned int	bind_net;
	mctp_eid_t	bind_addr;
	__u8		bind_type;

	/* list of mctp_sk_key, for incoming tag lookup. updates protected
	 * by sk->net->keys_lock
	 */
	struct hlist_head keys;
};

/* Key for matching incoming packets to sockets or reassembly contexts.
 * Packets are matched on (src,dest,tag).
 *
 * Lifetime requirements:
 *
 *  - keys are free()ed via RCU
 *
 *  - a mctp_sk_key contains a reference to a struct sock; this is valid
 *    for the life of the key. On sock destruction (through unhash), the key is
 *    removed from lists (see below), and will not be observable after a RCU
 *    grace period.
 *
 *    any RX occurring within that grace period may still queue to the socket,
 *    but will hit the SOCK_DEAD case before the socket is freed.
 *
 * - these mctp_sk_keys appear on two lists:
 *     1) the struct mctp_sock->keys list
 *     2) the struct netns_mctp->keys list
 *
 *        updates to either list are performed under the netns_mctp->keys
 *        lock.
 *
 * - a key may have a sk_buff attached as part of an in-progress message
 *   reassembly (->reasm_head). The reassembly context is protected by
 *   reasm_lock, which may be acquired with the keys lock (above) held, if
 *   necessary. Consequently, keys lock *cannot* be acquired with the
 *   reasm_lock held.
 *
 * - there are two destruction paths for a mctp_sk_key:
 *
 *    - through socket unhash (see mctp_sk_unhash). This performs the list
 *      removal under keys_lock.
 *
 *    - where a key is established to receive a reply message: after receiving
 *      the (complete) reply, or during reassembly errors. Here, we clean up
 *      the reassembly context (marking reasm_dead, to prevent another from
 *      starting), and remove the socket from the netns & socket lists.
 */
struct mctp_sk_key {
	mctp_eid_t	peer_addr;
	mctp_eid_t	local_addr;
	__u8		tag; /* incoming tag match; invert TO for local */

	/* we hold a ref to sk when set */
	struct sock	*sk;

	/* routing lookup list */
	struct hlist_node hlist;

	/* per-socket list */
	struct hlist_node sklist;

	/* incoming fragment reassembly context */
	spinlock_t	reasm_lock;
	struct sk_buff	*reasm_head;
	struct sk_buff	**reasm_tailp;
	bool		reasm_dead;
	u8		last_seq;

	struct rcu_head	rcu;
};

struct mctp_skb_cb {
	unsigned int	magic;
	unsigned int	net;
	mctp_eid_t	src;
};

/* skb control-block accessors with a little extra debugging for initial
 * development.
 *
 * TODO: remove checks & mctp_skb_cb->magic; replace callers of __mctp_cb
 * with mctp_cb().
 *
 * __mctp_cb() is only for the initial ingress code; we should see ->magic set
 * at all times after this.
 */
static inline struct mctp_skb_cb *__mctp_cb(struct sk_buff *skb)
{
	struct mctp_skb_cb *cb = (void *)skb->cb;

	cb->magic = 0x4d435450;
	return cb;
}

static inline struct mctp_skb_cb *mctp_cb(struct sk_buff *skb)
{
	struct mctp_skb_cb *cb = (void *)skb->cb;

	WARN_ON(cb->magic != 0x4d435450);
	return (void *)(skb->cb);
}

/* Route definition.
 *
 * These are held in the pernet->mctp.routes list, with RCU protection for
 * removed routes. We hold a reference to the netdev; routes need to be
 * dropped on NETDEV_UNREGISTER events.
 *
 * Updates to the route table are performed under rtnl; all reads under RCU,
 * so routes cannot be referenced over a RCU grace period. Specifically: A
 * caller cannot block between mctp_route_lookup and passing the route to
 * mctp_do_route.
 */
struct mctp_route {
	mctp_eid_t		min, max;

	struct mctp_dev		*dev;
	unsigned int		mtu;
	unsigned char		type;
	int			(*output)(struct mctp_route *route,
					  struct sk_buff *skb);

	struct list_head	list;
	refcount_t		refs;
	struct rcu_head		rcu;
};

/* route interfaces */
struct mctp_route *mctp_route_lookup(struct net *net, unsigned int dnet,
				     mctp_eid_t daddr);

int mctp_do_route(struct mctp_route *rt, struct sk_buff *skb);

int mctp_local_output(struct sock *sk, struct mctp_route *rt,
		      struct sk_buff *skb, mctp_eid_t daddr, u8 req_tag);

/* routing <--> device interface */
unsigned int mctp_default_net(struct net *net);
int mctp_default_net_set(struct net *net, unsigned int index);
int mctp_route_add_local(struct mctp_dev *mdev, mctp_eid_t addr);
int mctp_route_remove_local(struct mctp_dev *mdev, mctp_eid_t addr);
void mctp_route_remove_dev(struct mctp_dev *mdev);

/* neighbour definitions */
enum mctp_neigh_source {
	MCTP_NEIGH_STATIC,
	MCTP_NEIGH_DISCOVER,
};

struct mctp_neigh {
	struct mctp_dev		*dev;
	mctp_eid_t		eid;
	enum mctp_neigh_source	source;

	unsigned char		ha[MAX_ADDR_LEN];

	struct list_head	list;
	struct rcu_head		rcu;
};

int mctp_neigh_init(void);
void mctp_neigh_exit(void);

// ret_hwaddr may be NULL, otherwise must have space for MAX_ADDR_LEN
int mctp_neigh_lookup(struct mctp_dev *dev, mctp_eid_t eid,
		      void *ret_hwaddr);
void mctp_neigh_remove_dev(struct mctp_dev *mdev);

int mctp_routes_init(void);
void mctp_routes_exit(void);

void mctp_device_init(void);
void mctp_device_exit(void);

#endif /* __NET_MCTP_H */
