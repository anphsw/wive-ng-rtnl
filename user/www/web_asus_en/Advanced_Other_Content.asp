<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onunLoad="return unload_body();">
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
    	
<input type="hidden" name="current_page" value="Advanced_Other_Content.asp">
<input type="hidden" name="next_page" value="Advanced_ASUSDDNS_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">

<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2"><#t1IP#> - <#t2Misc#></td>
</tr>
<!--
<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#LANHostConfig_x_WCNEnable_itemdesc#>', LEFT);" onMouseOut="return nd();">
		<#LANHostConfig_x_WCNEnable_itemname#>
	</td>
	<td class="content_input_td">
		<input type="radio" value="1" name="wcn_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'wcn_enable', '1')" <% nvram_match_x("LANHostConfig","wcn_enable", "1", "checked"); %>>Yes</input>
		<input type="radio" value="0" name="wcn_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'wcn_enable', '0')" <% nvram_match_x("LANHostConfig","wcn_enable", "0", "checked"); %>>No</input>
	</td>
</tr>
-->
<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#LANHostConfig_x_UPnPEnable_itemdesc#>', LEFT);" onMouseOut="return nd();">
		<#LANHostConfig_x_UPnPEnable_itemname#>
	</td>
	<td class="content_input_td">
		<input type="radio" value="1" name="upnp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'upnp_enable', '1')" <% nvram_match_x("LANHostConfig","upnp_enable", "1", "checked"); %>>Yes</input>
		<input type="radio" value="0" name="upnp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'upnp_enable', '0')" <% nvram_match_x("LANHostConfig","upnp_enable", "0", "checked"); %>>No</input>
	</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('<#LANHostConfig_x_ServerLogEnable_itemdesc#>', LEFT);" onMouseOut="return nd();"><#LANHostConfig_x_ServerLogEnable_itemname#>
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="log_ipaddr" value="<% nvram_get_x("LANHostConfig","log_ipaddr"); %>" onChange="page_changed()" onBlur="return validate_ipaddr(this, 'log_ipaddr')" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
	<td class="content_header_td" onMouseOver="return overlib('<#BOP_time_zone_desc#>', LEFT);" onMouseOut="return nd();"><#BOP_time_zone_item#></td>
	<td class="content_input_td">
		<select name="time_zone" class="content_input_fd" onChange="return change_common(this, 'LANHostConfig', 'time_zone')">
			<option class="content_input_fd" value="UCT12" <% nvram_match_x("LANHostConfig","time_zone", "UCT12","selected"); %>			>(GMT-12:00) <#TZ01#></option>
			<option class="content_input_fd" value="UCT11" <% nvram_match_x("LANHostConfig","time_zone", "UCT11","selected"); %>			>(GMT-11:00) <#TZ02#></option>
			<option class="content_input_fd" value="UCT10" <% nvram_match_x("LANHostConfig","time_zone", "UCT10","selected"); %>			>(GMT-10:00) <#TZ03#></option>
			<option class="content_input_fd" value="NAST9NADT" <% nvram_match_x("LANHostConfig","time_zone", "NAST9NADT","selected"); %>		>(GMT-09:00) <#TZ04#></option>
			<option class="content_input_fd" value="PST8PDT" <% nvram_match_x("LANHostConfig","time_zone", "PST8PDT","selected"); %>		>(GMT-08:00) <#TZ05#></option>
			<option class="content_input_fd" value="MST7MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST7MDT","selected"); %>		>(GMT-07:00) <#TZ06#></option>
			<option class="content_input_fd" value="MST7" <% nvram_match_x("LANHostConfig","time_zone", "MST7","selected"); %>			>(GMT-07:00) <#TZ07#></option>
			<option class="content_input_fd" value="MST7MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST7MDT","selected"); %>		>(GMT-07:00) <#TZ08#></option>
			<option class="content_input_fd" value="CST6CDT_1" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_1","selected"); %>		>(GMT-06:00) <#TZ09#></option>
			<option class="content_input_fd" value="CST6CDT_2" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_2","selected"); %>		>(GMT-06:00) <#TZ10#></option>
			<option class="content_input_fd" value="CST6CDT_3" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_3","selected"); %>		>(GMT-06:00) <#TZ11#></option>
			<option class="content_input_fd" value="CST6CDT_3_1" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_3_1","selected"); %>	>(GMT-06:00) <#TZ12#></option>
			<option class="content_input_fd" value="UCT6" <% nvram_match_x("LANHostConfig","time_zone", "UCT6","selected"); %>			>(GMT-06:00) <#TZ13#></option>
			<option class="content_input_fd" value="EST5EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST5EDT","selected"); %>		>(GMT-05:00) <#TZ14#></option>
			<option class="content_input_fd" value="UCT5_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT5_1","selected"); %>			>(GMT-05:00) <#TZ15#></option>
			<option class="content_input_fd" value="UCT5_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT5_2","selected"); %>			>(GMT-05:00) <#TZ16#></option>
			<option class="content_input_fd" value="AST4ADT" <% nvram_match_x("LANHostConfig","time_zone", "AST4ADT","selected"); %>		>(GMT-04:00) <#TZ17#></option>
			<option class="content_input_fd" value="UCT4_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT4_1","selected"); %>			>(GMT-04:00) <#TZ18#></option>
			<option class="content_input_fd" value="UCT4_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT4_2","selected"); %>			>(GMT-04:00) <#TZ19#></option>
			<option class="content_input_fd" value="NST3.30" <% nvram_match_x("LANHostConfig","time_zone", "NST3.30","selected"); %>		>(GMT-03:30) <#TZ20#></option>
			<option class="content_input_fd" value="EBST3EBDT_1" <% nvram_match_x("LANHostConfig","time_zone", "EBST3EBDT_1","selected"); %>	>(GMT-03:00) <#TZ21#></option>
			<option class="content_input_fd" value="UCT3" <% nvram_match_x("LANHostConfig","time_zone", "UCT3","selected"); %>			>(GMT-03:00) <#TZ22#></option>
			<option class="content_input_fd" value="EBST3EBDT_2" <% nvram_match_x("LANHostConfig","time_zone", "EBST3EBDT_2","selected"); %>	>(GMT-03:00) <#TZ23#></option>
			<option class="content_input_fd" value="NORO2" <% nvram_match_x("LANHostConfig","time_zone", "NORO2","selected"); %>			>(GMT-02:00) <#TZ24#></option>
			<option class="content_input_fd" value="EUT1EUTDST" <% nvram_match_x("LANHostConfig","time_zone", "EUT1EUTDST","selected"); %>		>(GMT-01:00) <#TZ25#></option>
			<option class="content_input_fd" value="UCT1" <% nvram_match_x("LANHostConfig","time_zone", "UCT1","selected"); %>			>(GMT-01:00) <#TZ26#></option>
			<option class="content_input_fd" value="GMT0BST_1" <% nvram_match_x("LANHostConfig","time_zone", "GMT0BST_1","selected"); %>		>(GMT) <#TZ27#></option>
			<option class="content_input_fd" value="GMT0BST_2" <% nvram_match_x("LANHostConfig","time_zone", "GMT0BST_2","selected"); %>		>(GMT) <#TZ28#></option>
			<option class="content_input_fd" value="UCT-1_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_1","selected"); %>		>(GMT+01:00) <#TZ29#></option>
			<option class="content_input_fd" value="UCT-1_1_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_1_1","selected"); %>		>(GMT+01:00) <#TZ30#></option>
			<option class="content_input_fd" value="UCT-1_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_2","selected"); %>		>(GMT+01:00) <#TZ31#></option>
			<option class="content_input_fd" value="UCT-1_2_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_2_1","selected"); %>		>(GMT+01:00) <#TZ32#></option>
			<option class="content_input_fd" value="MET-1METDST" <% nvram_match_x("LANHostConfig","time_zone", "MET-1METDST","selected"); %>	>(GMT+01:00) <#TZ33#></option>
			<option class="content_input_fd" value="MET-1METDST_1" <% nvram_match_x("LANHostConfig","time_zone", "MET-1METDST_1","selected"); %>	>(GMT+01:00) <#TZ34#></option>
			<option class="content_input_fd" value="MEZ-1MESZ" <% nvram_match_x("LANHostConfig","time_zone", "MEZ-1MESZ","selected"); %>		>(GMT+01:00) <#TZ35#></option>
			<option class="content_input_fd" value="MEZ-1MESZ_1" <% nvram_match_x("LANHostConfig","time_zone", "MEZ-1MESZ_1","selected"); %>	>(GMT+01:00) <#TZ36#></option>
			<option class="content_input_fd" value="UCT-1_3" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_3","selected"); %>		>(GMT+01:00) <#TZ37#></option>
			<option class="content_input_fd" value="ET-2EETDST" <% nvram_match_x("LANHostConfig","time_zone", "ET-2EETDST","selected"); %>		>(GMT+02:00) <#TZ38#></option>
			<option class="content_input_fd" value="EST-2EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST-2EDT","selected"); %>		>(GMT+02:00) <#TZ39#></option>
			<option class="content_input_fd" value="UCT-2_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-2_1","selected"); %>		>(GMT+02:00) <#TZ40#></option>
			<option class="content_input_fd" value="UCT-2_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-2_2","selected"); %>		>(GMT+02:00) <#TZ41#></option>
			<option class="content_input_fd" value="IST-2IDT" <% nvram_match_x("LANHostConfig","time_zone", "IST-2IDT","selected"); %>		>(GMT+02:00) <#TZ42#></option>
			<option class="content_input_fd" value="SAST-2" <% nvram_match_x("LANHostConfig","time_zone", "SAST-2","selected"); %>			>(GMT+02:00) <#TZ43#></option>
			<option class="content_input_fd" value="MST-3MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST-3MDT","selected"); %>		>(GMT+03:00) <#TZ44#></option>
			<option class="content_input_fd" value="MST-3MDT_1" <% nvram_match_x("LANHostConfig","time_zone", "MST-3MDT_1","selected"); %>		>(GMT+03:00) <#TZ45#></option>
			<option class="content_input_fd" value="UCT-3_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3_1","selected"); %>		>(GMT+03:00) <#TZ46#></option>
			<option class="content_input_fd" value="UCT-3_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3_2","selected"); %>		>(GMT+03:00) <#TZ47#></option>
			<option class="content_input_fd" value="IST-3IDT" <% nvram_match_x("LANHostConfig","time_zone", "IST-3IDT","selected"); %>		>(GMT+03:00) <#TZ48#></option>
			<option class="content_input_fd" value="UCT-3.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3.30","selected"); %>		>(GMT+03:30) <#TZ49#></option>
			<option class="content_input_fd" value="UCT-4_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4_1","selected"); %>		>(GMT+04:00) <#TZ50#></option>
			<option class="content_input_fd" value="UCT-4_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4_2","selected"); %>		>(GMT+04:00) <#TZ51#></option>
			<option class="content_input_fd" value="UCT-4.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4.30","selected"); %>		>(GMT+04:30) <#TZ52#></option>
			<option class="content_input_fd" value="RFT-5RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-5RFTDST","selected"); %>	>(GMT+05:00) <#TZ53#></option>
			<option class="content_input_fd" value="UCT-5" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5","selected"); %>			>(GMT+05:00) <#TZ54#></option>
			<option class="content_input_fd" value="UCT-5.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30","selected"); %>		>(GMT+05:30) <#TZ55#></option>
			<option class="content_input_fd" value="UCT-5.30_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30_1","selected"); %>		>(GMT+05:30) <#TZ56#></option>
			<option class="content_input_fd" value="UCT-5.45" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.45","selected"); %>		>(GMT+05:45) <#TZ57#></option>
			<option class="content_input_fd" value="UCT-6" <% nvram_match_x("LANHostConfig","time_zone", "UCT-6","selected"); %>			>(GMT+06:00) <#TZ58#></option>
			<option class="content_input_fd" value="UCT-5.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30","selected"); %>		>(GMT+06:00) <#TZ59#></option>
			<option class="content_input_fd" value="RFT-6RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-6RFTDST","selected"); %>	>(GMT+06:00) <#TZ60#></option>
			<option class="content_input_fd" value="UCT-6.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-6.30","selected"); %>		>(GMT+06:30) <#TZ61#></option>
			<option class="content_input_fd" value="UCT-7" <% nvram_match_x("LANHostConfig","time_zone", "UCT-7","selected"); %>			>(GMT+07:00) <#TZ62#></option>
			<option class="content_input_fd" value="RFT-7RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-7RFTDST","selected"); %>	>(GMT+07:00) <#TZ63#></option>
			<option class="content_input_fd" value="CST-8" <% nvram_match_x("LANHostConfig","time_zone", "CST-8","selected"); %>			>(GMT+08:00) <#TZ64#></option>
			<option class="content_input_fd" value="CST-8_1" <% nvram_match_x("LANHostConfig","time_zone", "CST-8_1","selected"); %>		>(GMT+08:00) <#TZ65#></option>
			<option class="content_input_fd" value="SST-8" <% nvram_match_x("LANHostConfig","time_zone", "SST-8","selected"); %>			>(GMT+08:00) <#TZ66#></option>
			<option class="content_input_fd" value="CCT-8" <% nvram_match_x("LANHostConfig","time_zone", "CCT-8","selected"); %>			>(GMT+08:00) <#TZ67#></option>
			<option class="content_input_fd" value="WAS-8WAD" <% nvram_match_x("LANHostConfig","time_zone", "WAS-8WAD","selected"); %>		>(GMT+08:00) <#TZ68#></option>
			<option class="content_input_fd" value="UCT_8" <% nvram_match_x("LANHostConfig","time_zone", "UCT_8","selected"); %>			>(GMT+08:00) <#TZ69#></option>
			<option class="content_input_fd" value="UCT-9_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9_1","selected"); %>		>(GMT+09:00) <#TZ70#></option>
			<option class="content_input_fd" value="JST" <% nvram_match_x("LANHostConfig","time_zone", "JST","selected"); %>			>(GMT+09:00) <#TZ71#></option>
			<option class="content_input_fd" value="UCT-9_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9_2","selected"); %>		>(GMT+09:00) <#TZ72#></option>
			<option class="content_input_fd" value="CST-9.30CDT" <% nvram_match_x("LANHostConfig","time_zone", "CST-9.30CDT","selected"); %>	>(GMT+09:30) <#TZ73#></option>
			<option class="content_input_fd" value="UCT-9.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9.30","selected"); %>		>(GMT+09:30) <#TZ74#></option>
			<option class="content_input_fd" value="UCT-10_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_1","selected"); %>		>(GMT+10:00) <#TZ75#></option>
			<option class="content_input_fd" value="UCT-10_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_2","selected"); %>		>(GMT+10:00) <#TZ76#></option>
			<option class="content_input_fd" value="TST-10TDT" <% nvram_match_x("LANHostConfig","time_zone", "TST-10TDT","selected"); %>		>(GMT+10:00) <#TZ77#></option>
			<option class="content_input_fd" value="RFT-10RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-10RFTDST","selected"); %>	>(GMT+10:00) <#TZ78#></option>
			<option class="content_input_fd" value="UCT-10_5" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_5","selected"); %>		>(GMT+10:00) <#TZ79#></option>
			<option class="content_input_fd" value="UCT-11" <% nvram_match_x("LANHostConfig","time_zone", "UCT-11","selected"); %>			>(GMT+11:00) <#TZ80#></option>
			<option class="content_input_fd" value="UCT-11_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-11_1","selected"); %>		>(GMT+11:00) <#TZ81#></option>
			<option class="content_input_fd" value="UCT-12" <% nvram_match_x("LANHostConfig","time_zone", "UCT-12","selected"); %>			>(GMT+12:00) <#TZ82#></option>
			<option class="content_input_fd" value="NZST-12NZDT" <% nvram_match_x("LANHostConfig","time_zone", "NZST-12NZDT","selected"); %>	>(GMT+12:00) <#TZ83#></option>
			<option class="content_input_fd" value="UCT-13" <% nvram_match_x("LANHostConfig","time_zone", "UCT-13","selected"); %>			>(GMT+13:00) <#TZ84#></option>
		</select>
	</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('<#LANHostConfig_x_NTPServer1_itemdesc#>', LEFT);" onMouseOut="return nd();"><#LANHostConfig_x_NTPServer1_itemname#>
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="ntp_server0" value="<% nvram_get_x("LANHostConfig","ntp_server0"); %>" onChange="page_changed()" onKeyPress="return is_string(this)" onBlur="validate_string3(this)"><a href="javascript:openLink('x_NTPServer1')" class="content_input_link" name="x_NTPServer1_link"><#LANHostConfig_x_NTPServer1_linkname#>
             </a></td>
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
