<html>
<head>
<title>Internet Services Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("internet");
Butterlate.setTextDomain("services");

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

function dhcpTypeSwitch()
{
	var form = document.dhcpCfg;
	var dhcp_off = form.lanDhcpType.options.selectedIndex != 1;
	
	form.dhcpStart.disabled = dhcp_off;
	form.dhcpEnd.disabled = dhcp_off;
	form.dhcpMask.disabled = dhcp_off;
	form.dhcpPriDns.disabled = dhcp_off;
	form.dhcpSecDns.disabled = dhcp_off;
	form.dhcpGateway.disabled = dhcp_off;
	form.dhcpLease.disabled = dhcp_off;
	form.dhcpStatic1Mac.disabled = dhcp_off;
	form.dhcpStatic1Ip.disabled = dhcp_off;
	form.dhcpStatic2Mac.disabled = dhcp_off;
	form.dhcpStatic2Ip.disabled = dhcp_off;
	form.dhcpStatic3Mac.disabled = dhcp_off;
	form.dhcpStatic3Ip.disabled = dhcp_off;
}

function initTranslation()
{
	_TR("lTitle", "services dhcp title");
	_TR("lIntroduction", "services dhcp introduction");
	_TR("lSetup", "services dhcp setup");

	_TR("lDhcpType", "lan dhcp type");
	_TR("lDhcpTypeD", "inet disable");
	_TR("lDhcpTypeS", "lan dhcp type server");
	_TR("lDhcpStart", "lan dhcp start");
	_TR("lDhcpEnd", "lan dhcp end");
	_TR("lDhcpNetmask", "inet netmask");
	_TR("lDhcpPriDns", "inet pri dns");
	_TR("lDhcpSecDns", "inet sec dns");
	_TR("lDhcpGateway", "inet gateway");
	_TR("lDhcpLease", "lan dhcp lease");
	_TR("lDhcpStatic1", "lan dhcp static");
	_TR("lDhcpStatic2", "lan dhcp static");
	_TR("lDhcpStatic3", "lan dhcp static");
	
	_TRV("lApply", "inet apply");
	_TRV("lCancel", "inet cancel");
}

function initValue()
{
	var form = document.dhcpCfg;
	var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;

	initTranslation();

	form.lanDhcpType.options.selectedIndex = 1*dhcp;
	dhcpTypeSwitch();
}

function CheckValue()
{
	var form = document.dhcpCfg;
	
	if (form.lanDhcpType.options.selectedIndex == 1)
	{
		if (!validateIP(form.dhcpStart.value, true))
		{
			form.dhcpStart.focus();
			return false;
		}
		if (!validateIP(form.dhcpEnd.value, true))
		{
			form.dhcpEnd.focus();
			return false;
		}
		if (!validateIP(form.dhcpMask.value, true))
		{
			form.dhcpMask.focus();
			return false;
		}
		if (form.dhcpPriDns.value != "")
		{
			if (!validateIP(form.dhcpPriDns.value, true))
			{
				form.dhcpPriDns.focus();
				return false;
			}
		}
		if (form.dhcpSecDns.value != "")
		{
			if (!validateIP(form.dhcpSecDns.value, true))
			{
				form.dhcpSecDns.focus();
				return false;
			}
		}
		if (!validateIP(form.dhcpGateway.value, true))
		{
			form.dhcpGateway.focus();
			return false;
		}
		if (form.dhcpStatic1Mac.value != "")
		{
			if (!validateMAC(form.dhcpStatic1Mac.value))
			{
				form.dhcpStatic1Mac.focus();
				return false;
			}
			if (!validateIP(form.dhcpStatic1Ip.value, true))
			{
				form.dhcpStatic1Ip.focus();
				return false;
			}
			form.dhcpStatic1.value = form.dhcpStatic1Mac.value + ';' + form.dhcpStatic1Ip.value;
		}
		if (form.dhcpStatic2Mac.value != "")
		{
			if (!validateMAC(form.dhcpStatic2Mac.value))
			{
				form.dhcpStatic2Mac.focus();
				return false;
			}
			if (!validateIP(form.dhcpStatic2Ip.value, true))
			{
				form.dhcpStatic2Ip.focus();
				return false;
			}
			form.dhcpStatic2.value = form.dhcpStatic2Mac.value + ';' + form.dhcpStatic2Ip.value;
		}
		if (form.dhcpStatic3Mac.value != "")
		{
			if (!validateMAC(form.dhcpStatic3Mac.value))
			{
				form.dhcpStatic3Mac.focus();
				return false;
			}
			if (!validateIP(form.dhcpStatic3Ip.value, true))
			{
				form.dhcpStatic3Ip.focus();
				return false;
			}
			form.dhcpStatic3.value = form.dhcpStatic3Mac.value + ';' + form.dhcpStatic3Ip.value;
		}
	}
	
	return true;
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="lTitle"></h1>
<p id="lIntroduction"></p>
<hr />

<form method=post name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue()">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="lSetup">DHCP Server Setup</td>
</tr>

<tr>
  <td class="head" id="lDhcpType">DHCP Type</td>
  <td>
    <select name="lanDhcpType" size="1" onChange="dhcpTypeSwitch();">
      <option value="DISABLE" id="lDhcpTypeD">Disable</option>
      <option value="SERVER" id="lDhcpTypeS">Server</option>
    </select>
  </td>
</tr>
<tr id="start">
  <td class="head" id="lDhcpStart" align="right">DHCP Start IP</td>
  <td><input name="dhcpStart" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpStart"); %>"></td>
</tr>
<tr id="end">
  <td class="head" id="lDhcpEnd" align="right">DHCP End IP</td>
  <td><input name="dhcpEnd" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpEnd"); %>"></td>
</tr>
<tr id="mask">
  <td class="head" id="lDhcpNetmask" align="right">DHCP Subnet Mask</td>
  <td><input name="dhcpMask" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpMask"); %>"></td>
</tr>
<tr id="pridns">
  <td class="head" id="lDhcpPriDns" align="right">DHCP Primary DNS</td>
  <td><input name="dhcpPriDns" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpPriDns"); %>"></td>
</tr>
<tr id="secdns">
  <td class="head" id="lDhcpSecDns" align="right">DHCP Secondary DNS</td>
  <td><input name="dhcpSecDns" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpSecDns"); %>"></td>
</tr>
<tr id="gateway">
  <td class="head" id="lDhcpGateway" align="right">DHCP Default Gateway</td>
  <td><input name="dhcpGateway" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpGateway"); %>"></td>
</tr>
<tr id="lease">
  <td class="head" id="lDhcpLease" align="right">DHCP Lease Time</td>
  <td><input name="dhcpLease" maxlength="8"
             value="<% getCfgGeneral(1, "dhcpLease"); %>"></td>
</tr>
<tr id="staticlease1">
  <td class="head" id="lDhcpStatic1" align="right">Statically Assigned</td>
  <td><input type="hidden" name="dhcpStatic1" value="">
      MAC: <input name="dhcpStatic1Mac" maxlength=17
             value="<% getCfgNthGeneral(1, "dhcpStatic1", 0); %>"><br />
      IP: <input name="dhcpStatic1Ip" maxlength=15
             value="<% getCfgNthGeneral(1, "dhcpStatic1", 1); %>"></td>
</tr>
<tr id="staticlease2">
  <td class="head" id="lDhcpStatic2" align="right">Statically Assigned</td>
  <td><input type="hidden" name="dhcpStatic2" value="">
      MAC: <input name="dhcpStatic2Mac" maxlength=17
             value="<% getCfgNthGeneral(1, "dhcpStatic2", 0); %>"><br />
      IP: <input name="dhcpStatic2Ip" maxlength=15
             value="<% getCfgNthGeneral(1, "dhcpStatic2", 1); %>"></td>
</tr>
<tr id="staticlease3">
  <td class="head" id="lDhcpStatic3" align="right">Statically Assigned</td>
  <td><input type="hidden" name="dhcpStatic3" value="">
      MAC: <input name="dhcpStatic3Mac" maxlength=17
             value="<% getCfgNthGeneral(1, "dhcpStatic3", 0); %>"><br />
      IP: <input name="dhcpStatic3Ip" maxlength=15
             value="<% getCfgNthGeneral(1, "dhcpStatic3", 1); %>"></td>
</tr>

</table>

<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type=submit style="{width:120px;}" value="Apply" id="lApply"  onClick="TimeoutReload(20)">&nbsp;&nbsp;
    <input type=reset  style="{width:120px;}" value="Cancel" id="lCancel" onClick="window.location.reload()">
  </td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>

