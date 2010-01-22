EXTRA_CFLAGS = -Idrivers/net/wireless/rt2860v2/include
#EXTRA_CFLAGS   += -DDBG
#EXTRA_CFLAGS   += -DBLOCK_NET_IF

obj-$(CONFIG_RT2860V2_STA) += rt2860v2_sta.o

rt2860v2_sta-objs += ../rt2860v2/common/crypt_md5.o
rt2860v2_sta-objs += ../rt2860v2/common/crypt_sha2.o
rt2860v2_sta-objs += ../rt2860v2/common/crypt_biginteger.o
rt2860v2_sta-objs += ../rt2860v2/common/crypt_hmac.o
rt2860v2_sta-objs += ../rt2860v2/common/mlme.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_wep.o
rt2860v2_sta-objs += ../rt2860v2/common/action.o
rt2860v2_sta-objs += ../rt2860v2/common/ba_action.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_data.o
rt2860v2_sta-objs += ../rt2860v2/common/rtmp_init.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_tkip.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_aes.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_sync.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_sanity.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_info.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_wpa.o
rt2860v2_sta-objs += ../rt2860v2/common/dfs.o
#rt2860v2_sta-objs += ../rt2860v2/common/dfs_mcu.o
rt2860v2_sta-objs += ../rt2860v2/common/spectrum.o

rt2860v2_sta-objs += ../rt2860v2/common/rtmp_timer.o
rt2860v2_sta-objs += ../rt2860v2/common/rt_channel.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_profile.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_asic.o
rt2860v2_sta-objs += ../rt2860v2/common/rtmp_swmcu.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_cfg.o
rt2860v2_sta-objs += ../rt2860v2/common/eeprom.o
rt2860v2_sta-objs += ../rt2860v2/common/ee_flash.o
#rt2860v2_sta-objs += ../rt2860v2/common/rtmp_mcu.o

rt2860v2_sta-objs += ../rt2860v2/common/cmm_mac_pci.o
rt2860v2_sta-objs += ../rt2860v2/common/cmm_data_pci.o


rt2860v2_sta-objs += ../rt2860v2/sta/assoc.o
rt2860v2_sta-objs += ../rt2860v2/sta/auth.o
rt2860v2_sta-objs += ../rt2860v2/sta/auth_rsp.o
rt2860v2_sta-objs += ../rt2860v2/sta/sync.o
rt2860v2_sta-objs += ../rt2860v2/sta/sanity.o
rt2860v2_sta-objs += ../rt2860v2/sta/rtmp_data.o
rt2860v2_sta-objs += ../rt2860v2/sta/connect.o
rt2860v2_sta-objs +=  ../rt2860v2/sta/wpa.o


rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_linux.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_profile.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_main_dev.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/sta_ioctl.o
#rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_eeprom.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_ate.o
#rt2860v2_sta-objs +=  ../rt2860v2/common/netif_block.o

#rt2860v2_sta-objs +=  ../rt2860v2/os/linux/pci_main_dev.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rbus_main_dev.o
rt2860v2_sta-objs +=  ../rt2860v2/os/linux/rt_pci_rbus.o


ifeq ($(CONFIG_RALINK_RT2883),y)
rt2860v2_sta-objs += ../rt2860v2/common/mfb_tlv.o
endif

ifeq ($(CONFIG_RALINK_RT2880),y)
rt2860v2_sta-objs +=  ../rt2860v2/chips/rt2880.o
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
rt2860v2_sta-objs +=  ../rt2860v2/chips/rt2883.o
endif

ifeq ($(CONFIG_RALINK_RT3052),y)
rt2860v2_sta-objs +=  ../rt2860v2/common/rt_rf.o
rt2860v2_sta-objs +=  ../rt2860v2/chips/rt305x.o
endif

ifeq ($(CONFIG_RT2860V2_STA_LED),y)
rt2860v2_sta-objs += ../rt2860v2/common/led.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WAPI),y)
obj-$(CONFIG_RT2860V2_WAPI) += wapi_lib.o
wapi_lib-objs += ../rt2860v2/common/sms4.o
wapi_lib-objs += ../rt2860v2/common/wpi_pcrypt.o
wapi_lib-objs += ../rt2860v2/common/cmm_wapi.o

rt2860v2_sta-objs += wapi_lib.o
endif

ifeq ($(CONFIG_RT2860V2_STA_IDS),y)
rt2860v2_ap-objs += ../rt2860v2/ap/ap_ids.o
endif

ifeq ($(CONFIG_RT2860V2_STA_MESH),y)
rt2860v2_sta-objs += ../rt2860v2/common/mesh_bmpkt.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_ctrl.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_link_mng.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_sanity.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_tlv.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_forwarding.o 
rt2860v2_sta-objs += ../rt2860v2/common/mesh_path_mng.o 
rt2860v2_sta-objs += ../rt2860v2/common/mlme_ex.o 
endif

ifeq ($(CONFIG_RT2860V2_STA_DLS),y)
rt2860v2_sta-objs += ../rt2860v2/sta/dls.o
endif

ifeq ($(CONFIG_RT2860V2_STA_WSC),y)
rt2860v2_sta-objs += ../rt2860v2/common/wsc.o
rt2860v2_sta-objs += ../rt2860v2/common/wsc_tlv.o
rt2860v2_sta-objs += ../rt2860v2/common/crypt_dh.o
rt2860v2_sta-objs += ../rt2860v2/common/crypt_aes.o
rt2860v2_sta-objs += ../rt2860v2/common/dh_key.o
rt2860v2_sta-objs += ../rt2860v2/common/evp_enc.o
endif

###################
#  CFLAGS
##################
EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -DLINUX \
	       -Wall -Wstrict-prototypes -Wno-trigraphs 

EXTRA_CFLAGS += -DCONFIG_STA_SUPPORT -DDBG -DRALINK_ATE -DRALINK_28xx_QA -DRTMP_RBUS_SUPPORT -DRTMP_MAC_PCI -DDOT11_N_SUPPORT -DRELASE_EXCLUDE 
EXTRA_CFLAGS += -DMERGE_ARCH_TEAM

ifeq ($(CONFIG_RALINK_RT2880),y)
EXTRA_CFLAGS += -DRT2880
endif

ifeq ($(CONFIG_RALINK_RT2883),y)
EXTRA_CFLAGS += -DRT2883
endif

ifeq ($(CONFIG_RALINK_RT3052),y)
EXTRA_CFLAGS += -DRT3052 -DRT305x -DRTMP_RF_RW_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_WPA_SUPPLICANT),y)
EXTRA_CFLAGS += -DWPA_SUPPLICANT_SUPPORT
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

ifeq ($(CONFIG_RT2860V2_STA_WAPI),y)
EXTRA_CFLAGS += -DWAPI_SUPPORT
endif

ifeq ($(CONFIG_RT2860V2_STA_MESH),y)
EXTRA_CFLAGS += -DMESH_SUPPORT -DMLME_EX
endif

ifeq ($(CONFIG_RT2860V2_STA_WSC),y)
EXTRA_CFLAGS += -DWSC_STA_SUPPORT
endif


