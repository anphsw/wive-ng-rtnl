﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>AiDisk Wizard</title>
<link rel="stylesheet" type="text/css" href="../NM_style.css">
<link rel="stylesheet" type="text/css" href="aidisk.css">

<script type="text/javascript" src="/state.js"></script>
<script>
var dummyShareway = '<% nvram_get_x("LANHostConfig", "dummyShareway"); %>';

var FTP_status = parent.get_ftp_status();  // FTP  0=disable 1=enable
var FTP_mode = parent.get_share_management_status("ftp");  // if share by account. 1=no 2=yes
var accounts = [<% get_all_accounts(); %>];
var ddns_enable = '<% nvram_get_x("LANHostConfig", "ddns_enable_x"); %>';
var ddns_server = '<% nvram_get_x("LANHostConfig", "ddns_server_x"); %>';
var ddns_hostname = '<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %>';
var format_of_first_partition = parent.pool_types()[0]; //"ntfs";

function initial(){
	//parent.show_help_iframe(1);
	parent.hideLoading();
	showdisklink();
	parent.openHint(15, 1);
}

function showdisklink(){
	if(detect_mount_status() == 0){ // No USB disk plug.
		$("Nodisk_hint").style.display = 'block';
		$("AiDiskWelcome_desp").style.display = 'none';
		$("linkdiskbox").style.display = 'none';
		$("gotonext").style.display = 'none';
		return;
	}
	else if(dummyShareway != ""){  // Ever config aidisk wizard
		$("AiDiskWelcome_desp").style.display = 'none';
		$("linkdiskbox").style.display = 'block';
		$("long_btn_go").innerHTML = "<#AiDiskWelcome_set_again#>";
		
		show_share_link();
	}
	else{  // Never config aidisk wizard
		$("linkdiskbox").style.display = 'none';
	}	
	// access the disk from LAN
}

function show_share_link(){

	/*if(NN_status == 1 && nav == false)
		$("ie_link").style.display = "block";
	if(FTP_status == 1 && FTP_mode == 1)
		$("notie_link").style.display = "block";
	else{
		$("noLAN_link").style.display = "block";
		
		if(NN_status != 1)
			showtext($("noLAN_link"), "<#linktodisk_no_1#>");
		else
			showtext($("noLAN_link"), "<#linktodisk_no_2#>");
	}*/
	
	//alert("FTP"+FTP_status);
	// access the disk from WAN
	if(FTP_status == 1 && ddns_enable == 1 && ddns_server.length > 0 && ddns_hostname.length > 0){
		if(FTP_mode == 1 || dummyShareway == 0)
			$("ddnslink1").style.display = "block"; 
		else if(FTP_mode == 2){
			$("ddnslink2").style.display = "block";
		}
	}
	else{
		$("noWAN_link").style.display = "block";
		
		if(FTP_status != 1){
			//showtext($("noWAN_link"), "<linktoFTP_no_1>");
			$("noWAN_link").innerHTML = "<#linktoFTP_no_1#>";
		}
		else if(ddns_enable != 1)
			showtext($("noWAN_link"), "<#linktoFTP_no_2#>");
		else if(ddns_hostname.length <= 0)
			showtext($("noWAN_link"), "<#linktoFTP_no_3#>");
		else
			alert("FTP and ddns exception");
	}
}

function detect_mount_status(){
	var mount_num = 0;
	
	for(var i = 0; i < parent.foreign_disk_total_mounted_number().length; ++i)
		mount_num += parent.foreign_disk_total_mounted_number()[i];
	return mount_num;
}

function go_next_page(){
	document.redirectForm.action = "/aidisk/Aidisk-2.asp";
	//document.redirectForm.target = "_self";
	document.redirectForm.submit();
}
</script>
</head>

<body onload="initial();">
<form method="GET" name="redirectForm" action="">
<input type="hidden" name="flag" value="">
</form>
<table width="400" border="0" align="center" cellpadding="0" cellspacing="0" >
  <tr>
    <td height="30" valign="top" class="Bigtitle" ><#AiDiskWelcome_title#></td>
  </tr>
  <tr>
    <td class="textbox">
	<div id="AiDiskWelcome_desp">
	  <#AiDiskWelcome_desp#>
	  	<ul>
			<li><#AiDiskWelcome_desp1#></li>
	  		<li><#AiDiskWelcome_desp2#></li>
	  	</ul>
	</div>
	
	<div id="linkdiskbox" >
	<#AiDisk_wizard_text_box_title3#><br/>
	<ul>
	<!--li>	  
	  <span id="ie_link" style="display:none;"><#linktodisk#> <a href="\\<% nvram_get_x("Storage", "computer_name"); %>" target="_blank">\\<% nvram_get_x("Storage", "computer_name"); %></a></span>
	  <span id="notie_link" style="display:none;"><#linktodisk#> <a href="ftp://<% nvram_get_x("Storage", "computer_name"); %>" target="_blank">ftp://<% nvram_get_x("Storage", "computer_name"); %></a></span>
	  <span id="noLAN_link" style="display:none;"></span>
	</li-->
	<li> 
	  <span id="ddnslink1" style="display:none;">
	  	<#AiDisk_linktoFTP_fromInternet#> <a href="ftp://<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %>" target="_blank">ftp://<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %></a>
	  </span>
	  <span id="ddnslink2" style="display:none;">
	  	<#AiDisk_linktoFTP_fromInternet#> <a href="ftp://<% nvram_get_x("LANHostConfig", "acc_username0"); %>@<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %>" target="_blank">ftp://<% nvram_get_x("LANHostConfig", "acc_username0"); %>@<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %></a>
	  </span>
	  <span id="noWAN_link" style="display:none;"></span>
    </li>
	</ul>
	</div>	
	<span id="Nodisk_hint" class="alert_string"><#AiDisk_wizard_text_box_title1#></span>
	</td>
  </tr>
  <tr align="center">
    <td height="40">
      <div id="gotonext" class="long_btn">
      	<a href="javascript:go_next_page();" id="long_btn_go"><#btn_go#></a>
     </div>
    </td>  
  </tr>
</table>
</body>
</html>
