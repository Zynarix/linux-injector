obj-m += main.o

# Путь к директории с исходниками ядра
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Параметры для использования Clang
# LLVM=1 включает использование всей цепочки инструментов LLVM (clang, ld.lld, llvm-objcopy и т.д.)
CLANG_FLAGS := LLVM=1 CC=clang

all:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CLANG_FLAGS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean