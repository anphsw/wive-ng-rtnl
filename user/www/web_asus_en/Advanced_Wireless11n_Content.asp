<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onLoad="load_body()" onunLoad="return unload_body();">
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
    	
<input type="hidden" name="current_page" value="Advanced_Wireless_Content.asp">
<input type="hidden" name="next_page" value="Advanced_WMode_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">
<input type="hidden" name="wl_wpa_mode" value="<% nvram_get_x("WLANConfig11b","wl_wpa_mode"); %>">
<input type="hidden" name="HT_EXTCHA_old" value="<% nvram_get_x("WLANConfig11b","HT_EXTCHA"); %>">

<input type="hidden" name="wl_wpa_psk_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_wpa_psk"); %>">
<input type="hidden" name="wl_phrase_x_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_phrase_x"); %>">
<input type="hidden" name="wl_key1_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key1"); %>">
<input type="hidden" name="wl_key2_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key2"); %>">
<input type="hidden" name="wl_key3_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key3"); %>">
<input type="hidden" name="wl_key4_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_key4"); %>">

<input type="hidden" name="wl_ssid_org" value="<% nvram_char_to_ascii("WLANConfig11b","wl_ssid"); %>">
<input type="hidden" name="wl_wme" value="<% nvram_char_to_ascii("WLANConfig11b","wl_wme"); %>">

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
	<td class="content_header_td_title" colspan="2"><#t1Wireless#> - <#t2IF#></td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_SSID_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_SSID_itemname#></td>
	<td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_ssid" value="<% nvram_get_x("WLANConfig11b","wl_ssid"); %>" onChange="page_changed()" onKeyPress="return is_string(this)" onBlur="validate_string_ssid(this)"></td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_x_Mode11g_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_x_Mode11g_itemname#></td>
	<td class="content_input_td">
		<select name="wl_gmode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_gmode')">
			<option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_gmode", "2","selected"); %>>Auto</option>
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_gmode", "1","selected"); %>>b/g Mixed</option>
			<option class="content_input_fd" value="3" <% nvram_match_x("WLANConfig11b","wl_gmode", "3","selected"); %>>n Only</option>
			<option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","wl_gmode", "4","selected"); %>>g Only</option>
			<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_gmode", "0","selected"); %>>b Only</option>
		</select>
		<input onMouseOver="return overlib('<#WLANConfig11b_x_GProtection_itemdesc#>', LEFT);" onMouseOut="return nd();" type="checkbox" style="margin-left:15" name="wl_gmode_check" value="" onClick="return change_common(this, 'WLANConfig11b', 'wl_gmode_check', '1')"><#WLANConfig11b_x_GProtection_itemname#></input>
		<input type="hidden" name="wl_gmode_protection" value="<% nvram_get_x("WLANConfig11b","wl_gmode_protection"); %>">
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_ChannelBW_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_ChannelBW_itemname#></td>
	<td>
		<select name="HT_BW" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'HT_BW')">
			<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","HT_BW", "0","selected"); %>>20 MHz</option>
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","HT_BW", "1","selected"); %>>20/40 MHz</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_Channel_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_Channel_itemname#></td>
	<td class="content_input_td">
		<select name="wl_channel" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_channel')">
			<% select_channel("WLANConfig11b"); %>
                </select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_EChannel_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_EChannel_itemname#></td>
    	<td class="content_input_td">
		<select name="HT_EXTCHA" class="content_input_fd">
			<option class="content_input_fd" value="1" selected>Auto</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_AuthenticationMethod_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_AuthenticationMethod_itemname#></td>
	<td class="content_input_td">
		<select name="wl_auth_mode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_auth_mode')">
			<option class="content_input_fd" value="open" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "open","selected"); %>>Open System</option>
			<option class="content_input_fd" value="shared" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "shared","selected"); %>>Shared Key</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "1", "selected"); %>>WPA-Personal</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "2", "selected"); %>>WPA2-Personal</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "0", "selected"); %>>WPA-Auto-Personal</option>
			<option class="content_input_fd" value="wpa" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "wpa", "WLANConfig11b", "wl_wpa_mode", "1", "selected"); %>>WPA-Enterprise</option>
                        <option class="content_input_fd" value="wpa" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "wpa", "WLANConfig11b", "wl_wpa_mode", "2", "selected"); %>>WPA2-Enterprise</option>
                        <option class="content_input_fd" value="wpa" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "wpa", "WLANConfig11b", "wl_wpa_mode", "0", "selected"); %>>WPA-Auto-Enterprise</option>
			<option class="content_input_fd" value="radius" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "radius","selected"); %>>Radius with 802.1x</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_WPAType_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_WPAType_itemname#></td>
	<td class="content_input_td">
		<select name="wl_crypto" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_crypto')">
			<option class="content_input_fd" value="tkip" <% nvram_match_x("WLANConfig11b","wl_crypto", "tkip","selected"); %>>TKIP</option>
			<option class="content_input_fd" value="aes" <% nvram_match_x("WLANConfig11b","wl_crypto", "aes","selected"); %>>AES</option>
			<option class="content_input_fd" value="tkip+aes" <% nvram_match_x("WLANConfig11b","wl_crypto", "tkip+aes","selected"); %>>TKIP+AES</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_x_PSKKey_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_x_PSKKey_itemname#></td>
	<td class="content_input_td">
		<script language="JavaScript" type="text/javascript" src="md5.js"></script>
		<input type="text" maxlength="64" class="content_input_fd" size="32" name="wl_wpa_psk" value="<% nvram_get_x("WLANConfig11b","wl_wpa_psk"); %>" onChange="page_changed()" onKeyUp="return is_wlphrase('WLANConfig11b', 'wl_wpa_psk', this)" onBlur="return validate_wlphrase('WLANConfig11b', 'wl_wpa_psk', this)">
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_WEPType_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_WEPType_itemname#></td>
	<td class="content_input_td">
		<select name="wl_wep_x" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_wep_x')">
			<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_wep_x", "0","selected"); %>>None</option>
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_wep_x", "1","selected"); %>>WEP-64bits</option>
			<option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_wep_x", "2","selected"); %>>WEP-128bits</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_WEPKeyType_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_WEPKeyType_itemname#></td>
	<td class="content_input_td">
		<select name="wl_key_type" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key_type')">
			<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_key_type", "0","selected"); %>>HEX</option>
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_key_type", "1","selected"); %>>ASCII</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_x_Phrase_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_x_Phrase_itemname#></td>
	<td class="content_input_td">
		<script language="JavaScript" type="text/javascript" src="md5.js"></script>
		<input type="text" maxlength="64" class="content_input_fd" size="32" name="wl_phrase_x" value="<% nvram_get_x("WLANConfig11b","wl_phrase_x"); %>" onChange="page_changed()" onKeyUp="return is_wlphrase('WLANConfig11b', 'wl_phrase_x', this)" onBlur="return validate_wlphrase('WLANConfig11b', 'wl_phrase_x', this)">
	</td>
</tr>

<tr>
	<td class="content_header_td"><#WLANConfig11b_WEPKey1_itemname#></td>
	<td class="content_input_td">
		<input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key1" value="<% nvram_get_x("WLANConfig11b","wl_key1"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')">
	</td>
</tr>

<tr>
	<td class="content_header_td"><#WLANConfig11b_WEPKey2_itemname#></td>
	<td class="content_input_td">
		<input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key2" value="<% nvram_get_x("WLANConfig11b","wl_key2"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')">
	</td>
</tr>

<tr>
	<td class="content_header_td"><#WLANConfig11b_WEPKey3_itemname#></td>
	<td class="content_input_td">
		<input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key3" value="<% nvram_get_x("WLANConfig11b","wl_key3"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')">
	</td>
</tr>

<tr>
	<td class="content_header_td"><#WLANConfig11b_WEPKey4_itemname#></td>
	<td class="content_input_td">
		<input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key4" value="<% nvram_get_x("WLANConfig11b","wl_key4"); %>" onChange="page_changed()" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')">
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_WEPDefaultKey_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_WEPDefaultKey_itemname#></td>
	<td class="content_input_td">
		<select name="wl_key" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key')">
			<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_key", "1","selected"); %>>Key1</option>
			<option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_key", "2","selected"); %>>Key2</option>
			<option class="content_input_fd" value="3" <% nvram_match_x("WLANConfig11b","wl_key", "3","selected"); %>>Key3</option>
			<option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","wl_key", "4","selected"); %>>Key4</option>
		</select>
	</td>
</tr>

<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#WLANConfig11b_x_Rekey_itemdesc#>', LEFT);" onMouseOut="return nd();"><#WLANConfig11b_x_Rekey_itemname#></td>
	<td class="content_input_td">
		<input type="text" maxlength="5" size="5" name="wl_wpa_gtk_rekey" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b", "wl_wpa_gtk_rekey"); %>" onChange="page_changed()" onBlur="validate_range(this, 0, 86400)" onKeyPress="return is_number(this)">
	</td>
</tr>

</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="return onSubmitCtrl(this, ' Restore ');" type="submit" value=" <#CTL_restore#> " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="return onSubmitCtrl(this, ' Finish ');" type="submit" value=" <#CTL_finish#> " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="return onSubmitCtrl(this, ' Apply ');" type="submit" value=" <#CTL_apply#> " name="action"></font></div> 
   </td>    
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr>
    <td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left"><#CTL_restore#>: </td>
    <td class="content_input_td_padding" align="left"><#CTL_desc_restore#></td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left"><#CTL_finish#>: </td>
    <td class="content_input_td_padding" align="left"><#CTL_desc_finish#></td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left"><#CTL_apply#>: </td>
    <td class="content_input_td_padding" align="left"><#CTL_desc_apply#></td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
