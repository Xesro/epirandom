ifeq ($(KERNELRELEASE),)
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

clean:
	$(RM) *.o *~ core .depend .*.cmd *.ko *.mod.c *.tmp_versions *.mod *.order *.symvers

.PHONY: modules modules_install clean

else

obj-m := epirandom.o
endif
