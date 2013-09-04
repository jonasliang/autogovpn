#ifndef __AGV_IPRULE_TABLE_H__
#define __AGV_IPRULE_TABLE_H__

#include <linux/types.h>

/* iprule规则，主机字节序 */
struct agv_iprule {
	uint32_t m_srcip;
	uint32_t m_sipmask;
	uint32_t m_dstip;
	uint32_t m_dipmask;
};

/*
 * @brief: iprule_table_init - ip规则列表初始化
 * @return: 成功返回0；失败返回-1
 */
int iprule_table_init(void);

/*
 * @brief: iprule_table_cleanup - ip规则列表清理函数
 * @return: 无
 */
void iprule_table_cleanup(void);


#endif