#ifndef __AGV_COMMON_H__
#define __AGV_COMMON_H__
/*
 * common header for app and module, define the common structure here. 
 * author: jonas1q84@gmail.com
 */

struct domain_list_st {
	int m_size;					// size of m_domains
	unsigned int m_domains[0];	// domain in crc32 format
};

#endif
