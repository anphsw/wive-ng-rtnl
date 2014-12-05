.EXPORT_ALL_VARIABLES:

SUBDIRS_APP = mii_mgr nvram reg switch gpio mtd hw_nat switch_rtl eth_mac

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

#allways agressive optimize for size
CFLAGS	+= -Os -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables -femit-struct-debug-baseonly -fno-var-tracking
CFLAGS	+= -ffunction-sections -fdata-sections
CFLAGS	+= -I$(ROOTDIR)/$(LINUXDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
LDFLAGS	+= -Wl,--gc-sections

#ralink specific applications
ra_dir_$(CONFIG_RT2880_REG_RW)      += reg
ra_dir_$(CONFIG_RALINKAPP_HWNAT)    += hw_nat
ra_dir_$(CONFIG_RT2880_MII_MGR)     += mii_mgr
ra_dir_$(CONFIG_RT2880_NVRAM)       += nvram
ra_dir_$(CONFIG_RT2880_RAINIT)      += nvram
ra_dir_$(CONFIG_RT2880_MTD)         += mtd
ra_dir_$(CONFIG_RT2880_MTD_VERF)    += mtd_verifier
ra_dir_$(CONFIG_RT2880_GPIO_TEST)   += gpio
ra_dir_$(CONFIG_RT3052_SWITCH)      += switch
ra_dir_$(CONFIG_RTL8367M_SWITCH)    += switch_rtl
ra_dir_$(CONFIG_ETHMAC_TOOL)        += eth_mac

#always install radio firmware
ra_dir_y			    += rt_firmware

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(SUBDIRS_APP) ; do make -C $$i clean ; done
