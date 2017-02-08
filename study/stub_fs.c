#include <linux/fs.h>
#include <linux/splice.h>

// fs/fs-writeback.c
void inode_wait_for_writeback(struct inode *inode)
{
	printk("inode_wait_for_writeback %p\n", inode);
}

// fs/namei.c
void path_put(const struct path *path)
{
	printk("path_put %p\n", path);
	// dput(path->dentry);
	// mntput(path->mnt);
}
EXPORT_SYMBOL(path_put);

// fs/notify/fsnotify.c
void __fsnotify_inode_delete(struct inode *inode)
{
	printk("__fsnotify_inode_delete %p\n", inode);
}

// fs/read_write.c
loff_t no_llseek(struct file *file, loff_t offset, int whence)
{
	return -ESPIPE;
}
EXPORT_SYMBOL(no_llseek);

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
