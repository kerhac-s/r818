KERN_DIR= /media/roger/f_land/tina/lichee/linux-4.9
CROSS_COMPILE=aarch64-openwrt-linux-
#KERNEL_DIR = /lib/modules/$(KERN_VER)/build
ARCH=arm64
CURRENT_DIR= $(shell pwd)
obj-m +=first_drv.o
all:
	make -C $(KERN_DIR) M=$(CURRENT_DIR) modules

.PHONY: clean
clean:
	make -C $(KERN_DIR) M=$(CURRENT_DIR) modules clean
	rm -rf modules.order
#obj-m  +=first_drv.o

