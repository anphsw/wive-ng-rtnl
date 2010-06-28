<html>
<head>
<title>System Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

function initTranslation()
{
	_TR("sysfwTitle", "sysfw title");
	_TR("sysfwIntroduction", "sysfw introduction");

	_TR("sysfwPingFrmWANFilterTitle", "sysfw wanping title");
	_TR("sysfwPingFrmWANFilterHead", "sysfw wanping head");
	_TR("sysfwPingFrmWANFilterEnable", "firewall enable");
	_TR("sysfwPingFrmWANFilterDisable", "firewall disable");

	_TRV("sysfwApply", "sysfw apply");
	_TRV("sysfwReset", "sysfw reset");
}

function updateState()
{
	initTranslation();
	var wpf = "<% getCfgGeneral(1, "WANPingFilter"); %>";
	document.websSysFirewall.pingFrmWANFilterEnabled.options.selectedIndex = (wpf == "1") ? 1 : 0;
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

<form method="POST" name="websSysFirewall" action="/goform/websSysFirewall">
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2" id="sysfwPingFrmWANFilterTitle">Ping form WAN Filter</td>
</tr>
<tr>
	<td class="head" id="sysfwPingFrmWANFilterHead">
	Ping form WAN Filter
	</td>
	<td>
	<select name="pingFrmWANFilterEnabled" size="1">
	<option value=0 id="sysfwPingFrmWANFilterDisable">Disable</option>
	<option value=1 id="sysfwPingFrmWANFilterEnable">Enable</option>
	</select>
	</td>
</tr>
</table>
<hr />
<br />
<p>
	<input type="submit" value="Apply" id="sysfwApply" name="sysfwApply" > &nbsp;&nbsp;
	<input type="reset" value="Reset" id="sysfwReset" name="sysfwReset">
</p>
</form>

<br>

</tr></td></table>
</body>
</html>
