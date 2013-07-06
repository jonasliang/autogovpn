#ifndef __DOMAIN_TABLE_H__
#define __DOMAIN_TABLE_H__

#include "agv_common.h"

int domain_list_init(struct domain_list_st* dlist);

int domain_list_cleanup();

int domain_list_show();

int domain_match(const char* domain);

#endif
