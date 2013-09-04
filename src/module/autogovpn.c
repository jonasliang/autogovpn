#include <linux/kernel.h>
#include <linux/module.h>
#include "agv_domain_table.h"
#include "agv_iprule_table.h"
#include "agv_debug.h"

static int agv_init(void)
{
	int ret = -1;
	ret = domain_table_init();
	AGV_CHECK_AND_GO(ret < 0, errout, "domain_table_init failed!\n");

	ret = iprule_table_init();
	AGV_CHECK_AND_GO(ret < 0, errout, "iprule_table_init failed!\n");
	
	AGV_PRINT("autogovpn drv succeeded to initialize! Built at: %s %s\n", 
		__DATE__, __TIME__);

_errout:
    return ret;
}

static void agv_exit(void)
{
	domain_table_cleanup();
	iprule_table_cleanup();
	AGV_PRINT("autogovpn drv exit.\n");
    return;
}

module_init(agv_init);
module_exit(agv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Liang (email: jonas1q84@gmail.com)");
MODULE_DESCRIPTION("Packet mark driver for autogovpn");
