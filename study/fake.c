extern void abort(void);
extern void puts(const char*);

void __alloc_pages_nodemask(void) { puts("UNDEF __alloc_pages_nodemask"); abort(); }

void bpf_prog_free(void) { puts("UNDEF bpf_prog_free"); abort(); }

void _cond_resched(void) { puts("UNDEF _cond_resched"); abort(); }

void __copy_from_user(void) { puts("UNDEF __copy_from_user"); abort(); }

void __copy_to_user(void) { puts("UNDEF __copy_to_user"); abort(); }

void crypto_alloc_base(void) { puts("UNDEF crypto_alloc_base"); abort(); }

void crypto_destroy_tfm(void) { puts("UNDEF crypto_destroy_tfm"); abort(); }

void default_wake_function(void) { puts("UNDEF default_wake_function"); abort(); }

void __do_once_done(void) { puts("UNDEF __do_once_done"); abort(); }

void __do_once_start(void) { puts("UNDEF __do_once_start"); abort(); }

void dump_stack(void) { puts("UNDEF dump_stack"); abort(); }

void fib_compute_spec_dst(void) { puts("UNDEF fib_compute_spec_dst"); abort(); }

void file_ns_capable(void) { puts("UNDEF file_ns_capable"); abort(); }

void __free_page_frag(void) { puts("UNDEF __free_page_frag"); abort(); }

void get_random_bytes(void) { puts("UNDEF get_random_bytes"); abort(); }

void gfp_pfmemalloc_allowed(void) { puts("UNDEF gfp_pfmemalloc_allowed"); abort(); }

void hrtimer_start_range_ns(void) { puts("UNDEF hrtimer_start_range_ns"); abort(); }

void inet_addr_type(void) { puts("UNDEF inet_addr_type"); abort(); }

void ip_getsockopt(void) { puts("UNDEF ip_getsockopt"); abort(); }

void ip_local_error(void) { puts("UNDEF ip_local_error"); abort(); }

void ip_recv_error(void) { puts("UNDEF ip_recv_error"); abort(); }

void ip_rt_get_source(void) { puts("UNDEF ip_rt_get_source"); abort(); }

void ip_setsockopt(void) { puts("UNDEF ip_setsockopt"); abort(); }

void kill_fasync(void) { puts("UNDEF kill_fasync"); abort(); }

void kthread_should_stop(void) { puts("UNDEF kthread_should_stop"); abort(); }

void ns_capable(void) { puts("UNDEF ns_capable"); abort(); }

void __put_cred(void) { puts("UNDEF __put_cred"); abort(); }

void __put_page(void) { puts("UNDEF __put_page"); abort(); }

void put_pid(void) { puts("UNDEF put_pid"); abort(); }

void schedule(void) { puts("UNDEF schedule"); abort(); }

void schedule_hrtimeout_range(void) { puts("UNDEF schedule_hrtimeout_range"); abort(); }

void send_sig(void) { puts("UNDEF send_sig"); abort(); }

void send_sigurg(void) { puts("UNDEF send_sigurg"); abort(); }

void sock_diag_broadcast_destroy(void) { puts("UNDEF sock_diag_broadcast_destroy"); abort(); }

void __strncpy_from_user(void) { puts("UNDEF __strncpy_from_user"); abort(); }

void tcp_fastopen_cache_get(void) { puts("UNDEF tcp_fastopen_cache_get"); abort(); }

void tcp_fastopen_cache_set(void) { puts("UNDEF tcp_fastopen_cache_set"); abort(); }

void tcp_fetch_timewait_stamp(void) { puts("UNDEF tcp_fetch_timewait_stamp"); abort(); }

void tcp_peer_is_proven(void) { puts("UNDEF tcp_peer_is_proven"); abort(); }

void tcp_remember_stamp(void) { puts("UNDEF tcp_remember_stamp"); abort(); }

void tcp_tw_remember_stamp(void) { puts("UNDEF tcp_tw_remember_stamp"); abort(); }

void tcp_update_metrics(void) { puts("UNDEF tcp_update_metrics"); abort(); }

void wake_up_process(void) { puts("UNDEF wake_up_process"); abort(); }

void warn_slowpath_fmt(void) { puts("UNDEF warn_slowpath_fmt"); abort(); }

void arp_ioctl(void) { puts("UNDEF arp_ioctl"); abort(); }

void devinet_ioctl(void) { puts("UNDEF devinet_ioctl"); abort(); }

void ip_mc_drop_socket(void) { puts("UNDEF ip_mc_drop_socket"); abort(); }

void ip_rt_ioctl(void) { puts("UNDEF ip_rt_ioctl"); abort(); }

void splice_to_pipe(void) { puts("UNDEF splice_to_pipe"); abort(); }

void bd_forget(void) { puts("UNDEF bd_forget"); abort(); }

void cd_forget(void) { puts("UNDEF cd_forget"); abort(); }

void inode_io_list_del(void) { puts("UNDEF inode_io_list_del"); abort(); }

void list_lru_add(void) { puts("UNDEF list_lru_add"); abort(); }

void list_lru_del(void) { puts("UNDEF list_lru_del"); abort(); }

void __mark_inode_dirty(void) { puts("UNDEF __mark_inode_dirty"); abort(); }

void write_inode_now(void) { puts("UNDEF write_inode_now"); abort(); }

