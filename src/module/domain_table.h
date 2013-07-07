#ifndef __DOMAIN_TABLE_H__
#define __DOMAIN_TABLE_H__

#include "agv_common.h"

/*
 * @brief: 域名列表初始化
 * @return: 
 */
int domain_table_init(void);

void domain_table_cleanup(void);

int domain_table_load(struct domain_list_st* dlist);

int domain_table_show(void);

int domain_match(const char* domain);

#endif
