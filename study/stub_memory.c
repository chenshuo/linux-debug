#include <linux/slab.h>
#include <linux/slab_def.h>

void* zalloc(size_t size);

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
