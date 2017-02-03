#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/slab_def.h>

void *zalloc(size_t size);

//*****************************************************************************
// mm/page_alloc.c
//*****************************************************************************

unsigned long totalram_pages;

static unsigned long __meminitdata nr_kernel_pages;
static unsigned long __meminitdata nr_all_pages;

unsigned long nr_free_buffer_pages(void)
{
	return 1024;
}

#ifndef __HAVE_ARCH_RESERVED_KERNEL_PAGES
/*
 * Returns the number of pages that arch has reserved but
 * is not known to alloc_large_system_hash().
 */
static unsigned long __init arch_reserved_kernel_pages(void)
{
	return 0;
}
#endif

/*
 * allocate a large system hash table from bootmem
 * - it is assumed that the hash table must contain an exact power-of-2
 *   quantity of entries
 * - limit is the number of hash buckets, not the total allocation size
 */
void *__init alloc_large_system_hash(const char *tablename,
				     unsigned long bucketsize,
				     unsigned long numentries,
				     int scale,
				     int flags,
				     unsigned int *_hash_shift,
				     unsigned int *_hash_mask,
				     unsigned long low_limit,
				     unsigned long high_limit)
{
	unsigned long long max = high_limit;
	unsigned long log2qty, size;
	void *table = NULL;

	/* allow the kernel cmdline to have a say */
	if (!numentries) {
		/* round applicable memory size up to nearest megabyte */
		numentries = nr_kernel_pages;
		numentries -= arch_reserved_kernel_pages();

		/* It isn't necessary when PAGE_SIZE >= 1MB */
		if (PAGE_SHIFT < 20)
			numentries = round_up(numentries, (1<<20)/PAGE_SIZE);

		/* limit to 1 bucket per 2^scale bytes of low memory */
		if (scale > PAGE_SHIFT)
			numentries >>= (scale - PAGE_SHIFT);
		else
			numentries <<= (PAGE_SHIFT - scale);

		/* Make sure we've got at least a 0-order allocation.. */
		if (unlikely(flags & HASH_SMALL)) {
			/* Makes no sense without HASH_EARLY */
			WARN_ON(!(flags & HASH_EARLY));
			if (!(numentries >> *_hash_shift)) {
				numentries = 1UL << *_hash_shift;
				BUG_ON(!numentries);
			}
		} else if (unlikely((numentries * bucketsize) < PAGE_SIZE))
			numentries = PAGE_SIZE / bucketsize;
	}
	numentries = roundup_pow_of_two(numentries);

	/* limit allocation size to 1/16 total memory by default */
	if (max == 0) {
		max = ((unsigned long long)nr_all_pages << PAGE_SHIFT) >> 4;
		do_div(max, bucketsize);
	}
	max = min(max, 0x80000000ULL);

	if (numentries < low_limit)
		numentries = low_limit;
	if (numentries > max)
		numentries = max;

	log2qty = ilog2(numentries);

	do {
		size = bucketsize << log2qty;
		table = zalloc(size);
#if 0
		if (flags & HASH_EARLY)
			table = memblock_virt_alloc_nopanic(size, 0);
		else if (hashdist)
			table = __vmalloc(size, GFP_ATOMIC, PAGE_KERNEL);
		else {
			/*
			 * If bucketsize is not a power-of-two, we may free
			 * some pages at the end of hash table which
			 * alloc_pages_exact() automatically does
			 */
			if (get_order(size) < MAX_ORDER) {
				table = alloc_pages_exact(size, GFP_ATOMIC);
				kmemleak_alloc(table, size, 1, GFP_ATOMIC);
			}
		}
#endif
	} while (!table && size > PAGE_SIZE && --log2qty);

	if (!table)
		panic("Failed to allocate %s hash table\n", tablename);

	pr_info("%s hash table entries: %ld (order: %d, %lu bytes)\n",
		tablename, 1UL << log2qty, ilog2(size) - PAGE_SHIFT, size);

	if (_hash_shift)
		*_hash_shift = log2qty;
	if (_hash_mask)
		*_hash_mask = (1 << log2qty) - 1;

	return table;
}

//*****************************************************************************
// mm/slab_common.c
//*****************************************************************************

// struct kmem_cache *kmalloc_caches[KMALLOC_SHIFT_HIGH + 1];

struct kmem_cache *
kmem_cache_create(const char *name, size_t size, size_t align,
		  unsigned long flags, void (*ctor)(void *))
{
	struct kmem_cache *s;
        printk("kmem_cache_create %s %d\n", name, size);
	s = zalloc(sizeof(struct kmem_cache));
	s->object_size = s->size = size;
	s->align = 0;  // FIXME: calculate_alignment(flags, align, size);
	s->ctor = ctor;
	s->name = name;
	return s;
}

void kmem_cache_destroy(struct kmem_cache *s)
{
	// FIXME
	printk("kmem_cache_destroy %p\n", s);
}

// mm/slab.c

void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags)
{
	void *p;
	printk("kmem_cache_alloc %s %zd\n", cachep->name, cachep->object_size);
	p = zalloc(cachep->object_size);
	if (cachep->ctor)
		cachep->ctor(p);
	return p;
}

void kmem_cache_free(struct kmem_cache *cachep, void *objp)
{
	// FIXME
	printk("kmem_cache_free %s %p\n", cachep->name, objp);
}

void *__kmalloc(size_t size, gfp_t flags)
{
	printk("__kmalloc %zd\n", size);
	return zalloc(size);
}

void *__kmalloc_track_caller(size_t size, gfp_t flags, unsigned long caller)
{
	// printk("__kmalloc_track_caller %zd\n", size);
	return zalloc(size);
}

void kfree(const void *objp)
{
	// FIXME
	printk("kfree %p\n", objp);
}

// mm/percpu.c

void __percpu *__alloc_percpu(size_t size, size_t align)
{
	return zalloc(size);
}

// mm/truncate.c
void truncate_inode_pages_final(struct address_space *mapping)
{
	printk("truncate_inode_pages_final %p\n", mapping);
}

// mm/util.c
void kvfree(const void *addr)
{
	printk("kvfree %p\n", addr);
}

// mm/vmalloc.c
void *__vmalloc(unsigned long size, gfp_t gfp_mask, pgprot_t prot)
{
	return zalloc(size);
}
