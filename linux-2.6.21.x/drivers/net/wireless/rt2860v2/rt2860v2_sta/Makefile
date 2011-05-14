EXTRA_CFLAGS = -Idrivers/net/wireless/rt2860v2/include

obj-$(CONFIG_RT2860V2_STA) += rt2860v2_sta.o

rt2860v2_sta-y += ../common/crypt_md5.o
rt2860v2_sta-y += ../common/crypt_sha2.o
rt2860v2_sta-y += ../common/crypt_hmac.o
rt2860v2_sta-y += ../common/mlme.o
rt2860v2_sta-y += ../common/cmm_wep.o
rt2860v2_sta-y += ../common/action.o
rt2860v2_sta-y += ../common/ba_action.o
rt2860v2_sta-y += ../common/cmm_data.o
rt2860v2_sta-y += ../common/rtmp_init.o
rt2860v2_sta-y += ../common/rtmp_init_inf.o
rt2860v2_sta-y += ../common/cmm_tkip.o
rt2860v2_sta-y += ../common/cmm_aes.o
rt2860v2_sta-y += ../common/cmm_sync.o
rt2860v2_sta-y += ../common/cmm_sanity.o
rt2860v2_sta-y += ../common/cmm_info.o
rt2860v2_sta-y += ../common/cmm_wpa.o
rt2860v2_sta-y += ../common/dfs.o
rt2860v2_sta-y += ../common/spectrum.o
rt2860v2_sta-y += ../common/rtmp_timer.o
rt2860v2_sta-y += ../common/rt_channel.o
rt2860v2_sta-y += ../common/cmm_profile.o
rt2860v2_sta-y += ../common/cmm_asic.o
rt2860v2_sta-y += ../common/rtmp_swmcu.o
rt2860v2_sta-y += ../common/cmm_cfg.o
rt2860v2_sta-y += ../common/eeprom.o
rt2860v2_sta-y += ../common/ee_flash.o

rt2860v2_sta-y += ../common/cmm_mac_pci.o
rt2860v2_sta-y += ../common/cmm_data_pci.o
rt2860v2_sta-y += ../common/crypt_aes.o
rt2860v2_sta-y += ../common/crypt_arc4.o
rt2860v2_sta-y += ../common/cmm_cmd.o
rt2860v2_sta-y += ../common/cmm_wpa_adhoc.o

rt2860v2_sta-y += ../sta/assoc.o
rt2860v2_sta-y += ../sta/auth.o
rt2860v2_sta-y += ../sta/auth_rsp.o
rt2860v2_sta-y += ../sta/sync.o
rt2860v2_sta-y += ../sta/sanity.o
rt2860v2_sta-y += ../sta/rtmp_data.o
rt2860v2_sta-y += ../sta/connect.o
rt2860v2_sta-y += ../sta/wpa.o
rt2860v2_sta-y += ../sta/sta_cfg.o

rt2860v2_sta-y +=  ../os/linux/rt_proc.o
rt2860v2_sta-y +=  ../os/linux/rt_linux.o
rt2860v2_sta-y +=  ../os/linux/rt_profile.o
rt2860v2_sta-y +=  ../os/linux/rt_main_dev.o
rt2860v2_sta-y +=  ../os/linux/sta_ioctl.o
rt2860v2_sta-y += ../common/rt_ate.o


rt2860v2_sta-y +=  ../os/linux/rbus_main_dev.o
rt2860v2_sta-y +=  ../os/linux/rt_pci_rbus.o
rt2860v2_sta-y += ../os/linux/rt_rbus_pci_util.o

ifeq ($(CONFIG_RALINK_RT2880),y)
rt2860v2_sta-y +=  ../chips/rt2880.o
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
rt2860v2_sta-y +=  ../common/rt_rf.o
rt2860v2_sta-y +=  ../chips/rt2883.o
endif

ifeq ($(CONFIG_RALINK_RT3883),y)
rt2860v2_sta-y +=  ../common/rt_rf.o
rt2860v2_sta-y +=  ../chips/rt3883.o
endif

ifeq ($(CONFIG_RALINK_RT3052),y)
rt2860v2_sta-y +=  ../common/rt_rf.o
rt2860v2_sta-y +=  ../chips/rt305x.o
endif

ifeq ($(CONFIG_RALINK_RT3352),y)
rt2860v2_sta-y +=  ../common/rt_rf.o
rt2860v2_sta-y +=  ../chips/rt305x.o
endif

ifeq ($(CONFIG_RT2860V2_STA_LED),y)
rt2860v2_sta-y += ../common/led.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WMM_ACM),y)
rt2860v2_sta-y += ../common/acm_edca.o
rt2860v2_sta-y += ../common/acm_comm.o
rt2860v2_sta-y += ../common/acm_iocl.o
endif

ifeq ($(CONFIG_RT2860V2_RT3XXX_STA_ANTENNA_DIVERSITY),y)
rt2860v2_sta-y += ../os/linux/ap_diversity.o
endif

ifeq ($(CONFIG_RT2860V2_STA_DLS),y)
rt2860v2_sta-y += ../sta/dls.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WSC),y)
rt2860v2_sta-y += ../common/wsc.o
rt2860v2_sta-y += ../common/wsc_tlv.o
rt2860v2_sta-y += ../common/crypt_biginteger.o
rt2860v2_sta-y += ../common/crypt_dh.o
endif

ifeq ($(CONFIG_RT2860V2_STA_VIDEO_TURBINE),y)
rt2860v2_sta-y += ../common/cmm_video.o
endif

ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
rt2860v2_sta-y +=  ../common/cmm_txbf.o
rt2860v2_sta-y +=  ../common/cmm_txbf_cal.o
endif

rt2860v2_sta-y += ../common/cmm_mat.o
rt2860v2_sta-y += ../common/cmm_mat_iparp.o
rt2860v2_sta-y += ../common/cmm_mat_pppoe.o
rt2860v2_sta-y += ../common/cmm_mat_ipv6.o

###################
#  CFLAGS
##################
EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -DLINUX \
	       -Wall -Wstrict-prototypes -Wno-trigraphs 

EXTRA_CFLAGS += -DCONFIG_STA_SUPPORT  -DRTMP_RBUS_SUPPORT -DRTMP_MAC_PCI -DDOT11_N_SUPPORT -DRELASE_EXCLUDE
EXTRA_CFLAGS += -DSTATS_COUNT_SUPPORT -DCONFIG_RT2880_ATE_CMD_NEW -DNEW_TXCONT
EXTRA_CFLAGS += -DRESOURCE_PRE_ALLOC -DRALINK_ATE -DRALINK_28xx_QA
EXTRA_CFLAGS += -DMAT_SUPPORT

ifeq ($(CONFIG_RALINK_RT2880_STA_DBG),y)
EXTRA_CFLAGS += -DDBG
endif

ifeq ($(CONFIG_RALINK_RT2880),y)
EXTRA_CFLAGS += -DRT2880
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
EXTRA_CFLAGS += -DRT2883 -DRTMP_RF_RW_SUPPORT -DDOT11N_SS3_SUPPORT -DCONFIG_RALINK_RT2883
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
EXTRA_CFLAGS += -DTXBF_SUPPORT
endif
endif

ifeq ($(CONFIG_RALINK_RT3883),y)
EXTRA_CFLAGS += -DRT3883 -DRTMP_RF_RW_SUPPORT -DDOT11N_SS3_SUPPORT -DCONFIG_RALINK_RT3883
EXTRA_CFLAGS += -DSPECIFIC_BCN_BUF_SUPPORT
ifeq ($(CONFIG_RT2860V2_STA_TXBF),y)
EXTRA_CFLAGS += -DTXBF_SUPPORT
endif
EXTRA_CFLAGS   += -DSTREAM_MODE_SUPPORT
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
EXTRA_CFLAGS += -DWLAN_LED -DLED_CONTROL_SUPPORT
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

ifeq ($(CONFIG_RT2860V2_STA_VIDEO_TURBINE),y)
EXTRA_CFLAGS += -DVIDEO_TURBINE_SUPPORT
endif

ifeq ($(CONFIG_RA_NETWORK_WORKQUEUE_BH),y)
EXTRA_CFLAGS   += -DWORKQUEUE_BH
endif

ifeq ($(CONFIG_RT2860V2_STA_INTELLIGENT_RATE_ADAPTION),y)
EXTRA_CFLAGS   += -DNEW_RATE_ADAPT_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_80211N_DRAFT3),y)
EXTRA_CFLAGS += -DDOT11N_DRAFT3
endif
