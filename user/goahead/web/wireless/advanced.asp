<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<title>Advanced Wireless Settings</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var bgProtection = '<% getCfgZero(1, "BGProtection"); %>';
var shortPreamble = '<% getCfgZero(1, "TxPreamble"); %>';
var shortSlot = '<% getCfgZero(1, "ShortSlot"); %>';
var txBurst = '<% getCfgZero(1, "TxBurst"); %>';
var pktAggregate = '<% getCfgZero(1, "PktAggregate"); %>';
var wmmCapable = '<% getCfgZero(1, "WmmCapable"); %>';
var APSDCapable = '<% getCfgZero(1, "APSDCapable"); %>';
var DLSCapable = '<% getCfgZero(1, "DLSCapable"); %>';
var countrycode = '<% getCfgGeneral(1, "CountryCode"); %>';
var DLSBuilt = '<% getDLSBuilt(); %>';
var m2uBuilt = '<% getWlanM2UBuilt(); %>';
var m2uEnabled = '<% getCfgZero(1, "M2UEnabled"); %>';
var carrierib = '<% getCarrierBuilt(); %>';

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function initTranslation()
{
	_TR("advTitle", "adv title");
	_TR("advIntroduction", "adv introduction");

	_TR("advWireless", "adv wireless");
	_TR("advBGProtect", "adv bgpro");
	_TR("advBGProAuto", "wireless auto");
	_TR("advBGProOn", "wireless on");
	_TR("advBGProOff", "wireless off");

	_TR("advBeaconInterval", "adv beacon interval");
	_TR("advBeaconIntervalRange", "adv beacon interval range");
	_TR("advDTIM", "adv dtim");
	_TR("advDTIMRange", "adv dtim range");
	_TR("advFrag", "adv fragment threshold");
	_TR("advFragRange", "adv fragment threshold range");
	_TR("advRTS", "adv rts threshold");
	_TR("advRTSRange", "adv rts threshold range");
	_TR("advTxPW", "adv tx power");
	_TR("advTxPWRange", "adv tx power range");
	_TR("advShortPre", "adv short preamble");
	_TR("advShortPreEnable", "wireless enable");
	_TR("advShortPreDisable", "wireless disable");
	_TR("advShortSlot", "adv short slot");
	_TR("advShortSlotEnable", "wireless enable");
	_TR("advShortSlotDisable", "wireless disable");
	_TR("advTxBurst", "adv tx burst");
	_TR("advTxBurstEnable", "wireless enable");
	_TR("advTxBurstDisable", "wireless disable");
	_TR("advPktAggr", "adv pkt aggregate");
	_TR("advPktAggrEnable", "wireless enable");
	_TR("advPktAggrDisable", "wireless disable");

	_TR("advCountryCode", "adv country code");
	_TR("advCountryCodeUS", "adv country code us");
	_TR("advCountryCodeJP", "adv country code jp");
	_TR("advCountryCodeFR", "adv country code fr");
	_TR("advCountryCodeTW", "adv country code tw");
	_TR("advCountryCodeIE", "adv country code ie");
	_TR("advCountryCodeHK", "adv country code hk");
	_TR("advCountryCodeNONE", "wireless none");
	_TR("advCarrierDetect", "adv carrier");
	_TR("advCarrierDetectEnable", "wireless enable");
	_TR("advCarrierDetectDisable", "wireless disable");

	_TR("advWiFiMM", "adv wmm");
	_TR("advWMM", "adv wmm capable");
	_TR("advWMMEnable", "wireless enable");
	_TR("advWMMDisable", "wireless disable");
	_TR("advAPDS", "adv apds capable");
	_TR("advAPDSEnable", "wireless enable");
	_TR("advAPDSDisable", "wireless disable");
	_TR("advDLS", "adv dls capable");
	_TR("advDLSEnable", "wireless enable");
	_TR("advDLSDisable", "wireless disable");
	_TR("advWMMParameter", "adv wmm parameter");
	_TRV("advWMMConf", "adv wmm configuration");

	_TR("advMul2UniConver", "adv multicast2unicast converter");
	_TR("advMul2Uni", "adv multicast2unicast");
	_TR("advMul2UniEnable", "wireless enable");
	_TR("advMul2UniDisable", "wireless disable");

	_TRV("advApply", "wireless apply");
	_TRV("advCancel", "wireless cancel");
}

function initValue()
{
	var datarateArray;

	initTranslation();
	bgProtection = 1*bgProtection;
	document.wireless_advanced.bg_protection.options.selectedIndex = bgProtection;

	shortPreamble = 1*shortPreamble;
	if (shortPreamble == 1)
	{
		document.wireless_advanced.short_preamble[0].checked = true;
		document.wireless_advanced.short_preamble[1].checked = false;
	}
	else
	{
		document.wireless_advanced.short_preamble[0].checked = false;
		document.wireless_advanced.short_preamble[1].checked = true;
	}

	shortSlot = 1*shortSlot;
	if (shortSlot == 1)
		document.wireless_advanced.short_slot[0].checked = true;
	else
		document.wireless_advanced.short_slot[1].checked = true;

	txBurst = 1*txBurst;
	if (txBurst == 1)
	{
		document.wireless_advanced.tx_burst[0].checked = true;
		document.wireless_advanced.tx_burst[1].checked = false;
	}
	else
	{
		document.wireless_advanced.tx_burst[0].checked = false;
		document.wireless_advanced.tx_burst[1].checked = true;
	}

	pktAggregate = 1*pktAggregate;
	if (pktAggregate == 1)
	{
		document.wireless_advanced.pkt_aggregate[0].checked = true;
		document.wireless_advanced.pkt_aggregate[1].checked = false;
	}
	else
	{
		document.wireless_advanced.pkt_aggregate[0].checked = false;
		document.wireless_advanced.pkt_aggregate[1].checked = true;
	}
	
	carrierib = 1*carrierib;
	if (carrierib == 1)
	{
		document.getElementById("div_carrier_detect").style.visibility = "visible";
		document.getElementById("div_carrier_detect").style.display = style_display_on();
		document.wireless_advanced.carrier_detect.disabled = false;
		var carrierebl = '<% getCfgZero(1, "CarrierDetect"); %>';
		if (carrierebl == "1")
		{
			document.wireless_advanced.carrier_detect[0].checked = true;
			document.wireless_advanced.carrier_detect[1].checked = false;
		}
		else
		{
			document.wireless_advanced.carrier_detect[0].checked = false;
			document.wireless_advanced.carrier_detect[1].checked = true;
		}
	}
	else
	{
		document.getElementById("div_carrier_detect").style.visibility = "hidden";
		document.getElementById("div_carrier_detect").style.display = "none";
		document.wireless_advanced.carrier_detect.disabled = true;
	}

	if (wmmCapable.indexOf("1") >= 0)
	{
		document.wireless_advanced.wmm_capable[0].checked = true;
		document.wireless_advanced.wmm_capable[1].checked = false;
	}
	else
	{
		document.wireless_advanced.wmm_capable[0].checked = false;
		document.wireless_advanced.wmm_capable[1].checked = true;
	}

	wmm_capable_enable_switch();

	APSDCapable = 1*APSDCapable;
	if (APSDCapable == 1)
	{
		document.wireless_advanced.apsd_capable[0].checked = true;
		document.wireless_advanced.apsd_capable[1].checked = false;
	}
	else
	{
		document.wireless_advanced.apsd_capable[0].checked = false;
		document.wireless_advanced.apsd_capable[1].checked = true;
	}

	if (DLSBuilt == 1)
	{
		DLSCapable = 1*DLSCapable;
		if (DLSCapable == 1)
		{
			document.wireless_advanced.dls_capable[0].checked = true;
			document.wireless_advanced.dls_capable[1].checked = false;
		}
		else
		{
			document.wireless_advanced.dls_capable[0].checked = false;
			document.wireless_advanced.dls_capable[1].checked = true;
		}
	}

	if (countrycode == "US")
		document.wireless_advanced.country_code.options.selectedIndex = 0;
	else if (countrycode == "JP")
		document.wireless_advanced.country_code.options.selectedIndex = 1;
	else if (countrycode == "FR")
		document.wireless_advanced.country_code.options.selectedIndex = 2;
	else if (countrycode == "TW")
		document.wireless_advanced.country_code.options.selectedIndex = 3;
	else if (countrycode == "IE")
		document.wireless_advanced.country_code.options.selectedIndex = 4;
	else if (countrycode == "HK")
		document.wireless_advanced.country_code.options.selectedIndex = 5;
	else if (countrycode == "NONE")
		document.wireless_advanced.country_code.options.selectedIndex = 6;
	else
		document.wireless_advanced.country_code.options.selectedIndex = 6;

	//multicase to unicast converter
	m2uBuilt = 1*m2uBuilt;
	document.getElementById("div_m2u").style.display = "none";
	if (m2uBuilt == 1)
	{
		if (window.ActiveXObject) // IE
			document.getElementById("div_m2u").style.display = "block";
		else
			document.getElementById("div_m2u").style.display = "table";

		m2uEnabled = 1*m2uEnabled;
		if (m2uEnabled == 1)
		{
			document.wireless_advanced.m2u_enable[0].checked = true;
			document.wireless_advanced.m2u_enable[1].checked = false;
		}
		else
		{
			document.wireless_advanced.m2u_enable[0].checked = false;
			document.wireless_advanced.m2u_enable[1].checked = true;
		}
	}
}

function CheckValue()
{
	if (document.wireless_advanced.beacon.value == "")
	{
		alert('Please specify Beacon Interval');
		document.wireless_advanced.beacon.focus();
		document.wireless_advanced.beacon.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.beacon.value) || document.wireless_advanced.beacon.value < 20 || document.wireless_advanced.beacon.value > 999)
	{
		alert('Invalid Beacon Interval');
		document.wireless_advanced.beacon.focus();
		document.wireless_advanced.beacon.select();
		return false;
	}

	if (document.wireless_advanced.dtim.value == "" )
	{
		alert('Please specify DTIM Interval');
		document.wireless_advanced.dtim.focus();
		document.wireless_advanced.dtim.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.dtim.value) || document.wireless_advanced.dtim.value < 1 || document.wireless_advanced.dtim.value > 255)
	{
		alert('Invalid DTIM Interval');
		document.wireless_advanced.dtim.focus();
		document.wireless_advanced.dtim.select();
		return false;
	}

	if (document.wireless_advanced.fragment.value == "" )
	{
		alert('Please specify Fragmentation Length');
		document.wireless_advanced.fragment.focus();
		document.wireless_advanced.fragment.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.fragment.value) || document.wireless_advanced.fragment.value < 1 || document.wireless_advanced.fragment.value > 2346)
	{
		alert('Invalid Fragmentation Length');
		document.wireless_advanced.fragment.focus();
		document.wireless_advanced.fragment.select();
		return false;
	}

	if (document.wireless_advanced.rts.value == "" )
	{
		alert('Please specify RTS Threshold');
		document.wireless_advanced.rts.focus();
		document.wireless_advanced.rts.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.rts.value) || document.wireless_advanced.rts.value < 1 || document.wireless_advanced.rts.value > 2347)
	{
		alert('Invalid RTS Threshold');
		document.wireless_advanced.rts.focus();
		document.wireless_advanced.rts.select();
		return false;
	}

	DLSBuilt = 1*DLSBuilt;
	if (document.wireless_advanced.wmm_capable[0].checked == true)
	{
		if (1*wmmCapable == 0)
			document.wireless_advanced.rebootAP.value = 1;
	}
	else
	{
		document.wireless_advanced.apsd_capable[1].checked = true;
		if (DLSBuilt == 1)
			document.wireless_advanced.dls_capable[1].checked = true;

		if (1*wmmCapable == 1)
			document.wireless_advanced.rebootAP.value = 1;
	}

	if (document.wireless_advanced.apsd_capable[0].checked == true)
	{
		if (1*APSDCapable == 0)
			document.wireless_advanced.rebootAP.value = 1;
	}
	else
	{
		if (1*APSDCapable == 1)
			document.wireless_advanced.rebootAP.value = 1;
	}

	if (DLSBuilt == 1)
	{
		if (document.wireless_advanced.dls_capable[0].checked == true)
		{
			if (1*DLSCapable == 0)
				document.wireless_advanced.rebootAP.value = 1;
		}
		else
		{
			if (1*DLSCapable == 1)
				document.wireless_advanced.rebootAP.value = 1;
		}
	}

	return true;
}

function open_wmm_window()
{
	window.open("wmm.asp","WMM_Parameters_List","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=640, height=480")
}

function wmm_capable_enable_switch()
{
	document.getElementById("div_apsd_capable").style.visibility = "hidden";
	document.getElementById("div_apsd_capable").style.display = "none";
	document.wireless_advanced.apsd_capable.disabled = true;
	document.getElementById("div_dls_capable").style.visibility = "hidden";
	document.getElementById("div_dls_capable").style.display = "none";
	document.wireless_advanced.dls_capable.disabled = true;

	DLSBuilt = 1*DLSBuilt;
	if (document.wireless_advanced.wmm_capable[0].checked == true)
	{
		document.getElementById("div_apsd_capable").style.visibility = "visible";
		document.getElementById("div_apsd_capable").style.display = style_display_on();
		document.wireless_advanced.apsd_capable.disabled = false;
		if (DLSBuilt == 1)
		{
			document.getElementById("div_dls_capable").style.visibility = "visible";
			document.getElementById("div_dls_capable").style.display = style_display_on();
			document.wireless_advanced.dls_capable.disabled = false;
		}
	}
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>


<h1 id="advTitle">Advanced Wireless Settings </h1>
<p id="advIntroduction">Use the Advanced Setup page to make detailed settings for the Wireless. Advanced Setup includes items that are not available from the Basic Setup page, such as Beacon Interval, Control Tx Rates and Basic Data Rates. </p>
<hr>



<form method=post name=wireless_advanced action="/goform/wirelessAdvanced" onSubmit="return CheckValue()">
<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2" id="advWireless">Advanced Wireless</td>
</tr>
<tr>
	<td class="head" id="advBGProtect">BG Protection Mode</td>
	<td>
		<select name="bg_protection" size="1">
			<option value="0" selected id="advBGProAuto">Auto</option>
			<option value="1" id="advBGProOn">On</option>
			<option value="2" id="advBGProOff">Off</option>
		</select>
	</td>
</tr>
<tr> 
	<td class="head" id="advBeaconInterval">Beacon Interval</td>
	<td>
		<input type="text" name="beacon" size="5" maxlength="3" value="<% getCfgZero(1, "BeaconPeriod"); %>"> ms <font color="#808080" id="advBeaconIntervalRange">(range 20 - 999, default 100)</font>
	</td>
</tr>
<tr>
	<td class="head" id="advDTIM">Data Beacon Rate (DTIM) </td>
	<td>
		<input type="text" name="dtim" size="5" maxlength="3" value="<% getCfgZero(1, "DtimPeriod"); %>"> ms <font color="#808080" id="advDTIMRange">(range 1 - 255, default 1)</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advFrag">Fragment Threshold</td>
	<td>
		<input type="text" name="fragment" size="5" maxlength="4" value="<% getCfgZero(1, "FragThreshold"); %>"> <font color="#808080" id="advFragRange">(range 256 - 2346, default 2346)</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advRTS">RTS Threshold</td>
	<td>
		<input type="text" name="rts" size="5" maxlength="4" value="<% getCfgZero(1, "RTSThreshold"); %>"> <font color="#808080" id="advRTSRange">(range 1 - 2347, default 2347)</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advTxPW">TX Power</td>
	<td>
		<input type="text" name="tx_power" size="5" maxlength="3" value="<% getCfgZero(1, "TxPower"); %>"> <font color="#808080" id="advTxPWRange">(range 1 - 100, default 100)</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advShortPre">Short Preamble</td>
	<td>
		<input type="radio" name="short_preamble" value="1"><font id="advShortPreEnable">Enable</font>&nbsp;
		<input type="radio" name="short_preamble" value="0"><font id="advShortPreDisable">Disable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advShortSlot">Short Slot</td>
	<td>
		<input type="radio" name="short_slot" value="1" checked><font id="advShortSlotEnable">Enable</font>&nbsp;
		<input type="radio" name="short_slot" value="0"><font id="advShortSlotDisable">Disable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advTxBurst">Tx Burst</td>
	<td>
		<input type="radio" name="tx_burst" value="1" checked><font id="advTxBurstEnable">Enable</font>&nbsp;
		<input type="radio" name="tx_burst" value="0"><font id="advTxBurstDisable">Disable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advPktAggr">Pkt_Aggregate</td>
	<td>
		<input type="radio" name="pkt_aggregate" value="1"><font id="advPktAggrEnable">Enable</font>&nbsp;
		<input type="radio" name="pkt_aggregate" value="0" checked><font id="advPktAggrDisable">Disable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advCountryCode">Country Code</td>
	<td>
		<select name="country_code">
			<option value="US" id="advCountryCodeUS">US (United States)</option>
			<option value="JP" id="advCountryCodeJP">JP (Japan)</option>
			<option value="FR" id="advCountryCodeFR">FR (France)</option>
			<option value="TW" id="advCountryCodeTW">TW (Taiwan)</option>
			<option value="IE" id="advCountryCodeIE">IE (Ireland)</option>
			<option value="HK" id="advCountryCodeHK">HK (Hong Kong)</option>
			<option value="NONE" selected id="advCountryCodeNONE">NONE</option>
		</select>
	</td>
</tr>
<tr id="div_carrier_detect" name="div_carrier_detect">
	<td class="head" id="advCarrierDetect">Carrier Detect</td>
	<td>
		<input type="radio" name="carrier_detect" value="1"><font id="advCarrierDetectEnable">Enable</font>&nbsp;
		<input type="radio" name="carrier_detect" value="0" checked><font id="advCarrierDetectDisable">Disable</font>
	</td>
</tr>
</table>

<hr>

<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
<tr> 
	<td class="title" colspan="2" id="advWiFiMM">Wi-Fi Multimedia</td>
</tr>
<tr> 
	<td class="head" id="advWMM">WMM Capable</td>
	<td>
		<input type="radio" name="wmm_capable" value="1" onClick="wmm_capable_enable_switch()" checked><font id="advWMMEnable">Enable</font>&nbsp;
		<input type="radio" name="wmm_capable" value="0" onClick="wmm_capable_enable_switch()"><font id="advWMMDisable">Disable</font>
	</td>
</tr>
<tr id="div_apsd_capable" name="div_apsd_capable">
	<td class="head" id="advAPDS">APSD Capable</td>
	<td>
		<input type="radio" name="apsd_capable" value="1"><font id="advAPDSEnable">Enable</font>&nbsp;
		<input type="radio" name="apsd_capable" value="0" checked><font id="advAPDSDisable">Disable</font>
	</td>
</tr>
<tr id="div_dls_capable" name="div_dls_capable">
	<td class="head" id="advDLS">DLS Capable</td>
	<td>
		<input type="radio" name="dls_capable" value="1"><font id="advDLSEnable">Enable</font>&nbsp;
		<input type="radio" name="dls_capable" value="0" checked><font id="advDLSDisable">Disable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="advWMMParameter">WMM Parameters</td>
	<td>
		<input type="button" name="wmm_list" value="WMM Configuration" id="advWMMConf" onClick="open_wmm_window()">
	</td>
</tr>

<input type="hidden" name="rebootAP" value="0">
</table>

<table id="div_m2u" name="div_m2u" width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2" id="advMul2UniConver">Multicast-to-Unicast Converter (IGMP Snooping)</td>
</tr>
<tr>
	<td class="head" id="advMul2Uni">Multicast-to-Unicast</td>
	<td>
		<input type="radio" name="m2u_enable" value="1"><font id="advMul2UniEnable">Enable</font>&nbsp;
		<input type="radio" name="m2u_enable" value="0"><font id="advMul2UniDisable">Disable</font>
	</td>
</tr>
</table>

<br>

<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
<tr align="center">
	<td>
		<input type="submit" class="half" value="Apply" id="advApply">&nbsp;&nbsp;
		<input type="reset"  class="half" value="Cancel" id="advCancel" onClick="window.location.reload()">
	</td>
</tr>
</table>

</form>
</td></tr></table>
</body>
</html>
