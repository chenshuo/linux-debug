#include <linux/fs.h>
#include <linux/splice.h>

// fs/inode.c

void iput(struct inode *inode)
{
	printk("iput %p\n", inode);
}

// fs/splice.c

const struct pipe_buf_operations page_cache_pipe_buf_ops = {
	.can_merge = 0,
};

const struct pipe_buf_operations default_pipe_buf_ops = {
	.can_merge = 0,
};

const struct pipe_buf_operations nosteal_pipe_buf_ops = {
	.can_merge = 0,
};
