﻿<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<!-- 2005.12.20 Jiahao -->
<script language="JavaScript">
	function Add_Del_Switch(obj) {
		if (top.Mkdir_Flag == "on") {
			top.Mkdir_Flag = "";
			top.Edit_Flag = "on";
			return markGroup(obj, 'Storage_SharedList', 32, ' Mkdir ');
		} else if (top.Add_Flag == "on") {
			top.Add_Flag = "";
			return markGroup(obj, 'Storage_SharedList', 32, ' Add ');
		}
		else {
			return markGroup(obj, 'Storage_SharedList', 32, ' Del ');
		}
	}
        function xfr()
        {
        	if(document.form.computer_name2.value!="")
        	{
        		document.form.computer_name.value = decodeURIComponent(document.form.computer_name2.value);
        		document.form.computer_nameb.value = encodeURIComponent(document.form.computer_name.value);
        	}
        	else
        	{
        		document.form.computer_name.value = document.form.computer_name3.value;
        		document.form.computer_nameb.value = encodeURIComponent(document.form.computer_name.value);
        	}
        	if(document.form.samba_workgroup.value!="")
        	{
        		document.form.st_samba_workgroup.value = decodeURIComponent(document.form.samba_workgroup.value);
        		document.form.st_samba_workgroupb.value = encodeURIComponent(document.form.st_samba_workgroup.value);
        	}
        	else
        	{
        		document.form.st_samba_workgroup.value = document.form.samba_workgroup2.value;
        		document.form.st_samba_workgroupb.value = encodeURIComponent(document.form.st_samba_workgroup.value);
        	}
	}
	function blanktest(o) {
		if (o.value=="")
		{
			if (document.form.computer_name2.value!="")
				o.value=decodeURIComponent(document.form.computer_name2.value);
			else
				document.form.computer_name.value = document.form.computer_name3.value;
			alert("<#JS_Shareblanktest#>");
		}
	}
	function blanktest2(o) {
		if (o.value=="")
		{
			if (document.form.samba_workgroup.value!="")
				o.value=decodeURIComponent(document.form.samba_workgroup.value);
			else
				document.form.st_samba_workgroup.value = document.form.samba_workgroup2.value;
			alert("<#JS_Shareblanktest#>");
		}
	}
	function copytob()
	{
		document.form.computer_nameb.value = encodeURIComponent(document.form.computer_name.value);
	}
	function copytob2()
	{
		document.form.st_samba_workgroupb.value = encodeURIComponent(document.form.st_samba_workgroup.value);
	}
</script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onLoad="load_body();xfr()" onunLoad="return unload_body();">
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
    	
<input type="hidden" name="current_page" value="Advanced_StorageRight_Content.asp">
<input type="hidden" name="next_page" value="Advanced_StorageUserList_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Storage;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="sh_path_0" value="">
<input type="hidden" name="sh_name_0" value="">
<input type="hidden" name="sh_nameb_0" value="">
<input type="hidden" name="sh_comment_0" value="">
<input type="hidden" name="sh_commentb_0" value="">
<input type="hidden" name="sh_rright_x_0" value="">
<input type="hidden" name="sh_wright_x_0" value="">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">
<input type="hidden" name="computer_name2" value="<% nvram_get_x("Storage","computer_nameb"); %>">
<input type="hidden" name="computer_name3" value="<% nvram_get_x("Storage","computer_name"); %>">
<input type="hidden" name="samba_workgroup" value="<% nvram_get_x("Storage","st_samba_workgroupb"); %>">
<input type="hidden" name="samba_workgroup2" value="<% nvram_get_x("Storage","st_samba_workgroup"); %>">

<tr>
<td>
<!-- Start of table A -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">

<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2"><#t1USB#> - <#t2BC#></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('<#JS_basiconfig8#>', LEFT);" onMouseOut="return nd();">
<#BasicConfig_EnableMediaServer_itemname#></td>
<td class="content_input_td">
        <input type="radio" value="1" name="apps_dms" <% nvram_match_x("Storage", "apps_dms", "1", "checked"); %>>Yes</input>        <input type="radio" value="0" name="apps_dms" <% nvram_match_x("Storage", "apps_dms", "0", "checked"); %>>No</input>
</td>
</tr>

<tr>
<td class="content_header_td"><#ShareNode_NetworkNeighborhoodMode_itemname#></td>
<td class="content_input_td">
	<select name="st_samba_mode" class="content_input_fd" onChange="return change_common(this, 'Storage', 'st_samba_mode')">
	<option class="content_input_fd" value="0" <% nvram_match_x("Storage","st_samba_mode", "0","selected"); %>>
	Disable</option>
	<option class="content_input_fd" value="1" <% nvram_match_x("Storage","st_samba_mode", "1","selected"); %>>
	Share all partitions in disk</option>
	<option class="content_input_fd" value="2" <% nvram_match_x("Storage","st_samba_mode", "2","selected"); %>>
	Apply rules in shared node list</option>
	</select>
</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('<#JS_storageright#>', LEFT);" onMouseOut="return nd();"><#ShareNode_DeviceName_itemname#></td>
<td class="content_input_td">
	<input type="text" maxlength="32" class="content_input_fd" size="32" name="computer_name" value="" onKeyPress="return is_string2(this)" onBlur="blanktest(this);copytob()">
</td>
<input type="hidden" name="computer_nameb" value="">
</tr>

<tr>
<td class="content_header_td"><#ShareNode_WorkGroup_itemname#></td>
<td class="content_input_td">
	<input type="text" maxlength="32" class="content_input_fd" size="32" name="st_samba_workgroup" value="" onKeyPress="return is_string(this)" onBlur="blanktest2(this);copytob2()">
</td>
<input type="hidden" name="st_samba_workgroupb" value="">
</tr>

<tr>
<td class="content_header_td"><#ShareNode_FTPMode_itemname#></td>
<td class="content_input_td">
	<select name="st_ftp_mode" class="content_input_fd" onChange="return change_common(this, 'Storage', 'st_ftp_mode')">
	<option class="content_input_fd" value="0" <% nvram_match_x("Storage","st_ftp_mode", "0","selected"); %>>
	Disable</option>
	<option class="content_input_fd" value="1" <% nvram_match_x("Storage","st_ftp_mode", "1","selected"); %>>
	Login to first partition</option>
	<option class="content_input_fd" value="2" <% nvram_match_x("Storage","st_ftp_mode", "2","selected"); %>>
	Login to first matched shared node</option>
	</select>
</td>
</tr>

<tr>
<td class="content_header_td"><#ShareNode_FTPLANG_itemname#></td>
<td class="content_input_td">
	<select name="ftp_lang" class="content_input_fd" onChange="return change_common(this, 'Storage', 'ftp_lang')">
	<option class="content_input_fd" value="EN" <% nvram_match_x("Storage","ftp_lang", "EN","selected"); %>>
	<#LANG_EN#></option>
	<option class="content_input_fd" value="TW" <% nvram_match_x("Storage","ftp_lang", "TW","selected"); %>>
	<#LANG_TW#></option>
	<option class="content_input_fd" value="CN" <% nvram_match_x("Storage","ftp_lang", "CN","selected"); %>>
	<#LANG_CN#></option>
	<option class="content_input_fd" value="KR" <% nvram_match_x("Storage","ftp_lang", "KR","selected"); %>>
        <#LANG_KR#></option>
	</select>
</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('<#JS_storageMLU#>', LEFT);" onMouseOut="return nd();"><#ShareNode_MaximumLoginUser_itemname#></td>
<td class="content_input_td">
	<input type="text" maxlength="5" size="5" name="st_max_user" class="content_input_fd" value="<% nvram_get_x("Storage", "st_max_user"); %>" onBlur="validate_range(this, 1, 6)" onKeyPress="return is_number(this)">
</td>
</tr>

<tr>
<td class="content_header_td"><#ShareNode_InitialScript_itemname#></td>
<td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="run_prog" value="<% nvram_get_x("Storage","run_prog"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
</td>
</tr>

</table>
<!-- End of table A -->
</td>
</tr>

<tr>
<td height="3"></td>
</tr>

<tr>
<td>
<!-- Start of table B -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">

<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="SharedList">
<#USB_ShareNodeList_groupitemdesc#>
	<input type="hidden" name="sh_num_0" value="<% nvram_get_x("Storage", "sh_num"); %>" readonly="1">
</td>
<!-- 2005.12.23 Jiahao start -->
<td width="10%">
<div align="center">
	<input type="button" value="<#CTL_add#>" onClick="window.open('Advanced_StorageSelectFolder_Content.asp','FM','width=450,height=380')" name="SelectDirectory" size="12">
</div>
</td>
<!-- 2005.12.23 Jiahao end -->
<td width="10%">
<div align="center">
	<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return Add_Del_Switch(this)" name="Storage_SharedList" value="<#CTL_del#>" size="12">
</div>
</td>
<td width="10%">
<div align="center">
	<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'Storage_SharedList', 32, ' Edit ');" name="Storage_SharedList2" value="<#CTL_edit#>" size="12">
</div>
</td>
<td width="5%">
</td>
</tr>

<tr>
<td class="content_desc_td" colspan="6"><#USB_ShareNodeList_sectiondesc#></td>
</tr>

<!-- Start of table C -->
<table class="content_list_table" width="639" border="0" cellspacing="0" cellpadding="0">

<tr>
<td colspan="3">
<div align="center">

<!-- Start of table D -->
<table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0" width="665">

<!-- 2005.12.23 Jiahao
<tr>
<td class="content_list_field_header_td" width="359">Folders and Partitions</td>
<td class="content_list_field_header_td" width="292">
	<input type="button" value="Select Folder" onClick="window.open('Advanced_StorageSelectFolder_Content.asp','FM','width=450,height=380')" name="SelectDirectory">
</td>
</tr>
-->

<tr>
<td class="content_list_field_header_td" width="359">
<#USB_ShareNodeList_Path_itemname#></td>
<td class="content_list_field_header_td" width="292">
<#USB_ShareNodeList_SharedName_itemname#></td>
</tr>

<tr>
<td colspan="2">
	<select size="8" name="Storage_SharedList_s" multiple="true" style="font-family: 'fixedsys'">
	<% nvram_get_table_x("Storage","Storage_SharedList"); %>
	</select>
</td>
</tr>

</table>
<!-- End of table D -->
</div>
</td>
</tr>

<tr>
<td height="3"></td>
</tr>

</table>
<!-- End of table C -->

</table>
<!-- End of table B -->
</td>
</tr>

<tr>
<td>
<!-- Start of table E -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp;</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
<td id ="Confirm" height="25" width="34%">  
<div align="center">
	<font face="Arial">
	<input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" <#CTL_restore#> " name="action">
	</font>
</div> 
</td>  
<td height="25" width="33%">  
<div align="center">
	<font face="Arial">
	<input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" <#CTL_finish#> " name="action">
	</font>
</div> 
</td>
<td height="25" width="33%">  
<div align="center">
	<font face="Arial">
	<input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" <#CTL_apply#> " name="action">
	</font>
</div> 
</td>    
</tr>
</table>
<!-- End of table E -->
</td>
</tr>

<tr>
<td>
<!-- Start of table F -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">

<tr>
<td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   

<tr bgcolor="#FFFFFF">
<td class="content_header_td_15" align="left"><#CTL_restore#>:</td>
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
<!-- End of table F -->
</td>
</tr>

</table>
</form>
</body>
