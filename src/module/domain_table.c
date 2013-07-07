#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include "domain_table.h"
#include "agv_debug.h"

/* Could be modified while loading module with param */
static size_t s_domain_tsize = 1024;
//module_param(s_domain_tsize, size_t, 0644);
//MODULE_PARM_DESC(s_domain_tsize, "modify the size of domain table buckets");

struct domain_node {
	struct list_head list;
	uint32_t domain_crc;
};

struct domain_node* s_domain_table = NULL;

int domain_table_init(void)
{
	int ret = -1;
	if (s_domain_table) {
		AGV_PRINT("init error! s_domain_table should be NULL.\n");
		goto done;
	}

	if (s_domain_tsize < 1024) 
		s_domain_tsize = 1024;
	else if (s_domain_tsize > 65535)
		s_domain_tsize = 65535;

	s_domain_table = vmalloc(sizeof(struct domain_node) * s_domain_tsize);
	if (!s_domain_table) {
		AGV_PRINT("vmalloc failed! total size = %u\n", 
			sizeof(struct domain_node) * s_domain_tsize);
		goto done;
	}

	AGV_PRINT("succeeded! s_domain_tsize: %u\n", s_domain_tsize);
	ret = 0;

done:
	return ret;
}

void domain_table_cleanup(void)
{
	struct domain_node* dtp = rcu_dereference(s_domain_table);
	rcu_assign_pointer(s_domain_table, NULL);
	synchronize_rcu();
	vfree(dtp);
	AGV_PRINT("cleanup domain table.\n");
	return;
}
