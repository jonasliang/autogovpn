#include <linux/types.h>

/*
 * @brief: agv_init_dev - 初始化字符设备
 * @return: 成功返回0，失败返回<0
 */
int32_t agv_init_dev(void);

/*
 * @brief: agv_cleanup_dev - 注销字符设备
 */
void agv_cleanup_dev(void);

