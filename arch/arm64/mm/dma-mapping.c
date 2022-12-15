// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 ARM Ltd.
 * Author: Catalin Marinas <catalin.marinas@arm.com>
 */

#include <linux/gfp.h>
#include <linux/cache.h>
#include <linux/dma-map-ops.h>
#include <linux/iommu.h>
#include <xen/xen.h>

#include <asm/cacheflush.h>
#include <asm/xen/xen-ops.h>

void arch_sync_dma_for_device(phys_addr_t paddr, size_t size,
			      enum dma_data_direction dir)
{
	unsigned long start = (unsigned long)phys_to_virt(paddr);

	dcache_clean_poc(start, start + size);
}

void arch_sync_dma_for_cpu(phys_addr_t paddr, size_t size,
			   enum dma_data_direction dir)
{
	unsigned long start = (unsigned long)phys_to_virt(paddr);

	if (dir == DMA_TO_DEVICE)
		return;

	dcache_inval_poc(start, start + size);
}

void arch_dma_prep_coherent(struct page *page, size_t size)
{
	unsigned long start = (unsigned long)page_address(page);

	/*
	 * The architecture only requires a clean to the PoC here in order to
	 * meet the requirements of the DMA API. However, some vendors (i.e.
	 * Qualcomm) abuse the DMA API for transferring buffers from the
	 * non-secure to the secure world, resetting the system if a non-secure
	 * access shows up after the buffer has been transferred:
	 *
	 * https://lore.kernel.org/r/20221114110329.68413-1-manivannan.sadhasivam@linaro.org
	 *
	 * Using clean+invalidate appears to make this issue less likely, but
	 * the drivers themselves still need fixing as the CPU could issue a
	 * speculative read from the buffer via the linear mapping irrespective
	 * of the cache maintenance we use. Once the drivers are fixed, we can
	 * relax this to a clean operation.
	 */
	dcache_clean_inval_poc(start, start + size);
}

#ifdef CONFIG_IOMMU_DMA
void arch_teardown_dma_ops(struct device *dev)
{
	dev->dma_ops = NULL;
}
#endif

void arch_setup_dma_ops(struct device *dev, u64 dma_base, u64 size,
			const struct iommu_ops *iommu, bool coherent)
{
	int cls = cache_line_size_of_cpu();

	WARN_TAINT(!coherent && cls > ARCH_DMA_MINALIGN,
		   TAINT_CPU_OUT_OF_SPEC,
		   "%s %s: ARCH_DMA_MINALIGN smaller than CTR_EL0.CWG (%d < %d)",
		   dev_driver_string(dev), dev_name(dev),
		   ARCH_DMA_MINALIGN, cls);

	dev->dma_coherent = coherent;
	if (iommu)
		iommu_setup_dma_ops(dev, dma_base, dma_base + size - 1);

	xen_setup_dma_ops(dev);
}
