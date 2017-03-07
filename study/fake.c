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

void ip_rt_get_source(void) { puts("UNDEF ip_rt_get_source"); abort(); }

void kill_fasync(void) { puts("UNDEF kill_fasync"); abort(); }

void kthread_should_stop(void) { puts("UNDEF kthread_should_stop"); abort(); }

void ns_capable(void) { puts("UNDEF ns_capable"); abort(); }

void __put_page(void) { puts("UNDEF __put_page"); abort(); }

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

void inet_addr_type_dev_table(void) { puts("UNDEF inet_addr_type_dev_table"); abort(); }

void inet_getpeer(void) { puts("UNDEF inet_getpeer"); abort(); }

void inet_peer_xrlim_allow(void) { puts("UNDEF inet_peer_xrlim_allow"); abort(); }

void inet_putpeer(void) { puts("UNDEF inet_putpeer"); abort(); }

void inet_select_addr(void) { puts("UNDEF inet_select_addr"); abort(); }

void ip_route_input_noref(void) { puts("UNDEF ip_route_input_noref"); abort(); }

void ping_rcv(void) { puts("UNDEF ping_rcv"); abort(); }

void __qdisc_run(void) { puts("UNDEF __qdisc_run"); abort(); }

void sch_direct_xmit(void) { puts("UNDEF sch_direct_xmit"); abort(); }

void __skb_get_hash(void) { puts("UNDEF __skb_get_hash"); abort(); }

void call_netevent_notifiers(void) { puts("UNDEF call_netevent_notifiers"); abort(); }

void free_pages(void) { puts("UNDEF free_pages"); abort(); }

void full_name_hash(void) { puts("UNDEF full_name_hash"); abort(); }

void __get_free_pages(void) { puts("UNDEF __get_free_pages"); abort(); }

void inet_addr_onlink(void) { puts("UNDEF inet_addr_onlink"); abort(); }

void inet_confirm_addr(void) { puts("UNDEF inet_confirm_addr"); abort(); }

void iptunnel_metadata_reply(void) { puts("UNDEF iptunnel_metadata_reply"); abort(); }

void nla_put(void) { puts("UNDEF nla_put"); abort(); }

void rtnl_is_locked(void) { puts("UNDEF rtnl_is_locked"); abort(); }

void rtnl_lock(void) { puts("UNDEF rtnl_lock"); abort(); }

void rtnl_notify(void) { puts("UNDEF rtnl_notify"); abort(); }

void rtnl_set_sk_err(void) { puts("UNDEF rtnl_set_sk_err"); abort(); }

void rtnl_unlock(void) { puts("UNDEF rtnl_unlock"); abort(); }

void ip_check_mc_rcu(void) { puts("UNDEF ip_check_mc_rcu"); abort(); }

void ip_mc_sf_allow(void) { puts("UNDEF ip_mc_sf_allow"); abort(); }

void ipv4_sk_redirect(void) { puts("UNDEF ipv4_sk_redirect"); abort(); }

void ipv4_sk_update_pmtu(void) { puts("UNDEF ipv4_sk_update_pmtu"); abort(); }

void PDE_DATA(void) { puts("UNDEF PDE_DATA"); abort(); }

void proc_create_data(void) { puts("UNDEF proc_create_data"); abort(); }

void put_cmsg(void) { puts("UNDEF put_cmsg"); abort(); }

void register_pernet_subsys(void) { puts("UNDEF register_pernet_subsys"); abort(); }

void remove_proc_entry(void) { puts("UNDEF remove_proc_entry"); abort(); }

void seq_lseek(void) { puts("UNDEF seq_lseek"); abort(); }

void seq_open_net(void) { puts("UNDEF seq_open_net"); abort(); }

void seq_pad(void) { puts("UNDEF seq_pad"); abort(); }

void seq_printf(void) { puts("UNDEF seq_printf"); abort(); }

void seq_puts(void) { puts("UNDEF seq_puts"); abort(); }

void seq_read(void) { puts("UNDEF seq_read"); abort(); }

void seq_release_net(void) { puts("UNDEF seq_release_net"); abort(); }

void get_task_mm(void) { puts("UNDEF get_task_mm"); abort(); }

void __lock_task_sighand(void) { puts("UNDEF __lock_task_sighand"); abort(); }

void mmput(void) { puts("UNDEF mmput"); abort(); }

void task_cputime_adjusted(void) { puts("UNDEF task_cputime_adjusted"); abort(); }

void thread_group_cputime_adjusted(void) { puts("UNDEF thread_group_cputime_adjusted"); abort(); }

void __ip_dev_find(void) { puts("UNDEF __ip_dev_find"); abort(); }

void ip_mc_gsfget(void) { puts("UNDEF ip_mc_gsfget"); abort(); }

void ip_mc_join_group(void) { puts("UNDEF ip_mc_join_group"); abort(); }

void ip_mc_leave_group(void) { puts("UNDEF ip_mc_leave_group"); abort(); }

void ip_mc_msfget(void) { puts("UNDEF ip_mc_msfget"); abort(); }

void ip_mc_msfilter(void) { puts("UNDEF ip_mc_msfilter"); abort(); }

void ip_mc_source(void) { puts("UNDEF ip_mc_source"); abort(); }

void ip_tos2prio(void) { puts("UNDEF ip_tos2prio"); abort(); }

void kmem_cache_free_bulk(void) { puts("UNDEF kmem_cache_free_bulk"); abort(); }

void free_percpu(void) { puts("UNDEF free_percpu"); abort(); }

void kobject_put(void) { puts("UNDEF kobject_put"); abort(); }

void queue_work_on(void) { puts("UNDEF queue_work_on"); abort(); }

void system_wq(void) { puts("UNDEF system_wq"); abort(); }
