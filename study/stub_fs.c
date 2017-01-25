#include <linux/fs.h>
#include <linux/slab.h>

// fs/inode.c

void iput(struct inode *inode)
{
	printk("iput %p\n", inode);
}

