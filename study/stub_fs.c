#include <linux/fs.h>

void iput(struct inode *inode)
{
	printk("iput %p\n", inode);
}
