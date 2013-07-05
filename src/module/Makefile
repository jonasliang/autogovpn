TARGET=autogovpn_drv

KDIR:=/lib/modules/$(shell uname -r)/build

MODULE_C_SOURCES 	= $(wildcard $(M)/*.c)
MODULE_C_OBJS		= $(patsubst $(M)/%.c, %.o, $(MODULE_C_SOURCES))

$(TARGET)-objs		:= $(MODULE_C_OBJS)
obj-m				:= $(TARGET).o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
