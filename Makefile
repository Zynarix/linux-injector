obj-m += main.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CLANG_FLAGS := LLVM=1 CC=clang

all:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CLANG_FLAGS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
