#ifndef __AGV_COMMON_H__
#define __AGV_COMMON_H__
/*
 * common header for app and module, define the common structure here. 
 * author: jonas1q84@gmail.com
 */

#include <linux/ioctl.h>

typedef struct {
	int32_t m_size;				// size of m_domains
	uint32_t m_domains_crc[0];	// domain in crc32 format
} domain_list_st;

#define AGV_DEV_PATH "/dev/autogovpn"

#define AGV_IOCTL_MAGIC         0x09062013 // get the date wrote this code
#define AGV_IO(num)             _IO(AGV_IOCTL_MAGIC, num)
#define AGV_IOWR(num, dtype)    _IOWR(AGV_IOCTL_MAGIC, num, dtype)

#define AGV_IOCTL_ENABLE        AGV_IO(1)
#define AGV_IOCTL_DISABLE       AGV_IO(2)
#define AGV_IOCTL_LOAD_RULE     AGV_IOWR(3, domain_list_st)
#define AGV_IOCTL_CLEAR_RULE    AGV_IO(4)

#endif
