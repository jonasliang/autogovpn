#ifndef __AGV_DOMAIN_TABLE_H__
#define __AGV_DOMAIN_TABLE_H__

#include "agv_common.h"

/*
 * @brief: domain_table_load - 域名配置下发函数，更新域名列表
 * @param: dlist [in] 由应用层下发的配置列表
 * @return: 成功返回0；失败返回<0
 */
int32_t domain_table_load(const domain_list_st* dlist);

/*
 * @brief: domain_match - 域名匹配函数，匹配域名是否命中
 * @param: domain [in] 域名字符串指针
 * @param: len [in] 字符串长度，不含\0结束符
 * @return: 命中返回1；不命中返回0；错误返回<0
 */
int32_t domain_match(const char* domain, size_t len);

/*
 * @brief: domain_table_show - 当前域名列表输出
 * @return: 成功返回0；失败返回<0
 */
int32_t domain_table_show(void);

/*
 * @brief: domain_table_init - 域名列表初始化
 * @return: 成功返回0；失败返回-1
 */
int32_t domain_table_init(void);

/*
 * @brief: domain_table_cleanup - 域名列表清理函数
 * @return: 无
 */
void domain_table_cleanup(void);

#endif
