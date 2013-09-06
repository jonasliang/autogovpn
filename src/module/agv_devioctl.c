#include <linux/types.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include "agv_common.h"
#include "agv_debug.h"
#include "agv_devioctl.h"

#define AGV_MAX_DEBUGCMD 64

uint32_t g_agv_debug = 0;
static int32_t s_agv_dev_major;

static int32_t agv_chr_open(struct inode *inode, struct file * file)
{
	if ( unlikely(NULL == inode || NULL == file) ) {
		return -ENODEV;
	}

	AGV_PRINT("AGV chr dev opened.\n");

	file->private_data = NULL;
	return 0;	
}

static ssize_t agv_chr_read(struct file * file, char __user * buf, size_t count, 
		loff_t *pos)
{
	const char *help_msg;
	size_t help_msg_len;

	if (!buf)
		return -EINVAL;
	
	if (*pos != 0)
		return 0;
	
	help_msg ="\tHow to enable debug message:\n"
		"\t\t1. echo \"debug=0|1\" > "AGV_DEV_PATH"\n"
		"\t\twhile :; do dmesg -c; sleep 1; done\n"
		"\n";
	help_msg_len = strlen(help_msg) + 1;

	if ( help_msg_len > (*pos) )
	{
		size_t read_len = min_t(size_t, count, help_msg_len - (*pos));
		size_t len = copy_to_user(buf, &help_msg[*pos], read_len);
		*pos += len;
		return len;
	}

	return 0;
}

static ssize_t agv_chr_write(struct file * file, const char __user * buf, 
		size_t count, loff_t *pos)
{
	char cmdbuf[AGV_MAX_DEBUGCMD];
	const char* cmdp = NULL;
	size_t cmdlen = 0;
	
	if ( count >= AGV_MAX_DEBUGCMD )
		return count;
	
	cmdlen = copy_from_user(cmdbuf, buf, count);
	cmdbuf[cmdlen] = '\0';

	cmdp = strstr(cmdbuf, "debug=");
	if (cmdp) {
		int32_t new_debug;
		int32_t n = sscanf(cmdp, "debug=%d", &new_debug);
		if (1 == n ) {
			g_agv_debug = new_debug == 0 ? 0 : 1;
			AGV_PRINT("set g_agv_debug=%d\n", g_agv_debug);
		}		
	}
	
	return count;
}

static long agv_chr_ioctl(struct file *file, unsigned int cmd, 
		unsigned long arg)
{
	return 0;
}

static int agv_chr_close(struct inode *inode, struct file *file)
{
	AGV_PRINT("AGV chr dev closed.\n");
	return 0;
}

static struct file_operations s_agv_dev_fops =
{
	owner:
		THIS_MODULE,
	open:
		agv_chr_open,
	read:
		agv_chr_read,
	write:
		agv_chr_write,
	unlocked_ioctl:
		agv_chr_ioctl,
	release:
		agv_chr_close,
};

/*
 * @brief: agv_init_dev - 初始化字符设备
 * @return: 成功返回0，失败返回<0
 */
int32_t agv_init_dev(void)
{
	int32_t ret = -1;
	
	s_agv_dev_major = register_chrdev(0, AGV_DEV_PATH, &s_agv_dev_fops);
	if (s_agv_dev_major < 0) {
		AGV_PRINT("register_chrdev faile, s_agv_dev_major = %d\n", 
				s_agv_dev_major);
		return ret;
	}

	return ret;
}

/*
 * @brief: agv_cleanup_dev - 注销字符设备
 */
void agv_cleanup_dev(void)
{
	unregister_chrdev(s_agv_dev_major, AGV_DEV_PATH); 
}

