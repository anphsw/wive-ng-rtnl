<html>
<head>
<title>Local Area Network (LAN) Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>

<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("internet");

var secs;
var timerID = null;
var timerRunning = false;

function StartTheTimer()
{
	if (secs==0)
	{
		TimeoutReload(5);
		//window.location.reload();
		window.location.href=window.location.href;	//reload page
	}
	else
	{
		self.status = secs;
		secs = secs - 1;
		timerRunning = true;
		timerID = self.setTimeout("StartTheTimer()", 1000);
	}
}

function TimeoutReload(timeout)
{
	secs = timeout;
	if (timerRunning)
		clearTimeout(timerID);
	timerRunning = false;
	StartTheTimer();
}

function display_on()
{
	if (window.ActiveXObject) // IE
		return "block";
	else if (window.XMLHttpRequest) // Mozilla, Firefox, Safari,...
		return "table-row";
}

function initTranslation()
{
	_TR("lTitle", "lan title");
	_TR("lIntroduction", "lan introduction");
	_TR("lSetup", "lan setup");

	_TR("lHostname", "inet hostname");
	_TR("lIp", "inet ip");
	_TR("lNetmask", "inet netmask");
	_TR("lLan2", "inet lan2");
	_TR("lLan2Enable", "inet enable");
	_TR("lLan2Disable", "inet disable");
	_TR("lLan2Ip", "inet lan2 ip");
	_TR("lLan2Netmask", "inet lan2 netmask");
	_TR("lGateway", "inet gateway");
	_TR("lPriDns", "inet pri dns");
	_TR("lSecDns", "inet sec dns");
	_TR("lMac", "inet mac");

	_TRV("lApply", "inet apply");
	_TRV("lCancel", "inet cancel");
}

function initValue()
{
	var form = document.lanCfg;
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	var lan2 = "<% getCfgZero(1, "Lan2Enabled"); %>";

	initTranslation();

	if (lan2 == "1")
	{
		var lan2_ip = '<% getCfgGeneral(1, "lan2_ipaddr"); %>';
		var lan2_nm = '<% getCfgGeneral(1, "lan2_netmask"); %>';

		form.lan2enabled[0].checked = true;
		form.lan2Ip.disabled = false;
		form.lan2Ip.value = lan2_ip;
		form.lan2Netmask.disabled = false;
		form.lan2Netmask.value = lan2_nm;
	}
	else
	{
		form.lan2enabled[1].checked = true;
		form.lan2Ip.disabled = true;
		form.lan2Netmask.disabled = true;
	}

	//gateway, dns only allow to configure at bridge mode
	if (opmode != "0")
	{
		document.getElementById("brGateway").style.visibility = "hidden";
		document.getElementById("brGateway").style.display = "none";
		document.getElementById("brPriDns").style.visibility = "hidden";
		document.getElementById("brPriDns").style.display = "none";
		document.getElementById("brSecDns").style.visibility = "hidden";
		document.getElementById("brSecDns").style.display = "none";
	}

	/* ppp0 is not a disabled interface anymore..
	if (wan == "PPPOE" || wan == "L2TP" || wan == "PPTP")
	{
		document.getElementById("igmpProxy").style.visibility = "hidden";
		document.getElementById("igmpProxy").style.display = "none";
	}
	else
	{
		document.getElementById("igmpProxy").style.visibility = "visible";
		document.getElementById("igmpProxy").style.display = display_on();
	}
	*/
}

function CheckValue()
{
	var form = document.lanCfg;

	if (form.hostname.value.indexOf(" ") >= 0)
	{
		alert('Don\'t enter Blank Space in this field');
		document.lanCfg.hostname.focus();
		document.lanCfg.hostname.select();
		return false;
	}
	if (!validateIP(form.lanIp, true))
	{
		form.lanIp.focus();
		return false;
	}
	if (!validateIP(form.lanNetmask, true))
	{
		form.lanNetmask.focus();
		return false;
	}
	if (document.lanCfg.lan2enabled[0].checked)
	{
		if (!validateIP(form.lan2Ip.value, true))
		{
			form.lan2Ip.focus();
			return false;
		}
		if (!validateIP(form.lan2Netmask.value, true))
		{
			form.lan2Netmask.focus();
			return false;
		}
	}
	return true;
}

function lan2_enable_switch()
{
	var form = document.lanCfg;
	var lan2_dis = !form.lan2enabled[1].checked;
	
	form.lan2Ip.disabled = lan2_dis;
	form.lan2Netmask.disabled = lan2_dis;
}

</script>
</head>

<body onload="initValue()">
<table class="body">
<tr><td>

<h1 id="lTitle"></h1>
<p id="lIntroduction"></p>
<hr />

<form method=post name="lanCfg" action="/goform/setLan" onSubmit="return CheckValue();">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="lSetup">LAN Interface Setup</td>
</tr>
<tr <% var hashost = getHostSupp();
      if (hashost != "1") write("style=\"visibility:hidden;display:none\""); %>>
  <td class="head" id="lHostname">Hostname</td>
  <td><input name="hostname" maxlength="16" value="<% getCfgGeneral(1, "HostName"); %>"></td>
</tr>
<tr>
  <td class="head" id="lIp">IP Address</td>
  <td><input name="lanIp" maxlength="15" value="<% getLanIp(); %>" ></td>
</tr>
<tr>
  <td class="head" id="lNetmask">Subnet Mask</td>
  <td><input name="lanNetmask" maxlength="15" value="<% getLanNetmask(); %>"></td>
</tr>
<tr>
  <td class="head" id="lLan2">LAN2</td>
  <td>
    <input type="radio" name="lan2enabled" value="1" onclick="lan2_enable_switch();"><font id="lLan2Enable">Enable</font>&nbsp;
    <input type="radio" name="lan2enabled" value="0" onclick="lan2_enable_switch();" checked><font id="lLan2Disable">Disable</font>
  </td>
</tr>
<tr>
  <td class="head" id="lLan2Ip">LAN2 IP Address</td>
  <td><input name="lan2Ip" maxlength="15" value=""></td>
</tr>
<tr>
  <td class="head" id="lLan2Netmask">LAN2 Subnet Mask</td>
  <td><input name="lan2Netmask" maxlength="15" value=""></td>
</tr>
<tr id="brGateway">
  <td class="head" id="lGateway">Default Gateway</td>
  <td><input name="lanGateway" maxlength="15" value="<% getWanGateway(); %>"></td>
</tr>
<tr id="brPriDns">
  <td class="head" id="lPriDns">Primary DNS Server</td>
  <td><input name="lanPriDns" maxlength="15" value="<% getDns(1); %>"></td>
</tr>
<tr id="brSecDns">
  <td class="head" id="lSecDns">Secondary DNS Server</td>
  <td><input name="lanSecDns" maxlength="15" value="<% getDns(2); %>"></td>
</tr>
<tr>
  <td class="head" id="lMac">MAC Address</td>
  <td><% getLanMac(); %></td>
</tr>
</table>

<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type="submit" style="{width:120px;}" value="Apply" id="lApply" onClick="TimeoutReload(20);">&nbsp;&nbsp;
    <input type="reset"  style="{width:120px;}" value="Cancel" id="lCancel" onClick="window.location.reload();">
  </td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>

