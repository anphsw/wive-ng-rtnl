<html>
<head>
<title>Access Point Status</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

<style type="text/css">
td.port_status {
	background-position: center center;
	background-repeat: no-repeat;
	width: 35px;
	height: 24px;
	text-align: center;
	vertical-align: middle;
	cursor: default;
}
</style>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

var wan_port = '<% getCfgZero(1, "wan_port"); %>';
var timer = null;

function showOpMode()
{
	var el = document.getElementById('inpOperationMode');
	if (el == null)
		return;
	
	var opmode = el.value;
	var s_opmode = 'Unknown';
	if (opmode == '0')
		s_opmode = 'Bridge Mode';
	else if (opmode == '1')
		s_opmode = 'Gateway Mode';
	else if (opmode == '2')
		s_opmode = 'Ethernet Converter Mode';
	else if (opmode == '3')
		s_opmode = 'AP Client Mode';
	
	ajaxModifyElementHTML('tdOperationMode', s_opmode);
}

function showPortStatus()
{
	var el = document.getElementById('inpWanPort');
	if (el == null)
		return;
	
	var pstatus = el.value.split(';');

	if (pstatus.length <= 0)
	{
		ajaxModifyElementHTML('portStatusRow', '<td>not supported</td>');
		return;
	}
	
	var wan = 1 * document.setWanForm.wan_port.value;
	var content = '';

	for (i=0; i<pstatus.length; i++)
	{
		var port = pstatus[i].split(',');
		var image = 'empty';
		
		if (port[0] == '1')
		{
			if (port[1] == '10')
				image = '10';
			else if (port[1] == '100')
				image = '100';
			else
				image = '100';
			
			if (port[2] == 'H')
				image += '_h';
		}
		
		var text = (i == wan) ? 'WAN' : (i+1);
		content = content + '<td class="port_status" style="background-image: url(\'/graphics/' + image + '.gif\'); "><b>' + text + '</b></td>';
	}
	
	ajaxModifyElementHTML('portStatusRow', '<table><td>' + content + '</td></table>');
}

function initTranslation()
{
	_TR("statusTitle", "status title");
	_TR("statusIntroduction", "status introduction");
	
	_TR("statusSysInfo", "status system information");
	_TR("statusSDKVersion", "status sdk version");
	_TR("statusSysUpTime", "status system up time");
	_TR("statusSysPlatform", "status system platform");

	_TR("statusInternetConfig", "status internet config");
	_TR("statusConnectedType", "status connect type");
	_TR("statusWANIPAddr", "status wan ipaddr");
	_TR("statusSubnetMask", "status subnet mask");
	_TR("statusDefaultGW", "status default gateway");
	_TR("statusPrimaryDNS", "status primary dns");
	_TR("statusSecondaryDNS", "status secondary dns");
	_TR("statusWANMAC", "status mac");

	_TR("statusLocalNet", "status local network");
	_TR("statusLANIPAddr", "status lan ipaddr");
	_TR("statusLocalNetmask", "status local netmask");
	_TR("statusLANMAC", "status mac");

	_TR("statusEthPortStatus", "status ethernet port status");
}

function PageInit()
{
	var ethtoolb = "<% getETHTOOLBuilt(); %>";
	if (ethtoolb == "1")
		showElement('div_ethtool');
	
	if (!((wan_port >= '0') && (wan_port <= '4')))
		wan_port = '4';
	
	document.setWanForm.wan_port.value = wan_port;
	
	reloadPage();
}

function reloadPage()
{
	ajaxLoadElement("sysinfoTable", "/adm/sysinfo.asp", onPageReload);
	timer = setTimeout('reloadPage();', 3000);
}

function onPageReload()
{
	initTranslation();
	showOpMode();
	showPortStatus();
}

function setWanPort(form)
{
	if (confirm('Changing WAN port needs to reboot you router. Do you want to proceed?'))
	{
		clearTimeout(timer);
		postForm(null, form, 'setwanReloader', '/messages/rebooting.asp');
	}
}

</script>
</head>

<body onload="PageInit()">
<table class="body"><tr><td>
<H1 id="statusTitle">Access Point Status</H1>
<P id="statusIntroduction">Let's take a look at the status. </P>

<table width="95%" border="1" cellpadding="2" cellspacing="1" id="sysinfoTable">
</table>

<form name="setWanForm" method="POST" action="/goform/setWanPort">
<table width="95%" border="1" cellpadding="2" cellspacing="1" id="sysinfoTable">
	<td class="head">
		Wan port
	</td>
	<td>
		<select name="wan_port" onchange="showPortStatus();" class="short">
			<option value="0">1</option>
			<option value="1">2</option>
			<option value="2">3</option>
			<option value="3">4</option>
			<option value="4">5</option>
		</select>
		<input type="button" class="half" value="Change port" onclick="setWanPort(this.form);" />
		<iframe id="setwanReloader" name="setwanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
	</td>
</table>
</form>

</td></tr></table>
</body>
</html>
