<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title></title>
<link href="/NM_style.css" rel="stylesheet" type="text/css" />
<link href="/form_style.css" rel="stylesheet" type="text/css" />

<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="formcontrol.js"></script>
<script type="text/javascript" src="/ajax.js"></script>
<script>
var had_wrong_wep_key = false;

function initial(){
	
	flash_button();
	loadXML();
	
	document.form.wl_ssid.value = decodeURIComponent(document.form.wl_ssid2.value);
	document.form.wl_wpa_psk.value = decodeURIComponent(document.form.wl_wpa_psk_org.value);
	document.form.wl_key1.value = decodeURIComponent(document.form.wl_key1_org.value);
	document.form.wl_key2.value = decodeURIComponent(document.form.wl_key2_org.value);
	document.form.wl_key3.value = decodeURIComponent(document.form.wl_key3_org.value);
	document.form.wl_key4.value = decodeURIComponent(document.form.wl_key4_org.value);
	
	if(document.form.wl_wpa_psk.value.length <= 0)
		document.form.wl_wpa_psk.value = "Please type Password";
	
	wl_auth_mode_change(1);
	
	if(sw_mode == "2"){
		UIunderRepeater();
	}
	if(parent.wan_proto == "3g"){
		$("wps_pbc_tr").style.display = "none";
		$("Router_domore").remove(2);
		stopFlag=1;
	}
	
	show_LAN_info();
	domore_create();
}
function UIunderRepeater(){
	stopFlag=1;
	$("apply_tr").style.display = "none";
	$("wl_radio_tr").style.display = "none";
	$("wps_pbc_tr").style.display = "none";
		
	document.form.wl_ssid.readOnly = true;
	document.form.wl_auth_mode.disabled = true;
	document.form.wl_wep_x.disabled = true;
	document.form.wl_key.disabled = true;
	document.form.wl_asuskey1.readOnly = true;
	document.form.wl_wpa_psk.readOnly = true;
	document.form.wl_crypto.disabled = true;
		
	$("sta_ssid").className = "inputinfo";
	$("sta_asuskey1").className = "inputinfo";
	$("sta_wpa_psk").className = "inputinfo";
}


function domore_create(){
	var option_AP = new Array();
	option_AP[0] = document.createElement("option");
	option_AP[0].text = "<#menu5_7_4#>";
	option_AP[0].value = "../Main_WStatus_Content.asp"
	
	if(sw_mode == "2"){
		$("Router_domore").remove(6);
		$("Router_domore").remove(5);
		$("Router_domore").remove(4);
		$("Router_domore").options[3].value="../Advanced_FirmwareUpgrade_Content.asp";
		$("Router_domore").options[3].text="<#menu5_6_3#>";
		$("Router_domore").remove(2);
		$("Router_domore").options[1].value="../Advanced_WAdvanced_Content.asp";
		$("Router_domore").options[1].text="<#menu5_1#> - <#menu5_1_6#>";
		try{
    	$("Router_domore").add(option_AP[0],null);
    }
  	catch(ex){
    	$("Router_domore").add(option_AP[0],3);
    }
	}
	else if(parent.wan_route_x == "IP_Bridged" || sw_mode == "3"){
		$("Router_domore").remove(6);
		$("Router_domore").remove(5);
		$("Router_domore").remove(4);
		$("Router_domore").options[3].value="../Advanced_APLAN_Content.asp";
		try{
    	$("Router_domore").add(option_AP[0],null);
    }
  	catch(ex){
    	$("Router_domore").add(option_AP[0],4);
    }
	}
}

function wl_auth_mode_change(isload){
	var mode = document.form.wl_auth_mode.value;
	var opts = document.form.wl_auth_mode.options;
	var new_array;
	var cur_crypto;
	var cur_key_index, cur_key_obj;
	
	//if(mode == "open" || mode == "shared" || mode == "radius"){ //2009.03 magic
	if(mode == "open" || mode == "shared"){ //2009.03 magic
		blocking("all_related_wep", 1);
		change_wep_type(mode);
	}
	else{
		blocking("all_related_wep", 0);
	}
	
	/* enable/disable crypto algorithm */
	if(mode == "wpa" || mode == "wpa2" || mode == "psk")
		blocking("wl_crypto", 1);
	else
		blocking("wl_crypto", 0);
	
	/* enable/disable psk passphrase */
	if(mode == "psk")
		blocking("wl_wpa_psk", 1);
	else
		blocking("wl_wpa_psk", 0);
	
	/* update wl_crypto */
	for(var i = 0; i < document.form.wl_crypto.length; ++i)
		if(document.form.wl_crypto[i].selected){
			cur_crypto = document.form.wl_crypto[i].value;
			break;
		}
	
	/* Reconstruct algorithm array from new crypto algorithms */
	if(mode == "psk"){
		/* Save current crypto algorithm */
		if(isModel() == "SnapAP" || isBand() == 'b')
			new_array = new Array("TKIP");
		else{
			if(opts[opts.selectedIndex].text == "WPA-Personal")
				new_array = new Array("TKIP");
			else if(opts[opts.selectedIndex].text == "WPA2-Personal")
				new_array = new Array("AES");
			else
				new_array = new Array("TKIP", "AES", "TKIP+AES");
		}
		
		free_options(document.form.wl_crypto);
		for(var i in new_array){
			document.form.wl_crypto[i] = new Option(new_array[i], new_array[i].toLowerCase());
			document.form.wl_crypto[i].value = new_array[i].toLowerCase();
			if(new_array[i].toLowerCase() == cur_crypto)
				document.form.wl_crypto[i].selected = true;
		}
	}
	else if(mode == "wpa"){
		if(opts[opts.selectedIndex].text == "WPA-Enterprise")
			new_array = new Array("TKIP");
		else
			new_array = new Array("TKIP", "AES", "TKIP+AES");
		
		free_options(document.form.wl_crypto);
		for(var i in new_array){
			document.form.wl_crypto[i] = new Option(new_array[i], new_array[i].toLowerCase());
			document.form.wl_crypto[i].value = new_array[i].toLowerCase();
			if(new_array[i].toLowerCase() == cur_crypto)
				document.form.wl_crypto[i].selected = true;
		}
	}
	else if(mode == "wpa2"){
		new_array = new Array("AES");
		
		free_options(document.form.wl_crypto);
		for(var i in new_array){
			document.form.wl_crypto[i] = new Option(new_array[i], new_array[i].toLowerCase());
			document.form.wl_crypto[i].value = new_array[i].toLowerCase();
			if(new_array[i].toLowerCase() == cur_crypto)
				document.form.wl_crypto[i].selected = true;
		}
	}
	
	/* Save current network key index */
	for(var i = 0; i < document.form.wl_key.length; ++i)
		if(document.form.wl_key[i].selected){
			cur_key_index = document.form.wl_key[i].value;
			break;
		}
	
	/* Define new network key indices */
	//if(mode == "psk" || mode == "wpa" || mode == "wpa2" || mode == "radius")
	if(mode == "psk" || mode == "wpa" || mode == "wpa2")
		new_array = new Array("2", "3");
	else{
		new_array = new Array("1", "2", "3", "4");
		
		if(!isload)
			cur_key_index = "1";
	}
	
	/* Reconstruct network key indices array from new network key indices */
	free_options(document.form.wl_key);
	for(var i in new_array){
		document.form.wl_key[i] = new Option(new_array[i], new_array[i]);
		document.form.wl_key[i].value = new_array[i];
		if(new_array[i] == cur_key_index)
			document.form.wl_key[i].selected = true;
	}
	
	wl_wep_change();
}

function change_wep_type(mode){
	var cur_wep = document.form.wl_wep_x.value;
	var wep_type_array;
	var value_array;
	
	free_options(document.form.wl_wep_x);
	
	//if(mode == "shared" || mode == "radius"){ //2009.03 magic
	if(mode == "shared"){ //2009.03 magic
		wep_type_array = new Array("WEP-64bits", "WEP-128bits");
		value_array = new Array("1", "2");
	}
	else{
		wep_type_array = new Array("None", "WEP-64bits", "WEP-128bits");
		value_array = new Array("0", "1", "2");
	}
	
	add_options_x2(document.form.wl_wep_x, wep_type_array, value_array, cur_wep);
	
	if(mode == "psk" || mode == "wpa" || mode == "wpa2") //2009.03 magic
	//if(mode == "psk" || mode == "wpa" || mode == "wpa2" || mode == "radius") //2009.03 magic
		document.form.wl_wep_x.value = "0";
	
	change_wlweptype(document.form.wl_wep_x);
}

function change_wlweptype(wep_type_obj){
	var mode = document.form.wl_auth_mode.value; //2009.03 magic
	
	//if(wep_type_obj.value == "0" || mode == "radius") //2009.03 magic
	if(wep_type_obj.value == "0")  //2009.03 magic
		blocking("all_wep_key", 0);
	else
		blocking("all_wep_key", 1);
	
	wl_wep_change();
}

function wl_wep_change(){
	var mode = document.form.wl_auth_mode.value;
	var wep = document.form.wl_wep_x.value;
	
	if(mode == "psk" || mode == "wpa" || mode == "wpa2"){
		if(mode == "psk"){
			blocking("wl_crypto", 1);
			blocking("wl_wpa_psk", 1);
		}
		
		blocking("all_related_wep", 0);
	}
	else{
		blocking("wl_crypto", 0);
		blocking("wl_wpa_psk", 0);
		
		//if(mode == "radius") //2009.03 magic
		//	blocking("all_related_wep", 0); //2009.03 magic
		//else //2009.03 magic
		//	blocking("all_related_wep", 1);
		
		if(wep == "0" || mode == "radius")
			blocking("all_wep_key", 0);
		else{
			blocking("all_wep_key", 1);
			
			show_key();
		}
	}
	
	change_key_des();
}

function change_key_des(){
	var objs = getElementsByName_iefix("span", "key_des");
	var wep_type = document.form.wl_wep_x.value;
	var str = "";
	
	if(wep_type == "1")
		str = " (<#WLANConfig11b_WEPKey_itemtype1#>)";
	else if(wep_type == "2")
		str = " (<#WLANConfig11b_WEPKey_itemtype2#>)";
	
	str += ":";
	
	for(var i = 0; i < objs.length; ++i)
		showtext(objs[i], str);
}

function change_auth_mode(auth_mode_obj){
	wl_auth_mode_change(0);
	
	if(auth_mode_obj.value == "psk" || auth_mode_obj.value == "wpa"){
		var opts = document.form.wl_auth_mode.options;
		
		if(opts[opts.selectedIndex].text == "WPA-Personal")
			document.form.wl_wpa_mode.value = "1";
		else if(opts[opts.selectedIndex].text == "WPA2-Personal")
			document.form.wl_wpa_mode.value="2";
		else if(opts[opts.selectedIndex].text == "WPA-Auto-Personal")
			document.form.wl_wpa_mode.value="0";
		else if(opts[opts.selectedIndex].text == "WPA-Enterprise")
			document.form.wl_wpa_mode.value="3";
		else if(opts[opts.selectedIndex].text == "WPA-Auto-Enterprise")
			document.form.wl_wpa_mode.value = "4";
		
		if(auth_mode_obj.value == "psk"){
			document.form.wl_wpa_psk.focus();
			document.form.wl_wpa_psk.select();
		}
	}
	else if(auth_mode_obj.value == "shared")
		show_key();	
	//else if(auth_mode_obj.value == "shared" || auth_mode_obj.value == "radius")
}

function show_key(){
	var wep_type = document.form.wl_wep_x.value;
	var keyindex = document.form.wl_key.value;
	var cur_key_obj = eval("document.form.wl_key"+keyindex);
	var cur_key_length = cur_key_obj.value.length;
	
	if(wep_type == 1){
		if(cur_key_length == 5 || cur_key_length == 10)
			document.form.wl_asuskey1.value = cur_key_obj.value;
		else
			document.form.wl_asuskey1.value = "0000000000";
	}
	else if(wep_type == 2){
		if(cur_key_length == 13 || cur_key_length == 26)
			document.form.wl_asuskey1.value = cur_key_obj.value;
		else
			document.form.wl_asuskey1.value = "00000000000000000000000000";
	}
	else
		document.form.wl_asuskey1.value = "";
	
	document.form.wl_asuskey1.focus();
	document.form.wl_asuskey1.select();
}

function show_LAN_info(){
	var lan_ipaddr_t = '<% nvram_get_x("LANHostConfig", "lan_ipaddr_t"); %>';
	if(lan_ipaddr_t != '')
		showtext($("LANIP"), '<% nvram_get_x("LANHostConfig", "lan_ipaddr_t"); %>');
	else	
		showtext($("LANIP"), '<% nvram_get_x("LANHostConfig", "lan_ipaddr"); %>');
	showtext($("PINCode"), '<% nvram_get_x("", "secret_code"); %>');
	showtext($("MAC"), '<% nvram_get_x("", "il0macaddr"); %>');
}

function show_wepkey_help(){
	if(document.form.wl_wep_x.value == 1)
		parent.showHelpofDrSurf(0, 12);
	else if(document.form.wl_wep_x.value == 2)
		parent.showHelpofDrSurf(0, 13);
}

var secs;
var timerID = null;
var timerRunning = false;
var timeout = 1000;
var delay = 500;
var stopFlag=0;

function resetTimer()
{
	if (stopFlag==1)
	{
		stopFlag=0;
		InitializeTimer();
	}
}

function InitializeTimer()
{
	if(document.form.wl_auth_mode.value == "shared"
		|| document.form.wl_auth_mode.value == "wpa"
		|| document.form.wl_auth_mode.value == "wpa2"
		|| document.form.wl_auth_mode.value == "radius")
		return;

	msecs = timeout;
	StopTheClock();
	StartTheTimer();
}

function StopTheClock()
{
	if(timerRunning)
		clearTimeout(timerID);
	timerRunning = false;
}

function StartTheTimer(){
	if(msecs == 0){
		StopTheClock();
		
		if(stopFlag == 1)
			return;
		
		updateWPS();
		msecs = timeout;
		StartTheTimer();
	}
	else{
		msecs = msecs-500;
		timerRunning = true;
		timerID = setTimeout("StartTheTimer();", delay);
	}
}

function updateWPS()
{
	var ie = window.ActiveXObject;

	if (ie)
		makeRequest_ie('/WPS_info.asp');
	else
		makeRequest('/WPS_info.asp');
}

function loadXML()
{
	updateWPS();
	InitializeTimer();
}

function refresh_wpsinfo(xmldoc){
	var wpss = xmldoc.getElementsByTagName("wps");
	
	if(wpss==null || wpss[0]==null){
		stopFlag=1;
		return;
	}
	
	var wps_infos = wpss[0].getElementsByTagName("wps_info");
	
	if(wps_infos[0].firstChild.nodeValue == "Start WPS Process"){
		$("WPS_PBCbutton_hint_span").innerHTML = "<#WPS_PBCbutton_hint_waiting#>";
		if($("wpsPBC_button").src != "/images/EZSetup_button_s.gif")
			$("wpsPBC_button").src = "/images/EZSetup_button_s.gif";
		
		PBC_shining();
	}
	else{
		$("WPS_PBCbutton_hint_span").innerHTML = "<#WPS_PBCbutton_hint_timeout#>";
		PBC_normal();
	}
	
	/*if(wps_infos[9].firstChild.nodeValue == "0"){
		$("WPS_PBCbutton_hint_span").innerHTML = "<a href='../Advanced_WWPS_Content.asp' target='_parent'><#WPS_PBCbutton_hint_notEnable#></a>"
		$("wpsPBC_button").title = "<#WPS_PBCbutton_hint_notEnable#>";
	}
	else{
		if(wps_infos[2].firstChild){
			if (wps_infos[2].firstChild.nodeValue == "1"){
				$("WPS_PBCbutton_hint_span").innerHTML = "<#WPS_PBCbutton_hint_waiting#>";
				if($("wpsPBC_button").src != "/images/EZSetup_button_s.gif")
					$("wpsPBC_button").src = "/images/EZSetup_button_s.gif";
			}
			else if (wps_infos[2].firstChild.nodeValue == "2"){
				$("WPS_PBCbutton_hint_span").innerHTML = "<#WPS_PBCbutton_hint_success#>";
				PBC_normal();
			}
		}
		else{
			$("WPS_PBCbutton_hint_span").innerHTML = "<#WPS_PBCbutton_hint_timeout#>";
			PBC_normal();
		}
	}//*/
}

function PBC_shining(){
	$("wpsPBC_button").src = "/images/EZSetup_button_s.gif";
	$("wpsPBC_button").onmousedown = function(){};
}

function PBC_normal(){
	$("wpsPBC_button").src = "/images/EZSetup_button.gif";
	$("wpsPBC_button").onmousedown = function(){
			$("wpsPBC_button").src = "/images/EZSetup_button_0.gif";
		};
}

function submitForm(){
	var auth_mode = document.form.wl_auth_mode.value;
		
	if(!validate_string_ssid(document.form.wl_ssid))
		return false;
	
	if(auth_mode == "psk" || auth_mode == "wpa" || auth_mode == "wpa2"){
		if(!validate_psk(document.form.wl_wpa_psk))
			return false;
	}
	else{
		if(!validate_wlkey(document.form.wl_asuskey1))
			return false;
	}
	
	stopFlag = 1;
	
	document.form.current_page.value = "";
	document.form.next_page.value = "/";
	document.form.action_mode.value = " Apply ";
	
	var wep11 = eval('document.form.wl_key'+document.form.wl_key.value);
	wep11.value = document.form.wl_asuskey1.value;
	
	if((auth_mode == "shared" || auth_mode == "wpa" || auth_mode == "wpa2" || auth_mode == "radius")
			&& document.form.wps_enable.value == "1"){
		document.form.wps_enable.value = "0";
		document.form.action_script.value = "WPS_apply";
	}
	document.form.wsc_config_state.value = "1";
	
	if(auth_mode == "wpa" || auth_mode == "wpa2" || auth_mode == "radius"){
		document.form.target = "";
		document.form.next_page.value = "/Advanced_WSecurity_Content.asp";
	}
	
	parent.showLoading();
	document.form.submit();
	
	return true;
}

function startPBCmethod(){
	stopFlag = 1;
	
	document.WPSForm.action_script.value = "WPS_push_button";
	document.WPSForm.current_page.value = "/";
	document.WPSForm.submit();
}

function wpsPBC(obj){
	obj.src = "/images/EZSetup_button_0.gif";
	
	startPBCmethod();
}
</script>
</head>

<body class="statusbody" onload="initial();">
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="form" action="/start_apply2.htm">
<input type="hidden" name="current_page" value="">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="sid_list" value="WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="productid" value="<% nvram_get_x("",  "productid"); %>">

<input type="hidden" name="wps_enable" value="<% nvram_get_x("WLANConfig11b", "wps_enable"); %>">
<input type="hidden" name="wsc_config_state" value="<% nvram_get_x("WLANConfig11b", "wsc_config_state"); %>">
<input type="hidden" name="wl_wpa_mode" value="<% nvram_get_x("WLANConfig11b", "wl_wpa_mode"); %>">
<input type="hidden" name="wl_key1" value="">
<input type="hidden" name="wl_key2" value="">
<input type="hidden" name="wl_key3" value="">
<input type="hidden" name="wl_key4" value="">

<input type="hidden" name="wl_ssid2" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_ssid"); %>">
<input type="hidden" name="wl_wpa_psk_org" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_wpa_psk"); %>">
<input type="hidden" name="wl_key_type" value="<% nvram_get_x("WLANConfig11b","wl_key_type"); %>"><!--Lock Add 1125 for ralink platform-->
<input type="hidden" name="wl_key1_org" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_key1"); %>">
<input type="hidden" name="wl_key2_org" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_key2"); %>">
<input type="hidden" name="wl_key3_org" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_key3"); %>">
<input type="hidden" name="wl_key4_org" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_key4"); %>">

<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr>
    <th width="95"><#Wireless_name#>(SSID)</th>
    <td>
      <input id="sta_ssid" type="text" name="wl_ssid" onfocus="parent.showHelpofDrSurf(0, 1);" value="<% nvram_get_x("WLANConfig11b", "wl_ssid"); %>" maxlength="32" size="22" class="input"/>
    </td>
  </tr>
  <tr>
    <th width="95"><#WLANConfig11b_AuthenticationMethod_itemname#></th>
    <td>
      <select name="wl_auth_mode" class="input" onfocus="parent.showHelpofDrSurf(0, 5);" onchange="change_auth_mode(this);">
		<option value="open" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "open","selected"); %>>Open System</option>
		<option value="shared" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "shared","selected"); %>>Shared Key</option>
		<option value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "1", "selected"); %>>WPA-Personal</option>
		<option value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "2", "selected"); %>>WPA2-Personal</option>
		<option value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "0", "selected"); %>>WPA-Auto-Personal</option>
		<option value="wpa" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "wpa", "WLANConfig11b", "wl_wpa_mode", "3", "selected"); %>>WPA-Enterprise</option>
		<option value="wpa2" <% nvram_match_x("WLANConfig11b", "wl_auth_mode", "wpa2", "selected"); %>>WPA2-Enterprise</option>
		<option value="wpa" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "wpa", "WLANConfig11b", "wl_wpa_mode", "4", "selected"); %>>WPA-Auto-Enterprise</option>
		<option value="radius" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "radius","selected"); %>>Radius with 802.1x</option>
	  </select>
    </td>
  </tr>
</table>

<div id='all_related_wep' style='display:none;'>
<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr>
	<th width="95"><#WLANConfig11b_WEPType_itemname#></th>
	<td>
	  <select name="wl_wep_x" id="wl_wep_x" class="input" onfocus="parent.showHelpofDrSurf(0, 9);" onchange="change_wlweptype(this);">
		<option value="0" <% nvram_match_x("WLANConfig11b", "wl_wep_x", "0", "selected"); %>>None</option>
		<option value="1" <% nvram_match_x("WLANConfig11b", "wl_wep_x", "1", "selected"); %>>WEP-64bits</option>
		<option value="2" <% nvram_match_x("WLANConfig11b", "wl_wep_x", "2", "selected"); %>>WEP-128bits</option>
	  </select>
	</td>
  </tr>
</table>

<div id='all_wep_key' style='display:none;'>
<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr>
    <th width="95"><#WLANConfig11b_WEPDefaultKey_itemname#></th>
    <td>
      <select name="wl_key" class="input" onfocus="parent.showHelpofDrSurf(0, 10);" onchange="show_key();">
        <option value="1" <% nvram_match_x("WLANConfig11b", "wl_key", "1", "selected"); %>>Key1</option>
        <option value="2" <% nvram_match_x("WLANConfig11b", "wl_key", "2", "selected"); %>>Key2</option>
        <option value="3" <% nvram_match_x("WLANConfig11b", "wl_key", "3", "selected"); %>>Key3</option>
        <option value="4" <% nvram_match_x("WLANConfig11b", "wl_key", "4", "selected"); %>>Key4</option>
      </select>
    </td>
  </tr>
  <tr>
    <th width="95"><#WLANConfig11b_WEPKey_itemname#></th>
    <td>
      <input type="text" id="sta_asuskey1" name="wl_asuskey1" onfocus="show_wepkey_help();" onKeyUp="return change_wlkey(this, 'WLANConfig11b');" value="" size="22" class="input"/>
    </td>
  </tr>
</table>
</div>
</div>

<div id='wl_crypto' style='display:none;'>
<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr>
	<th width="95"><#WLANConfig11b_WPAType_itemname#></th>
	<td>
	  <select name="wl_crypto" class="input" onfocus="parent.showHelpofDrSurf(0, 6);" onchange="wl_auth_mode_change(0);">
		<option value="tkip" <% nvram_match_x("WLANConfig11b", "wl_crypto", "tkip", "selected"); %>>TKIP</option>
		<option value="aes" <% nvram_match_x("WLANConfig11b", "wl_crypto", "aes", "selected"); %>>AES</option>
		<option value="tkip+aes" <% nvram_match_x("WLANConfig11b", "wl_crypto", "tkip+aes", "selected"); %>>TKIP+AES</option>
	  </select>
	</td>
  </tr>
</table>
</div>

<div id='wl_wpa_psk' style='display:none'>
<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr>
    <th width="95"><#WPA-PSKKey#></th>
    <td>
      <input type="text" id="sta_wpa_psk" name="wl_wpa_psk" onfocus="parent.showHelpofDrSurf(0, 7);" value="" size="22" maxlength="63" class="input"/>
    </td>
  </tr>
</table>
</div>

<table width="270" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
  <tr id="wl_radio_tr">
    <th width="95"><#Wireless_Radio#></th>
		<td>
	  	<input type="radio" name="wl_radio_x" value="1" <% nvram_match_x("WLANConfig11b", "wl_radio_x", "1", "checked"); %>>on
	  	<input type="radio" name="wl_radio_x" value="0" <% nvram_match_x("WLANConfig11b", "wl_radio_x", "0", "checked"); %>>off
		</td>
  </tr>
  <tr id="apply_tr">
    <td colspan="2"><input id="applySecurity" type="button" class="button" value="<#CTL_apply#>" onclick="submitForm();" style="margin-left:110px;"></td>
  <tr>
    <th width="95"><#LAN_IP#></th>
    <td id="LANIP"></td>
  </tr>
  <tr>
    <th width="95"><#PIN_code#></th>
    <td id="PINCode"></td>
  </tr>
  <tr>
    <th width="95"><#MAC_Address#></th>
    <td id="MAC"></td>
  </tr>
  <tr id="wps_pbc_tr">
    <th width="95">WPS</th>
    <td>
	<img src="/images/EZSetup_button.gif" style="cursor:pointer;" align="absmiddle" title="<#WPS_PBCbutton_hint#>" id="wpsPBC_button" onClick="wpsPBC(this);">
	<span id="WPS_PBCbutton_hint_span"></span>
	</td>
	</tr>
</table>
</form>

<select id="Router_domore" class="domore" onchange="domore_link(this);">
	<option><#MoreConfig#>...</option>
	<option value="../Advanced_Wireless_Content.asp"><#menu5_1#>-<#menu5_1_1#></option>
	<option value="../Advanced_WWPS_Content.asp"><#menu5_1_2#></option>
	<option value="../Advanced_LAN_Content.asp"><#menu5_2_1#></option>
	<option value="../Advanced_DHCP_Content.asp"><#menu5_2_2#></option>
	<option value="../Advanced_GWStaticRoute_Content.asp"><#menu5_2_3#></option>
	<option value="../Main_LogStatus_Content.asp"><#menu5_7_2#></option>
</select>

<form method="post" name="WPSForm" id="WPSForm" action="/start_apply.htm">
<input type="hidden" name="current_page" value="">
<input type="hidden" name="sid_list" value="">
<input type="hidden" name="action_script" value="">
</form>

<form method="post" name="stopPINForm" id="stopPINForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="current_page" value="">
<input type="hidden" name="sid_list" value="WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="wsc_config_command" value="<% nvram_get_x("WLANConfig11b", "wsc_config_command"); %>">
</form>
</body>
</html>