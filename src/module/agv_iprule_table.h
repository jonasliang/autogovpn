#ifndef __AGV_IPRULE_TABLE_H__
#define __AGV_IPRULE_TABLE_H__

#include <linux/types.h>

/* iprule规则，主机字节序 */
typedef struct {
	uint32_t m_srcip;
	uint32_t m_dstip;
} agv_iprule;

/*
 * @brief: iprule_cmp - for iprule struct compare
 */
static inline bool iprule_cmp(const agv_iprule* a, const agv_iprule* b)
{
	return (a->m_srcip == b->m_srcip && 
			a->m_dstip == b->m_dstip);
}

/*
 * @brief: iprule_assign - for iprule struct assignment
 */
static inline void iprule_assign(agv_iprule* to, const agv_iprule* from)
{
	to->m_srcip = from->m_srcip;
	to->m_dstip = from->m_dstip;
}

/*
 * @brief: iprule_table_insert - 插入一条iprule规则
 * @param: iprule [in] - iprule规则
 * @return: 成功返回0；失败返回-1
 */
int32_t iprule_table_insert(const agv_iprule* iprule);

/*
 * @brief: iprule_table_match - 匹配一条iprule规则
 * @param: iprule [in] - iprule规则
 * @return: 命中返回1；不命中返回0；错误返回<0
 */
int32_t iprule_table_match(const agv_iprule* iprule);

/*
 * @brief: iprule_table_init - ip规则列表初始化
 * @return: 成功返回0；失败返回-1
 */
int32_t iprule_table_init(void);

/*
 * @brief: iprule_table_cleanup - ip规则列表清理函数
 * @return: 无
 */
void iprule_table_cleanup(void);


#endif
