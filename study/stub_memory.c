#include <linux/slab.h>
#include <linux/slab_def.h>

void *zalloc(size_t size);

// mm/slab_common.c

struct kmem_cache *
kmem_cache_create(const char *name, size_t size, size_t align,
                  unsigned long flags, void (*ctor)(void *))
{
	struct kmem_cache *s = zalloc(sizeof(struct kmem_cache));
	// printf("kmem_cache_create %s\n", name);
	s->object_size = s->size = size;
	s->align = 0;  // FIXME: calculate_alignment(flags, align, size);
	s->ctor = ctor;
	s->name = name;
	return s;
}

// mm/slab.c

void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags)
{
	return zalloc(cachep->object_size);
}

void kmem_cache_free(struct kmem_cache *cachep, void *objp)
{
	// FIXME
	printk("kmem_cache_free %p %p\n", cachep, objp);
}

void *__kmalloc(size_t size, gfp_t flags)
{
	return zalloc(size);
}

void kfree(const void *objp)
{
	// FIXME
	printk("kfree %p\n", objp);
}
