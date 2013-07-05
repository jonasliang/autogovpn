#include <linux/kernel.h>
#include <linux/module.h>

static int agv_init()
{
    printk("hello!");
    return 0;
}

static void agv_exit()
{
    return;
}

module_init(agv_init);
module_exit(agv_exit);
MODULE_LICENSE("GPL\0 Author: jonas1q84@gmail.com");
MODULE_DESCRIPTION("Packet mark driver for autogovpn");

