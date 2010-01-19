<html>
<head>
<title>System Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
<META HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

function initTranslation()
{
	var e = document.getElementById("sysfwTitle");
	e.innerHTML = _("sysfw title");
	e = document.getElementById("sysfwIntroduction");
	e.innerHTML = _("sysfw introduction");
	e = document.getElementById("sysfwRemoteManagementTitle");
	e.innerHTML = _("sysfw remote management title");
	e = document.getElementById("sysfwRemoteManagementHead");
	e.innerHTML = _("sysfw remote management head");
	e = document.getElementById("sysfwRemoteManagementEnable");
	e.innerHTML = _("sysfw remote management enable");
	e = document.getElementById("sysfwRemoteManagementDisable");
	e.innerHTML = _("sysfw remote management disable");
	e = document.getElementById("sysfwRemoteManagementApply");
	e.value = _("sysfw remote management apply");
	e = document.getElementById("sysfwRemoteManagementReset");
	e.value = _("sysfw remote management reset");
}

function updateState()
{
	initTranslation();

	var rm = <% getCfgGeneral(1, "RemoteManagement"); %> ;
	if(rm)
		document.remoteManagement.remoteManagementEnabled.options.selectedIndex = 1;
	else
		document.remoteManagement.remoteManagementEnabled.options.selectedIndex = 0;
}
</script>
</head>


<!--     body      -->
<body onload="updateState()">
<table class="body"><tr><td>
<h1 id="sysfwTitle"> System Firewall Settings </h1>
<% checkIfUnderBridgeModeASP(); %>
<p id="sysfwIntroduction"> You may configure the system firewall to protect itself from attacking.</p>
<hr />

<form method=post name="remoteManagement" action=/goform/remoteManagement>
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2" id="sysfwRemoteManagementTitle">Remote management</td>
</tr>
<tr>
	<td class="head" id="sysfwRemoteManagementHead">
		Remote management (via WAN)
	</td>
	<td>
	<select name="remoteManagementEnabled" size="1">
	<option value=0 id="sysfwRemoteManagementDisable">Disable</option>
	<option value=1 id="sysfwRemoteManagementEnable">Enable</option>
	</select>
	</td>
</tr>
</table>

<p>
	<input type="submit" value="Apply" id="sysfwRemoteManagementApply" name="sysfwRemoteManagementApply" > &nbsp;&nbsp;
	<input type="reset" value="Reset" id="sysfwRemoteManagementReset" name="sysfwRemoteManagementReset">
</p>
</form>

<br>

</tr></td></table>
</body>
</html>
