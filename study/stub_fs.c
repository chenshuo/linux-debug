#include <linux/fs.h>

// fs/inode.c

void iput(struct inode *inode)
{
	printk("iput %p\n", inode);
}

