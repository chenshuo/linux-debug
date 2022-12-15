// SPDX-License-Identifier: GPL-2.0
/*
 * Access kernel memory without faulting -- s390 specific implementation.
 *
 * Copyright IBM Corp. 2009, 2015
 *
 */

#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/gfp.h>
#include <linux/cpu.h>
#include <linux/uio.h>
#include <asm/asm-extable.h>
#include <asm/ctl_reg.h>
#include <asm/io.h>
#include <asm/abs_lowcore.h>
#include <asm/stacktrace.h>
#include <asm/maccess.h>

unsigned long __bootdata_preserved(__memcpy_real_area);
static __ro_after_init pte_t *memcpy_real_ptep;
static DEFINE_MUTEX(memcpy_real_mutex);

static notrace long s390_kernel_write_odd(void *dst, const void *src, size_t size)
{
	unsigned long aligned, offset, count;
	char tmp[8];

	aligned = (unsigned long) dst & ~7UL;
	offset = (unsigned long) dst & 7UL;
	size = min(8UL - offset, size);
	count = size - 1;
	asm volatile(
		"	bras	1,0f\n"
		"	mvc	0(1,%4),0(%5)\n"
		"0:	mvc	0(8,%3),0(%0)\n"
		"	ex	%1,0(1)\n"
		"	lg	%1,0(%3)\n"
		"	lra	%0,0(%0)\n"
		"	sturg	%1,%0\n"
		: "+&a" (aligned), "+&a" (count), "=m" (tmp)
		: "a" (&tmp), "a" (&tmp[offset]), "a" (src)
		: "cc", "memory", "1");
	return size;
}

/*
 * s390_kernel_write - write to kernel memory bypassing DAT
 * @dst: destination address
 * @src: source address
 * @size: number of bytes to copy
 *
 * This function writes to kernel memory bypassing DAT and possible page table
 * write protection. It writes to the destination using the sturg instruction.
 * Therefore we have a read-modify-write sequence: the function reads eight
 * bytes from destination at an eight byte boundary, modifies the bytes
 * requested and writes the result back in a loop.
 */
static DEFINE_SPINLOCK(s390_kernel_write_lock);

notrace void *s390_kernel_write(void *dst, const void *src, size_t size)
{
	void *tmp = dst;
	unsigned long flags;
	long copied;

	spin_lock_irqsave(&s390_kernel_write_lock, flags);
	if (!(flags & PSW_MASK_DAT)) {
		memcpy(dst, src, size);
	} else {
		while (size) {
			copied = s390_kernel_write_odd(tmp, src, size);
			tmp += copied;
			src += copied;
			size -= copied;
		}
	}
	spin_unlock_irqrestore(&s390_kernel_write_lock, flags);

	return dst;
}

void __init memcpy_real_init(void)
{
	memcpy_real_ptep = vmem_get_alloc_pte(__memcpy_real_area, true);
	if (!memcpy_real_ptep)
		panic("Couldn't setup memcpy real area");
}

size_t memcpy_real_iter(struct iov_iter *iter, unsigned long src, size_t count)
{
	size_t len, copied, res = 0;
	unsigned long phys, offset;
	void *chunk;
	pte_t pte;

	while (count) {
		phys = src & PAGE_MASK;
		offset = src & ~PAGE_MASK;
		chunk = (void *)(__memcpy_real_area + offset);
		len = min(count, PAGE_SIZE - offset);
		pte = mk_pte_phys(phys, PAGE_KERNEL_RO);

		mutex_lock(&memcpy_real_mutex);
		if (pte_val(pte) != pte_val(*memcpy_real_ptep)) {
			__ptep_ipte(__memcpy_real_area, memcpy_real_ptep, 0, 0, IPTE_GLOBAL);
			set_pte(memcpy_real_ptep, pte);
		}
		copied = copy_to_iter(chunk, len, iter);
		mutex_unlock(&memcpy_real_mutex);

		count -= copied;
		src += copied;
		res += copied;
		if (copied < len)
			break;
	}
	return res;
}

int memcpy_real(void *dest, unsigned long src, size_t count)
{
	struct iov_iter iter;
	struct kvec kvec;

	kvec.iov_base = dest;
	kvec.iov_len = count;
	iov_iter_kvec(&iter, WRITE, &kvec, 1, count);
	if (memcpy_real_iter(&iter, src, count) < count)
		return -EFAULT;
	return 0;
}

/*
 * Find CPU that owns swapped prefix page
 */
static int get_swapped_owner(phys_addr_t addr)
{
	phys_addr_t lc;
	int cpu;

	for_each_online_cpu(cpu) {
		lc = virt_to_phys(lowcore_ptr[cpu]);
		if (addr > lc + sizeof(struct lowcore) - 1 || addr < lc)
			continue;
		return cpu;
	}
	return -1;
}

/*
 * Convert a physical pointer for /dev/mem access
 *
 * For swapped prefix pages a new buffer is returned that contains a copy of
 * the absolute memory. The buffer size is maximum one page large.
 */
void *xlate_dev_mem_ptr(phys_addr_t addr)
{
	void *ptr = phys_to_virt(addr);
	void *bounce = ptr;
	struct lowcore *abs_lc;
	unsigned long flags;
	unsigned long size;
	int this_cpu, cpu;

	cpus_read_lock();
	this_cpu = get_cpu();
	if (addr >= sizeof(struct lowcore)) {
		cpu = get_swapped_owner(addr);
		if (cpu < 0)
			goto out;
	}
	bounce = (void *)__get_free_page(GFP_ATOMIC);
	if (!bounce)
		goto out;
	size = PAGE_SIZE - (addr & ~PAGE_MASK);
	if (addr < sizeof(struct lowcore)) {
		abs_lc = get_abs_lowcore(&flags);
		ptr = (void *)abs_lc + addr;
		memcpy(bounce, ptr, size);
		put_abs_lowcore(abs_lc, flags);
	} else if (cpu == this_cpu) {
		ptr = (void *)(addr - virt_to_phys(lowcore_ptr[cpu]));
		memcpy(bounce, ptr, size);
	} else {
		memcpy(bounce, ptr, size);
	}
out:
	put_cpu();
	cpus_read_unlock();
	return bounce;
}

/*
 * Free converted buffer for /dev/mem access (if necessary)
 */
void unxlate_dev_mem_ptr(phys_addr_t addr, void *ptr)
{
	if (addr != virt_to_phys(ptr))
		free_page((unsigned long)ptr);
}
