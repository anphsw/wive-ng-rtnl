﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>Del Folder</title>
<link rel="stylesheet" href="../form_style.css"  type="text/css">

<script type="text/javascript" src="../state.js"></script>
<script type="text/javascript">
var selectedPool = parent.getSelectedPool();
var selectedFolder = parent.getSelectedFolder();

function initial(){
	showtext($("selected_Folder"), selectedFolder);
	document.deleteFolderForm.Cancel.focus();
	clickevent();
}

function clickevent(){
	$("Submit").onclick = function(){
			$("pool").value = selectedPool;
			$("folder").value = selectedFolder;
			
			parent.showLoading();
			document.deleteFolderForm.submit();
			parent.hidePop("apply");
		};
}
</script>
</head>

<body onLoad="initial();">
<form method="post" name="deleteFolderForm" action="delete_sharedfolder.asp" target="hidden_frame">
<input type="hidden" name="pool" id="pool" value="">
<input type="hidden" name="folder" id="folder" value="">
  <table width="100%" class="popTable" border="0" align="center" cellpadding="0" cellspacing="0">
  <thead>
    <tr>
      <td><span><#DelFolderTitle#></span><img src="../images/button-close.gif" onClick="parent.hidePop('OverlayMask');"></td>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td height="70" valign="middle"><#DelFolderAlert#> <span id="selected_Folder" style="color:#333333; "></span> ?</td>
	</tr>

    <tr>
      <td height="30" align="right" bgcolor="#E6E6E6">
	  <input name="Submit" id="Submit" type="button" class="button" value="<#CTL_del#>">
	  <input name="Cancel" id="Cancel" type="button" class="button" value="<#CTL_Cancel#>" onClick="parent.hidePop('OverlayMask');"></td>
    </tr>
	</tbody>
  </table>
</form>  
</body>
</html>
