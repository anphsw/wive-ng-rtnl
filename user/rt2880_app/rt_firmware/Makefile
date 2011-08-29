all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
#1T1R-MODE-3050
ifdef CONFIG_RALINK_RT3050_1T1R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3050_1T1R	RT3050_AP_1T1R_V1_0.bin		/etc/Wireless/RT3050_AP_1T1R_V1_0.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3050_1T1R	defaults/RT2860.dat-1T1R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3050_1T1R	defaults/default-1T1R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3050_AP_1T1R_V1_0.bin /etc/Wireless/rf.bin
endif
#1T2R-MODE-3051
ifdef CONFIG_RALINK_RT3051_1T2R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3051_1T2R	RT3051_AP_1T2R_V1_0.bin		/etc/Wireless/RT3051_AP_1T2R_V1_0.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3051_1T2R	defaults/RT2860.dat-1T2R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3051_1T2R	defaults/default-1T2R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3051_AP_1T2R_V1_0.bin /etc/Wireless/rf.bin
endif
#2T2R-MODE-3052
ifdef CONFIG_RALINK_RT3052_2T2R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3052_2T2R	RT3052_AP_2T2R_V1_1.bin		/etc/Wireless/RT3052_AP_2T2R_V1_1.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3052_2T2R	defaults/RT2860.dat-2T2R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3052_2T2R	defaults/default-2T2R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3052_AP_2T2R_V1_1.bin /etc/Wireless/rf.bin
endif
#1T1R-MODE-3350
ifdef CONFIG_RALINK_RT3350
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3350		RT3350_AP_1T1R_V1_0.bin		/etc/Wireless/RT3350_AP_1T1R_V1_0.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3350		defaults/RT2860.dat-1T1R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3350		defaults/default-1T1R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3350_AP_1T1R_V1_0.bin /etc/Wireless/rf.bin
endif
#2T2R-MODE-3352
ifdef CONFIG_RALINK_RT3352_2T2R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3352_2T2R	RT3350_AP_1T1R_V1_0.bin		/etc/Wireless/RT3350_AP_1T1R_V1_0.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3352_2T2R	defaults/RT2860.dat-2T2R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3352_2T2R	defaults/default-2T2R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3350_AP_1T1R_V1_0.bin /etc/Wireless/rf.bin
endif
#2T2R-MODE-3662
ifdef CONFIG_RALINK_RT3662_2T2R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3662_2T2R	RT3662_AP_2T2R_V0_0.bin		/etc/Wireless/RT3662_AP_2T2R_V0_0.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3662_2T2R	defaults/RT2860.dat-2T2R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3662_2T2R	defaults/default-2T2R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3662_AP_2T2R_V0_0.bin /etc/Wireless/rf.bin
endif
#3T3R-MODE-3662
ifdef CONFIG_RALINK_RT3883_3T3R
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3883_3T3R	RT3883_AP_3T3R_V0_1.bin		/etc/Wireless/RT3883_AP_3T3R_V0_1.bin
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3883_3T3R	defaults/RT2860.dat-3T3R	/etc/Wireless/RT2860/RT2860.dat
	$(ROMFSINST) -S -e CONFIG_RALINK_RT3883_3T3R	defaults/default-3T3R		/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/RT3883_AP_3T3R_V0_1.bin /etc/Wireless/rf.bin
endif
#5GHz-MODE-3090
	$(ROMFSINST) -S -e CONFIG_RT3090_AP		RT3092_PCIe_LNA_2T2R_ALC_V1_2.bin /etc/Wireless/RT3092_PCIe_LNA_2T2R_ALC_V1_2.bin
ifdef CONFIG_RT2860V2_AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860AP
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860AP/RT2860AP.dat
endif
ifdef CONFIG_RT2860V2_STA
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860STA
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860STA/RT2860STA.dat
endif
#mesh support
ifdef CONFIG_RT2860V2_AP_MESH
	$(ROMFSINST) -S -e CONFIG_RT2860V2_AP_MESH	scripts/mesh.sh			/etc/scripts/mesh.sh
	$(ROMFSINST) -s /etc/scripts/mesh.sh /bin/mesh.sh
endif
