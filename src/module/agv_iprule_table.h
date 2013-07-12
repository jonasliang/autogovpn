#ifndef __AGV_IPRULE_TABLE_H__
#define __AGV_IPRULE_TABLE_H__

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