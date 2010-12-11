<html>
<head>
<title>Operation Mode</title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="style/windows.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("main");

var opmode;
var old_mode;

function changeMode()
{
	var dpbsta = "<% getDpbSta(); %>";
	var ec_en = "<% getCfgZero(1, "ethConvert"); %>";
	var form = document.opmode;

	hideElement("eth_conv");

	if (document.opmode.opMode[0].checked)
	{
		opmode = 0;
		if (dpbsta == "1")
		{
			showElement("eth_conv");
			if (ec_en == "1")
				form.ethConv.options.selectedIndex = 1;
		}
	}
	else if (form.opMode[1].checked || form.opMode[3].checked)
		opmode = 1;
	else if (form.opMode[2].checked)
		opmode = 2;
}

function initTranslation()
{
	_TR("oTitle", "opmode title");
	_TR("oIntroduction", "opmode introduction");

	_TR("oModeB", "opmode mode b");
	_TR("oModeBIntro", "opmode mode b intro");
	_TR("oModeG", "opmode mode g");
	_TR("oModeGIntro", "opmode mode g intro");
	_TR("oModeE", "opmode mode e");
	_TR("stadd", "opmode mode e intro");
	_TR("oModeA", "opmode mode a");
	_TR("apclidd", "opmode mode a intro");

	_TR("oEthConv", "opmode eth conv");
	_TR("oEthConvD", "main disable");
	_TR("oEthConvE", "main enable");

	_TRV("oApply", "main apply");
	_TRV("oCancel", "main cancel");
}

function initValue()
{
	opmode = "<% getCfgZero(1, "OperationMode"); %>";
	old_mode = opmode;

	var gwb = "<% getGWBuilt(); %>";
	var apcli = "<% getWlanApcliBuilt(); %>";
	var sta = "<% getStationBuilt(); %>";
	var form = document.opmode;

	initTranslation();

	if (gwb == "0")
	{
		hideElement("gwdt");
		hideElement("oModeGIntro");
	}
	if (apcli == "0")
	{
		hideElement("apclidt");
		hideElement("apclidd");
	}
	if (sta == "0")
	{
		hideElement("stadt");
		hideElement("stadd");
	}

	if (opmode == "1")
		form.opMode[1].checked = true;
	else if (opmode == "2")
		form.opMode[2].checked = true;
	else if (opmode == "3")
		form.opMode[3].checked = true;
	else
		form.opMode[0].checked = true;
	changeMode();
}

function msg()
{
	if ((document.opmode.opMode[1].checked == true) && (<% isOnePortOnly(); %>))
		alert("In order to access web page please \nchange or alias your IP address to 172.32.1.1");
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="oTitle"></h1>
<p id="oIntroduction"></p>
<hr>

<form method="POST" name="opmode" action="/goform/setOpMode">
<dl>
	<dt>
		<input type="radio" name="opMode" id="opMode" value="0" onClick="changeMode()"><b id="oModeB">Bridge:</b>
	</dt>
	<dd id="oModeBIntro"></dd>
	<dt id="gwdt">
		<input type="radio" name="opMode" id="opMode" value="1" onClick="changeMode()"><b id="oModeG">Gateway:</b>
	</dt>
	<dd id="oModeGIntro"></dd>
	<dt id="stadt">
		<input type="radio" name="opMode" id="opMode" value="2" onClick="changeMode()"><b id="oModeE">Ethernet Converter:</b>
	</dt>
	<dd id="stadd"></dd>
	<dt id="apclidt">
		<input type="radio" name="opMode" id="opMode" value="3" onClick="changeMode()"><b id="oModeA">AP Client:</b>
	</dt>
	<dd id="apclidd"></dd>
</dl>
<table id="eth_conv" border="0" cellpadding="2" cellspacing="1">
<tr>
	<td id="oEthConv">Ethernet Converter Enabled:<td>
	<td>
		<select id="ethConv" name="ethConv" size="1">
			<option value="0" id="oEthConvD">Disable</option>
			<option value="1" id="oEthConvE">Enable</option>
		</select>
	</td>
</tr>
</table>

<p></p>

<center>
	<!--input type="button" style="{width:120px;}" value="Apply" id="oApply" onClick="msg(); document.opmode.submit(); parent.menu.location.reload();"-->
	<input type="button" style="{width:120px;}" value="Apply" id="oApply" onClick="msg(); postForm('Changing operation mode needs to reboot you router. Do you want to proceed?', this.form, 'setmodeReloader', '/messages/rebooting.asp');">
	&nbsp;&nbsp;
	<input type="reset" style="{width:120px;}" value="Reset" id="oCancel" onClick="window.location.reload()">
	<iframe id="setmodeReloader" name="setmodeReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
</center>
</form>

</td></tr></table>
</body>
</html>
