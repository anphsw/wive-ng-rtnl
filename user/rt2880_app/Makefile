ra_dir_$(CONFIG_RT2880_REG_RW)      += reg
ra_dir_$(CONFIG_RT2880_ATED)        += ated
ra_dir_$(CONFIG_RT2880_CSR_TEST)    += csr_test
ra_dir_$(CONFIG_RT2880_FLASH_TEST)  += flash
ra_dir_$(CONFIG_RT2880_MII_MGR)     += mii_mgr
ra_dir_$(CONFIG_RT2880_NVRAM)       += nvram
ra_dir_$(CONFIG_RT2880_RAINIT)      += ralink_init
ra_dir_$(CONFIG_RT2880_MTD)         += mtd
ra_dir_$(CONFIG_RT2880_GPIO_TEST)   += gpio
ra_dir_$(CONFIG_RT2880_SPI)         += spicmd
ra_dir_$(CONFIG_RT2880_I2C)         += i2ccmd
ra_dir_$(CONFIG_RT2880_SCRIPTS)     += scripts
ra_dir_$(CONFIG_RT2880_MEMORY_USAGE_TEST)		+= mem_usage
ra_dir_$(CONFIG_RT3052_SWITCH)      += switch
ra_dir_$(CONFIG_RALINKAPP_I2S)      += i2scmd
ra_dir_$(CONFIG_RALINKAPP_PCM)      += pcmcmd
ra_dir_$(CONFIG_RALINKAPP_SWQOS)    += sw_qos
ra_dir_$(CONFIG_RT2880_WANDUCK)     += wanduck
ra_dir_$(CONFIG_USB_TEST)     	    += usbtest
ra_dir_$(CONFIG_CRC_GEN)     	    += crc_gen
ra_dir_$(CONFIG_RT2880_NETWORK_MAP) += network_map
ra_dir_$(CONFIG_ASUS_NTFS3G)   	    += ntfs-3g
#ra_dir_y   	    		    += sig_test	# only for test
ra_dir_y   	    		    += xfr_test	# only for test


all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(ra_dir_y) ; do make -C $$i clean ; done

