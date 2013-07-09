#ifndef __DOMAIN_TABLE_H__
#define __DOMAIN_TABLE_H__

#include "agv_common.h"

/*
 * @brief: 域名配置下发函数，更新域名列表
 * @param: dlist [in] 由应用层下发的配置列表
 * @return: 成功返回0；失败返回<0
 */
int domain_table_load(const struct domain_list_st* dlist);

/*
 * @brief: 域名匹配函数，匹配域名是否命中
 * @param: domain [in] 域名字符串
 * @return: 命中返回1；不命中返回0；错误返回<0
 */
int domain_match(const char* domain);

/*
 * @brief: 当前域名列表输出
 * @return: 成功返回0；失败返回<0
 */
int domain_table_show(void);

/*
 * @brief: 域名列表初始化
 * @return: 成功返回0；失败返回-1
 */
int domain_table_init(void);

/*
 * @brief: 域名列表清理函数
 * @return: 无
 */
void domain_table_cleanup(void);

#endif
