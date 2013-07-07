#ifndef __AGV_DEBUG_H__
#define __AGV_DEBUG_H__

#include <linux/kernel.h>

/* Return the last part of a pathname */
static inline const char *basename(const char *path)
{
	const char *tail = strrchr(path, '/');
	return tail ? tail+1 : path;
}

#define AGV_PRINT(fmt...) do \
{ \
	printk("%s:%d:%s: ", basename(__FILE__), __LINE__, __FUNCTION__); \
	printk(fmt); \
} while (0)

#endif
