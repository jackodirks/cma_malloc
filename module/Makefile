PWD   := $(shell pwd)
obj-m := cma_malloc.o 
KDIR  := $(PWD)/../../linux-4.13.5

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean: 
	$(MAKE) -C $(KDIR) M=$(PWD) clean
