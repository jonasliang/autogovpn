#include <linux/kernel.h>
#include <linux/module.h>
#include "domain_table.h"
#include "agv_debug.h"

static int agv_init(void)
{
	int ret = -1;
	ret = domain_table_init();
	AGV_PRINT("autogovpn succeeded to initialize! Built at: %s %s\n", 
		__DATE__, __TIME__);
    return ret;
}

static void agv_exit(void)
{
	domain_table_cleanup();
    return;
}

module_init(agv_init);
module_exit(agv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Liang (email: jonas1q84@gmail.com)");
MODULE_DESCRIPTION("Packet mark driver for autogovpn");
