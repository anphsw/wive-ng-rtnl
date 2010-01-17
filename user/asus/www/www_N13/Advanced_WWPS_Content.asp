﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_1_2#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/ajax.js"></script>
<script type="text/javascript" src="/detect.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

var wsc_config_state_old = '<% nvram_get_x("WLANConfig11b", "wsc_config_state"); %>';
var wps_enable_old = '<% nvram_get_x("WLANConfig11b", "wps_enable"); %>';
var wps_mode_old = '<% nvram_get_x("WLANConfig11b", "wps_mode"); %>';

var secs;
var timerID = null;
var timerRunning = false;
var timeout = 2000;
var delay = 1000;
var stopFlag = 0;

function initial(){
	show_banner(1);
	
	if(sw_mode == "2")
		show_menu(5,1,1);
	else
		show_menu(5,1,2);		
		
	show_footer();
	
	enable_auto_hint(13, 0);
	loadXML();
}

function done_validating(action){
	refreshpage();
}

// use nvram "wps_enable" and "wps_mode" to operate the WPS, and don't need to set them.
// So the sid_list = "".
function applyRule(){
	showLoading();
	stopFlag = 1;
	
	document.form.current_page.value = "/Advanced_WWPS_Content.asp";
	document.form.next_page.value = "";
	
	document.form.submit();
}

function enableWPS(){
	if(wps_enable_old == "1")
		document.form.wps_enable.value = "0";
	else if(wps_enable_old == "0")
		document.form.wps_enable.value = "1";
	
	document.form.action_script.value = "WPS_apply";
	
	applyRule();
}

function configCommand(){
	if(PIN_PBC_Check()){
		document.form.wps_mode.value = "1";
		
		document.form.action_script.value = "WPS_apply";
		
		applyRule();
	}
}

function resetWPS(){
	document.form.action_script.value = "Reset_OOB";
	
	applyRule();
}

function resetTimer()
{
	if (stopFlag == 1)
	{
		stopFlag = 0;
		InitializeTimer();
	}
}

//<!--------------------WPS code by Jiahao---------------------->
function ValidateChecksum(PIN)
{
	var accum = 0;

	accum += 3 * (parseInt(PIN / 10000000) % 10);
	accum += 1 * (parseInt(PIN / 1000000) % 10);
	accum += 3 * (parseInt(PIN / 100000) % 10);
	accum += 1 * (parseInt(PIN / 10000) % 10);
	accum += 3 * (parseInt(PIN / 1000) % 10);
	accum += 1 * (parseInt(PIN / 100) % 10);
	accum += 3 * (parseInt(PIN / 10) % 10);
	accum += 1 * (parseInt(PIN / 1) % 10);

	//return ((accum % 10) == 0);
	return true;
}

function PIN_PBC_Check(){
	if(document.form.wps_mode.value == "1"){
		if(document.form.wps_pin.value != ""){
			if(document.form.wps_pin.value.length != 8 || !ValidateChecksum(document.form.wps_pin.value)){
				alert("<#JS_InvalidPIN#>");
				document.form.wps_pin.focus();
				document.form.wps_pin.select();
				return false;
			}
		}	
	}
	
	return true;
}

function InitializeTimer()
{
	if(document.form.wl_auth_mode.value == "shared"
			|| document.form.wl_auth_mode.value == "wpa"
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
	
	if(wpss == null || wpss[0] == null){
		if (confirm('<#JS_badconnection#>'))
			;
		else
			stopFlag=1;
		
		return;
	}
	
	var wps_infos = wpss[0].getElementsByTagName("wps_info");
	show_wsc_status(wps_infos);
}

function show_wsc_status(wps_infos){
	if(wps_infos[11].firstChild.nodeValue == "shared"
			|| wps_infos[11].firstChild.nodeValue == "wpa"
			|| wps_infos[11].firstChild.nodeValue == "wpa2"
			|| wps_infos[11].firstChild.nodeValue == "radius"){
		$("wps_enable_block").style.display = "none";
		$("wps_enable_hint").innerHTML = "<#wsc_mode_hint1#><a href=\"Advanced_Wireless_Content.asp\"> <#menu5_1_1#></a> <#wsc_mode_hint2#>"
		$("wps_state_tr").style.display = "none";
		$("devicePIN_tr").style.display = "none";
		$("pin_tr").style.display = "none";
		$("addEnrolleebtn").style.display = "none";
		$("Reset_OOB").style.display = "none";
		return;
	}
	
	if(wps_infos[9].firstChild.nodeValue != wps_enable_old)
		wps_enable_old = wps_infos[9].firstChild.nodeValue;
	
	// enable button
	if(wps_enable_old == "1"){
		$("wps_enable_word").innerHTML = "<#btn_Enabled#>";
		$("enableWPSbtn").value = "<#btn_disable#>";
	}
	else{
		$("wps_enable_word").innerHTML = "<#btn_Disabled#>"
		$("enableWPSbtn").value = "<#btn_Enable#>";
	}
	
	$("wps_enable_block").style.display = "";
	
	// WPS status
	if(wps_enable_old == "0"){
		$("wps_state_tr").style.display = "";
		$("wps_state_td").innerHTML = "Not used";
	}
	else{
		$("wps_state_tr").style.display = "";
		$("wps_state_td").innerHTML = wps_infos[0].firstChild.nodeValue;
	}
	
	// device's PIN code
	$("devicePIN_tr").style.display = "";
	$("devicePIN").value = wps_infos[7].firstChild.nodeValue;
	
	// the input of the client's PIN code
	$("pin_tr").style.display = "";
	if(wps_enable_old == "1"
			&& wps_infos[10].firstChild.nodeValue == "1")
		document.form.wps_pin.disabled = 0;
	else
		document.form.wps_pin.disabled = 1;
	
	if(wps_enable_old == "1"){
		$("Reset_OOB").style.display = "";
		$("addEnrolleebtn").style.display = "";
	}
	else{
		$("Reset_OOB").style.display = "none";
		$("addEnrolleebtn").style.display = "none";
	}
}
</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(13, 0);return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="POST" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">

<table class="content" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td width="23">&nbsp;</td>		
		<td valign="top" width="202">
		<div  id="mainMenu"></div>
		<div  id="subMenu"></div>	
		</td>
		<td valign="top">
	<div id="tabMenu" class="submenuBlock"></div><br />
		<!--===================================Beginning of Main Content===========================================-->
<input type="hidden" name="current_page" value="/Advanced_WWPS_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_char_to_ascii("WLANConfig11b", "wl_ssid"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<input type="hidden" name="wps_enable" value="<% nvram_get_x("WLANConfig11b", "wps_enable"); %>">
<input type="hidden" name="wps_mode" value="<% nvram_get_x("WLANConfig11b", "wps_mode"); %>">
<!--input type="hidden" name="wsc_config_state" value="<% nvram_get_x("WLANConfig11b", "wsc_config_state"); %>">
<input type="hidden" name="wsc_config_command" value="<% nvram_get_x("WLANConfig11b", "wsc_config_command"); %>">
<input type="hidden" name="wsc_client_role" value="<% nvram_get_x("WLANConfig11b", "wsc_client_role"); %>"-->

<input type="hidden" name="wl_auth_mode" value="<% nvram_get_x("WLANConfig11b", "wl_auth_mode"); %>">
<input type="hidden" name="wl_wep_x" value="<% nvram_get_x("WLANConfig11b", "wl_wep_x"); %>">

<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td valign="top" >
		
<table width="500" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle" table>
	<thead>
	<tr>
		<td><#menu5_1#> - <#t2WPS#></td>
	</tr>
	</thead>
	<tbody>
	<tr>
		<td bgcolor="#FFFFFF"><#WLANConfig11b_display6_sectiondesc#></td>
	</tr>
	</tbody>	
	<tr>
	  <td bgcolor="#FFFFFF">
		<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
			<tr>
			  <th width="30%"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(13,1);"><#WLANConfig11b_x_WPS_itemname#></a></th>
			  <td>
			    <div id="wps_enable_block"><span style="color:#000;" id="wps_enable_word"></span>&nbsp;&nbsp;<input type="button" name="enableWPSbtn" id="enableWPSbtn" value="" class="button" onClick="enableWPS();"><br></div>
					<span id="wps_enable_hint"></span>
		  	  </td>
			</tr>
			
			<tr id="wps_state_tr">
				<th><#WLANConfig11b_x_WPSConf_itemname#></th>
				<td width="300" id="wps_state_td">&nbsp;</td>
			</tr>
			
			<tr id="devicePIN_tr">
			  <th>
			  	<span id="devicePIN_name"><a class="hintstyle" href="javascript:void(0);" onclick="openHint(13,4);"><#WLANConfig11b_x_DevicePIN_itemname#></a></span>			  
			  </th>
			  <td>
			  	<input type="text" name="devicePIN" id="devicePIN" value="" onmouseover="this.select();" class="devicepin" readonly="1" style="float:left;"></input>
			  </td>
			</tr>
			
			<tr id="pin_tr">
				<th>
			  	<span id="wpsPIN_name"><a class="hintstyle" href="javascript:void(0);" onclick="openHint(13,3);"><#WLANConfig11b_x_WPSPIN_itemname#></a></span>
			  </th>
			  <td>
			  	<input type="text" name="wps_pin" id="wps_pin" value="" size="8" maxlength="8" class="inputpin" style="float:left;">
			  				  	
			  </td>
			</tr>
			
			<tr>
				<td colspan="2" align="right">								
					<table>
						<tr>
						<td><input class="sbtn" type="button" onClick="configCommand();" id="addEnrolleebtn" name="addEnrolleebtn"  value="<#CTL_Add_enrollee#>"></td>
						<td><input class="button" type="button" onClick="resetWPS();" id="Reset_OOB" name="Reset_OOB" value="<#CTL_Reset_OOB#>" style="display:none; padding:0 0.3em 0 0.3em;" ></td>
						</tr>
					</table>
				</td>					
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
            	<img src="images/help.gif"/>
            </div>
            <div id="hintofPM"  style="display:none; ">
              <table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
			  	<thead>
                <tr>
                  <td><div id="helpname" class="AiHintTitle"></div><a href="javascript:void(0);" onclick="closeHint()" ><img src="images/button-close.gif" class="closebutton" /></a></td>
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
		<!--===================================Ending of Main Content===========================================-->		
	</td>
		
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</body>
</html>
