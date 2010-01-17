<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_3_1#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">

<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/detect.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

var original_wan_type = wan_proto;
var original_wan_dhcpenable = parseInt('<% nvram_get_x("Layer3Forwarding", "x_DHCPClient"); %>');
var original_dnsenable = parseInt('<% nvram_get_x("IPConnection", "wan_dnsenable_x"); %>');

function initial(){
	show_banner(1);
	show_menu(5,3,1);
	show_footer();
	
	enable_auto_hint(7, 19);
	
	change_wan_type(document.form.wan_proto.value, 0);
	fixed_change_wan_type(document.form.wan_proto.value);
	
	if(document.form.wan_pppoe_txonly_x.value == "1")
		document.form.wan_pppoe_idletime_check.checked = true;
		
	$("wan_proto_menu").style.display = (wan_proto == "3g")?"none":"block";
	$("wan_proto_hint").style.display = (wan_proto == "3g")?"block":"none";
	if(wan_proto == "3g"){
		$("ip_sect").style.display = "none";
		$("dns_sect").style.display = "none";
		$("account_sect").style.display = "none";
		$("isp_sect").style.display = "none";
	}
}

function applyRule(){
	if(validForm()){
		showLoading();
		
		inputCtrl(document.form.x_DHCPClient[0], 1);
		inputCtrl(document.form.x_DHCPClient[1], 1);
		if(!document.form.x_DHCPClient[0].checked){
			inputCtrl(document.form.wan_ipaddr, 1);
			inputCtrl(document.form.wan_netmask, 1);
			inputCtrl(document.form.wan_gateway, 1);
		}
		
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
		if(!document.form.wan_dnsenable_x[0].checked){
			inputCtrl(document.form.wan_dns1_x, 1);
			inputCtrl(document.form.wan_dns2_x, 1);
		}
		
		document.form.next_page.value = "";
		document.form.action_mode.value = " Apply ";
		
		document.form.submit();
	}
}

function validForm(){
	if(!document.form.x_DHCPClient[0].checked){
		if(!validate_ipaddr_final(document.form.wan_ipaddr, 'wan_ipaddr')
				|| !validate_ipaddr_final(document.form.wan_netmask, 'wan_netmask')
				|| !validate_ipaddr_final(document.form.wan_gateway, 'wan_gateway')
				)
			return false;
		
		if(document.form.wan_gateway.value == document.form.wan_ipaddr.value){
			alert("<#IPConnection_warning_WANIPEQUALGatewayIP#>");
			document.form.wan_gateway.select();
			document.form.wan_gateway.focus();
			return false;
		}

		if(!wan_netmask_check(document.form.wan_netmask))
			return false;
	}
	
	if(!document.form.wan_dnsenable_x[0].checked){
		/*
		if(document.form.wan_dns1_x.value.length <= 0 && document.form.wan_dns2_x.value.length <= 0){
			alert("<#JS_fieldblank#>");
			document.form.wan_dns1_x.focus();
			document.form.scrollIntoView("true");
			
			return false;
		}
		*/
		
		if(!validate_ipaddr_final(document.form.wan_dns1_x, 'wan_dns1_x')){
			document.form.wan_dns1_x.select();
			document.form.wan_dns1_x.focus();
			
			return false;
		}
		if(!validate_ipaddr_final(document.form.wan_dns2_x, 'wan_dns2_x')){
			document.form.wan_dns2_x.select();
			document.form.wan_dns2_x.focus();
			
			return false;
		}
	}
	
	if(document.form.wan_proto.value == "pppoe"
			|| document.form.wan_proto.value == "pptp"
			|| document.form.wan_proto.value == "l2tp"
			){
		if(!validate_string(document.form.wan_pppoe_username)
				|| !validate_string(document.form.wan_pppoe_passwd)
				)
			return false;
		
		if(!validate_range(document.form.wan_pppoe_idletime, 0, 4294967295))
			return false;
	}
	
	if(document.form.wan_proto.value == "pppoe"){
		if(!validate_range(document.form.wan_pppoe_mtu, 576, 1492)
				|| !validate_string(document.form.wan_pppoe_mru, 576, 1492))
			return false;
		
		if(!validate_string(document.form.wan_pppoe_service)
				|| !validate_string(document.form.wan_pppoe_ac))
			return false;
	}
	
	if(document.form.wan_hostname.value.length > 0)
		 if(!validate_string(document.form.wan_hostname))
		 	return false;
	
	if(document.form.wan_hwaddr_x.value.length > 0)
		 if(!validate_hwaddr(document.form.wan_hwaddr_x))
		 	return false;
	
	if(document.form.wan_heartbeat_x.value.length > 0)
		 if(!validate_string(document.form.wan_heartbeat_x))
		 	return false;
	
	return true;
}

function done_validating(action){
	refreshpage();
}

function change_wan_type(wan_type, flag){
	if(typeof(flag) != "undefined")
		change_wan_dhcp_enable(flag);
	else
		change_wan_dhcp_enable(1);
	
	if(wan_type == "pppoe"){
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
		
		inputCtrl(document.form.wan_pppoe_username, 1);
		inputCtrl(document.form.wan_pppoe_passwd, 1);
		inputCtrl(document.form.wan_pppoe_idletime, 1);
		inputCtrl(document.form.wan_pppoe_idletime_check, 1);
		inputCtrl(document.form.wan_pppoe_mtu, 1);
		inputCtrl(document.form.wan_pppoe_mru, 1);
		inputCtrl(document.form.wan_pppoe_service, 1);
		inputCtrl(document.form.wan_pppoe_ac, 1);
		
		// 2008.03 James. patch for Oleg's patch. {
		inputCtrl(document.form.wan_pppoe_options_x, 1);
		inputCtrl(document.form.wan_pptp_options_x, 0);
		// 2008.03 James. patch for Oleg's patch. }
		
		if(isModel3() != 'WL550gE')
			inputRCtrl1(document.form.wan_pppoe_relay_x, 1);
	}
	else if(wan_type == "pptp"){
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
		
		inputCtrl(document.form.wan_pppoe_username, 1);
		inputCtrl(document.form.wan_pppoe_passwd, 1);
		inputCtrl(document.form.wan_pppoe_idletime, 1);
		inputCtrl(document.form.wan_pppoe_idletime_check, 1);
		inputCtrl(document.form.wan_pppoe_mtu, 0);
		inputCtrl(document.form.wan_pppoe_mru, 0);
		inputCtrl(document.form.wan_pppoe_service, 0);
		inputCtrl(document.form.wan_pppoe_ac, 0);
		
		// 2008.03 James. patch for Oleg's patch. {
		inputCtrl(document.form.wan_pppoe_options_x, 1);
		inputCtrl(document.form.wan_pptp_options_x, 1);
		// 2008.03 James. patch for Oleg's patch. }
		
		if(isModel3() != 'WL550gE')
			inputRCtrl1(document.form.wan_pppoe_relay_x, 1);
	}
	else if(wan_type == "l2tp"){
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
		
		inputCtrl(document.form.wan_pppoe_username, 1);
		inputCtrl(document.form.wan_pppoe_passwd, 1);
		inputCtrl(document.form.wan_pppoe_idletime, 0);
		inputCtrl(document.form.wan_pppoe_idletime_check, 0);
		inputCtrl(document.form.wan_pppoe_mtu, 0);
		inputCtrl(document.form.wan_pppoe_mru, 0);
		inputCtrl(document.form.wan_pppoe_service, 0);
		inputCtrl(document.form.wan_pppoe_ac, 0);
		
		// 2008.03 James. patch for Oleg's patch. {
		inputCtrl(document.form.wan_pppoe_options_x, 1);
		inputCtrl(document.form.wan_pptp_options_x, 0);
		// 2008.03 James. patch for Oleg's patch. }
		
		if(isModel3() != 'WL550gE')
			inputRCtrl1(document.form.wan_pppoe_relay_x, 1);
	}
	else if(wan_type == "static"){
		inputCtrl(document.form.wan_dnsenable_x[0], 0);
		inputCtrl(document.form.wan_dnsenable_x[1], 0);
		
		inputCtrl(document.form.wan_pppoe_username, 0);
		inputCtrl(document.form.wan_pppoe_passwd, 0);
		inputCtrl(document.form.wan_pppoe_idletime, 0);
		inputCtrl(document.form.wan_pppoe_idletime_check, 0);
		inputCtrl(document.form.wan_pppoe_mtu, 0);
		inputCtrl(document.form.wan_pppoe_mru, 0);
		inputCtrl(document.form.wan_pppoe_service, 0);
		inputCtrl(document.form.wan_pppoe_ac, 0);
		
		// 2008.03 James. patch for Oleg's patch. {
		inputCtrl(document.form.wan_pppoe_options_x, 0);
		inputCtrl(document.form.wan_pptp_options_x, 0);
		// 2008.03 James. patch for Oleg's patch. }
		
		if(isModel3() != 'WL550gE')
			inputRCtrl1(document.form.wan_pppoe_relay_x, 1);
	}
	else{	// Automatic IP
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
		
		inputCtrl(document.form.wan_pppoe_username, 0);
		inputCtrl(document.form.wan_pppoe_passwd, 0);
		inputCtrl(document.form.wan_pppoe_idletime, 0);
		inputCtrl(document.form.wan_pppoe_idletime_check, 0);
		inputCtrl(document.form.wan_pppoe_mtu, 0);
		inputCtrl(document.form.wan_pppoe_mru, 0);
		inputCtrl(document.form.wan_pppoe_service, 0);
		inputCtrl(document.form.wan_pppoe_ac, 0);
		
		// 2008.03 James. patch for Oleg's patch. {
		inputCtrl(document.form.wan_pppoe_options_x, 0);
		inputCtrl(document.form.wan_pptp_options_x, 0);
		// 2008.03 James. patch for Oleg's patch. }
		
		if(isModel3() != 'WL550gE')
			inputRCtrl1(document.form.wan_pppoe_relay_x, 1);
	}
}

function fixed_change_wan_type(wan_type){
	var flag = false;
	
	if(!document.form.x_DHCPClient[0].checked){
		if(document.form.wan_ipaddr.value.length == 0)
			document.form.wan_ipaddr.focus();
		else if(document.form.wan_netmask.value.length == 0)
			document.form.wan_netmask.focus();
		else if(document.form.wan_gateway.value.length == 0)
			document.form.wan_gateway.focus();
		else
			flag = true;
	}
	else
		flag = true;
	
	if(wan_type == "pppoe"){
		if(wan_type == original_wan_type){
			document.form.wan_dnsenable_x[0].checked = original_dnsenable;
			document.form.wan_dnsenable_x[1].checked = !original_dnsenable;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', original_dnsenable);
			
			if(flag == true && document.form.wan_dns1_x.value.length == 0)
				document.form.wan_dns1_x.focus();
		}
		else{
			document.form.wan_dnsenable_x[0].checked = 1;
			document.form.wan_dnsenable_x[1].checked = 0;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', 0);
			
			inputCtrl(document.form.wan_dns1_x, 0);
			inputCtrl(document.form.wan_dns2_x, 0);
		}
	}
	else if(wan_type == "pptp"
			|| wan_type == "l2tp"
			){
		if(wan_type == original_wan_type){
			document.form.wan_dnsenable_x[0].checked = original_dnsenable;
			document.form.wan_dnsenable_x[1].checked = !original_dnsenable;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', original_dnsenable);
			
			if(flag == true && document.form.wan_dns1_x.value.length == 0)
				document.form.wan_dns1_x.focus();
		}
		else{
			document.form.wan_dnsenable_x[0].checked = 0;
			document.form.wan_dnsenable_x[1].checked = 1;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', 0);
			
			inputCtrl(document.form.wan_dnsenable_x[0], 0);
			inputCtrl(document.form.wan_dnsenable_x[1], 0);
		}
	}
	else if(wan_type == "static"){
		document.form.wan_dnsenable_x[0].checked = 0;
		document.form.wan_dnsenable_x[1].checked = 1;
		change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', 0);
		
		if(flag == true && document.form.wan_dns1_x.value.length == 0)
			document.form.wan_dns1_x.focus();
	}
	else{	// wan_type == "dhcp"
		
		if(wan_type == original_wan_type){
			document.form.wan_dnsenable_x[0].checked = original_dnsenable;
			document.form.wan_dnsenable_x[1].checked = !original_dnsenable;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', original_dnsenable);
			//if(flag == true && document.form.wan_dns1_x.value.length == 0 && document.form.wan_dns1_x.disabled == false)
				//document.form.wan_dns1_x.focus();
		}
		else{
			document.form.wan_dnsenable_x[0].checked = 1;
			document.form.wan_dnsenable_x[1].checked = 0;
			change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', 0);
			
			inputCtrl(document.form.wan_dns1_x, 0);
			inputCtrl(document.form.wan_dns2_x, 0);
		}
	}
}

function change_wan_dhcp_enable(flag){
	var wan_type = document.form.wan_proto.value;
	
	// 2008.03 James. patch for Oleg's patch. {
	if(wan_type == "pppoe"){
		if(flag == 1){
			if(wan_type == original_wan_type){
				document.form.x_DHCPClient[0].checked = original_wan_dhcpenable;
				document.form.x_DHCPClient[1].checked = !original_wan_dhcpenable;
			}
			else{
				document.form.x_DHCPClient[0].checked = 1;
				document.form.x_DHCPClient[1].checked = 0;
			}
		}
		
		inputCtrl(document.form.x_DHCPClient[0], 1);
		inputCtrl(document.form.x_DHCPClient[1], 1);
		
		var wan_dhcpenable = document.form.x_DHCPClient[0].checked;
		
		inputCtrl(document.form.wan_ipaddr, !wan_dhcpenable);
		inputCtrl(document.form.wan_netmask, !wan_dhcpenable);
		inputCtrl(document.form.wan_gateway, !wan_dhcpenable);
	}
	// 2008.03 James. patch for Oleg's patch. }
	else if(wan_type == "pptp"
			|| wan_type == "l2tp"
			){
		if(flag == 1){
			if(wan_type == original_wan_type){
				document.form.x_DHCPClient[0].checked = original_wan_dhcpenable;
				document.form.x_DHCPClient[1].checked = !original_wan_dhcpenable;
			}
			else{
				document.form.x_DHCPClient[0].checked = 0;
				document.form.x_DHCPClient[1].checked = 1;
			}
		}
		
		inputCtrl(document.form.x_DHCPClient[0], 1);
		inputCtrl(document.form.x_DHCPClient[1], 1);
		
		var wan_dhcpenable = document.form.x_DHCPClient[0].checked;
		
		inputCtrl(document.form.wan_ipaddr, !wan_dhcpenable);
		inputCtrl(document.form.wan_netmask, !wan_dhcpenable);
		inputCtrl(document.form.wan_gateway, !wan_dhcpenable);
	}
	else if(wan_type == "static"){
		document.form.x_DHCPClient[0].checked = 0;
		document.form.x_DHCPClient[1].checked = 1;
		
		inputCtrl(document.form.x_DHCPClient[0], 0);
		inputCtrl(document.form.x_DHCPClient[1], 0);
		
		inputCtrl(document.form.wan_ipaddr, 1);
		inputCtrl(document.form.wan_netmask, 1);
		inputCtrl(document.form.wan_gateway, 1);
	}
	else{	// wan_type == "dhcp" && "pppoe"
		document.form.x_DHCPClient[0].checked = 1;
		document.form.x_DHCPClient[1].checked = 0;
		
		inputCtrl(document.form.x_DHCPClient[0], 0);
		inputCtrl(document.form.x_DHCPClient[1], 0);
		
		inputCtrl(document.form.wan_ipaddr, 0);
		inputCtrl(document.form.wan_netmask, 0);
		inputCtrl(document.form.wan_gateway, 0);
	}
	
	if(document.form.x_DHCPClient[0].checked){
		inputCtrl(document.form.wan_dnsenable_x[0], 1);
		inputCtrl(document.form.wan_dnsenable_x[1], 1);
	}
	else{
		document.form.wan_dnsenable_x[0].checked = 0;
		document.form.wan_dnsenable_x[1].checked = 1;
		change_common_radio(document.form.wan_dnsenable_x, 'IPConnection', 'wan_dnsenable_x', 0);
		
		inputCtrl(document.form.wan_dnsenable_x[0], 0);
		inputCtrl(document.form.wan_dnsenable_x[1], 0);
	}
}
</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(7, 19);return unload_body();">
<script>
	if(sw_mode != 1){
		alert("<#page_not_support_mode_hint#>");
		location.href = "/as.asp";
	}
</script>
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log", "productid"); %>">
<input type="hidden" name="support_cdma" value="<% nvram_get_x("IPConnection", "support_cdma"); %>">

<input type="hidden" name="current_page" value="Advanced_WAN_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;LANHostConfig;IPConnection;PPPConnection;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<input type="hidden" name="wan_pppoe_txonly_x" value="<% nvram_get_x("PPPConnection","wan_pppoe_txonly_x"); %>" />

<input type="hidden" name="lan_ipaddr" value="<% nvram_get_x("LANHostConfig", "lan_ipaddr"); %>" />
<input type="hidden" name="lan_netmask" value="<% nvram_get_x("LANHostConfig", "lan_netmask"); %>" />

<table border="0" class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="23">&nbsp;</td>
	
	<!--=====Beginning of Main Menu=====-->
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
	<td height="430" valign="top">
	  <div id="tabMenu" class="submenuBlock"></div><br>
	  
	  <!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td align="left" valign="top">
			<table width="98%" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle">
				<thead>
				<tr>
					<td><#menu5_3#> - <#menu5_3_1#></td>
				</tr>
				</thead>
				
				<tbody>
				<tr>
					<td bgcolor="#FFFFFF"><#Layer3Forwarding_x_ConnectionType_sectiondesc#></td>
				</tr>
				</tbody>	
				
				<tr>
					<th bgcolor="#FFFFFF">
						<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
							<tr>
								<th width="30%"><#Layer3Forwarding_x_ConnectionType_itemname#></th>
								<td  align="left">
									<select id="wan_proto_menu" class="input" name="wan_proto" onchange="change_wan_type(this.value);fixed_change_wan_type(this.value);">
										<option value="dhcp" <% nvram_match_x("Layer3Forwarding", "wan_proto", "dhcp", "selected"); %>>Automatic IP</option>
										<option value="pppoe" <% nvram_match_x("Layer3Forwarding", "wan_proto", "pppoe", "selected"); %>>PPPoE</option>
										<option value="pptp" <% nvram_match_x("Layer3Forwarding", "wan_proto", "pptp", "selected"); %>>PPTP</option>
										<option value="l2tp" <% nvram_match_x("Layer3Forwarding", "wan_proto", "l2tp", "selected"); %>>L2TP</option>
										<option value="static" <% nvram_match_x("Layer3Forwarding", "wan_proto", "static", "selected"); %>>Static IP</option>
									</select>
									<span style="font-weight:normal;" id="wan_proto_hint"><span style="color:#000;font-size:14px;">3G/3.5G</span><br/>若需要更改連線類型，請先至<a href="/Advanced_HSDPA_others.asp">HSDPA設定</a>停用3G/3.5G網卡</span>
								</td>
							</tr>
							<tr>
								<th width="30%"><#Layer3Forwarding_x_STB_itemname#></th>
								<td align="left">
									<select name="wan_stb_x" class="input">
										<option value="0" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "0", "selected"); %>>None</option>
										<option value="1" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "1", "selected"); %>>LAN1</option>
										<option value="2" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "2", "selected"); %>>LAN2</option>
										<option value="3" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "3", "selected"); %>>LAN3</option>
										<option value="4" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "4", "selected"); %>>LAN4</option>
										<option value="5" <% nvram_match_x("Layer3Forwarding", "wan_stb_x", "5", "selected"); %>>LAN3 & LAN4</option>
									</select>
								</td>
							</tr>
							<tr>
								<th width="30%"><#BasicConfig_EnableMediaServer_itemname#></th>                 
								<td style="font-weight:normal;" align="left">
									<input type="radio" name="upnp_enable"  class="input" value="1" onclick="return change_common_radio(this, 'LANHostConfig', 'upnp_enable', '1')" <% nvram_match_x("LANHostConfig","upnp_enable", "1", "checked"); %>>Yes
									<input type="radio" name="upnp_enable"  class="input" value="0" onclick="return change_common_radio(this, 'LANHostConfig', 'upnp_enable', '0')" <% nvram_match_x("LANHostConfig","upnp_enable", "0", "checked"); %>>No
								</td>
							</tr>
							
						</table>
					</td>
				</tr>	
				
				<tr>
					<td bgcolor="#FFFFFF" id="ip_sect">
						<table  width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
							<thead>
							<tr>
								<td colspan="2"><#IPConnection_ExternalIPAddress_sectionname#></td>
							</tr>
							</thead>
							
							<tr>
								<th width="30%"><#Layer3Forwarding_x_DHCPClient_itemname#></th>
								<td>
									<input type="radio" name="x_DHCPClient" class="input" value="1" onclick="change_wan_dhcp_enable(0);" <% nvram_match_x("Layer3Forwarding", "x_DHCPClient", "1", "checked"); %>>Yes
									<input type="radio" name="x_DHCPClient" class="input" value="0" onclick="change_wan_dhcp_enable(0);" <% nvram_match_x("Layer3Forwarding", "x_DHCPClient", "0", "checked"); %>>No
								</td>
							</tr>
							
							<tr>
								<th width="30%"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,1);"><#IPConnection_ExternalIPAddress_itemname#></a></th>
								<td><input type="text" name="wan_ipaddr" maxlength="15" class="input" size="15" value="<% nvram_get_x("IPConnection","wan_ipaddr"); %>" onKeyPress="return is_ipaddr(this);" onKeyUp="change_ipaddr(this);"></td>
							</tr>
							
							<tr>
								<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,2);"><#IPConnection_x_ExternalSubnetMask_itemname#></a></th>
								<td><input type="text" name="wan_netmask" maxlength="15" class="input" size="15" value="<% nvram_get_x("IPConnection","wan_netmask"); %>" onKeyPress="return is_ipaddr(this);" onKeyUp="change_ipaddr(this);"></td>
							</tr>
							
							<tr>
								<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,3);"><#IPConnection_x_ExternalGateway_itemname#></a></th>
								<td><input type="text" name="wan_gateway" maxlength="15" class="input" size="15" value="<% nvram_get_x("IPConnection","wan_gateway"); %>" onKeyPress="return is_ipaddr(this);" onKeyUp="change_ipaddr(this);"></td>
							</tr>
						</table>
					</td>
	  		</tr>
	  		<tr>
	    		<td bgcolor="#FFFFFF" id="dns_sect">
						<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
          		<thead>
            	<tr>
              <td colspan="2"><#IPConnection_x_DNSServerEnable_sectionname#></td>
            	</tr>
          		</thead>
         			<tr>
            		<th width="30%"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,12);"><#IPConnection_x_DNSServerEnable_itemname#></a></th>
								<td>
			  					<input type="radio" name="wan_dnsenable_x" value="1" onclick="return change_common_radio(this, 'IPConnection', 'wan_dnsenable_x', 1)" <% nvram_match_x("IPConnection", "wan_dnsenable_x", "1", "checked"); %> />Yes
			  					<input type="radio" name="wan_dnsenable_x" value="0" onclick="return change_common_radio(this, 'IPConnection', 'wan_dnsenable_x', 0)" <% nvram_match_x("IPConnection", "wan_dnsenable_x", "0", "checked"); %> />No
								</td>
          		</tr>
          		<tr>
            		<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,13);"><#IPConnection_x_DNSServer1_itemname#></a></th>
            		<td><input type="text" maxlength="15" class="input" size="15" name="wan_dns1_x" value="<% nvram_get_x("IPConnection","wan_dns1_x"); %>" onkeypress="return is_ipaddr(this)" onkeyup="change_ipaddr(this)" /></td>
          		</tr>
          		<tr>
            		<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,14);"><#IPConnection_x_DNSServer2_itemname#></a></th>
            		<td><input type="text" maxlength="15" class="input" size="15" name="wan_dns2_x" value="<% nvram_get_x("IPConnection","wan_dns2_x"); %>" onkeypress="return is_ipaddr(this)" onkeyup="change_ipaddr(this)" /></td>
          		</tr>
        		</table>
        	</td>
	  		</tr>
	  
	  		<tr>
	    		<td bgcolor="#FFFFFF" id="account_sect">
		  			<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
            	<thead>
            	<tr>
              	<td colspan="2"><#PPPConnection_UserName_sectionname#></td>
            	</tr>
            	</thead>
            	<tr>
              	<th width="30%"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,4);"><#PPPConnection_UserName_itemname#></a></th>
              	<td><input type="text" maxlength="64" class="input" size="32" name="wan_pppoe_username" value="<% nvram_get_x("PPPConnection","wan_pppoe_username"); %>" onkeypress="return is_string(this)"></td>
            	</tr>
            	<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,5);"><#PPPConnection_Password_itemname#></a></th>
              	<td><input type="password" maxlength="64" class="input" size="32" name="wan_pppoe_passwd" value="<% nvram_get_x("PPPConnection","wan_pppoe_passwd"); %>"></td>
            	</tr>
							<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,6);"><#PPPConnection_IdleDisconnectTime_itemname#></a></th>
              	<td>
                	<input type="text" maxlength="10" class="input" size="10" name="wan_pppoe_idletime" value="<% nvram_get_x("PPPConnection","wan_pppoe_idletime"); %>" onkeypress="return is_number(this)" />
                	<input type="checkbox" style="margin-left:30" name="wan_pppoe_idletime_check" value="" onclick="return change_common_radio(this, 'PPPConnection', 'wan_pppoe_idletime', '1')" />Tx Only
              	</td>
            	</tr>
            	<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,7);"><#PPPConnection_x_PPPoEMTU_itemname#></a></th>
              	<td><input type="text" maxlength="5" size="5" name="wan_pppoe_mtu" class="input" value="<% nvram_get_x("PPPConnection", "wan_pppoe_mtu"); %>" onkeypress="return is_number(this)" /></td>
            	</tr>
            	<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,8);"><#PPPConnection_x_PPPoEMRU_itemname#></a></th>
              	<td><input type="text" maxlength="5" size="5" name="wan_pppoe_mru" class="input" value="<% nvram_get_x("PPPConnection", "wan_pppoe_mru"); %>" onkeypress="return is_number(this)" /></td>
            	</tr>
            	<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,9);"><#PPPConnection_x_ServiceName_itemname#></a></th>
              	<td><input type="text" maxlength="32" class="input" size="32" name="wan_pppoe_service" value="<% nvram_get_x("PPPConnection","wan_pppoe_service"); %>" onkeypress="return is_string(this)" /></td>
            	</tr>
            	<tr>
              	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,10);"><#PPPConnection_x_AccessConcentrator_itemname#></a></th>
              	<td><input type="text" maxlength="32" class="input" size="32" name="wan_pppoe_ac" value="<% nvram_get_x("PPPConnection","wan_pppoe_ac"); %>" onkeypress="return is_string(this)" /></td>
            	</tr>
            <!-- 2008.03 James. patch for Oleg's patch. { -->
						<tr>
							<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,17);"><#PPPConnection_x_PPTPOptions_itemname#></a></th>
							<td>
								<select name="wan_pptp_options_x" class="input">
									<option value="" <% nvram_match_x("Layer3Forwarding","wan_pptp_options_x", "","selected"); %>>None</option>
									<option value="-mppc" <% nvram_match_x("Layer3Forwarding","wan_pptp_options_x", "-mppc","selected"); %>>No Encryption</option>
									<option value="+mppe-40" <% nvram_match_x("Layer3Forwarding","wan_pptp_options_x", "+mppe-40","selected"); %>>MPPE 40</option>
									<!--option value="+mppe-56" <% nvram_match_x("Layer3Forwarding","wan_pptp_options_x", "+mppe-56","selected"); %>>MPPE 56</option-->
									<option value="+mppe-128" <% nvram_match_x("Layer3Forwarding","wan_pptp_options_x", "+mppe-128","selected"); %>>MPPE 128</option>
								</select>
							</td>
						</tr>

						<tr>
							<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,18);"><#PPPConnection_x_AdditionalOptions_itemname#></a></th>
							<td>
								<input type="text" name="wan_pppoe_options_x" value="<% nvram_get_x("PPPConnection", "wan_pppoe_options_x"); %>" class="input" maxlength="255" size="32" onKeyPress="return is_string(this)" onBlur="validate_string(this)"></td>
						</tr>
						<!-- 2008.03 James. patch for Oleg's patch. } -->

            <tr>
              <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,11);"><#PPPConnection_x_PPPoERelay_itemname#></a></th>
              <td>
              	<input type="radio" value="1" name="wan_pppoe_relay_x" class="input" onclick="return change_common_radio(this, 'PPPConnection', 'wan_pppoe_relay_x', '1')" <% nvram_match_x("PPPConnection","wan_pppoe_relay_x", "1", "checked"); %> />Yes
                <input type="radio" value="0" name="wan_pppoe_relay_x" class="input" onclick="return change_common_radio(this, 'PPPConnection', 'wan_pppoe_relay_x', '0')" <% nvram_match_x("PPPConnection","wan_pppoe_relay_x", "0", "checked"); %> />No
              </td>
            </tr>
          </table>
        </td>
	  </tr>
	  
	  <tr>
	    <td bgcolor="#FFFFFF" id="isp_sect">
		<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
			<thead>
				<tr>
            <td colspan="2"><#PPPConnection_x_HostNameForISP_sectionname#></td>
            </tr>
		</thead>
				
				<tr>
          <th width="30%"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,19);"><#PPPConnection_x_HeartBeat_itemname#></a></th>
          <td>
          	<!-- 2008.03 James. patch for Oleg's patch. { -->
          	<input type="text" name="wan_heartbeat_x" class="input" maxlength="256" size="32" value="<% nvram_get_x("PPPConnection","wan_heartbeat_x"); %>" onKeyPress="return is_string(this)"></td>
          	<!-- 2008.03 James. patch for Oleg's patch. } -->
        </tr>
        <tr>
          <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,15);"><#PPPConnection_x_HostNameForISP_itemname#></a></th>
          <td><input type="text" name="wan_hostname" class="input" maxlength="32" size="32" value="<% nvram_get_x("PPPConnection","wan_hostname"); %>" onkeypress="return is_string(this)"></td>
        </tr>
        <tr>
          <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(7,16);"><#PPPConnection_x_MacAddressForISP_itemname#></a></th>
          <td><input type="text" name="wan_hwaddr_x" class="input" maxlength="12" size="12" value="<% nvram_get_x("PPPConnection","wan_hwaddr_x"); %>" onKeyPress="return is_hwaddr()"></td>
        </tr>
        
      </table>
      </td>
	</tr>
	<tr>
	  <td bgcolor="#FFFFFF">
		
		<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
			<tr align="right">
				<td align="right"><input class="button" onclick="applyRule();" type="button" value="<#CTL_apply#>"/></td>
			</tr>
		</table>
		</td>
	</tr>
</table>
</td>
</form>

					<td id="help_td" style="width:15px;" valign="top">
						<form name="hint_form"></form>
            <div id="helpicon" onClick="openHint(0,0);" title="<#Help_button_default_hint#>">
            	<img src="images/help.gif">
            </div>
						<div id="hintofPM" style="display:none;">
							<table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
								<thead>
								<tr>
									<td>
										<div id="helpname" class="AiHintTitle"></div>
										<a href="javascript:void(0);" onclick="closeHint()">
											<img src="images/button-close.gif" class="closebutton">
										</a>
									</td>
								</tr>
								</thead>
								
								<tr>
									<td valign="top" >
										<div class="hint_body2" id="hint_body"></div>
										<iframe id="statusframe" name="statusframe" class="statusframe" src="" frameborder="0"></iframe>
									</td>
								</tr>
							</table>
						</div>
					</td>
				</tr>
			</table>
		</td>
		<!--===================================Ending of Main Content===========================================-->
	
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>

</body>
</html>
