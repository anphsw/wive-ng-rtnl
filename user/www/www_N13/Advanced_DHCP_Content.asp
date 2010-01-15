<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_2_2#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" language="JavaScript" src="/help.js"></script>
<script type="text/javascript" language="JavaScript" src="/detect.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function initial(){
	show_banner(1);
	show_menu(5,2,2);
	show_footer();
	
	enable_auto_hint(5, 7);

	load_body();
}

function applyRule(){
	if(validForm()){
		showLoading();
		
		document.form.action_mode.value = " Restart ";
		document.form.current_page.value = "/as.asp";
		document.form.next_page.value = "";
		
		document.form.submit();
	}
}

function validForm(){
	 if(!validate_string(document.form.lan_domain))
	 	return false;
	
	if(!validate_ipaddr_final(document.form.dhcp_start, 'dhcp_start') ||
			!validate_ipaddr_final(document.form.dhcp_end, 'dhcp_end') ||
			!validate_ipaddr_final(document.form.dhcp_gateway_x, 'dhcp_gateway_x') ||
			!validate_ipaddr_final(document.form.dhcp_dns1_x, 'dhcp_dns1_x') ||
			!validate_ipaddr_final(document.form.dhcp_wins_x, 'dhcp_wins_x'))
		return false;
	
	if(!validate_range(document.form.dhcp_lease, 1, 86400))
		return false;
	
	if(intoa(document.form.dhcp_start.value) > intoa(document.form.dhcp_end.value)){
		tmp = document.form.dhcp_start.value;
		document.form.dhcp_start.value = document.form.dhcp_end.value;
		document.form.dhcp_end.value = tmp;
	}
	
	if(!validate_ipaddr(document.form.dhcp_wins_x, 'dhcp_wins_x'))
		return false;
	
	return true;
}

function done_validating(action){
	refreshpage();
}
</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(5, 7);return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">

<input type="hidden" name="current_page" value="Advanced_DHCP_Content.asp">
<input type="hidden" name="next_page" value="Advanced_GWStaticRoute_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="LANHostConfig;">
<input type="hidden" name="group_id" value="ManualDHCPList">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<input type="hidden" name="lan_ipaddr" value="<% nvram_get_x("LANHostConfig","lan_ipaddr"); %>">
<input type="hidden" name="lan_netmask" value="<% nvram_get_x("LANHostConfig","lan_netmask"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="23">&nbsp;</td>
	
	<!--=====Beginning of Main Menu=====-->
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div><br />

<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td align="left" valign="top">
	  <table width="500" border="0" align="center" cellpadding="4" cellspacing="0" class="FormTitle" table>
		<thead>
		  <tr>
			<td><#menu5_2#> - <#menu5_2_2#></td>
		  </tr>
		</thead>
		
		<tr>
		  <td bgcolor="#FFFFFF"><#LANHostConfig_DHCPServerConfigurable_sectiondesc#></td>
		</tr>
		
		<tbody>
		<tr>
		  <td bgcolor="#FFFFFF">
			<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
			  <tr>
				<th width="200"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,1);"><#LANHostConfig_DHCPServerConfigurable_itemname#></a></th>
				<td>
				  <input type="radio" value="1" name="dhcp_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_enable_x', '1')" <% nvram_match_x("LANHostConfig","dhcp_enable_x", "1", "checked"); %>>Yes</input>
				  <input type="radio" value="0" name="dhcp_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_enable_x', '0')" <% nvram_match_x("LANHostConfig","dhcp_enable_x", "0", "checked"); %>>No</input>
				</td>
			  </tr>
			  
			  <tr>
				<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,2);"><#LANHostConfig_DomainName_itemname#></a></th>
				<td>
				  <input type="text" maxlength="32" class="input" size="32" name="lan_domain" value="<% nvram_get_x("LANHostConfig", "lan_domain"); %>" onKeyPress="return is_string(this)">
				</td>
			  </tr>
			  
			  <tr>
			  <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,3);"><#LANHostConfig_MinAddress_itemname#></a></th>
			  <td>
				<input type="text" maxlength="15" class="input" size="15" name="dhcp_start" value="<% nvram_get_x("LANHostConfig","dhcp_start"); %>" onKeyPress="return is_ipaddr(this);" onKeyUp="change_ipaddr(this);">
			  </td>
			  </tr>
			  
			  <tr>
            <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,4);"><#LANHostConfig_MaxAddress_itemname#></a></th>
            <td>
              <input type="text" maxlength="15" class="input" size="15" name="dhcp_end" value="<% nvram_get_x("LANHostConfig","dhcp_end"); %>" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)">
            </td>
			  </tr>
			  
			  <tr>
            <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,5);"><#LANHostConfig_LeaseTime_itemname#></a></th>
            <td>
              <input type="text" maxlength="5" size="5" name="dhcp_lease" class="input" value="<% nvram_get_x("LANHostConfig", "dhcp_lease"); %>" onKeyPress="return is_number(this)">
            </td>
			  </tr>
			  
			  <tr>
            <th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,6);"><#LANHostConfig_x_LGateway_itemname#></a></th>
            <td>
              <input type="text" maxlength="15" class="input" size="15" name="dhcp_gateway_x" value="<% nvram_get_x("LANHostConfig","dhcp_gateway_x"); %>" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)">
            </td>
			  </tr>
			</table>
		  </td>
		</tr>
		
		<tr>
		  <td bgcolor="#FFFFFF">
			<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
			  <thead>
			  <tr>
				<td colspan="2"><#LANHostConfig_x_LDNSServer1_sectionname#></td>
			  </tr>
			  </thead>		
			  
			  <tr>
				<th width="200"><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,7);"><#LANHostConfig_x_LDNSServer1_itemname#></a></th>
				<td>
				  <input type="text" maxlength="15" class="input" size="15" name="dhcp_dns1_x" value="<% nvram_get_x("LANHostConfig","dhcp_dns1_x"); %>" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)">
				</td>
			  </tr>
			  
			  <tr>
				<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,8);"><#LANHostConfig_x_WINSServer_itemname#></a></th>
				<td>
				  <input type="text" maxlength="15" class="input" size="15" name="dhcp_wins_x" value="<% nvram_get_x("LANHostConfig","dhcp_wins_x"); %>" onkeypress="return is_ipaddr(this)" onkeyup="change_ipaddr(this)" />
				</td>
			  </tr>
			</table>
		  </td>
		</tr>
		
		<!-- manually assigned the DHCP List -->
		<tr>
		  <td bgcolor="#FFFFFF">
			<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
			  <thead>
			  <tr>
				<td colspan="3" id="GWStatic"><#LANHostConfig_ManualDHCPList_groupitemdesc#></td>
			  </tr>
			  </thead>
			  
			  <tr>
            <th><#LANHostConfig_ManualDHCPEnable_itemname#></th>
            <td colspan="2">
              <input type="radio" value="1" name="dhcp_static_x"  onclick="return change_common_radio(this, 'LANHostConfig', 'dhcp_static_x', '1')" <% nvram_match_x("LANHostConfig","dhcp_static_x", "1", "checked"); %> />Yes
      		  <input type="radio" value="0" name="dhcp_static_x"  onclick="return change_common_radio(this, 'LANHostConfig', 'dhcp_static_x', '0')" <% nvram_match_x("LANHostConfig","dhcp_static_x", "0", "checked"); %> />No
      		</td>
			  </tr>
			  
			  <tr bgcolor="#FFFFFF">
		  	<th style="text-align:center;"><#LANHostConfig_ManualMac_itemname#></th>
            <th style="text-align:center;"><#LANHostConfig_ManualIP_itemname#></th>
            <th width="40"></th>
			  </tr>
			  
			  <tr bgcolor="#FFFFFF">
            <td align="center"><input type="hidden" name="dhcp_staticnum_x_0" value="<% nvram_get_x("LANHostConfig", "dhcp_staticnum_x"); %>" readonly="1" />
                <input type="text" maxlength="12" class="input" size="12" name="dhcp_staticmac_x_0" onkeypress="return is_hwaddr()" /></td>
            <td align="center"><input type="text" maxlength="15" class="input" size="15" name="dhcp_staticip_x_0" onkeypress="return is_ipaddr(this)" onkeyup="change_ipaddr(this)" /></td>
            <td width="40"><input class="button" type="submit" onclick="return markGroup(this, 'ManualDHCPList', 8, ' Add ');" name="ManualDHCPList2" value="<#CTL_add#>" size="12" /></td>
			  </tr>
			  
			  <tr bgcolor="#FFFFFF">
            <td colspan="2" align="center">
			  <select class="input" size="4" name="ManualDHCPList_s" multiple="multiple" style="width:100%; font-size:12px; font-family:'fixedsys', Courier, mono;">
                <% nvram_get_table_x("LANHostConfig","ManualDHCPList"); %>
              </select>
			</td>
            <td>
              <input class="button" type="submit" onclick="return markGroup(this, 'ManualDHCPList', 8, ' Del ');" name="ManualDHCPList" value="<#CTL_del#>" size="12" />
            </td>
			  </tr>
			  
			  <tr align="right">
            <td colspan="3">
              <input type="button" name="button" class="button" onclick="applyRule();" value="<#CTL_apply#>"/>
            </td>
          	  </tr>
        	</table>
      	  </td>
		</tr>
		</tbody>
	  </table>		
	</td>
</form>

	<!-- help block -->
	<td id="help_td" style="width:15px;" valign="top">
<form name="hint_form"></form>
	  <div id="helpicon" onClick="openHint(0,0);" title="<#Help_button_default_hint#>">
	  	<img src="images/help.gif" />
	  </div>
      
	  <div id="hintofPM" style="display:none;">
		<table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
		  <thead>
		  <tr>
			<td>
			  <div id="helpname" class="AiHintTitle"></div>
			  <a href="javascript:;" onclick="closeHint()" ><img src="images/button-close.gif" class="closebutton" /></a>
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
	<!--==============Ending of hint content=============-->
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
