<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_4_4#></title>
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
wan_proto = '<% nvram_get_x("Layer3Forwarding", "wan_proto"); %>';

wan_proto_t = '<% nvram_get_x("Layer3Forwarding", "wan_proto_t"); %>';
wan_proto_t = wan_proto_t.toLowerCase();

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function initial(){
	show_banner(1);
	show_menu(5, 4, 3);
	show_footer();
	
	enable_auto_hint(21, 7);
	document.form.Dev3G.disabled = (document.form.wan_proto.value == "3g")?false:true;
	//document.form.Dev3G.disabled = (document.form.wan_proto.value == "3g")?false:true;
	$("hsdpa_hint").style.display = (document.form.wan_proto.value == "3g")?"":"none";
}

function switch_hsdap_mode(mode){
	document.form.Dev3G.disabled = (document.form.wan_proto.value == "3g")?false:true;
	$("hsdpa_hint").style.display = (document.form.wan_proto.value == "3g")?"":"none";
	
	if(document.form.wan_proto.value != "3g"){
		if(wan_proto_t == "automatic ip"){
			document.form.wan_proto.options[0].value = "dhcp";
		}
		else if(wan_proto_t){
			document.form.wan_proto.options[0].value = wan_proto_t;
		}
	}
}

function applyRule(){
	showLoading();
	//alert(document.form.wan_proto.value);
	document.form.action_mode.value = " Apply ";
	document.form.current_page.value = "/Advanced_HSDPA_others.asp";
	document.form.next_page.value = "";
		
	document.form.submit();
}

function done_validating(action){
	refreshpage();
}
</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(21, 7);return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log", "productid"); %>">

<input type="hidden" name="current_page" value="Advanced_AiDisk_others.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="General;Layer3Forwarding;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<input type="hidden" name="computer_nameb" value="">
<input type="hidden" name="computer_name2" value="<% nvram_get_x("Storage", "computer_nameb"); %>">
<input type="hidden" name="computer_name3" value="<% nvram_get_x("Storage", "computer_name"); %>">
<!--input type="hidden" name="st_samba_workgroupb" value="">
<input type="hidden" name="samba_workgroup2" value="<% nvram_get_x("Storage", "st_samba_workgroupb"); %>">
<input type="hidden" name="samba_workgroup3" value="<% nvram_get_x("Storage", "st_samba_workgroup"); %>"-->

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="23">&nbsp;</td>
	
	<!--=====Beginning of Main Menu=====-->
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div>
		<br />
		<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td align="left" valign="top" >
		
<table width="500" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle" table>
	<thead>
	<tr>
		<td><#menu5_4_4#></td>
	</tr>
	</thead>
	<tbody>
	  <tr>
	    <td bgcolor="#FFFFFF"><#HSDPAConfig_hsdpa_mode_itemdesc#></td>
	  </tr>
	</tbody>
	<tr>
		<td bgcolor="#FFFFFF">
			<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
				<tr>
					<th width="40%">
						<a class="hintstyle" href="javascript:openHint(21,1);"><#HSDPAConfig_hsdpa_enable_itemname#></a>
					</th>
					<td>
						<select name="wan_proto" class="input" onchange="switch_hsdap_mode(this.value);">
							<option value="dhcp" <% nvram_match_x("Storage", "wan_proto", "dhcp", "selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
							<option value="3g" <% nvram_match_x("Storage", "wan_proto", "3g", "selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
						</select>
						<br/><span id="hsdpa_hint" style="display:none;"><#HSDPAConfig_hsdpa_enable_hint1#></span>
					</td>
				</tr>
				
				<tr>
				<th><a class="hintstyle"  href="javascript:void(0);" onClick="openHint(21,2);"><#HSDPAConfig_USBAdapter_itemname#></a></th>
				<td>
    			<select name="Dev3G" class="input" disabled="disabled">
	      		<option value="MU-Q101" id="MU-Q101" <% nvram_match_x("General","Dev3G", "MU-Q101","selected"); %>>NU MU-Q101</option>
	      		<option value="HUAWEI-E169" id="E169" <% nvram_match_x("General","Dev3G", "HUAWEI-E169","selected"); %>>HUAWEI E169</option>
	      		<option value="HUAWEI-E220" id="E220" <% nvram_match_x("General","Dev3G", "HUAWEI-E220","selected"); %>>HUAWEI E220</option>
	    		  <option value="BandLuxe-C270" id="C270" <% nvram_match_x("General","Dev3G", "BandLuxe-C270","selected"); %>>BandLuxe C270</option>
			      <option value="OPTION-ICON225" id="ICON225" <% nvram_match_x("General","Dev3G", "OPTION-ICON225","selected"); %>>OPTION ICON 225</option>
    			</select>
				</td>
				</tr>
				<tr align="right">
					<td colspan="2">
						<input type="button" class="button" value="<#CTL_apply#>" onclick="applyRule();">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>		

		</td>
</form>

					<!--==============Beginning of hint content=============-->
					<td id="help_td" style="width:15px;" valign="top">
						<form name="hint_form"></form>
						<div id="helpicon" onClick="openHint(0,0);" title="<#Help_button_default_hint#>"><img src="images/help.gif" /></div>
						<div id="hintofPM" style="display:none;">
							<table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
								<thead>
								<tr>
									<td>
										<div id="helpname" class="AiHintTitle"></div>
										<a href="javascript:closeHint();">
											<img src="images/button-close.gif" class="closebutton">
										</a>
									</td>
								</tr>
								</thead>
								
								<tr>
									<td valign="top">
										<div class="hint_body2" id="hint_body"></div>
										<iframe id="statusframe" name="statusframe" class="statusframe" src="" frameborder="0"></iframe>
									</td>
								</tr>
							</table>
						</div>
					</td>
					<!--==============Ending of hint content=============-->
					
				</tr>
			</table>				
		</td>
		
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</body>
</html>
