// SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)
// Copyright (c) 2022 Google
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

/* maximum stack trace depth */
#define MAX_STACKS   8

/* default buffer size */
#define MAX_ENTRIES  10240

struct contention_key {
	__s32 stack_id;
};

struct contention_data {
	__u64 total_time;
	__u64 min_time;
	__u64 max_time;
	__u32 count;
	__u32 flags;
};

struct tstamp_data {
	__u64 timestamp;
	__u64 lock;
	__u32 flags;
	__s32 stack_id;
};

/* callstack storage  */
struct {
	__uint(type, BPF_MAP_TYPE_STACK_TRACE);
	__uint(key_size, sizeof(__u32));
	__uint(value_size, MAX_STACKS * sizeof(__u64));
	__uint(max_entries, MAX_ENTRIES);
} stacks SEC(".maps");

/* maintain timestamp at the beginning of contention */
struct {
	__uint(type, BPF_MAP_TYPE_TASK_STORAGE);
	__uint(map_flags, BPF_F_NO_PREALLOC);
	__type(key, int);
	__type(value, struct tstamp_data);
} tstamp SEC(".maps");

/* actual lock contention statistics */
struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(key_size, sizeof(struct contention_key));
	__uint(value_size, sizeof(struct contention_data));
	__uint(max_entries, MAX_ENTRIES);
} lock_stat SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(key_size, sizeof(__u32));
	__uint(value_size, sizeof(__u8));
	__uint(max_entries, 1);
} cpu_filter SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(key_size, sizeof(__u32));
	__uint(value_size, sizeof(__u8));
	__uint(max_entries, 1);
} task_filter SEC(".maps");

/* control flags */
int enabled;
int has_cpu;
int has_task;
int stack_skip;

/* error stat */
int lost;

static inline int can_record(void)
{
	if (has_cpu) {
		__u32 cpu = bpf_get_smp_processor_id();
		__u8 *ok;

		ok = bpf_map_lookup_elem(&cpu_filter, &cpu);
		if (!ok)
			return 0;
	}

	if (has_task) {
		__u8 *ok;
		__u32 pid = bpf_get_current_pid_tgid();

		ok = bpf_map_lookup_elem(&task_filter, &pid);
		if (!ok)
			return 0;
	}

	return 1;
}

SEC("tp_btf/contention_begin")
int contention_begin(u64 *ctx)
{
	struct task_struct *curr;
	struct tstamp_data *pelem;

	if (!enabled || !can_record())
		return 0;

	curr = bpf_get_current_task_btf();
	pelem = bpf_task_storage_get(&tstamp, curr, NULL,
				     BPF_LOCAL_STORAGE_GET_F_CREATE);
	if (!pelem || pelem->lock)
		return 0;

	pelem->timestamp = bpf_ktime_get_ns();
	pelem->lock = (__u64)ctx[0];
	pelem->flags = (__u32)ctx[1];
	pelem->stack_id = bpf_get_stackid(ctx, &stacks, BPF_F_FAST_STACK_CMP | stack_skip);

	if (pelem->stack_id < 0)
		lost++;
	return 0;
}

SEC("tp_btf/contention_end")
int contention_end(u64 *ctx)
{
	struct task_struct *curr;
	struct tstamp_data *pelem;
	struct contention_key key;
	struct contention_data *data;
	__u64 duration;

	if (!enabled)
		return 0;

	curr = bpf_get_current_task_btf();
	pelem = bpf_task_storage_get(&tstamp, curr, NULL, 0);
	if (!pelem || pelem->lock != ctx[0])
		return 0;

	duration = bpf_ktime_get_ns() - pelem->timestamp;

	key.stack_id = pelem->stack_id;
	data = bpf_map_lookup_elem(&lock_stat, &key);
	if (!data) {
		struct contention_data first = {
			.total_time = duration,
			.max_time = duration,
			.min_time = duration,
			.count = 1,
			.flags = pelem->flags,
		};

		bpf_map_update_elem(&lock_stat, &key, &first, BPF_NOEXIST);
		pelem->lock = 0;
		return 0;
	}

	__sync_fetch_and_add(&data->total_time, duration);
	__sync_fetch_and_add(&data->count, 1);

	/* FIXME: need atomic operations */
	if (data->max_time < duration)
		data->max_time = duration;
	if (data->min_time > duration)
		data->min_time = duration;

	pelem->lock = 0;
	return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
