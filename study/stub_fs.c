#include <linux/fs.h>
#include <linux/splice.h>

// fs/fs-writeback.c
void inode_wait_for_writeback(struct inode *inode)
{
	printk("inode_wait_for_writeback %p\n", inode);
}

// fs/notify/fsnotify.c
void __fsnotify_inode_delete(struct inode *inode)
{
	printk("__fsnotify_inode_delete %p\n", inode);
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
