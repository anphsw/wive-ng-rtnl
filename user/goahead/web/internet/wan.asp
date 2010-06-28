<html>
<head>
<title>Wide Area Network (WAN) Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/share.js"></script>
<script language="JavaScript" type="text/javascript">
var http_request = false;
Butterlate.setTextDomain("internet");

function macCloneMacFillSubmit()
{
    http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert('Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = doFillMyMAC;

    http_request.open('POST', '/goform/getMyMAC', true);
    http_request.send('n\a');
}

function doFillMyMAC()
{
	if (http_request.readyState == 4)
	{
		if (http_request.status == 200)
		{
			document.getElementById("macCloneMac").value = http_request.responseText;
		}
		else
		{
			alert("Can\'t get the mac address.");
		}
	}
}


function macCloneSwitch()
{
	if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1)
	{
		document.getElementById("macCloneMacRow").style.visibility = "visible";
		document.getElementById("macCloneMacRow").style.display = style_display_on();
	}
	else
	{
		document.getElementById("macCloneMacRow").style.visibility = "hidden";
		document.getElementById("macCloneMacRow").style.display = "none";
	}
}

function connectionTypeSwitch()
{
	document.getElementById("static").style.visibility = "hidden";
	document.getElementById("static").style.display = "none";
	document.getElementById("dhcp").style.visibility = "hidden";
	document.getElementById("dhcp").style.display = "none";

	if (document.wanCfg.connectionType.options.selectedIndex == 0)
	{
		document.getElementById("static").style.visibility = "visible";
		document.getElementById("static").style.display = "block";
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 1)
	{
		document.getElementById("dhcp").style.visibility = "visible";
		document.getElementById("dhcp").style.display = "block";
	}
	else
	{
		document.getElementById("static").style.visibility = "visible";
		document.getElementById("static").style.display = "block";
	}
}

function CheckValue()
{
	if (document.wanCfg.connectionType.selectedIndex == 0) // STATIC
	{
		if (!validateIP(document.wanCfg.staticIp, true))
			return false;
		if (!validateIPMask(document.wanCfg.staticNetmask, true))
			return false;
		if (document.wanCfg.staticGateway.value != "")
			if (!validateIP(document.wanCfg.staticGateway, true))
				return false;
		if (document.wanCfg.staticPriDns.value != "")
			if (!validateIP(document.wanCfg.staticPriDns, true))
				return false;
		if (document.wanCfg.staticSecDns.value != "")
			if (!validateIP(document.wanCfg.staticSecDns, true))
				return false;
		if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1)
		{
			if (!validateMAC(document.wanCfg.macCloneMac.value, true))
				return false;
		}
	}
	else if (document.wanCfg.connectionType.selectedIndex == 1) // DHCP
	{
	}
	else
		return false;
	return true;
}

function initTranslation()
{
	_TR("wTitle", "wan title");
	_TR("wIntroduction", "wan introduction");

	_TR("wConnectionType", "wan connection type");
	_TR("wConnTypeStatic", "wan connection type static");
	_TR("wConnTypeDhcp", "wan connection type dhcp");
	_TR("wConnTypePppoe", "wan connection type pppoe");
	_TR("wConnTypeL2tp", "wan connection type l2tp");
	_TR("wConnTypePptp", "wan connection type pptp");

	_TR("wStaticMode", "wan static mode");
	_TR("wStaticIp", "inet ip");
	_TR("wStaticNetmask", "inet netmask");
	_TR("wStaticGateway", "inet gateway");
	_TR("wStaticPriDns", "inet pri dns");
	_TR("wStaticSecDns", "inet sec dns");

	_TR("wDhcpMode", "wan dhcp mode");
	_TR("wDhcpHost", "inet hostname");

	_TR("wMacClone", "wan mac clone");
	_TR("wMacCloneD", "inet disable");
	_TR("wMacCloneE", "inet enable");
	_TR("wMacCloneAddr", "inet mac");

	_TRV("wApply", "inet apply");
	_TRV("wCancel", "inet cancel");
}

function initValue()
{
	var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	var pptpMode = <% getCfgZero(1, "wan_pptp_mode"); %>;
	var clone = <% getCfgZero(1, "macCloneEnabled"); %>;
	var nat = <% getCfgZero(1, "wan_nat_enable"); %>;
	var form = document.wanCfg;

	initTranslation();
	if (mode == "STATIC")
		form.connectionType.options.selectedIndex = 0;
	else if (mode == "DHCP")
		form.connectionType.options.selectedIndex = 1;
	else
		form.connectionType.options.selectedIndex = 0;
	form.natEnable.checked = (nat == "on");

	connectionTypeSwitch();

	if (clone == 1)
		form.macCloneEnbl.options.selectedIndex = 1;
	else
		form.macCloneEnbl.options.selectedIndex = 0;
	macCloneSwitch();
}
</script>
</head>

<body onLoad="initValue()">

<table class="body"><tr><td>

<h1 id="wTitle"></h1>
<p id="wIntroduction"></p>
<hr />

<form method="POST" name="wanCfg" action="/goform/setWan" onSubmit="return CheckValue();">
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
	<td><b id="wConnectionType"></b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
	<td>
		<select name="connectionType" size="1" onChange="connectionTypeSwitch();">
			<option value="STATIC" id="wConnTypeStatic">Static Mode (fixed IP)</option>
			<option value="DHCP" id="wConnTypeDhcp">DHCP (Auto Config)</option>
		</select>
	</td>
</tr>
</table>

<!-- ================= STATIC Mode ================= -->
<table id="static" width="90%" border="1" cellpadding="2" cellspacing="1">
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
<tr>
	<td class="head" id="wStaticPriDns">Primary DNS Server</td>
	<td><input name="staticPriDns" maxlength="15" value="<% getDns(1); %>"></td>
</tr>
<tr>
	<td class="head" id="wStaticSecDns">Secondary DNS Server</td>
	<td><input name="staticSecDns" maxlength="15" value="<% getDns(2); %>"></td>
</tr>
</table>
<br>

<!-- ================= DHCP Mode ================= -->
<table id="dhcp" width="90%" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2" id="wDhcpMode">DHCP Mode</td>
</tr>
<tr>
	<td class="head"><div id="wDhcpHost">Host Name</div> (optional)</td>
	<td><input type=text name="hostname" size=28 maxlength=32 value=""></td>
</tr>
</table>
<br>

<!-- =========== MAC Clone =========== -->
<table width="90%" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2">Additional Options</td>
</tr>
<tr>
	<td class="head" id="wMacAddressClone">Enable NAT</td>
	<td><input name="natEnable" type="checkbox" /></td>
</tr>
<tr>
	<td class="head" id="wMacAddressClone">MAC Clone</td>
	<td>
		<select name="macCloneEnbl" size="1" onChange="macCloneSwitch()">
			<option value="0" id="wMacCloneD">Disable</option>
			<option value="1" id="wMacCloneE">Enable</option>
		</select>
	</td>
</tr>
<tr id="macCloneMacRow">
	<td class="head" id="wMacCloneAddr">MAC Address</td>
	<td>
		<input name="macCloneMac" id="macCloneMac" maxlength="17" value="<% getCfgGeneral(1, "macCloneMac"); %>">
		<input type="button" name="macCloneMacFill" id="macCloneMacFill" value="Fill my MAC" onclick="macCloneMacFillSubmit();" >
	</td>
</tr>
</table>
<br>

<table width="90%" cellpadding="2" cellspacing="1">
<tr align="center">
	<td>
		<input type=submit style="{width:120px;}" value="Apply" id="wApply">&nbsp;&nbsp;
		<input type=reset  style="{width:120px;}" value="Cancel" id="wCancel" onClick="window.location.reload();">
	</td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>

