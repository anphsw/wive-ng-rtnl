<html>
<head>
<title>Wide Area Network (WAN) Settings</title>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

<script language="JavaScript" type="text/javascript">
var http_request = false;
Butterlate.setTextDomain("internet");

function connectionTypeSwitch(form)
{
	var static = form.connectionType.value == "STATIC"
	displayElement('staticDHCP', static);
	displayElement('dhcpReqIPRow', !static);
}

function CheckValue(form)
{
	if (form.connectionType.value == 'STATIC') // STATIC
	{
		if (!validateIP(form.staticIp, true))
		{
			form.staticIp.focus();
			return false;
		}
		if (!validateIPMask(form.staticNetmask, true))
		{
			form.staticNetmask.focus();
			return false;
		}
		if (form.staticGateway.value != '')
			if (!validateIP(form.staticGateway, true))
			{
				form.staticGateway.focus();
				return false;
			}
/*		if (form.macCloneEnbl.options.selectedIndex == 1)
			if (!validateMAC(form.macCloneMac.value, true))
				return false;*/
	}
	else if (form.connectionType.value == 'DHCP')
	{
		if (form.dhcpReqIP.value != '')
			if (!validateIP(form.dhcpReqIP, true))
			{
				form.dhcpReqIP.focus();
				return false;
			}
	}

	if (form.wStaticDnsEnable.checked)
	{
		if (!validateIP(form.staticPriDns, true))
		{
			form.staticPriDns.focus();
			return false;
		}

		if (form.staticSecDns.value != '')
			if (!validateIP(form.staticSecDns, true))
			{
				form.staticSecDns.focus();
				return false;
			}
	}
	
	// Validate MTU
	if (!validateNum(form.wan_mtu.value))
	{
		alert("Invalid MTU value");
		form.wan_mtu_type.focus();
		return false;
	}
	
	var wan_mtu = form.wan_mtu.value * 1;
	if ((wan_mtu < 80) && (wan_mtu != 0))
	{
		alert("Invalid MTU value");
		return false;
	}
	
	return true;
}

function initTranslation()
{
	_TR("wTitle", "wan title");
	_TR("wIntroduction", "wan introduction");

	_TR("wConnectionType", "wan connection type");
	_TR("wConnTypeStatic", "wan connection type static");
	_TR("wConnTypeDhcp", "wan connection type dhcp");

	_TR("wStaticMode", "wan static mode");
	_TR("wStaticIp", "inet ip");
	_TR("wStaticNetmask", "inet netmask");
	_TR("wStaticGateway", "inet gateway");
	_TR("wStaticPriDns", "inet pri dns");
	_TR("wStaticSecDns", "inet sec dns");

	_TR("wDhcpMode", "wan dhcp mode");

	_TRV("wApply", "inet apply");
	_TRV("wCancel", "inet cancel");
}

function initValue()
{
	var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	var pptpMode = <% getCfgZero(1, "wan_pptp_mode"); %>;
	var nat = "<% getCfgZero(1, "natEnabled"); %>";
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var static_dns = "<% getCfgZero(1, "wan_static_dns"); %>";
	var wan_mtu = defaultNumber("<% getCfgGeneral(1, "wan_manual_mtu"); %>", '0');
	var form = document.wanCfg;
	
	form.natEnabled.checked = (nat == "1");
	var element = document.getElementById("natRowDisplay");
	if (element!=null)
		element.style.display = (opmode != "0") ? "" : "none";
	
	initTranslation();
	if (mode == "STATIC")
		form.connectionType.options.selectedIndex = 0;
	else if (mode == "DHCP")
		form.connectionType.options.selectedIndex = 1;
	else
		form.connectionType.options.selectedIndex = 0;
	form.wStaticDnsEnable.checked = (static_dns == "on");
	
	/* Check if option was set */
	form.wan_mtu.value = wan_mtu;
	for (var i=0; i < form.wan_mtu_type.options.length; i++)
		if (form.wan_mtu_type.options[i].value == wan_mtu)
		{
			form.wan_mtu_type.value = form.wan_mtu_type.options[i].value;
			break;
		}
	
	connectionTypeSwitch(form);
	dnsSwitchClick(form);
	wanMtuChange(form);
}

function dnsSwitchClick(form)
{
	var visible = (form.wStaticDnsEnable.checked) ? '' : 'none';
	var row = document.getElementById("priDNSrow");
	row.style.display = visible;
	row = document.getElementById("secDNSrow");
	row.style.display = visible;
}

function wanMtuChange(form)
{
	if (form.wan_mtu_type.value == '1')
	{
		form.wan_mtu.style.display = '';
		form.wan_mtu.setAttribute('class', 'half');
		form.wan_mtu_type.setAttribute('class', 'half');
	}
	else
	{
		form.wan_mtu_type.setAttribute('class', 'mid');
		form.wan_mtu.style.display = 'none';
		form.wan_mtu.value = form.wan_mtu_type.value;
	}
}

</script>
</head>

<body onLoad="initValue()">

<table class="body"><tr><td>

<h1 id="wTitle"></h1>
<p id="wIntroduction"></p>
<hr>

<form method="POST" name="wanCfg" action="/goform/setWan" onSubmit="return CheckValue(this);">
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
	<td><b id="wConnectionType"></b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
	<td>
		<select name="connectionType" onChange="connectionTypeSwitch(this.form);">
			<option value="STATIC" id="wConnTypeStatic">Static Mode (fixed IP)</option>
			<option value="DHCP" id="wConnTypeDhcp">DHCP (Auto Config)</option>
		</select>
	</td>
</tr>
</table>

<!-- ================= STATIC Mode ================= -->
<table id="staticDHCP" width="90%" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2" id="wStaticMode">Static Mode</td>
</tr>
<tr>
	<td class="head" id="wStaticIp">IP Address</td>
	<td><input name="staticIp" maxlength="15" value="<% getWanIp(); %>"></td>
</tr>
<tr>
	<td class="head" id="wStaticNetmask">Subnet Mask</td>
	<td><input name="staticNetmask" maxlength="15" value="<% getWanNetmask(); %>">
</td>
</tr>
<tr>
	<td class="head" id="wStaticGateway">Default Gateway</td>
	<td><input name="staticGateway" maxlength="15" value="<% getWanGateway(); %>">
</td>
</tr>
</table>
<br>

<table width="90%" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2">Additional Options</td>
</tr>
<tr id="dhcpReqIPRow">
	<td class="head">Request IP from DHCP (optional)</td>
	<td><input name="dhcpReqIP" class="mid" value="<% getCfgGeneral(1, "dhcpRequestIP"); %>"></td>
</tr>
<tr>
	<td class="head">WAN MTU</td>
	<td>
		<input name="wan_mtu" type="text" class="half" style="display:none;">
		<select name="wan_mtu_type" onChange="wanMtuChange(this.form);" class="half">
			<option value="0">AUTO</option>
			<option value="1" selected="selected">Custom</option>
			<option value="1500">1500</option>
			<option value="1492">1492</option>
			<option value="1440">1440</option>
			<option value="1400">1400</option>
			<option value="1300">1300</option>
			<option value="1200">1200</option>
			<option value="1100">1100</option>
			<option value="1000">1000</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="wMacAddressClone">Assign static DNS Server</td>
	<td><input name="wStaticDnsEnable" type="checkbox" onclick="dnsSwitchClick(this.form);" ></td>
</tr>
<tr id="priDNSrow" style="display:none;" >
	<td class="head" id="wStaticPriDns">Primary DNS Server</td>
	<td><input name="staticPriDns" class="mid" value="<% getDns(1); %>"></td>
</tr>
<tr id="secDNSrow" style="display:none;" >
	<td class="head" id="wStaticSecDns">Secondary DNS Server</td>
	<td><input name="staticSecDns" class="mid" value="<% getDns(2); %>"></td>
</tr>
<tr id="natRowDisplay">
	<td class="head" id="wMacAddressClone">Enable NAT</td>
	<td><input name="natEnabled" type="checkbox"></td>
</tr>
</table>
<br>

<table width="90%" cellpadding="2" cellspacing="1">
<tr align="center">
	<td>
		<input type="submit" class="half" value="Apply" id="wApply">&nbsp;&nbsp;
		<input type="reset" class="half" value="Cancel" id="wCancel" onClick="window.location.reload();">
		<input type="hidden" value="/internet/wan.asp" name="submit-url">
	</td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>

