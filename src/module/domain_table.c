#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/mm.h>
#include "domain_table.h"
#include "agv_debug.h"

#define MIN_DOMAIN_BUCKETS 1024
#define MAX_DOMAIN_BUCKETS 65535

/* Could be modified while loading module with param */
static size_t s_domain_htable_size = MIN_DOMAIN_BUCKETS;
module_param(s_domain_htable_size, uint, 0644);

struct domain_node {
	struct list_head m_dnode;
	uint32_t m_domain_crc;
};

struct domain_htable {
	size_t m_htable_size;
	struct domain_node* m_htable;
};

static struct domain_htable* s_domain_htable = NULL;

/*
 * @brief: alloc_domain_htable - 初始化域名哈希表及内部结构
 * @param: htable_size [in] 哈希表桶数
 * @return: 成功返回哈希表指针；失败返回NULL
 */
static struct domain_htable* alloc_domain_htable(size_t htable_size)
{
	struct domain_htable* newdht = NULL;
	struct domain_node* newtable = NULL;
	size_t dt_size = sizeof(struct domain_node) * htable_size;
	int i = 0;

	newdht = kmalloc(sizeof(struct domain_htable), GFP_KERNEL);
	AGV_CHECK_AND_GO(!newdht, done, "kmalloc newdht failed!\n");

	newtable = vmalloc(dt_size);
	AGV_CHECK_AND_GO(!newtable, vfailed, \
		"vmalloc failed! total size = %u\n", dt_size);

	memset(newtable, 0, dt_size);

	for (i = 0; i < htable_size; ++i) {
		INIT_LIST_HEAD(&newtable[i].m_dnode);
	}
	newdht->m_htable_size = htable_size;
	newdht->m_htable = newtable;

_done:
	return newdht;

_vfailed:
	kfree(newdht);
	newdht = NULL;
	goto _done;
}

static void free_domain_htable(struct domain_htable** pdhtable)
{
	int i = 0;
	struct domain_htable* dhtable = NULL;

	if (!pdhtable || !*pdhtable) 
		return;

	dhtable = *pdhtable;

	for (i = 0; i < dhtable->m_htable_size; ++i) {
		struct list_head* head = &dhtable->m_htable[i].m_dnode;
		struct domain_node* pos = NULL;
		struct domain_node* next = NULL;

		list_for_each_entry_safe(pos, next, head, m_dnode) {
			list_del(&pos->m_dnode);
			kfree(pos);
		}
	}

	kfree(dhtable);
	*pdhtable = NULL;
}

/*
 * @brief: 域名配置下发函数，更新域名列表
 * @param: dlist [in] 由应用层下发的配置列表
 * @return: 成功返回0；失败返回<0
 */
int domain_table_load(const struct domain_list_st* dlist)
{
	int ret = -1;
	int i = 0;
	struct domain_htable* newdht = NULL;
	AGV_CHECK_AND_GO((!dlist || 0 == dlist->m_size || !dlist->m_domains_crc), \
		done, "error! dlist is NULL or dlist size is 0!\n");

	newdht = alloc_domain_htable(s_domain_htable_size);
	AGV_CHECK_AND_GO(!newdht, done, "alloc new domain_bucket failed!\n");

	for (i = 0; i < dlist->m_size; ++i)	{
		size_t index = dlist->m_domains_crc[i] % s_domain_htable_size; // could be optimized
		struct domain_node* newnode = kmalloc(sizeof(struct domain_node), 
			GFP_KERNEL);
		AGV_CHECK_AND_GO(!newnode, allocfailed, "kmalloc failed!");

		newnode->m_domain_crc = dlist->m_domains_crc[i];
		INIT_LIST_HEAD(&newnode->m_dnode);
		list_add(&newnode->m_dnode, &newdht->m_htable[index].m_dnode);
	}

	ret = 0;
_done:
	return ret;

_allocfailed:
	free_domain_htable(&newdht);
	ret = -1;
	goto _done;
}

/*
 * @brief: 域名列表初始化
 * @return: 成功返回0；失败返回-1
 */
int domain_table_init(void)
{
	int ret = -1;
	AGV_CHECK_AND_GO(!s_domain_htable, \
		done, "init error! s_domain_htable should be NULL.\n");

	if (s_domain_htable_size < MIN_DOMAIN_BUCKETS) 
		s_domain_htable_size = MIN_DOMAIN_BUCKETS;
	else if (s_domain_htable_size > MAX_DOMAIN_BUCKETS)
		s_domain_htable_size = MAX_DOMAIN_BUCKETS;

	ret = 0;

_done:
	return ret;
}

/*
 * @brief: 域名列表清理函数
 * @return: 无
 */
void domain_table_cleanup(void)
{
	struct domain_htable* dt = rcu_dereference(s_domain_htable);
	rcu_assign_pointer(s_domain_htable, NULL);
	synchronize_rcu();
	free_domain_htable(&dt);
}
