ARCH=arm
obj-m += autogovpn.o
#KDIR:=/lib/modules/$(shell uname -r)/build
KDIR:=/home/lzj/github/rpi-linux


PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
