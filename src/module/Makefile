TARGET := autogovpn_drv

ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS := -I$(PWD)/../inc
	OBJECTS := $(patsubst $(M)/%.c, %.o, $(wildcard $(M)/*.c))
	$(TARGET)-objs := $(OBJECTS)
	obj-m := $(TARGET).o
else
	KDIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
