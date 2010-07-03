<html>
<head>
<title>Operation Mode</title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("main");

var opmode;
var old_mode;

function changeMode()
{
	var dpbsta = "<% getDpbSta(); %>";
	var ec_en = "<% getCfgZero(1, "ethConvert"); %>";
	var mii_built = "<% getMiiInicBuilt(); %>"
	var mii_en = "<% getCfg2Zero(1, "InicMiiEnable"); %>";

	document.getElementById("eth_conv").style.visibility = "hidden";
	document.getElementById("eth_conv").style.display = "none";
	document.getElementById("eth_conv").style.disable = true;
	document.getElementById("miiInic").style.visibility = "hidden";
	document.getElementById("miiInic").style.display = "none";
	document.getElementById("miiInic").style.disable = true;

	if (document.opmode.opMode[0].checked) {
		opmode = 0;
		if (dpbsta == "1") {
			document.getElementById("eth_conv").style.visibility = "visible";
			document.getElementById("eth_conv").style.display = "block";
			document.getElementById("eth_conv").style.disable = false;
			if (ec_en == "1") {
				document.opmode.ethConv.options.selectedIndex = 1;
			}
		}
		if (mii_built == "1") {
			document.getElementById("miiInic").style.visibility = "visible";
			document.getElementById("miiInic").style.display = "block";
			document.getElementById("miiInic").style.disable = false;
			if (mii_en == "1") {
				document.opmode.miiMode.options.selectedIndex = 1;
			}
		}
	}
	else if (document.opmode.opMode[1].checked || document.opmode.opMode[3].checked) {
		opmode = 1;
	}
	else if (document.opmode.opMode[2].checked) {
		opmode = 2;
	}
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

	initTranslation();

	if (gwb == "0") {
		document.getElementById("gwdt").style.visibility = "hidden";
		document.getElementById("gwdt").style.display = "none";
		document.getElementById("oModeGIntro").style.visibility = "hidden";
		document.getElementById("oModeGIntro").style.display = "none";
	}
	if (apcli == "0") {
		document.getElementById("apclidt").style.visibility = "hidden";
		document.getElementById("apclidt").style.display = "none";
		document.getElementById("apclidd").style.visibility = "hidden";
		document.getElementById("apclidd").style.display = "none";
	}
	if (sta == "0") {
		document.getElementById("stadt").style.visibility = "hidden";
		document.getElementById("stadt").style.display = "none";
		document.getElementById("stadd").style.visibility = "hidden";
		document.getElementById("stadd").style.display = "none";
	}

	if (opmode == "1")
		document.opmode.opMode[1].checked = true;
	else if (opmode == "2")
		document.opmode.opMode[2].checked = true;
	else if (opmode == "3")
		document.opmode.opMode[3].checked = true;
	else
		document.opmode.opMode[0].checked = true;
	changeMode();
}

function msg()
{
	if(document.opmode.opMode[1].checked == true && <% isOnePortOnly(); %> ){
		alert("In order to access web page please \nchange or alias your IP address to 172.32.1.1");
	}
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="oTitle"></h1>
<p id="oIntroduction"></p>
<hr>

<form method="post" name="opmode" action="/goform/setOpMode">
<dl>
  <dt><input type="radio" name="opMode" id="opMode" value="0" onClick="changeMode()"><b id="oModeB">Bridge:</b></dt>
  <dd id="oModeBIntro"></dd>
  <dt id="gwdt"><input type="radio" name="opMode" id="opMode" value="1" onClick="changeMode()"><b id="oModeG">Gateway:</b></dt>
  <dd id="oModeGIntro"></dd>
  <dt id="stadt"><input type="radio" name="opMode" id="opMode" value="2" onClick="changeMode()"><b id="oModeE">Ethernet Converter:</b></dt>
  <dd id="stadd"></dd>
  <dt id="apclidt"><input type="radio" name="opMode" id="opMode" value="3" onClick="changeMode()"><b id="oModeA">AP Client:</b></dt>
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
<table id="miiInic" border="0" cellpadding="2" cellspacing="1">
<tr>
  <td id="oMiiMode">INIC Mii Mode:<td>
  <td>
    <select id="miiMode" name="miiMode" size="1">
      <option value="0" id="oMiiModeD">Disable</option>
      <option value="1" id="oMiiModeE">Enable</option>
    </select>
  </td>
</tr>
</table>
<p />
<center>
<input type="button" style="{width:120px;}" value="Apply" id="oApply" onClick="msg(); document.opmode.submit(); parent.menu.location.reload();">&nbsp;&nbsp;
<input type="reset" style="{width:120px;}" value="Reset" id="oCancel" onClick="window.location.reload()">
</center>
</form>

</td></tr></table>
</body>
</html>
