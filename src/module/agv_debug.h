#ifndef __AGV_DEBUG_H__
#define __AGV_DEBUG_H__

#include <linux/kernel.h>

/* In GCC4, the __FILE__ is absolute path, so we need this get the basename 
 * of source file */
static inline const char *agv_basename(const char *path)
{
	const char *tail = strrchr(path, '/');
	return tail ? tail + 1 : path;
}

/* 自定义打印宏 */
#define AGV_PRINT(fmt...) do {												\
	printk("%s:%d:%s: ", agv_basename(__FILE__), __LINE__, __FUNCTION__);	\
	printk(fmt); 														 	\
} while (0)

/* 用于检查错误条件并跳转到函数错误处理路径，函数内需有跳转_label标签 */
#define AGV_CHECK_AND_GO(cond, label, fmt...) do {						\
	if (cond) {																\
		AGV_PRINT(fmt);														\
		goto _##label;														\
	}																		\
} while(0)

/* TODO: 等级日志打印接口 */

#endif