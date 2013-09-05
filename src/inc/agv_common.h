#ifndef __AGV_COMMON_H__
#define __AGV_COMMON_H__
/*
 * common header for app and module, define the common structure here. 
 * author: jonas1q84@gmail.com
 */

typedef struct {
	int32_t m_size;				// size of m_domains
	uint32_t m_domains_crc[0];	// domain in crc32 format
} domain_list_st;

#endif
