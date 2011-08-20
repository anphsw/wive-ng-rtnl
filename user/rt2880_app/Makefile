
SUBDIRS_APP = ated flash i2ccmd mii_mgr nvram reg switch csr_test gpio i2scmd mem_usage mtd pcmcmd spicmd mtd_verifier hw_nat

#ralink specific applications
ra_dir_$(CONFIG_RT2880_REG_RW)      += reg
ra_dir_$(CONFIG_RT2880_ATED)        += ated
ra_dir_$(CONFIG_RT2880_CSR_TEST)    += csr_test
ra_dir_$(CONFIG_RT2880_FLASH_TEST)  += flash
ra_dir_$(CONFIG_RALINKAPP_HWNAT)    += hw_nat
ra_dir_$(CONFIG_RT2880_MII_MGR)     += mii_mgr
ra_dir_$(CONFIG_RT2880_NVRAM)       += nvram
ra_dir_$(CONFIG_RT2880_RAINIT)      += nvram
ra_dir_$(CONFIG_RT2880_RADAEMON)    += nvram
ra_dir_$(CONFIG_RT2880_MTD)         += mtd
ra_dir_$(CONFIG_RT2880_MTD)	    += mtd_verifier
ra_dir_$(CONFIG_RT2880_GPIO_TEST)   += gpio
ra_dir_$(CONFIG_RT2880_SPI)         += spicmd
ra_dir_$(CONFIG_RT2880_I2C)         += i2ccmd
ra_dir_$(CONFIG_RT3052_SWITCH)      += switch
ra_dir_$(CONFIG_RALINKAPP_I2S)      += i2scmd
ra_dir_$(CONFIG_RALINKAPP_PCM)      += pcmcmd
ra_dir_$(CONFIG_RALINKAPP_SWQOS)    += sw_qos
ra_dir_$(CONFIG_RALINKAPP_CLS)	    += clscmd

#always install radio firmware
ra_dir_y			    += rt_firmware

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(SUBDIRS_APP) ; do make -C $$i clean ; done
