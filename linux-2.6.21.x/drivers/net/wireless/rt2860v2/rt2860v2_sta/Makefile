EXTRA_CFLAGS = -Idrivers/net/wireless/rt2860v2/include

obj-$(CONFIG_RT2860V2_STA) += rt2860v2_sta.o

rt2860v2_sta-objs += ../common/crypt_md5.o
rt2860v2_sta-objs += ../common/crypt_sha2.o
rt2860v2_sta-objs += ../common/crypt_biginteger.o
rt2860v2_sta-objs += ../common/crypt_hmac.o
rt2860v2_sta-objs += ../common/mlme.o
rt2860v2_sta-objs += ../common/cmm_wep.o
rt2860v2_sta-objs += ../common/action.o
rt2860v2_sta-objs += ../common/ba_action.o
rt2860v2_sta-objs += ../common/cmm_data.o
rt2860v2_sta-objs += ../common/rtmp_init.o
rt2860v2_sta-objs += ../common/cmm_tkip.o
rt2860v2_sta-objs += ../common/cmm_aes.o
rt2860v2_sta-objs += ../common/cmm_sync.o
rt2860v2_sta-objs += ../common/cmm_sanity.o
rt2860v2_sta-objs += ../common/cmm_info.o
rt2860v2_sta-objs += ../common/cmm_wpa.o
rt2860v2_sta-objs += ../common/dfs.o
rt2860v2_sta-objs += ../common/spectrum.o
rt2860v2_sta-objs += ../common/rtmp_timer.o
rt2860v2_sta-objs += ../common/rt_channel.o
rt2860v2_sta-objs += ../common/cmm_profile.o
rt2860v2_sta-objs += ../common/cmm_asic.o
rt2860v2_sta-objs += ../common/rtmp_swmcu.o
rt2860v2_sta-objs += ../common/cmm_cfg.o
rt2860v2_sta-objs += ../common/eeprom.o
rt2860v2_sta-objs += ../common/ee_flash.o
rt2860v2_sta-objs += ../common/cmm_mac_pci.o
rt2860v2_sta-objs += ../common/cmm_data_pci.o
rt2860v2_sta-objs += ../common/crypt_aes.o
rt2860v2_sta-objs += ../common/crypt_arc4.o
rt2860v2_sta-objs += ../common/cmm_cmd.o
rt2860v2_sta-objs += ../sta/assoc.o
rt2860v2_sta-objs += ../sta/auth.o
rt2860v2_sta-objs += ../sta/auth_rsp.o
rt2860v2_sta-objs += ../sta/sync.o
rt2860v2_sta-objs += ../sta/sanity.o
rt2860v2_sta-objs += ../sta/rtmp_data.o
rt2860v2_sta-objs += ../sta/connect.o
rt2860v2_sta-objs +=  ../sta/wpa.o
rt2860v2_sta-objs +=  ../os/linux/rt_proc.o
rt2860v2_sta-objs +=  ../os/linux/rt_linux.o
rt2860v2_sta-objs +=  ../os/linux/rt_profile.o
rt2860v2_sta-objs +=  ../os/linux/rt_main_dev.o
rt2860v2_sta-objs +=  ../os/linux/sta_ioctl.o
rt2860v2_sta-objs +=  ../os/linux/rt_ate.o
rt2860v2_sta-objs +=  ../os/linux/rbus_main_dev.o
rt2860v2_sta-objs +=  ../os/linux/rt_pci_rbus.o

ifeq ($(CONFIG_RALINK_RT2880),y)
rt2860v2_sta-objs +=  ../chips/rt2880.o
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
rt2860v2_sta-objs +=  ../chips/rt2883.o
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
rt2860v2_sta-objs +=  ../common/mfb_tlv.o
endif
endif

ifeq ($(CONFIG_RALINK_RT3883),y)
rt2860v2_sta-objs +=  ../common/rt_rf.o
rt2860v2_sta-objs +=  ../chips/rt3883.o
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
rt2860v2_sta-objs +=  ../common/mfb_tlv.o
endif
endif

ifeq ($(CONFIG_RALINK_RT3052),y)
rt2860v2_sta-objs +=  ../common/rt_rf.o
rt2860v2_sta-objs +=  ../chips/rt305x.o
endif

ifeq ($(CONFIG_RALINK_RT3352),y)
rt2860v2_sta-objs +=  ../common/rt_rf.o
rt2860v2_sta-objs +=  ../chips/rt305x.o
endif

ifeq ($(CONFIG_RT2860V2_STA_LED),y)
rt2860v2_sta-objs += ../common/led.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WMM_ACM),y)
rt2860v2_sta-objs += ../common/acm_edca.o
rt2860v2_sta-objs += ../common/acm_comm.o
rt2860v2_sta-objs += ../common/acm_iocl.o
endif

ifeq ($(CONFIG_RT2860V2_RT3XXX_STA_ANTENNA_DIVERSITY),y)
rt2860v2_sta-objs += ../os/linux/ap_diversity.o
endif

ifeq ($(CONFIG_RT2860V2_STA_DLS),y)
rt2860v2_sta-objs += ../sta/dls.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WSC),y)
rt2860v2_sta-objs += ../common/wsc.o
rt2860v2_sta-objs += ../common/wsc_tlv.o
rt2860v2_sta-objs += ../common/crypt_dh.o
rt2860v2_ap-objs += ../common/wsc_ufd.o
endif

###################
#  CFLAGS
##################
EXTRA_CFLAGS += -DLINUX
EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT
EXTRA_CFLAGS += -DCONFIG_STA_SUPPORT -DRTMP_RBUS_SUPPORT 
EXTRA_CFLAGS += -DRTMP_MAC_PCI -DDOT11_N_SUPPORT -DRELASE_EXCLUDE
EXTRA_CFLAGS += -DMERGE_ARCH_TEAM -DSTATS_COUNT_SUPPORT

ifeq ($(CONFIG_RALINK_RT2880_STA_DBG),y)
EXTRA_CFLAGS += -DDBG -DRALINK_ATE  -DCONFIG_RT2880_ATE_CMD_NEW  -DRALINK_28xx_QA
endif

ifeq ($(CONFIG_RALINK_RT2880),y)
EXTRA_CFLAGS += -DRT2880
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
EXTRA_CFLAGS += -DRT2883
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
EXTRA_CFLAGS += -DTXBF_SUPPORT
endif
endif

ifeq ($(CONFIG_RALINK_RT3883),y)
EXTRA_CFLAGS += -DRT3883 -DRTMP_RF_RW_SUPPORT
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
EXTRA_CFLAGS += -DTXBF_SUPPORT
endif
endif

ifeq ($(CONFIG_RALINK_RT3052),y)
EXTRA_CFLAGS += -DRT3052 -DRT305x -DRTMP_RF_RW_SUPPORT
endif

ifeq ($(CONFIG_RALINK_RT3352),y)
EXTRA_CFLAGS += -DRT3352 -DRT305x -DRTMP_RF_RW_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_WPA_SUPPLICANT),y)
EXTRA_CFLAGS += -DWPA_SUPPLICANT_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_WMM_ACM),y)
EXTRA_CFLAGS += -DWMM_ACM_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_LED),y)
EXTRA_CFLAGS += -DWLAN_LED
endif

ifeq ($(CONFIG_RT2860V2_SNMP),y)
EXTRA_CFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_CARRIER),y)
EXTRA_CFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_EXT_CHANNEL_LIST),y)
EXTRA_CFLAGS   += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(CONFIG_RT2860V2_STA_IDS),y)
EXTRA_CFLAGS += -DIDS_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_DLS),y)
EXTRA_CFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_RT3XXX_STA_ANTENNA_DIVERSITY),y)
EXTRA_CFLAGS   += -DRT3XXX_ANTENNA_DIVERSITY_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_WSC),y)
EXTRA_CFLAGS += -DWSC_STA_SUPPORT
endif

#ifeq ($(CONFIG_RA_NETWORK_WORKQUEUE_BH),y)
EXTRA_CFLAGS   += -DWORKQUEUE_BH
#endif

ifeq ($(CONFIG_RT2860V2_STA_INTELLIGENT_RATE_ADAPTION),y)
EXTRA_CFLAGS   += -DNEW_RATE_ADAPT_SUPPORT
endif
