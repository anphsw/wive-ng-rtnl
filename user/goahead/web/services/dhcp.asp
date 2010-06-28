<html>
<head>
<title>Internet Services Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("internet");
Butterlate.setTextDomain("services");

var secs;
var timerID = null;
var timerRunning = false;

var dhcpList=
[
	<% getDhcpStaticList(); %>
];

function genTable(disabled)
{
	disabled = (disabled) ? ' disabled="disabled"' : '';

	var table = '<table class="small" style="width: 100%"><tr><th style="text-align: left;">MAC address</th><th style="text-align: left;">IP address</th><th>Action</th></tr>';
	for (var i=0; i<dhcpList.length; i++)
	{
		var row = dhcpList[i];
		table += '<tr><td>' + row[0] + '</td>';
		table += '<td>' + row[1] + '</td>';
		table += '<td style="text-align: center;"><a style="color: #ff0000;" title="Delete record" href="javascript:deleteIPItem(' + i + ');"' + disabled + '><b>[x]</b></a></td></tr>';
	}
	table += '<tr><td><input class="normal" value="" name="dhcpStaticMAC"' + disabled + '></td>';
	table += '<td><input class="normal" value="" name="dhcpStaticIP"' + disabled + '></td>';
	table += '<td style="text-align: center;"><input type="button" class="short" title="Add record" value="Add" onclick="addIPItem(this.form);"' + disabled + '></td></tr>';
	table += '</table>';
	
	var elem = document.getElementById("dhcpStaticIPList");
	if (elem!=null)
		elem.innerHTML = table;
}

function genIPTableData(form)
{
	var values = "";
	for (var i=0; i<dhcpList.length; i++)
		values += dhcpList[i][0] + ' ' + dhcpList[i][1] + "\n";
	form.dhcpAssignIP.value = values;
}

function addIPItem(form)
{
	if (!validateMAC(form.dhcpStaticMAC.value, true))
		return;
	if (!validateIP(form.dhcpStaticIP, true))
		return;
	var row = [ form.dhcpStaticMAC.value, form.dhcpStaticIP.value ];
	dhcpList[dhcpList.length] = row;
	genTable();
}

function deleteIPItem(index)
{
	if ((index>=0) && (index < dhcpList.length))
	{
		dhcpList.splice(index, 1);
		genTable();
	}
}

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
/*	form.dhcpStatic1Mac.disabled = dhcp_off;
	form.dhcpStatic1Ip.disabled = dhcp_off;
	form.dhcpStatic2Mac.disabled = dhcp_off;
	form.dhcpStatic2Ip.disabled = dhcp_off;
	form.dhcpStatic3Mac.disabled = dhcp_off;
	form.dhcpStatic3Ip.disabled = dhcp_off;*/
	
	genTable(dhcp_off);
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
		if (!validateIP(form.dhcpStart, true))
		{
			form.dhcpStart.focus();
			return false;
		}
		if (!validateIP(form.dhcpEnd, true))
		{
			form.dhcpEnd.focus();
			return false;
		}
		if (!validateIP(form.dhcpMask, true))
		{
			form.dhcpMask.focus();
			return false;
		}
		if (form.dhcpPriDns.value != "")
		{
			if (!validateIP(form.dhcpPriDns, true))
			{
				form.dhcpPriDns.focus();
				return false;
			}
		}
		if (form.dhcpSecDns.value != "")
		{
			if (!validateIP(form.dhcpSecDns, true))
			{
				form.dhcpSecDns.focus();
				return false;
			}
		}
		if (!validateIP(form.dhcpGateway, true))
		{
			form.dhcpGateway.focus();
			return false;
		}
		
		genIPTableData(form);
	}
	
	return true;
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="lTitle"></h1>
<p id="lIntroduction"></p>
<hr>

<form method="POST" name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue()">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="lSetup">DHCP Server Setup</td>
</tr>

<tr>
  <td class="head" id="lDhcpType">DHCP Server</td>
  <td>
    <select name="lanDhcpType" size="1" onChange="dhcpTypeSwitch();">
      <option value="DISABLE" id="lDhcpTypeD">Disabled</option>
      <option value="SERVER" id="lDhcpTypeS">Enabled</option>
    </select>
  </td>
</tr>
<tr id="start">
  <td class="head" id="lDhcpStart" align="right">DHCP Pool Start IP</td>
  <td><input name="dhcpStart" maxlength="15"
             value="<% getCfgGeneral(1, "dhcpStart"); %>"></td>
</tr>
<tr id="end">
  <td class="head" id="lDhcpEnd" align="right">DHCP Pool End IP</td>
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
  <td class="head" id="lDhcpLease" align="right">DHCP Lease Time (in seconds)</td>
  <td><input name="dhcpLease" maxlength="8"
             value="<% getCfgGeneral(1, "dhcpLease"); %>"></td>
</tr>
<tr>
  <td class="title" colspan="2">Static IP address assignment table:</td>
</tr>
<tr>
  <td colspan="2" id="dhcpStaticIPList"></td>
</tr>
</table>

<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
<td>
	<input type="hidden" name="dhcpAssignIP" value="">
	<input type="submit" style="{width:120px;}" value="Apply" id="lApply"  onClick="TimeoutReload(20)">&nbsp;&nbsp;
	<input type="reset"  style="{width:120px;}" value="Cancel" id="lCancel" onClick="window.location.reload()">
</td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>
