<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<title>Wireless Station QoS</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initValues()
{
	var form = document.sta_qos;
	
	var wmmenable = '<% getCfgZero(1, "WmmCapable"); %>';
	var apsd = '<% getCfgZero(1, "APSDCapable"); %>';
	var dls  = '<% getCfgZero(1, "DLSCapable"); %>';
	var acbe = '<% getCfgNthZero(1, "APSDAC", 0); %>';
	var acbk = '<% getCfgNthZero(1, "APSDAC", 1); %>';
	var acvi = '<% getCfgNthZero(1, "APSDAC", 2); %>';
	var acvo = '<% getCfgNthZero(1, "APSDAC", 3); %>';
	
	form.wmm_enable.checked = (wmmenable == 1);
	
	if (form.wmm_enable.checked)
	{
		form.wmm_ps_enable.checked = (apsd == '1');

		if (form.wmm_ps_enable.checked)
		{
			form.wmm_ps_mode_acbk.checked = (acbk == '1');
			form.wmm_ps_mode_acbe.checked = (acbe == '1');
			form.wmm_ps_mode_acvo.checked = (acvo == '1');
			form.wmm_ps_mode_acvi.checked = (acvi == '1');
		}

		form.wmm_dls_enable.checked = (dls == '1');
	}

	WMM_Click(form);
}

function WMM_Click(form)
{
	form.wmm_ps_enable.disabled = false;
	form.wmm_dls_enable.disabled = false;

	if (!form.wmm_enable.checked)
	{
		form.wmm_ps_enable.disabled = true;
		form.wmm_dls_enable.disabled = true;
		form.wmm_ps_enable.checked = false;
		form.wmm_dls_enable.checked = false;
	}
	WMM_PS_Click(form);
	WMM_DLS_Click(form);
}

function WMM_PS_Click(form)
{
	var dis = !form.wmm_ps_enable.checked;

	form.wmm_ps_mode_acbe.disabled = dis;
	form.wmm_ps_mode_acbk.disabled = dis;
	form.wmm_ps_mode_acvi.disabled = dis;
	form.wmm_ps_mode_acvo.disabled = dis;
}

function WMM_DLS_Click(form)
{
	var dis = !form.wmm_dls_enable.checked;
	
	form.mac0.disabled = dis;
	form.mac1.disabled = dis;
	form.mac2.disabled = dis;
	form.mac3.disabled = dis;
	form.mac4.disabled = dis;
	form.mac5.disabled = dis;
	form.timeout.disabled = dis;
}

function submit_apply(form, btntype)
{
	form.button_type.value = btntype;  // 1: wmm , 2: dls setup, 3: tear down
	form.submit();
}

function initTranslation()
{
	_TR("qosTitle", "qos title");
	_TR("qosIntroduction", "qos introduction");

	_TR("qosConfig", "qos config");
	_TR("qosWMM", "qos wmm");
	_TR("qosWMMEnable", "station enable");
	_TR("qosWMMPWSave", "qos wmm power save");
	_TR("qosPSMode", "qos wmm ps mode");
	_TR("qosWMMPWSaveEnable", "station enable");
	_TR("qosWMMDLS", "qos dls");
	_TR("qosWMMDLSEnable", "station enable");
	_TRV("qosWMMApply", "wireless apply");

	_TR("qosDLS", "qos dls");
	_TR("qosDLSMac", "stalist macaddr");
	_TR("qosDLSTimeoutValue", "qos dls timeoutvalue");
	_TR("qosSecond", "qos second");
	_TRV("qosDLSAppy", "wireless apply");

	_TR("qosDLSStaus", "qos dls status");
	_TR("qosDLSStatusMAC", "stalist macaddr");
	_TR("qosDLSStatusTimeout", "qos dls timeout");
	_TRV("qosTearDown", "qos teardown");
}

function PageInit()
{
	initTranslation();
	initValues();
}
</script>
</head>


<body onload="PageInit();">
<table class="body"><tr><td>

<h1 id="qosTitle">Station QoS Configurations</h1>
<p id="qosIntroduction">The Status page shows the settings and current operation status of the Station.</p>
<hr />

<form method="post" name="sta_qos" action="/goform/setStaQoS">
<table class="form">
<tr>
	<td class="title" colspan="2" id="qosConfig">Qos Configuration</td>
</tr>
<tr>
	<td class="head" id="qosWMM">WMM</td>
	<td><input type="checkbox" name="wmm_enable" onclick="WMM_Click(this.form);"><font id="qosWMMEnable">enable</font></td>
</tr>
<tr>
	<td class="head" id="qosWMMPWSave">WMM Power Saving</td>
	<td><input type="checkbox" name="wmm_ps_enable" onclick="WMM_PS_Click(this.form);"><font id="qosWMMPWSaveEnable">enable</font></td>
</tr>
<tr>
	<td class="head" id="qosPSMode">PS Mode</td>
	<td>
		<input type="checkbox" name="wmm_ps_mode_acbe">AC_BE &nbsp;&nbsp;
		<input type="checkbox" name="wmm_ps_mode_acbk">AC_BK &nbsp;&nbsp;
		<input type="checkbox" name="wmm_ps_mode_acvi">AC_VI &nbsp;&nbsp;
		<input type="checkbox" name="wmm_ps_mode_acvo">AC_VO &nbsp;&nbsp;
	</td>
</tr>
<tr>
	<td class="head" id="qosWMMDLS">Direct Link Setup</td>
	<td><input type="checkbox" name="wmm_dls_enable" onclick="WMM_DLS_Click(this.form);"><font id="qosWMMDLSEnable">enable</font></td>
</tr>

<tr>
	<td class="title" id="qosDLS" colspan="2">Direct Link Setup</td>
</tr>
<tr>
	<td class="head" id="qosDLSMac">MAC Address</td>
	<td>
		<input type="text" class="xsmall" name="mac0" value="<% getStaDLSMacAddress(0); %>">&nbsp;-&nbsp;
		<input type="text" class="xsmall" name="mac1" value="<% getStaDLSMacAddress(1); %>">&nbsp;-&nbsp;
		<input type="text" class="xsmall" name="mac2" value="<% getStaDLSMacAddress(2); %>">&nbsp;-&nbsp;
		<input type="text" class="xsmall" name="mac3" value="<% getStaDLSMacAddress(3); %>">&nbsp;-&nbsp;
		<input type="text" class="xsmall" name="mac4" value="<% getStaDLSMacAddress(4); %>">&nbsp;-&nbsp;
		<input type="text" class="xsmall" name="mac5" value="<% getStaDLSMacAddress(5); %>">
	</td>
</tr>
<tr>
	<td class="head" id="qosDLSTimeoutValue">Timeout Value</td>
	<td><input type="text" name="timeout" align="right" id="qosSecond" value="<% getStaDLSTimeout(); %>"> sec</td>
</tr>
</table>

<table class="buttons">
<tr>
	<td><input type="button" name="DlsSetupButton" style="{width:120px;}" value="DLS Apply" id="qosDLSAppy" onClick="submit_apply(this.form, 1)"></td>
</tr>
</table>

<table div="wmm_dls_status" class="form">
<tr>
	<td class="title" colspan="2" id="qosDLSStaus">DLS Status</td>
</tr>
<tr>
	<td width="65%" bgcolor="#E8F8FF" id="qosDLSStatusMAC">MAC Address</td>
	<td bgcolor="#E8F8FF" id="qosDLSStatusTimeout">Timeout</td>
</tr>
<% getStaDLSList(); %>
</table>

<table class="buttons">
<tr>
	<td><input type="button" name="DlsStatusButton" class="normal" value="Tear Down" id="qosTearDown" onClick="submit_apply(this.form, 3)"></td>
</tr>
</table>

<input type="hidden" name="button_type" value="">
<input type="hidden" name="submit-url" value="/station/qos.asp" >
</form>

<div class="whitespace">&nbsp;</div>

</td></tr></table>
</body>
</html>
