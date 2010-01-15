﻿var helptitle = new Array(19);
// Wireless
helptitle[0] = [["", ""],
				["<#WLANConfig11b_SSID_itemname#>", "wl_ssid"],
				["<#WLANConfig11b_x_BlockBCSSID_itemname#>", "wl_closed"],				
				["<#WLANConfig11b_Channel_itemname#>", "wl_channel"],
				["<#WLANConfig11b_x_Mode11g_itemname#>", "wl_gmode"],
				["<#WLANConfig11b_AuthenticationMethod_itemname#>", "wl_auth_mode"],
				["<#WLANConfig11b_WPAType_itemname#>", "wl_crypto"],
				["<#WLANConfig11b_x_PSKKey_itemname#>", "wl_wpa_psk"],
				["<#WLANConfig11b_x_Phrase_itemname#>", "wl_phrase_x"],
				["<#WLANConfig11b_WEPType_itemname#>", "wl_wep_x"],
				["<#WLANConfig11b_WEPDefaultKey_itemname#>", "wl_key"],
				["<#WLANConfig11b_x_Rekey_itemname#>", "wl_wpa_gtk_rekey"],
				["<#WLANConfig11b_WEPKey_itemname#>", "wl_asuskey1"],
				["<#WLANConfig11b_WEPKey_itemname#>", "wl_asuskey1"],
				["<#WLANConfig11b_ChannelBW_itemname#>", "HT_BW"],
				["<#WLANConfig11b_EChannel_itemname#>", "HT_EXTCHA"]				
				];
helptitle[1] = [["", ""],
				["<#WLANConfig11b_x_APMode_itemname#>", "wl_mode_x"],
				["<#WLANConfig11b_Channel_itemname#>", "wl_channel"],
				["<#WLANConfig11b_x_BRApply_itemname#>", "wl_wdsapply_x"]];
helptitle[2] = [["", ""],
				["<#WLANAuthentication11a_ExAuthDBIPAddr_itemname#>", "wl_radius_ipaddr"],
				["<#WLANAuthentication11a_ExAuthDBPortNumber_itemname#>", "wl_radius_port"],
				["<#WLANAuthentication11a_ExAuthDBPassword_itemname#>", "wl_radius_key"]];
helptitle[3] = [["", ""],
				["<#WLANConfig11b_x_RadioEnable_itemname#>", "wl_radio_x"],
				["<#WLANConfig11b_x_RadioEnableDate_itemname#>", "wl_radio_date_x_"],
				["<#WLANConfig11b_x_RadioEnableTime_itemname#>", "wl_radio_time_x_"],
				["", ""],//["<#WLANConfig11b_x_AfterBurner_itemname#>", "wl_afterburner"],
				["<#WLANConfig11b_x_IsolateAP_itemname#>", "wl_ap_isolate"],
				["<#WLANConfig11b_DataRateAll_itemname#>", "wl_rate"],
				["<#WLANConfig11b_MultiRateAll_itemname#>", "wl_mrate"],
				["<#WLANConfig11b_DataRate_itemname#>", "wl_rateset"],
				["<#WLANConfig11b_x_Frag_itemname#>", "wl_frag"],
				["<#WLANConfig11b_x_RTS_itemname#>", "wl_rts"],
				["<#WLANConfig11b_x_DTIM_itemname#>", "wl_dtim"],
				["<#WLANConfig11b_x_Beacon_itemname#>", "wl_bcn"],
				["<#WLANConfig11b_x_TxBurst_itemname#>", "TxBurst"],
				["<#WLANConfig11b_x_WMM_itemname#>", "wl_wme"],
				["<#WLANConfig11b_x_NOACK_itemname#>", "wl_wme_no_ack"],
				["<#WLANConfig11b_x_PktAggregate_itemname#>", "PktAggregate"],
				["<#WLANConfig11b_x_APSD_itemname#>", "APSDCapable"],
				["<#WLANConfig11b_x_DLS_itemname#>", "DLSCapable"],
				["<#WLANConfig11b_x_HT_OpMode_itemname#>", "HT_OpMode"]
				];
// LAN
helptitle[4] = [["", ""],
				["<#LANHostConfig_IPRouters_itemname#>", "lan_ipaddr"],
				["<#LANHostConfig_SubnetMask_itemname#>", "lan_netmask"],
				["<#LANHostConfig_x_Gateway_itemname#>", "lan_gateway"]];
helptitle[5] = [["", ""],
			 	["<#LANHostConfig_DHCPServerConfigurable_itemname#>", "dhcp_enable_x"],
				["<#LANHostConfig_DomainName_itemname#>", "lan_domain"],
				["<#LANHostConfig_MinAddress_itemname#>", "dhcp_start"],
				["<#LANHostConfig_MaxAddress_itemname#>", "dhcp_end"],
				["<#LANHostConfig_LeaseTime_itemname#>", "dhcp_lease"],
				["<#LANHostConfig_x_LGateway_itemname#>", "dhcp_gateway_x"],
				["<#LANHostConfig_x_LDNSServer1_itemname#>", "dhcp_dns1_x"],
				["<#LANHostConfig_x_WINSServer_itemname#>", "dhcp_wins_x"]];
helptitle[6] = [["", ""],
				["<#RouterConfig_GWStaticIP_itemname#>", "sr_ipaddr_x_0"],
				["<#RouterConfig_GWStaticMask_itemname#>", "sr_netmask_x_0"],
				["<#RouterConfig_GWStaticGW_itemname#>", "sr_gateway_x_0"],
				["<#RouterConfig_GWStaticMT_itemname#>", "sr_matric_x_0"],
				["<#RouterConfig_GWStaticIF_itemname#>", "sr_if_x_0"]];
// WAN
helptitle[7] = [["", ""],
				["<#IPConnection_ExternalIPAddress_itemname#>", "wan_ipaddr"],
				["<#IPConnection_x_ExternalSubnetMask_itemname#>", "wan_netmask"],
				["<#IPConnection_x_ExternalGateway_itemname#>", "wan_gateway"],
				["<#PPPConnection_UserName_itemname#>", "wan_pppoe_username"],
				["<#PPPConnection_Password_itemname#>", "wan_pppoe_passwd"],
				["<#PPPConnection_IdleDisconnectTime_itemname#>", "wan_pppoe_idletime"],
				["<#PPPConnection_x_PPPoEMTU_itemname#>", "wan_pppoe_mtu"],
				["<#PPPConnection_x_PPPoEMRU_itemname#>", "wan_pppoe_mru"],
				["<#PPPConnection_x_ServiceName_itemname#>", "wan_pppoe_service"],
				["<#PPPConnection_x_AccessConcentrator_itemname#>", "wan_pppoe_ac"],
				["<#PPPConnection_x_PPPoERelay_itemname#>", "wan_pppoe_relay_x"],
				["<#IPConnection_x_DNSServerEnable_itemname#>", "wan_dnsenable_x"],
				["<#IPConnection_x_DNSServer1_itemname#>", "wan_dns1_x"],
				["<#IPConnection_x_DNSServer2_itemname#>", "wan_dns2_x"],
				["<#PPPConnection_x_HostNameForISP_itemname#>", "wan_hostname"],
				["<#PPPConnection_x_MacAddressForISP_itemname#>", "wan_hwaddr_x"],
				["<#PPPConnection_x_PPTPOptions_itemname#>", "wan_pptp_options_x"],
				["<#PPPConnection_x_AdditionalOptions_itemname#>", "wan_pppoe_options_x"],
				["<#PPPConnection_x_HeartBeat_itemname#>", "wan_heartbeat_x"],
				["<#IPConnection_BattleNet_itemname#>", "sp_battle_ips"]];
//Firewall
helptitle[8] = [["", ""],
				["<#FirewallConfig_WanLanLog_itemname#>", "fw_log_x"],
				["<#FirewallConfig_x_WanWebEnable_itemname#>", "misc_http_x"],
				["<#FirewallConfig_x_WanWebPort_itemname#>", "misc_httpport_x"],
				["<#FirewallConfig_x_WanLPREnable_itemname#>", "misc_lpr_x"],
				["<#FirewallConfig_x_WanPingEnable_itemname#>", "misc_ping_x"],
				["<#FirewallConfig_FirewallEnable_itemname#>", "fw_enable_x"]];
helptitle[9] = [["", ""],
				["<#FirewallConfig_URLActiveDate_itemname#>", "url_date_x_"],
				["<#FirewallConfig_URLActiveTime_itemname#>", "url_time_x_"]];
helptitle[10] = [["", ""],
				["<#FirewallConfig_LanWanActiveDate_itemname#>", "filter_lw_date_x_"],
				["<#FirewallConfig_LanWanActiveTime_itemname#>", "filter_lw_time_x_"],
				["<#FirewallConfig_LanWanDefaultAct_itemname#>", "filter_lw_default_x"],
				["<#FirewallConfig_LanWanICMP_itemname#>", "filter_lw_icmp_x"],
				["<#FirewallConfig_LanWanFirewallEnable_itemname#>", "fw_lw_enable_x"]];
//Administration
helptitle[11] = [["", ""],
				["<#LANHostConfig_x_ServerLogEnable_itemname#>", "log_ipaddr"],
				["<#LANHostConfig_x_TimeZone_itemname#>", "time_zone"],
				["<#LANHostConfig_x_NTPServer_itemname#>", "ntp_server0"]];
//Log
helptitle[12] = [["", ""],
				["<#General_x_SystemUpTime_itemname#>", "system_now_time"],
				["<#PrinterStatus_x_PrinterModel_itemname#>", ""],
				["<#PrinterStatus_x_PrinterStatus_itemname#>", ""],
				["<#PrinterStatus_x_PrinterUser_itemname#>", ""]];
//WPS
helptitle[13] = [["", ""],
				["<#WLANConfig11b_x_WPS_itemname#>", ""],
				["<#WLANConfig11b_x_WPSMode_itemname#>", ""],
				["<#WLANConfig11b_x_WPSPIN_itemname#>", ""],
				["<#WLANConfig11b_x_DevicePIN_itemname#>", ""]];
//UPnP
helptitle[14] = [["", ""],
				["<#UPnPMediaServer#>", ""]];
//AiDisk Wizard
helptitle[15] = [["", ""],
				["<a href='../Advanced_AiDisk_ftp.asp' target='_parent' hidefocus='true'><#t1USB#></a>", ""],
				["<#AiDisk_Step1_helptitle#>", ""],
				["<#AiDisk_Step2_helptitle#>", ""],
				["<#AiDisk_Step3_helptitle#>", ""]];

helptitle[16] = [["", ""],
				["<#EzQoS_helptitle#>", ""]];
//Others in the USB application
helptitle[17] = [["", ""],
				["<#ShareNode_MaximumLoginUser_itemname#>", "st_max_user"],
				["<#ShareNode_DeviceName_itemname#>", "computer_name"],
				["<#ShareNode_WorkGroup_itemname#>", "st_samba_workgroup"],
				["<#BasicConfig_EnableDownloadMachine_itemname#>", "apps_dl"],
				["<#BasicConfig_EnableDownloadShare_itemname#>", "apps_dl_share"],
				["<#BasicConfig_EnableMediaServer_itemname#>", "upnp_enable"],
				["Seeding", "apps_seed"],
				["Maximum Upload Rate", "apps_upload_max"]];
				
// MAC filter
helptitle[18] = [["", ""],
				["<#FirewallConfig_MFMethod_itemname#>", "macfilter_enable_x"]];
// Setting
helptitle[19] = [["", ""],
				["<#Setting_factorydefault_itemname#>", ""],
				["<#Setting_save_itemname#>", ""],
				["<#Setting_upload_itemname#>", ""]];
// QoS
helptitle[20] = [["", ""],
				["<#BM_measured_uplink_speed#>", ""],
				["<#BM_manual_uplink_speed#>", "qos_manual_ubw"]];
// HSDPA
helptitle[21] = [["", ""],
				["<#HSDPAConfig_hsdpa_enable_itemname#>", ""],
				["<#HSDPAConfig_USBAdapter_itemname#>", ""],
				["<#HSDPAConfig_private_apn_itemname#>", "private_apn"],
				["HSDPA<#PPPConnection_x_PPPoEMTU_itemname#>", "wan_hsdpa_mtu"],
				["HSDPA<#PPPConnection_x_PPPoEMRU_itemname#>", "wan_hsdpa_mru"],
				["<#IPConnection_x_DNSServer1_itemname#>", "wan2_dns1_x"],
				["<#IPConnection_x_DNSServer2_itemname#>", "wan2_dns2_x"]];

helptitle[22] = [["", ""],
								 ["Router(<#OP_RT_item#>)", ""],
								 ["Repeater(<#OP_RE_item#>)", ""],
								 ["AP(<#OP_AP_item#>)", ""]];

var helpcontent = new Array(19);
helpcontent[0] = new Array("",
						   "<#WLANConfig11b_SSID_itemdesc#>",
						   "<#WLANConfig11b_x_BlockBCSSID_itemdesc#>",						   
						   "<#WLANConfig11b_Channel_itemdesc#>",
						   "<#WLANConfig11b_x_Mode11g_itemdesc#>",
						   "<#WLANConfig11b_AuthenticationMethod_itemdesc#>",
						   "<#WLANConfig11b_WPAType_itemdesc#>",
						   "<#WLANConfig11b_x_PSKKey_itemdesc#>",
						   "<#WLANConfig11b_x_Phrase_itemdesc#>",
						   "<#WLANConfig11b_WEPType_itemdesc#>",
						   "<#WLANConfig11b_WEPDefaultKey_itemdesc#>",
						   "<#WLANConfig11b_x_Rekey_itemdesc#>",
						   "<#WLANConfig11b_WEPKey_itemtype1#>",
						   "<#WLANConfig11b_WEPKey_itemtype2#>",
						   "<#WLANConfig11b_ChannelBW_itemdesc#>",
						   "<#WLANConfig11b_EChannel_itemdesc#>");
helpcontent[1] = new Array("",
						   "<#WLANConfig11b_x_APMode_itemdesc#>",
						   "<#WLANConfig11b_Channel_itemdesc#>",
						   "<#WLANConfig11b_x_BRApply_itemdesc#>");
helpcontent[2] = new Array("",
						   "<#WLANAuthentication11a_ExAuthDBIPAddr_itemdesc#>",
						   "<#WLANAuthentication11a_ExAuthDBPortNumber_itemdesc#>",
						   "<#WLANAuthentication11a_ExAuthDBPassword_itemdesc#>");
helpcontent[3] = new Array("",
						   "<#WLANConfig11b_x_RadioEnable_itemdesc#>",
						   "<#WLANConfig11b_x_RadioEnableDate_itemdesc#>",
						   "<#WLANConfig11b_x_RadioEnableTime_itemdesc#>",						   
						   "<#WLANConfig11b_x_AfterBurner_itemdesc#>",
						   "<#WLANConfig11b_x_IsolateAP_itemdesc#>",
						   "<#WLANConfig11b_DataRateAll_itemdesc#>",
							 "<#WLANConfig11b_MultiRateAll_itemdesc#>",
							 "<#WLANConfig11b_DataRate_itemdesc#>",
						   "<#WLANConfig11b_x_Frag_itemdesc#>",
						   "<#WLANConfig11b_x_RTS_itemdesc#>",
						   "<#WLANConfig11b_x_DTIM_itemdesc#>",
						   "<#WLANConfig11b_x_Beacon_itemdesc#>",
						   "<#WLANConfig11b_x_TxBurst_itemdesc#>",
						   "<#WLANConfig11b_x_WMM_itemdesc#>",
						   "<#WLANConfig11b_x_NOACK_itemdesc#>",
						   "<#WLANConfig11b_x_PktAggregate_itemdesc#>",
						   "<#WLANConfig11b_x_APSD_itemdesc#>",
						   "<#WLANConfig11b_x_DLS_itemdesc#>",
						   "<#WLANConfig11b_x_HT_OpMode_itemdesc#>");
helpcontent[4] = new Array("",
						   "<#LANHostConfig_IPRouters_itemdesc#>",
						   "<#LANHostConfig_SubnetMask_itemdesc#>",
						   "<#LANHostConfig_x_Gateway_itemdesc#>");
helpcontent[5] = new Array("",
							 "<#LANHostConfig_DHCPServerConfigurable_itemdesc#>",
							 "<#LANHostConfig_DomainName_itemdesc#>",
							 "<#LANHostConfig_MinAddress_itemdesc#>",
							 "<#LANHostConfig_MaxAddress_itemdesc#>",
							 "<#LANHostConfig_LeaseTime_itemdesc#>",
							 "<#LANHostConfig_x_LGateway_itemdesc#>",
							 "<#LANHostConfig_x_LDNSServer1_itemdesc#>",
							 "<#LANHostConfig_x_WINSServer_itemdesc#>");
helpcontent[6] = new Array("",
						   "<#RHELP_desc4#>",
						   "<#RHELP_desc5#>",
						   "<#RHELP_desc6#>",
						   "<#RHELP_desc7#>",
						   "<#RHELP_desc8#>");
//WAN
helpcontent[7] = new Array("",
							 "<#IPConnection_ExternalIPAddress_itemdesc#>",
							 "<#IPConnection_x_ExternalSubnetMask_itemdesc#>",
							 "<#IPConnection_x_ExternalGateway_itemdesc#>",
							 "<#PPPConnection_UserName_itemdesc#>",
							 "<#PPPConnection_Password_itemdesc#>",
							 "<#PPPConnection_IdleDisconnectTime_itemdesc#>",
							 "<#PPPConnection_x_PPPoEMTU_itemdesc#>",
							 "<#PPPConnection_x_PPPoEMRU_itemdesc#>",
							 "<#PPPConnection_x_ServiceName_itemdesc#>",
							 "<#PPPConnection_x_AccessConcentrator_itemdesc#>",
							 "<#PPPConnection_x_PPPoERelay_itemdesc#>",
							 "<#IPConnection_x_DNSServerEnable_itemdesc#>",
							 "<#IPConnection_x_DNSServer1_itemdesc#>",
							 "<#IPConnection_x_DNSServer2_itemdesc#>",
							 "<#BOP_isp_host_desc#>",
							 "<#PPPConnection_x_MacAddressForISP_itemdesc#>",
							 "<#PPPConnection_x_PPTPOptions_itemdesc#>",
							 "<#PPPConnection_x_AdditionalOptions_itemdesc#>",
							 "<#BOP_isp_heart_desc#>",
							 "<#IPConnection_BattleNet_itemdesc#>");
//Firewall
helpcontent[8] = new Array("",
						   "<#FirewallConfig_WanLanLog_itemdesc#>",
						   "<#FirewallConfig_x_WanWebEnable_itemdesc#>",
						   "<#FirewallConfig_x_WanWebPort_itemdesc#>",
						   "<#FirewallConfig_x_WanLPREnable_itemdesc#>",
						   "<#FirewallConfig_x_WanPingEnable_itemdesc#>",
						   "<#FirewallConfig_FirewallEnable_itemdesc#>");
helpcontent[9] = new Array("",
						   "<#FirewallConfig_URLActiveDate_itemdesc#>",
						   "<#FirewallConfig_URLActiveTime_itemdesc#>");
helpcontent[10] = new Array("",
							"<#FirewallConfig_LanWanActiveDate_itemdesc#>",
							"<#FirewallConfig_LanWanActiveTime_itemdesc#>",
							"<#FirewallConfig_LanWanDefaultAct_itemdesc#>",
							"<#FirewallConfig_LanWanICMP_itemdesc#>",
							"<#FirewallConfig_LanWanFirewallEnable_itemdesc#>");
//Administration
helpcontent[11] = new Array("",
							"<#LANHostConfig_x_ServerLogEnable_itemdesc#>",
							"<#LANHostConfig_x_TimeZone_itemdesc#>",
							"<#LANHostConfig_x_NTPServer_itemdesc#>");
//Log
helpcontent[12] = new Array("",
							"<#General_x_SystemUpTime_itemdesc#>",
							"<#PrinterStatus_x_PrinterModel_itemdesc#>",
							"<#PrinterStatus_x_PrinterStatus_itemdesc#>",
							"<#PrinterStatus_x_PrinterUser_itemdesc#>");
//WPS
helpcontent[13] = new Array("",
							"<#WLANConfig11b_x_WPS_itemdesc#>",
							"<#WLANConfig11b_x_WPSMode_itemdesc#>",
							"<#WLANConfig11b_x_WPSPIN_itemdesc#>",
							"<#WLANConfig11b_x_DevicePIN_itemdesc#>");
//UPnP
helpcontent[14] = new Array("",
							"<#UPnPMediaServer_Help#>");
//AiDisk Wizard
helpcontent[15] = new Array("",
							"<#AiDisk_moreconfig#>",
							"<#AiDisk_Step1_help#><p><a href='../Advanced_AiDisk_ftp.asp' target='_parent' hidefocus='true'><#MoreConfig#></a></p><!--span style='color:#CC0000'><#AiDisk_Step1_help2#></span-->",
							"<#AiDisk_Step2_help#>",
							"<#AiDisk_Step3_help#>");
//EzQoS
helpcontent[16] = new Array("",
							"<#EZQoSDesc1#><p><#EZQoSDesc2#><a href='/Advanced_QOSUserSpec_Content.asp'><#BM_title_User#></a></p>");
//Others in the USB application
helpcontent[17] = new Array("",
							"<#JS_storageMLU#>",
							"<#JS_storageright#>",
							"<#Help_of_Workgroup#>",
							"<#JS_basiconfig1#>",
							"<#JS_basiconfig3#>",
							"<#JS_basiconfig8#>",
							"選擇[啟用]，下載完畢後會自動作種。",
							"最大上傳速率，單位為Mbps。預設值為0，最大值為99999。");
							
// MAC filter
helpcontent[18] = new Array("",
							"<#FirewallConfig_MFMethod_itemdesc#>");
// Setting
helpcontent[19] = new Array("",
							"<#Setting_factorydefault_itemdesc#>",
							"<#Setting_save_itemdesc#>",
							"<#Setting_upload_itemdesc#>");
// QoS
helpcontent[20] = new Array("",
							"<#BM_measured_uplink_speed_desc#>",
							"<#BM_manual_uplink_speed_desc#>");
// HSDPA
helpcontent[21] = new Array("",
							"<#HSDPAConfig_hsdpa_enable_itemdesc#>",
							"<#HSDPAConfig_USBAdapter_itemdesc#>",
							"<#HSDPAConfig_private_apn_itemdesc#>",
							"HSDPA<#PPPConnection_x_PPPoEMTU_itemdesc#>",
							"HSDPA<#PPPConnection_x_PPPoEMRU_itemdesc#>",
							"<#IPConnection_x_DNSServer1_itemdesc#>",
							"<#IPConnection_x_DNSServer2_itemdesc#>");
							
helpcontent[22] = new Array("",
														"<#OP_RT_desc1#>",
														"<#OP_RE_desc1#>",
														"<#OP_AP_desc1#>");

var clicked_help_string = "<#Help_init_word1#> <a class=\"hintstyle\" style=\"background-color:#7aa3bd\"><#Help_init_word2#></a> <#Help_init_word3#>";

function openHint(hint_array_id, hint_show_id, flag){
	$('helpicon').style.display = "none";
	$('hintofPM').style.display = "";
	
	showtext($('helpname'), "<#CTL_help#>");
	
	if($("statusframe")){
		$("statusframe").src = "";
		$("statusframe").style.display = "none";
	}
	
	$("hint_body").style.display = "";
	
	if(typeof(hint_show_id) == "number" && hint_show_id > 0){
		$('hint_body').style.display = "";
		$("statusframe").style.display = "none";
		
		showtext($('helpname'), "<#CTL_help#>");
		
		clicked_help_string = "<span>"+helptitle[hint_array_id][hint_show_id][0]+"</span><br>"+helpcontent[hint_array_id][hint_show_id];
		
		showtext($('hint_body'), clicked_help_string);
	}
	
	if(hint_array_id == 14
			|| hint_array_id == 15
			|| hint_array_id == 16)
		return;
	
	if(flag != "false")
		document.hint_form.scrollIntoView("true");
}

function closeHint(){
	$('helpicon').style.display = "";
	$('hintofPM').style.display = "none";
}

function enable_auto_hint(group_num, all_hint_num){
	var obj_name = "";
	var input_objs = new Array();
	var select_objs = new Array();
	
	for(var i = 1; i <= all_hint_num; ++i){
		obj_name = helptitle[group_num][i][1];
		input_objs = getElementsByName_iefix("input", obj_name);
		select_objs = getElementsByName_iefix("select", obj_name);
		
		if(input_objs.length > 0)
			for(var j = 0; j < input_objs.length; ++j){
				var temp_class_name = input_objs[j].hint_order;
				
				input_objs[j].hint_order = i;
				input_objs[j].onmouseup = function(){
						openHint(group_num, parseInt(this.hint_order), "false");
					};
			}
		
		if(select_objs.length > 0)
			for(var j = 0; j < select_objs.length; ++j){
				var temp_class_name = select_objs[j].hint_order;
				
				select_objs[j].hint_order = i;
				select_objs[j].onmouseup = function(){
						openHint(group_num, parseInt(this.hint_order), "false");
					};
			}
	}
}

function disable_auto_hint(group_num, all_hint_num){
	var obj_name = "";
	var input_objs = new Array();
	var select_objs = new Array();
	
	for(var i = 1; i <= all_hint_num; ++i){
		obj_name = helptitle[group_num][i][1];
		input_objs = getElementsByName_iefix("input", obj_name);
		select_objs = getElementsByName_iefix("select", obj_name);
		
		if(input_objs.length > 0)
			for(var j = 0; j < input_objs.length; ++j){
				var temp_class_name = input_objs[j].hint_order;
				
				input_objs[j].hint_order = i;
				input_objs[j].onfocus = function(){};
			}
		
		if(select_objs.length > 0)
			for(var j = 0; j < select_objs.length; ++j){
				var temp_class_name = select_objs[j].hint_order;
				
				select_objs[j].hint_order = i;
				select_objs[j].onfocus = function(){};
			}
	}
}
