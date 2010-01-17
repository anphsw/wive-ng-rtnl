<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<script language="JavaScript" type="text/javascript" src="quick.js"></script>
</head>
<body bgcolor="#FFFFFF" onLoad="loadQuick()">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->
<table width="666" border="0" cellpadding="0" cellspacing="0">
<input type="hidden" name="x_Mode" value="0">
<input type="hidden" name="x_Connection" value="">
<input type="hidden" name="x_ADSLType" value="">
<input type="hidden" name="x_DHCPClient" value="">
<input type="hidden" name="current_page" value="Basic_AOperation_Content.asp">
<input type="hidden" name="next_page" value="Basic_HomeGateway_SaveRestart.asp">
<!--
<input type="hidden" name="next_page" value="Basic_AccessPoint_SaveRestart.asp">
-->
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;PPPConnection;WLANConfig11b;LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="wan_proto" value="<% nvram_get_x("Layer3Forwarding","wan_proto"); %>">
<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get_x("IPConnection","wan_nat_x"); %>">
<input type="hidden" name="wl_auth_mode" value="<% nvram_get_x("WLANConfig11b","wl_auth_mode"); %>">
<input type="hidden" name="wl_crypto" value="<% nvram_get_x("WLANConfig11b","wl_crypto"); %>">
<input type="hidden" name="wl_wep_x" value="<% nvram_get_x("WLANConfig11b","wl_wep_x"); %>">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">
<input type="hidden" name="wl_wpa_mode" value="<% nvram_get_x("WLANConfig11b","wl_wpa_mode"); %>">

<input type="hidden" name="wl_wpa_psk_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_wpa_psk"); %>">
<input type="hidden" name="wl_key1_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key1"); %>">
<input type="hidden" name="wl_key2_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key2"); %>">
<input type="hidden" name="wl_key3_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key3"); %>">
<input type="hidden" name="wl_key4_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key4"); %>">

<input type="hidden" name="wl_ssid_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_ssid"); %>">

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr  id="Wireless"  class="content_header_tr">
<td class="content_header_td_title" colspan="2"><#BOP_title#></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2"><#BOP_wlan_title#></td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50"><#BOP_wlan_desc#></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('<#BOP_wlan_ssid_desc#>', LEFT);" onMouseOut="return nd();"><#BOP_wlan_ssid_item#></td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_ssid" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_ssid"); %>" onChange="page_changed()" onKeyPress="return is_string(this)" onBlur="validate_string_ssid(this)"></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('<#BOP_wlan_sec_desc#>', LEFT);" onMouseOut="return nd();"><#BOP_wlan_sec_item#></td>
<td class="content_input_td">
 <select name="SecurityLevel" class="content_input_fd" onChange="return change_security(this, 'WLANConfig11b', 0)">
     <option value="0">Low(Open System)</option>
     <option value="1">Medium(WEP-64bits)</option>
     <option value="2">Medium(WEP-128bits)</option>
     <option value="3">High(WPA-Personal)</option>
 </select>    
</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_WEPKeyType_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_WEPKeyType_itemname#></td>
	<td class="content_input_td">
		<select name="wl_key_type" class="content_input_fd" onChange="return change_security(document.form.SecurityLevel, 'WLANConfig11b', 0)">
			<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_key_type", "0","selected"); %>>HEX</option>
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_key_type", "1","selected"); %>>ASCII</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td_less" onMouseOver="return overlib('<#BOP_wlan_pass_desc#>', LEFT);" onMouseOut="return nd();"><#BOP_wlan_pass_item#></td>
	<td class="content_input_td"><script language="JavaScript" type="text/javascript" src="phrase.js"></script><script language="JavaScript" type="text/javascript" src="md5.js"></script><input type="text" maxlength="64" size="32" name="wl_wpa_psk" class="content_input_fd"  value="<% nvram_get_x("WLANConfig11b","wl_wpa_psk"); %>" onChange="page_changed()" onKeyUp="return is_wlphrase_q('WLANConfig11b', this)" onBlur="return validate_wlphrase_q('WLANConfig11b', this)"></td>
</tr>

<tr>
<td class="content_header_td_less"><#BOP_wlan_wep1_item#></td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key1" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key1"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>				   
<tr>
<td class="content_header_td_less"><#BOP_wlan_wep2_item#></td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key2" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key2"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less"><#BOP_wlan_wep3_item#></td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key3" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key3"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less"><#BOP_wlan_wep4_item#></td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key4" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key4"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less"><#BOP_wlan_defkey_item#></td><td class="content_input_td"><select name="wl_key" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key')"><option value="1" <% nvram_match_x("WLANConfig11b","wl_key", "1","selected"); %>>1</option><option value="2" <% nvram_match_x("WLANConfig11b","wl_key", "2","selected"); %>>2</option><option value="3" <% nvram_match_x("WLANConfig11b","wl_key", "3","selected"); %>>3</option><option value="4" <% nvram_match_x("WLANConfig11b","wl_key", "Key4","selected"); %>>4</option></select></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="34%">  
   </td>
   <td height="25" width="33%">  
   </td>
   <td height="25" width="33%">  
   <div align="center">
   <font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Wireless')" type="hidden" value="<#CTL_prev#>" name="action1"></font>&nbsp;&nbsp;&nbsp;&nbsp;
   <font face="Arial"><input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" type="submit" value="<#CTL_finish#>" name="action"  onClick="saveQuick(this)"></font></div>
   </td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr>
    <td colspan="2" width="666" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
</table>
</td>
</tr>
<tr><td colspan="2" height="240"></td></tr>
</table>
</form>

