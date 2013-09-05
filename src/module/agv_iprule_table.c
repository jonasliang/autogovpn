#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/list.h>
#include <linux/spinlock_types.h>
#include <net/netfilter/nf_conntrack.h>
#include <asm/cmpxchg.h>
#include <asm/atomic.h>
#include "agv_debug.h"
#include "agv_iprule_table.h"

typedef struct {
	struct list_head m_irnode;
	agv_iprule m_iprule;
} agv_iprule_node;

typedef struct {
	uint32_t m_enable;                  // indicate enable or not
	agv_iprule_node* m_iprule_htable;
	spinlock_t* m_iprule_locks;
} agv_iprule_table;


static agv_iprule_table s_iprule_table;
static atomic_t s_iprule_cnt;   // current iprule count

static size_t s_iprule_htsize __read_mostly;        // bucket size
static size_t s_iprule_max __read_mostly;           // max iprules
static uint32_t s_iprule_hash_rnd __read_mostly;    // random num for hash
static uint32_t s_iprule_drop_max __read_mostly = 8;// max rule every drop call

static inline void iprule_hash_rnd_init(void)
{
	uint32_t rand;
	do {
		get_random_bytes(&rand, sizeof(rand));
	} while(!rand);
	cmpxchg(&s_iprule_hash_rnd, 0, rand);
}

static inline uint32_t hashiprule(const agv_iprule* iprule)
{
	return jhash_2words(iprule->m_srcip, iprule->m_dstip, s_iprule_hash_rnd);
}

/*
 * @brief: iprule_table_drop - 删除部分规则
 * @param: bucket [in] - 准备在该桶里面丢弃
 * @return: 返回drop数目
 */
static uint32_t iprule_table_drop(const size_t bucket)
{
    uint32_t dropped = 0;
    struct list_head* head = NULL;
    agv_iprule_node* pos = NULL;
    agv_iprule_node* next = NULL;

    if (!s_iprule_table.m_enable || bucket >= s_iprule_htsize)
        return dropped;
    
    spin_lock_bh(&s_iprule_table.m_iprule_locks[bucket]);
    
    head = &s_iprule_table.m_iprule_htable[bucket].m_irnode;
    list_for_each_entry_safe(pos, next, head, m_irnode) {
        list_del(&pos->m_irnode);
        kfree(pos);
        POISON_POINTER(pos);
        dropped++;
        atomic_dec(&s_iprule_cnt);
        if (dropped == s_iprule_drop_max)
            break;
    }

    spin_unlock_bh(&s_iprule_table.m_iprule_locks[bucket]);
    return dropped;
}

/*
 * @brief: iprule_table_insert - 插入一条iprule规则
 * @param: iprule [in] - iprule规则
 * @return: 成功返回0；失败返回-1
 */
int32_t iprule_table_insert(const agv_iprule* iprule)
{
	int32_t ret = -1;
	int32_t index = 0;
	int32_t exist = 0;
	agv_iprule_node* pos = NULL;
	struct list_head* head = NULL;

	AGV_CHECK_AND_GO(!iprule, done, "invalid param.\n");

	if (!s_iprule_table.m_enable)
		goto _done;

	index = hashiprule(iprule) % s_iprule_htsize;

    if ( unlikely(atomic_read(&s_iprule_cnt) > s_iprule_max) ) {
        if (!iprule_table_drop(index)) {
            /* TODO: print out the rule */
            if(net_ratelimit())
                printk(KERN_WARNING"agv_iprule: table full, untracking"
                        " rule.\n");
            goto _done;
        }
    }

	spin_lock_bh(&s_iprule_table.m_iprule_locks[index]);
	
	// find the matched rule first, if exist, go out.
	head = &s_iprule_table.m_iprule_htable[index].m_irnode;
	list_for_each_entry(pos, head, m_irnode) {
		if ( iprule_cmp(&pos->m_iprule, iprule) ) {
			exist = 1;
			break;
		}
	}

	if (!exist) {
		agv_iprule_node* newnode = kmalloc(sizeof(agv_iprule_node), 
				GFP_ATOMIC);
		AGV_CHECK_AND_GO(!newnode, unlock, "kmalloc agv_iprule_node failed.\n");
		
		INIT_LIST_HEAD(&newnode->m_irnode);
		iprule_assign(&newnode->m_iprule, iprule);
		list_add(head, &newnode->m_irnode);
        atomic_inc(&s_iprule_cnt);
	}

_unlock:
	spin_unlock_bh(&s_iprule_table.m_iprule_locks[index]);

_done:
	return ret;
}

/*
 * @brief: iprule_table_match - 匹配一条iprule规则
 * @param: iprule [in] - iprule规则
 * @return: 命中返回1；不命中返回0；错误返回<0
 */
int32_t iprule_table_match(const agv_iprule* iprule)
{
	int32_t found = -1;
	int32_t index = 0;
	agv_iprule_node* pos = NULL;
	struct list_head* head = NULL;

	AGV_CHECK_AND_GO(!iprule, done, "invalid param.\n");
	if (!s_iprule_table.m_enable)
		goto _done;

	index = hashiprule(iprule) % s_iprule_htsize;
	found = 0;
	spin_lock_bh(&s_iprule_table.m_iprule_locks[index]);
	
	// find the matched rule first, if exist, go out.
	head = &s_iprule_table.m_iprule_htable[index].m_irnode;
	list_for_each_entry(pos, head, m_irnode) {
		if ( iprule_cmp(&pos->m_iprule, iprule) ) {
			found = 1;
			break;
		}
	}
	spin_unlock_bh(&s_iprule_table.m_iprule_locks[index]);

_done:
	return found;
}

/*
 * @brief: iprule_table_init - ip规则列表初始化
 * @return: 成功返回0；失败返回-1
 */
int32_t iprule_table_init(void)
{
	int32_t ret = -1;
	int32_t i = 0;
	agv_iprule_node* ipruleht = NULL;
	spinlock_t* iprulelks = NULL;
	size_t htsize = nf_conntrack_htable_size * sizeof(agv_iprule_node);
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

	s_iprule_htsize = nf_conntrack_htable_size;
	s_iprule_table.m_iprule_htable = ipruleht;
	s_iprule_table.m_iprule_locks = iprulelks;
    s_iprule_max = nf_conntrack_max;
    atomic_set(&s_iprule_cnt, 0);
	s_iprule_table.m_enable = 1;
	
	iprule_hash_rnd_init();

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
	int32_t i = 0;
	s_iprule_table.m_enable = 0;

	if (!s_iprule_table.m_iprule_locks || !s_iprule_table.m_iprule_htable) {
		AGV_PRINT("cleanup failed!\n");
		return;
	}

	synchronize_rcu();

	for (i = 0; i < s_iprule_htsize; ++i) {
		agv_iprule_node* pos = NULL;
		agv_iprule_node* next = NULL;
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

