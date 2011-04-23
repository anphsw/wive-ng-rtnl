<html>
<head>
<title>Internet Services Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

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

function initTranslation()
{
	_TR("lTitle", "services misc title");
	_TR("lIntroduction", "services misc introduction");

	_TR("lStp", "lan stp");
	_TR("lStpD", "inet disable");
	_TR("lStpE", "inet enable");

	_TR("lLltd", "lan lltd");
	_TR("lLltdD", "inet disable");
	_TR("lLltdE", "inet enable");

	_TR("lIgmpp", "lan igmpp");
	_TR("lIgmppD", "inet disable");
	_TR("lIgmppE", "inet enable");

	_TR("lUpnp", "lan upnp");
	_TR("lUpnpD", "inet disable");
	_TR("lUpnpE", "inet enable");

	_TR("lRadvd", "lan radvd");
	_TR("lRadvdD", "inet disable");
	_TR("lRadvdE", "inet enable");

	_TR("lPppoer", "lan pppoer");
	_TR("lPppoerD", "inet disable");
	_TR("lPppoerE", "inet enable");

	_TR("lDnsp", "lan dnsp");
	_TR("lDnspD", "inet disable");
	_TR("lDnspE", "inet enable");

	_TRV("lApply", "inet apply");
	_TRV("lCancel", "inet cancel");
}

function initValue()
{
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var stp = <% getCfgZero(1, "stpEnabled"); %>;
	var igmp = <% getCfgZero(1, "igmpEnabled"); %>;
	var upnp = <% getCfgZero(1, "upnpEnabled"); %>;
	var radvd = <% getCfgZero(1, "radvdEnabled"); %>;
	var pppoe = <% getCfgZero(1, "pppoeREnabled"); %>;
	var dns = <% getCfgZero(1, "dnsPEnabled"); %>;
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	var lltd = "<% getCfgZero(1, "lltdEnabled"); %>";
	var wpf = "<% getCfgGeneral(1, "WANPingFilter"); %>";
	var lltdb = "<% getLltdBuilt(); %>";
	var igmpb = "<% getIgmpProxyBuilt(); %>";
	var upnpb = "<% getUpnpBuilt(); %>";
	var radvdb = "<% getRadvdBuilt(); %>";
	var pppoeb = "<% getPppoeRelayBuilt(); %>";
	var dnsp = "<% getDnsmasqBuilt(); %>";
	var krnl_pppoe = "<% getCfgZero(1, "pppoe_pass"); %>";
	var krnl_ipv6 = "<% getCfgZero(1, "ipv6_pass"); %>";

	initTranslation();

	var form = document.miscServiceCfg;

	form.stpEnbl.options.selectedIndex = 1*stp;
	form.igmpEnbl.options.selectedIndex = 1*igmp;
	form.upnpEnbl.options.selectedIndex = 1*upnp;
	form.radvdEnbl.options.selectedIndex = 1*radvd;
	form.pppoeREnbl.options.selectedIndex = 1*pppoe;
	form.dnspEnbl.options.selectedIndex = 1*dns;
	form.lltdEnbl.options.selectedIndex = 1*lltd;
	form.krnlPppoePass.options.selectedIndex = 1*krnl_pppoe;
	form.krnlIpv6Pass.options.selectedIndex = 1*krnl_ipv6;
	form.pingWANEnbl.options.selectedIndex = (wpf == "1") ? 1 : 0;

	form.rmtHTTP.value = defaultNumber("<% getCfgGeneral(1, "RemoteManagement"); %>", "1");
	form.rmtSSH.value = defaultNumber("<% getCfgGeneral(1, "RemoteSSH"); %>", "1");
	form.udpxyMode.value = defaultNumber("<% getCfgGeneral(1, "UDPXYMode"); %>", "0");
	form.watchdogEnable.value = defaultNumber("<% getCfgGeneral(1, "WatchdogEnabled"); %>", "0");
	form.dhcpSwReset.value = defaultNumber("<% getCfgGeneral(1, "dhcpSwReset"); %>", "0");

	form.bridgeFastpath.value = defaultNumber("<% getCfgGeneral(1, "bridgeFastpath"); %>", "1");
	form.CrondEnable.value = defaultNumber("<% getCfgGeneral(1, "CrondEnable"); %>", "0");
	form.ForceRenewDHCP.value = defaultNumber("<% getCfgGeneral(1, "ForceRenewDHCP"); %>", "1");

	if (lltdb == "0")
	{
		hideElement("lltd");
		form.lltdEnbl.options.selectedIndex = 0;
	}
	if (igmpb == "0")
	{
		hideElement("igmpProxy");
		form.igmpEnbl.options.selectedIndex = 0;
	}
	if (upnpb == "0")
	{
		hideElement("upnp");
		form.upnpEnbl.options.selectedIndex = 0;
	}
	if (radvdb == "0")
	{
		hideElement("radvd");
		form.radvdEnbl.options.selectedIndex = 0;
	}
	if (pppoeb == "0")
	{
		hideElement("pppoerelay");
		form.pppoeREnbl.options.selectedIndex = 0;
	}
	if (dnsp == "0")
	{
		hideElement("dnsproxy");
		form.dnspEnbl.options.selectedIndex = 0;
	}
	
	// Set-up NAT fastpath
	var qos_en = defaultNumber("<% getCfgGeneral(1, "QoSEnable"); %>", "0");
	if (qos_en == '0')
	{
		form.natFastpath.options.add(new Option('Software', '1'));
		form.natFastpath.options.add(new Option('Complex', '3'));
	}
	
	if (opmode == "4")
	{
		form.natFastpath.value = defaultNumber("0", "1");
		form.natFastpath.disabled = true;
	}
	else
		form.natFastpath.value = defaultNumber("<% getCfgGeneral(1, "natFastpath"); %>", "1");

	natFastpathSelect(form);
}

function CheckValue(form)
{
	var thresh = form.natFastpath.value;
	
	if ((thresh == '2') || (thresh == '3'))
	{
		// Check threshold
		if (!validateNum(form.hwnatThreshold.value, false))
		{
			alert("Hardware NAT threshold must be a number");
			form.hwnatThreshold.focus();
			return false;
		}
		var thr = form.hwnatThreshold.value * 1;
		if ((thr < 0) || (thr >500))
		{
			alert("Hardware NAT threshold must be a value between 0 and 500");
			form.hwnatThreshold.focus();
			return false;
		}
	}
	return true;
}

function natFastpathSelect(form)
{
	var thresh = form.natFastpath.value;
	displayElement('hwnat_threshold_row', (thresh == '2') || (thresh == '3'))
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="lTitle">Miscellaneous Services Setup</h1>
<p id="lIntroduction"></p>
<hr>

<form method=post name="miscServiceCfg" action="/goform/setMiscServices" onSubmit="return CheckValue(this);">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2">Offload engine</td>
</tr>
<tr>
<td class="head"><a name="nat_fastpath_ref"></a>NAT fastpath</td>
<td>
	<select name="natFastpath" class="half" onchange="natFastpathSelect(this.form);">
		<option value="0">Disable</option>
		<option value="2">Hardware</option>
	</select>
</td>
</tr>
<tr id="hwnat_threshold_row" style="display: none;">
<td class="head">NAT Binding Threshold</td>
<td>
	<input name="hwnatThreshold" value="<% getCfgZero(1, "hw_nat_bind"); %>" class="half">&nbsp;
	<span style="color: #c0c0c0;">(0-500)</span>
</td>
</tr>
<tr>
<td class="head">Bridge fastpath</td>
<td>
	<select name="bridgeFastpath" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>

<tr>
	<td class="title" colspan="2">Remote management</td>
</tr>
<tr>
<td class="head">HTTP Remote Management</td>
<td>
	<select name="rmtHTTP" class="half">
		<option value="0">Disable</option>
		<option value="1">LAN</option>
		<option value="2">LAN &amp; WAN</option>
	</select>
</td>
</tr>
<tr>
<td class="head">SSH Remote Management</td>
<td>
	<select name="rmtSSH" class="half">
		<option value="0">Disable</option>
		<option value="1">LAN</option>
		<option value="2">LAN &amp; WAN</option>
	</select>
</td>
</tr>

<tr>
	<td class="title" colspan="2">Pass Through</td>
</tr>
<tr>
<td class="head">PPPOE pass through</td>
<td>
	<select name="krnlPppoePass" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head">IPv6 pass through</td>
<td>
	<select name="krnlIpv6Pass" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>

<tr>
	<td class="title" colspan="2">Services</td>
</tr>
<tr id="lltd">
<td class="head" id="lLltd">LLTD daemon</td>
<td>
	<select name="lltdEnbl" class="half">
		<option value="0" id="lLltdD">Disable</option>
		<option value="1" id="lLltdE">Enable</option>
	</select>
</td>
</tr>
<tr id="igmpProxy">
<td class="head" id="lIgmpp">IGMP proxy</td>
<td>
	<select name="igmpEnbl" class="half">
		<option value="0" id="lIgmppD">Disable</option>
		<option value="1" id="lIgmppE">Enable</option>
	</select>
</td>
</tr>
<tr id="upnp">
<td class="head" id="lUpnp">UPNP support</td>
<td>
	<select name="upnpEnbl" class="half">
		<option value="0" id="lUpnpD">Disable</option>
		<option value="1" id="lUpnpE">Enable</option>
	</select>
</td>
</tr>
<tr id="radvd">
<td class="head" id="lRadvd">Router Advertisement</td>
<td>
	<select name="radvdEnbl" class="half">
		<option value="0" id="lRadvdD">Disable</option>
		<option value="1" id="lRadvdE">Enable</option>
	</select>
</td>
</tr>
<tr id="pppoerelay">
<td class="head" id="lPppoer">PPPOE relay</td>
<td>
	<select name="pppoeREnbl" class="half">
		<option value="0" id="lPppoerD">Disable</option>
		<option value="1" id="lPppoerE">Enable</option>
	</select>
</td>
</tr>
<tr id="dnsproxy">
<td class="head" id="lDnsp">DNS cached proxy</td>
<td>
	<select name="dnspEnbl" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head">Multicast to http proxy (udpxy)</td>
<td>
	<select name="udpxyMode" class="half">
		<option value="0">Disable</option>
		<option value="1">LAN</option>
		<option value="2">LAN &amp; WAN</option>
	</select>
</td>
</tr>
<tr>
<td class="head">Watchdog</td>
<td>
	<select name="watchdogEnable" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head">Cron daemon</td>
<td>
	<select name="CrondEnable" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>

<tr>
	<td class="title" colspan="2">Others</td>
</tr>
<tr>
<td class="head">Switch reinit on DHCP lease fail</td>
<td>
	<select name="dhcpSwReset" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head">Force DHCP renew lease at WAN port status change</td>
<td>
	<select name="ForceRenewDHCP" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head" id="sysfwPingFrmWANFilterHead">Accept ping from WAN</td>
<td>
	<select name="pingWANEnbl" class="half">
		<option value="0">Disable</option>
		<option value="1">Enable</option>
	</select>
</td>
</tr>
<tr>
<td class="head" id="lStp">802.1d Spanning Tree</td>
<td>
	<select name="stpEnbl" class="half">
		<option value="0" id="lStpD">Disable</option>
		<option value="1" id="lStpE">Enable</option>
	</select>
</td>
</tr>

</table>

<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
<td>
	<input type="submit" class="normal" value="Apply"  id="lApply"  onClick="TimeoutReload(20)">&nbsp;
	<input type="reset"  class="normal" value="Cancel" id="lCancel" onClick="window.location.reload()">
	<input type="hidden" value="/services/misc.asp" name="submit-url">
</td>
</tr>
</table>
</form>

</td></tr></table>
</body>
</html>
