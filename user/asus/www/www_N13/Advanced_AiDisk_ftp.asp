﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_4_2#></title>
<link rel="stylesheet" type="text/css" href="/index_style.css">
<link rel="stylesheet" type="text/css" href="/form_style.css">
<link rel="stylesheet" type="text/css" href="/aidisk/AiDisk_style.css">
<link rel="stylesheet" type="text/css" href="/SubMenu/sub_menu.css">

<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/disk_functions.js"></script>
<script type="text/javascript" src="/aidisk/AiDisk_folder_tree.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript">
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% disk_pool_mapping_info(); %>
<% available_disk_names_and_sizes(); %>

<% get_AiDisk_status(); %>
<% get_permissions_of_account(); %>

var PROTOCOL = "ftp";

//var NN_status = get_cifs_status();  // Network-Neighborhood
var FTP_status = get_ftp_status(); // FTP
var AM_to_cifs = get_share_management_status("cifs");  // Account Management for Network-Neighborhood
var AM_to_ftp = get_share_management_status("ftp");  // Account Management for FTP

var accounts = [<% get_all_accounts(); %>];

var lastClickedAccount = 0;
var selectedAccount = "";

// changedPermissions[accountName][poolName][folderName] = permission
var changedPermissions = new Array();

var folderlist = new Array();

var ddns_enable = '<% nvram_get_x("LANHostConfig", "ddns_enable_x"); %>';

function initial(){
	show_banner(1);
	show_menu(5, 4, 1);
	show_footer();
	
	// show page's control
	showShareStatusControl(PROTOCOL);
	showAccountControl(PROTOCOL);
	
	// show accounts
	showAccountMenu();
	
	// show the kinds of permission
	showPermissionTitle();
	
	// show folder's tree
	setTimeout('get_disk_tree();', 1000);
	
	// the click event of the buttons
	onEvent();
}

function show_footer(){ 
	footer_code = '<div align="center" class="bottom-image"></div>';
	footer_code +='<div align="center" class="copyright"><#footer_copyright_desc#></div>';
	
	$("footer").innerHTML = footer_code;

	flash_button();
}

function get_disk_tree(){
	if(this.isLoading == 0){
		get_layer_items("0", "gettree");
		setTimeout('get_disk_tree();', 1000);
	}
	else
		;
}

function get_accounts(){
	return this.accounts;
}

function switchAppStatus(protocol){  // turn on/off the share
	var status;
	var confirm_str_on, confirm_str_off;
	
	if(protocol == "cifs"){
		status = this.NN_status;
		
		confirm_str_off= "<#confirm_disablecifs#>";
		confirm_str_on = "<#confirm_enablecifs#>";
	}
	else if(protocol == "ftp"){
		status = this.FTP_status;
		
		confirm_str_off = "<#confirm_disableftp#>";
		confirm_str_on = "<#confirm_enableftp#>";
	}
	
	switch(status){
		case 1:
			if(confirm(confirm_str_off)){
				showLoading();
				
				document.aidiskForm.action = "/aidisk/switch_AiDisk_app.asp";
				document.aidiskForm.protocol.value = protocol;
				document.aidiskForm.flag.value = "off";
				
				document.aidiskForm.submit();
			}
			break;
		case 0:
			if(confirm(confirm_str_on)){
				showLoading();
				
				document.aidiskForm.action = "/aidisk/switch_AiDisk_app.asp";
				document.aidiskForm.protocol.value = protocol;
				document.aidiskForm.flag.value = "on";
				
				document.aidiskForm.submit();
			}
			break;
	}
}

function resultOfSwitchAppStatus(){
	refreshpage();
}

function showShareStatusControl(protocol){
	var status;
	var str_on, str_off;
	
	if(protocol == "cifs"){
		status = this.NN_status;
		
		str_on = "<#enableCIFS#>";
		str_off = "<#disableCIFS#>";
	}
	else if(protocol == "ftp"){
		status = this.FTP_status;
		
		str_on = "<#enableFTP#>";
		str_off = "<#disableFTP#>";
	}
	else
		return;
	
	switch(status){
		case 1:
			$("sharebtn").value = str_off;
			$("tableMask").style.width = "0px";
			$("accountbtn").disabled = false;
			
			showDDNS();
			break;
		case 0:
			$("sharebtn").value = str_on;
			$("tableMask").style.width = "500px";
			$("accountbtn").disabled = true;
			
			showDDNS();
			break;
	}
}

function showDDNS(){
	if(FTP_status == 1){
		$("ShareClose").style.display = "none";
		$("DDNSinfo").style.display = "block";
		
		if(ddns_enable == "1"){
			if(AM_to_ftp == 1)
				$("haveDDNS").style.display = "block";
			else
				$("haveDDNS2").style.display = "block";
		}
		else{
			$("noDDNS").style.display = "block";
		}
	}
	else{
		$("ShareClose").style.display = "block";
		$("DDNSinfo").style.display = "none";
	}
}

function switchAccount(protocol){
	var status;
	var confirm_str_on, confirm_str_off;
	
	if(protocol == "cifs")
		status = this.AM_to_cifs;
	else if(protocol == "ftp")
		status = this.AM_to_ftp;
	else
		return;
	
	confirm_str_on = "<#confirm_enableAccount#>";
	confirm_str_off = "<#confirm_disableAccount#>";
	
	switch(status){
		case 2:
		case 4:
			if(confirm(confirm_str_off)){
				document.aidiskForm.action = "/aidisk/switch_share_mode.asp";
				$("protocol").value = protocol;
				$("mode").value = "share";
				
				showLoading();
				document.aidiskForm.submit();
			}
			break;
		case 1:
		case 3:
			if(confirm(confirm_str_on)){
				document.aidiskForm.action = "/aidisk/switch_share_mode.asp";
				$("protocol").value = protocol;
				$("mode").value = "account";
				
				showLoading();
				document.aidiskForm.submit();
			}
			break;
	}
}

function resultOfSwitchShareMode(){
	refreshpage();
}

function showAccountControl(protocol){
	var status;
	var str_on, str_off;
	
	if(protocol == "cifs")
		status = this.AM_to_cifs;
	else if(protocol == "ftp")
		status = this.AM_to_ftp;
	else
		return;
	
	str_on = "<#enableAccountManage#>";
	str_off = "<#disableAccountManage#>";
	
	switch(status){
		case 2:
		case 4:
			$("accountMask").style.display = "none";
			$("accountbtn").value = str_off;
			break;
		case 1:
		case 3:
			$("accountMask").style.display = "block";
			$("accountbtn").value = str_on;
			break;
	}
}

function showAccountMenu(){
	var account_menu_code = "";
	
	if(this.accounts.length <= 0)
		account_menu_code += '<div class="noAccount"><#Noaccount#></div>\n'
	else
		for(var i = 0; i < this.accounts.length; ++i){
			account_menu_code += '<div class="accountName" id="';
			account_menu_code += "account"+i;
			account_menu_code += '" onClick="setSelectAccount(this);">'
			account_menu_code += this.accounts[i];
			account_menu_code += '</div>\n';
		}
	
	$("account_menu").innerHTML = account_menu_code;
	
	if(this.accounts.length > 0){
		if(PROTOCOL == "cifs" && (AM_to_cifs == 2 || AM_to_cifs == 4))
			setSelectAccount($("account0"));
		else if(PROTOCOL == "ftp" && AM_to_ftp == 2)
			setSelectAccount($("account0"));
	}
}

function showPermissionTitle(){
	var code = "";
	
	code += '<table border="0"><tr>';
	
	if(PROTOCOL == "cifs"){
		code += '<td width="56" align="center">R/W</td>';
		code += '<td width="56" align="center">R</td>';
		code += '<td width="56" align="center">No</td>';
	}else if(PROTOCOL == "ftp"){
		code += '<td width="42" align="center">R/W</td>';
		code += '<td width="42" align="center">W</td>';
		code += '<td width="42" align="center">R</td>';
		code += '<td width="42" align="center">No</td>';
	}
	
	code += '</tr></table>';
	
	$("permissionTitle").innerHTML = code;
}

var controlApplyBtn = 0;
function showApplyBtn(){
	if(this.controlApplyBtn == 3)
		$("changePermissionBtn").disabled = 0;
	else
		$("changePermissionBtn").disabled = 1;
}

function setSelectAccount(selectedObj){
	this.selectedAccount = selectedObj.firstChild.nodeValue;
	
	if(this.controlApplyBtn == 0 || this.controlApplyBtn == 2)
		this.controlApplyBtn += 1;
	showApplyBtn();
	onEvent();
	$("account_FTP_address_link").href = "FTP://"+this.selectedAccount+"@"+'<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %>';
	show_permissions_of_account(selectedObj, PROTOCOL);
	contrastSelectAccount(selectedObj);
}

function getSelectedAccount(){
	return this.selectedAccount;
}

function show_permissions_of_account(selectedObj, protocol){
	var accountName = selectedObj.firstChild.nodeValue;
	var poolName;
	var permissions;
	
	for(var i = 0; i < pool_names().length; ++i){
		poolName = pool_names()[i];
		if(!this.clickedFolderBarCode[poolName])
			continue;
		
		permissions = get_account_permissions_in_pool(accountName, poolName);
		for(var j = 0; j < permissions.length; ++j){
			var folderBarCode = get_folderBarCode_in_pool(poolName, permissions[j][0]);
			
			if(protocol == "cifs")
				showPermissionRadio(folderBarCode, permissions[j][1]);
			else if(protocol == "ftp")
				showPermissionRadio(folderBarCode, permissions[j][2]);
			else{
				alert("Wrong protocol when get permission!");	// system error msg. must not be translate
				return;
			}
		}
	}
}

function get_permission_of_folder(accountName, poolName, folderName, protocol){
	var permissions = get_account_permissions_in_pool(accountName, poolName);
	
	for(var i = 0; i < permissions.length; ++i)
		if(permissions[i][0] == folderName){
			if(protocol == "cifs")
				return permissions[i][1];
			else if(protocol == "ftp")
				return permissions[i][2];
			else{
				alert("Wrong protocol when get permission!");	// system error msg. must not be translate
				return;
			}
		}
	
	alert("Wrong folderName when get permission!");	// system error msg. must not be translate
}

function contrastSelectAccount(selectedObj){
	if(this.lastClickedAccount != 0){
		this.lastClickedAccount.style.marginRight = "0px";
		this.lastClickedAccount.style.background = "url(/images/AiDisk/user_icon0.gif) #F8F8F8 left no-repeat";
		this.lastClickedAccount.style.cursor = "pointer";
		this.lastClickedAccount.style.fontWeight ="normal";
	}
	
	selectedObj.style.marginRight = "-1px";
	selectedObj.style.background = "url(/images/AiDisk/user_icon.gif) #FFF left no-repeat";
	selectedObj.style.cursor = "default";
	selectedObj.style.fontWeight ="bolder";
	
	this.lastClickedAccount = selectedObj;
}

function submitChangePermission(protocol){
	var orig_permission;
	
	for(var i = 0; i < accounts.length; ++i){
		if(!this.changedPermissions[accounts[i]])
			continue;
		
		for(var j = 0; j < pool_names().length; ++j){
			if(!this.changedPermissions[accounts[i]][pool_names()[j]])
				continue;
			
			folderlist = get_sharedfolder_in_pool(pool_names()[j]);
			
			for(var k = 0; k < folderlist.length; ++k){
				if(!this.changedPermissions[accounts[i]][pool_names()[j]][folderlist[k]])
					continue;
				
				orig_permission = get_permission_of_folder(accounts[i], pool_names()[j], folderlist[k], PROTOCOL);
				if(this.changedPermissions[accounts[i]][pool_names()[j]][folderlist[k]] == orig_permission)
					continue;
				
				// the item which was set already
				if(this.changedPermissions[accounts[i]][pool_names()[j]][folderlist[k]] == -1)
					continue;
				
				document.aidiskForm.action = "/aidisk/set_account_permission.asp";
				$("account").value = accounts[i];
				$("pool").value = pool_names()[j];
				$("folder").value = folderlist[k];
				$("protocol").value = protocol;
				$("permission").value = this.changedPermissions[accounts[i]][pool_names()[j]][folderlist[k]];
				
				// mark this item which is set
				this.changedPermissions[accounts[i]][pool_names()[j]][folderlist[k]] = -1;
				/*alert("account = "+$("account").value+"\n"+
					  "pool = "+$("pool").value+"\n"+
					  "folder = "+$("folder").value+"\n"+
					  "protocol = "+$("protocol").value+"\n"+
					  "permission = "+$("permission").value);//*/
				showLoading();
				document.aidiskForm.submit();
				return;
			}
		}
	}
	
	refreshpage();
}

function changeActionButton(selectedObj, type, action, flag){
	if(type == "User")
		if(this.accounts.length <= 0)
			if(action == "Del" || action == "Mod")
				return;
	
	if(typeof(flag) == "number")
		selectedObj.src = '/images/AiDisk/'+type+action+'_'+flag+'.gif';
	else
		selectedObj.src = '/images/AiDisk/'+type+action+'.gif';
}

function resultOfCreateAccount(){
	refreshpage();
}

function onEvent(){
	// account action buttons
	if(AM_to_ftp == 2 && accounts.length < 6){
		changeActionButton($("createAccountBtn"), 'User', 'Add', 0);
		
		$("createAccountBtn").onclick = function(){
				popupWindow('OverlayMask','/aidisk/popCreateAccount.asp');
			};
		$("createAccountBtn").onmouseover = function(){
				changeActionButton(this, 'User', 'Add', 1);
			};
		$("createAccountBtn").onmouseout = function(){
				changeActionButton(this, 'User', 'Add', 0);
			};
	}
	else{
		changeActionButton($("createAccountBtn"), 'User', 'Add');
		
		$("createAccountBtn").onclick = function(){};
		$("createAccountBtn").onmouseover = function(){};
		$("createAccountBtn").onmouseout = function(){};		
		$("createAccountBtn").title = (accounts.length < 6)?"<#AddAccountTitle#>":"<#account_overflow#>";
	}
	
	if(this.accounts.length > 0 && this.selectedAccount.length > 0){
		changeActionButton($("deleteAccountBtn"), 'User', 'Del', 0);
		changeActionButton($("modifyAccountBtn"), 'User', 'Mod', 0);
		
		$("deleteAccountBtn").onclick = function(){
				if(!selectedAccount){
					alert("No chosen account!");
					return;
				}
				
				popupWindow('OverlayMask','/aidisk/popDeleteAccount.asp');
			};
		$("deleteAccountBtn").onmouseover = function(){
				changeActionButton(this, 'User', 'Del', 1);
			};
		$("deleteAccountBtn").onmouseout = function(){
				changeActionButton(this, 'User', 'Del', 0);
			};
		
		$("modifyAccountBtn").onclick = function(){
				if(!selectedAccount){
					alert("No chosen account!");
					return;
				}
				
				popupWindow('OverlayMask','/aidisk/popModifyAccount.asp');
			};
		$("modifyAccountBtn").onmouseover = function(){
				changeActionButton(this, 'User', 'Mod', 1);
			};
		$("modifyAccountBtn").onmouseout = function(){
				changeActionButton(this, 'User', 'Mod', 0);
			};
	}
	else{
		changeActionButton($("deleteAccountBtn"), 'User', 'Del');
		changeActionButton($("modifyAccountBtn"), 'User', 'Mod');
		
		$("deleteAccountBtn").onclick = function(){};
		$("deleteAccountBtn").onmouseover = function(){};
		$("deleteAccountBtn").onmouseout = function(){};
		
		$("modifyAccountBtn").onclick = function(){};
		$("modifyAccountBtn").onmouseover = function(){};
		$("modifyAccountBtn").onmouseout = function(){};
	}
	
	// folder action buttons
	if(this.selectedPool.length > 0){
		changeActionButton($("createFolderBtn"), 'Folder', 'Add', 0);
		
		$("createFolderBtn").onclick = function(){
				if(!selectedDisk){
					alert("No chosen Disk for creating the shared-folder!");
					return;
				}
				if(!selectedPool){
					alert("No chosen Partition for creating the shared-folder!");
					return;
				}
				
				popupWindow('OverlayMask','/aidisk/popCreateFolder.asp');
			};
		$("createFolderBtn").onmouseover = function(){
				changeActionButton(this, 'Folder', 'Add', 1);
			};
		$("createFolderBtn").onmouseout = function(){
				changeActionButton(this, 'Folder', 'Add', 0);
			};
	}
	else{
		changeActionButton($("createFolderBtn"), 'Folder', 'Add');
		
		$("createFolderBtn").onclick = function(){};
		$("createFolderBtn").onmouseover = function(){};
		$("createFolderBtn").onmouseout = function(){};
	}
	
	if(this.selectedFolder.length > 0){
		changeActionButton($("deleteFolderBtn"), 'Folder', 'Del', 0);
		changeActionButton($("modifyFolderBtn"), 'Folder', 'Mod', 0);
		
		$("deleteFolderBtn").onclick = function(){
				if(!selectedFolder){
					alert("No chosen folder!");
					return;
				}
				
				popupWindow('OverlayMask','/aidisk/popDeleteFolder.asp');
			};
		$("deleteFolderBtn").onmouseover = function(){
				changeActionButton(this, 'Folder', 'Del', 1);
			};
		$("deleteFolderBtn").onmouseout = function(){
				changeActionButton(this, 'Folder', 'Del', 0);
			};
		
		$("modifyFolderBtn").onclick = function(){
				if(!selectedFolder){
					alert("No chosen folder!");
					return;
				}
				
				popupWindow('OverlayMask','/aidisk/popModifyFolder.asp');
			};
		$("modifyFolderBtn").onmouseover = function(){
				changeActionButton(this, 'Folder', 'Mod', 1);
			};
		$("modifyFolderBtn").onmouseout = function(){
				changeActionButton(this, 'Folder', 'Mod', 0);
			};
	}
	else{
		changeActionButton($("deleteFolderBtn"), 'Folder', 'Del');
		changeActionButton($("modifyFolderBtn"), 'Folder', 'Mod');
		
		$("deleteFolderBtn").onclick = function(){};
		$("deleteFolderBtn").onmouseover = function(){};
		$("deleteFolderBtn").onmouseout = function(){};
		
		$("modifyFolderBtn").onclick = function(){};
		$("modifyFolderBtn").onmouseover = function(){};
		$("modifyFolderBtn").onmouseout = function(){};
	}
	
	$("changePermissionBtn").onclick = function(){
			submitChangePermission(PROTOCOL);
		};
}

function unload_body(){
	$("createAccountBtn").onclick = function(){};
	$("createAccountBtn").onmouseover = function(){};
	$("createAccountBtn").onmouseout = function(){};
	
	$("deleteAccountBtn").onclick = function(){};
	$("deleteAccountBtn").onmouseover = function(){};
	$("deleteAccountBtn").onmouseout = function(){};
	
	$("modifyAccountBtn").onclick = function(){};
	$("modifyAccountBtn").onmouseover = function(){};
	$("modifyAccountBtn").onmouseout = function(){};
	
	$("createFolderBtn").onclick = function(){};
	$("createFolderBtn").onmouseover = function(){};
	$("createFolderBtn").onmouseout = function(){};
	
	$("deleteFolderBtn").onclick = function(){};
	$("deleteFolderBtn").onmouseover = function(){};
	$("deleteFolderBtn").onmouseout = function(){};
	
	$("modifyFolderBtn").onclick = function(){};
	$("modifyFolderBtn").onmouseover = function(){};
	$("modifyFolderBtn").onmouseout = function(){};
}
</script>
</head>

<body onLoad="initial();" onunload="unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0" scrolling="no"></iframe>

<form method="post" name="aidiskForm" action="" target="hidden_frame">
<input type="hidden" name="motion" id="motion" value="">
<input type="hidden" name="layer_order" id="layer_order" value="">
<input type="hidden" name="protocol" id="protocol" value="">
<input type="hidden" name="mode" id="mode" value="">
<input type="hidden" name="flag" id="flag" value="">
<input type="hidden" name="account" id="account" value="">
<input type="hidden" name="pool" id="pool" value="">
<input type="hidden" name="folder" id="folder" value="">
<input type="hidden" name="permission" id="permission" value="">
</form>

<form name="form">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">
</form>

<table width="983" border="0" align="center" cellpadding="0" cellspacing="0" class="content">
  <tr>
	<td width="23">&nbsp;</td>				
	
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
	<td valign="top">
	  <!--=====Beginning of Main Content=====-->
	  <div id="tabMenu" class="submenuBlock"></div>
	  <table width="95%" border="0" align="center" cellpadding="0" cellspacing="0">
	  <tr>
		<td valign="top">
    <table width="500" align="center" cellpadding="4" cellspacing="0" class="shareconfig">
	  <tr>
	    <td>
		  <input id="sharebtn" type="button" value="" class="button" onClick="switchAppStatus(PROTOCOL);">
	      <input id="accountbtn" type="button" value="" class="button" onClick="switchAccount(PROTOCOL);">
		</td>
	  </tr>
    </table>
		
		<!-- The table of share. -->
	    <div id="shareStatus">
		  <!-- The mask of all share table. -->
	      <div id="tableMask"></div>
		  
		  <!-- The mask of accounts. -->
	      <div id="accountMask"></div>
		  
		  <!-- The action buttons of accounts and folders. -->
	      <table height="35" cellpadding="2" cellspacing="0" class="accountBar">
		    <tr>
			  <!-- The action buttons of accounts. -->
    	      <td width="140" height="25" valign="bottom">	
		        <img id="createAccountBtn" src="/images/AiDisk/UserAdd.gif" hspace="1" title="<#AddAccountTitle#>">
				<img id="deleteAccountBtn" src="/images/AiDisk/UserDel.gif" hspace="1" title="<#DelAccountTitle#>">
				<img id="modifyAccountBtn" src="/images/AiDisk/UserMod.gif" hspace="1" title="<#ModAccountTitle#>">
		  	  </td>
			  
			  <!-- The action buttons of folders. -->
    	  	  <td valign="bottom">
				<img id="createFolderBtn" src="/images/AiDisk/FolderAdd.gif" hspace="1" title="<#AddFolderTitle#>">
				<img id="deleteFolderBtn" src="/images/AiDisk/FolderDel.gif" hspace="1" title="<#DelFolderTitle#>">
				<img id="modifyFolderBtn" src="/images/AiDisk/FolderMod.gif" hspace="1" title="<#ModFolderTitle#>">
		  	  </td>
  			</tr>
	  	  </table>
	  	</div>
		<!-- The table of accounts and folders. -->
	    <table width="500" height="200" style="border:1px solid #CCC" align="center" cellpadding="0" cellspacing="0" bgcolor="#999999" >
  		  <tr>
		    <!-- The table of accounts. -->
    	    <td valign="top" bgcolor="#F8F8F8" >
			  <div id="account_menu"></div>
		    </td>
		    
			<!-- The table of folders. -->
    	    <td width="352" valign="top" bgcolor="#FFFFFF">
			  <table width="345"  border="0" cellspacing="0" cellpadding="0" class="FileStatusTitle">
		  	    <tr>
		    	  <td width="175" height="20" align="left">
				    <div class="machineName"><% nvram_get_f("general.log","productid"); %></div>
				    <!--<img src="/images/500icon.gif" width="20" height="20" hspace="2" align="absmiddle">--></td>
				  <td>
				    <div id="permissionTitle"></div>
				  </td>
		    	</tr>
			  </table>

			  <!-- the tree of folders -->

  		      <div id="e0" class="FdTemp" style="font-size:10pt; margin-top:2px;"></div>
			  
			  <div style="text-align:right; margin:10px auto; border-top:1px dotted #CCC; width:95%; padding:2px;">
			    <input name="changePermissionBtn" id="changePermissionBtn" type="button" value="<#CTL_apply#>" class="button" disabled="disabled">
			  </div>
		    </td>
          </tr>
	    </table>
	</td>
    <td id="help_td"  style="width:15px;" align="center" valign="top">
	<!--==============Beginning of hint content=============-->
	  <form name="hint_form"></form>
	  <div id="helpicon" onClick="openHint(0,0);" title="Click to open Help" style="display:none;margin-top:20px;"><img src="images/help.gif" /></div>
	  <div id="hintofPM">
	  <table class="Help" bgcolor="#999999" width="180" border="0" cellpadding="0" cellspacing="1" style="margin-top:20px;">
		<thead>
		<tr>
		  <td>
			<div id="helpname" class="AiHintTitle">How to Share?</div>
			<a href="javascript:closeHint();">
				<img src="images/button-close.gif" class="closebutton">
			</a>			
		  </td>
		</tr>
		</thead>		
		<tr>
		  <td valign="top">
		  	<div id="hint_body" class="hint_body2">
			  <div id="ShareClose" class="ShareClose" style="display:none; "><#FTPClose#></div>			  
			  <!-- the condition when run share. -->
			  <div id="DDNSinfo" style="display:none;">
					<!-- the info of FTP. -->
					<div id="haveDDNS" style="display:none;">
						<a href="FTP://<% nvram_get_x("LANHostConfig", "ddns_hostname_x"); %>" target="_blank"><#Clickhere#></a>
						<#HowToSharebyFTP1#>
					</div>
					
				<div id="haveDDNS2" style="display:none;">
  				  <a id="account_FTP_address_link" href="" target="_blank"><#Clickhere#></a>
				          <#HowToSharebyFTP1#>
				</div>				  
				<div id="noDDNS" style="display:none;">
                  <#HowToSharebyFTP3#><a href="ftp://<% nvram_get_x("LANHostConfig", "lan_ipaddr"); %>" target="_blank">ftp://<% nvram_get_x("LANHostConfig", "lan_ipaddr"); %></a>
				  <#HowToSharebyFTP4#>
				  	
                </div>
              </div>
              <!--p><#AiDisk_Step1_help2#></p-->
			  </div>
              <iframe name="statusframe" id="statusframe" class="statusframe" src="" frameborder="0"></iframe>
		  </td>
		</tr>
	  </table>
	  </div>
	<!--==============Ending of hint content=============-->	</td>
  </tr>
</table>
	  <!-- The table of DDNS. -->
    </td>
  <td width="10"></td>
  </tr>
</table>

<div id="footer"></div>

<!-- mask for disabling AiDisk -->
<div id="OverlayMask" class="popup_bg">
	<div align="center">
	<iframe src="" frameborder="0" scrolling="no" id="popupframe" width="400" height="400" allowtransparency="true" style="margin-top:150px;"></iframe>
	</div>
<!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]-->
</div>

</body>
</html>
