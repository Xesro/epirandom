ifeq ($(KERNELRELEASE),)

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

obj-m := epirandom_level_1.o epirandom_level_2.o epirandom_level_3.o epirandom_level_4.o

endif
