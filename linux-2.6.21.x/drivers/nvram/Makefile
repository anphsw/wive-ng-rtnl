#O_TARGET := nvram_linux.o

# only support kernel module
#ifdef CONFIG_ASUS_EXT
ifeq ($(CONFIG_ASUS_EXT),y)
    obj-m	+= nvram_linux.o
endif
#obj-y	+= nvram_linux.o

#export-objs += nvram_linux.o

#include $(TOPDIR)/Rules.make
