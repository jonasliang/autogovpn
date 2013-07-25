#include <linux/mm.h>
#include <linux/list.h>
#include <linux/spinlock_types.h>
#include <net/netfilter/nf_conntrack.h>
#include "agv_debug.h"
#include "agv_iprule_table.h"

struct agv_iprule_node {
	struct list_head m_irnode;
	struct agv_iprule m_iprule;
};

struct agv_iprule_table {
	uint32_t m_enable;
	size_t m_iprule_htsize;
	struct agv_iprule_node* m_iprule_htable;
	spinlock_t* m_iprule_locks;
};

struct agv_iprule_table s_iprule_table;

/*
 * @brief: iprule_table_init - ip规则列表初始化
 * @return: 成功返回0；失败返回-1
 */
int iprule_table_init(void)
{
	int ret = -1;
	int i = 0;
	struct agv_iprule_node* ipruleht = NULL;
	spinlock_t* iprulelks = NULL;
	size_t htsize = nf_conntrack_htable_size * sizeof(struct agv_iprule_node);
	size_t lcsize = nf_conntrack_htable_size * sizeof(spinlock_t);

	s_iprule_table.m_iprule_htable = NULL;
	s_iprule_table.m_iprule_locks = NULL;
	
	/* 取nf_conn相同槽数 */
	ipruleht = vmalloc(htsize);
	AGV_CHECK_AND_GO(!ipruleht, vfailed, "vmalloc failed. size = %u\n", htsize);
	memset(ipruleht, 0, htsize);

	iprulelks = vmalloc(lcsize);
	AGV_CHECK_AND_GO(!iprulelks, vfailed, "vmalloc failed. size = %u\n", lcsize);

	for (i = 0; i < nf_conntrack_htable_size; ++i) {
		INIT_LIST_HEAD(&ipruleht[i].m_irnode);
		iprulelks[i] = __SPIN_LOCK_UNLOCKED(iprulelks[i]);
	}

	s_iprule_table.m_iprule_htsize = nf_conntrack_htable_size;
	s_iprule_table.m_iprule_htable = ipruleht;
	s_iprule_table.m_iprule_locks = iprulelks;
	s_iprule_table.m_enable = 1;

	ret = 0;

_done:
	return ret;

_vfailed:
	if (s_iprule_table.m_iprule_locks) {
		vfree(s_iprule_table.m_iprule_locks);
		POISON_POINTER(s_iprule_table.m_iprule_locks); 
	}

	if (s_iprule_table.m_iprule_htable) {
		vfree(s_iprule_table.m_iprule_htable);
		POISON_POINTER(s_iprule_table.m_iprule_htable);	
	}

	ret = -1;
	goto _done;
}

/*
 * @brief: iprule_table_cleanup - ip规则列表清理函数
 * @return: 无
 */
void iprule_table_cleanup(void)
{
	int i = 0;
	s_iprule_table.m_enable = 0;

	if (!s_iprule_table.m_iprule_locks || !s_iprule_table.m_iprule_htable) {
		AGV_PRINT("cleanup failed!\n");
		return;
	}

	synchronize_rcu();

	for (i = 0; i < s_iprule_table.m_iprule_htsize; ++i) {
		struct agv_iprule_node* pos = NULL;
		struct agv_iprule_node* next = NULL;
		struct list_head* head = NULL;

		spin_lock_bh(&s_iprule_table.m_iprule_locks[i]);
		head = &s_iprule_table.m_iprule_htable[i].m_irnode;
		list_for_each_entry_safe(pos, next, head, m_irnode) {
			list_del(&pos->m_irnode);
			kfree(pos);
			POISON_POINTER(pos);
		}
		spin_unlock_bh(&s_iprule_table.m_iprule_locks[i]);
	}

	vfree(s_iprule_table.m_iprule_locks);
	POISON_POINTER(s_iprule_table.m_iprule_locks); 

	vfree(s_iprule_table.m_iprule_htable);
	POISON_POINTER(s_iprule_table.m_iprule_htable);	

	return;
}