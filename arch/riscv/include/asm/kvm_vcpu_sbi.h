/* SPDX-License-Identifier: GPL-2.0-only */
/**
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *     Atish Patra <atish.patra@wdc.com>
 */

#ifndef __RISCV_KVM_VCPU_SBI_H__
#define __RISCV_KVM_VCPU_SBI_H__

#define KVM_SBI_IMPID 3

#define KVM_SBI_VERSION_MAJOR 1
#define KVM_SBI_VERSION_MINOR 0

struct kvm_vcpu_sbi_extension {
	unsigned long extid_start;
	unsigned long extid_end;
	/**
	 * SBI extension handler. It can be defined for a given extension or group of
	 * extension. But it should always return linux error codes rather than SBI
	 * specific error codes.
	 */
	int (*handler)(struct kvm_vcpu *vcpu, struct kvm_run *run,
		       unsigned long *out_val, struct kvm_cpu_trap *utrap,
		       bool *exit);
};

void kvm_riscv_vcpu_sbi_forward(struct kvm_vcpu *vcpu, struct kvm_run *run);
void kvm_riscv_vcpu_sbi_system_reset(struct kvm_vcpu *vcpu,
				     struct kvm_run *run,
				     u32 type, u64 flags);
const struct kvm_vcpu_sbi_extension *kvm_vcpu_sbi_find_ext(unsigned long extid);

#ifdef CONFIG_RISCV_SBI_V01
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_v01;
#endif
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_base;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_time;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_ipi;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_rfence;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_srst;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_hsm;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_experimental;
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_vendor;

#endif /* __RISCV_KVM_VCPU_SBI_H__ */
