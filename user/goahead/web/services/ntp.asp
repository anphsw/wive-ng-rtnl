<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/parse.js"></script>
<script type="text/javascript" src="/js/sprintf.js"></script>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>System Management</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");
Butterlate.setTextDomain("services");

function initTranslation()
{
	_TR("manTitle", "services ntp title");
	_TR("manIntroduction", "services ntp introduction");

	_TR("manNTPSet", "man ntp setting");
	_TR("manNTPTimeZone", "man ntp timezone");

	_TR("manNTPServer", "man ntp server");
	_TR("manNTPCurrentTime", "man ntp current time");

	_TRV("manNTPApply", "admin apply");
	_TRV("manNTPCancel", "admin cancel");
	_TRV("manNTPSyncWithHost", "man ntp sync with host");
}

function initValue()
{
	var tz = "<% getCfgGeneral(1, "TZ"); %>";
	var dateb = "<% getDATEBuilt(); %>";
	var ena = "<% getCfgGeneral(1, "NTPEnabled"); %>";
	var form = document.NTP;

	initTranslation();

	if (dateb == "1")
	{
		showElement('div_date');
		form.ntpcurrenttime.disabled = false;
	} 
	else
	{
		hideElement('div_date');
		form.ntpcurrenttime.disabled = true;
	}

	form.ntp_enabled.checked = ena == "on";
	ntpChange(form);

	form.time_zone.value = tz;
}

function syncWithHost()
{
	var cTime = new Date();
	
	var tmp = sprintf('%04d%02d%02d%02d%02d.%02d',
			cTime.getFullYear(), cTime.getMonth()+1, cTime.getDay(),
			cTime.getHours(), cTime.getMinutes(), cTime.getSeconds());
	
	ajaxPostRequest("/goform/NTPSyncWithHost", tmp, true);
}

function ntpChange(form)
{
	var dis = ! form.ntp_enabled.checked;
	form.ntpcurrenttime.disabled = dis;
	form.manNTPSyncWithHost.disabled = dis;
	form.time_zone.disabled = dis;
	form.NTPServerIP.disabled = dis;
}

</script>

</head>
<body onload="initValue()">
<table class="body"><tr><td>
<h1 id="manTitle">System Management</h1>
<p id="manIntroduction">You may configure administrator account and password, NTP settings, and Dynamic DNS settings here.</p>
<hr />

<!-- ================= NTP Settings ================= -->
<form method="POST" name="NTP" action="/goform/NTP" >
<table width="90%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tbody><tr>
	<td class="title" colspan="2" id="manNTPSet">NTP Settings</td>
</tr>
<tr id="ntp_enabled_row">
	<td class="head">Enable</td>
	<td>
		<input type="checkbox" onchange="ntpChange(this.form);" name="ntp_enabled">&nbsp;Enable NTP synchronization
	</td>
</tr>
<tr id="div_date">
	<td class="head" id="manNTPCurrentTime">Current Time</td>
	<td>
		<input size="24" name="ntpcurrenttime" value="<% getCurrentTimeASP(); %>" type="text" readonly="1">
		<input type="button" value="Sync with host" id="manNTPSyncWithHost" name="manNTPSyncWithHost" onClick="syncWithHost()">
	</td>
</tr>
<tr>
	<td class="head" id="manNTPTimeZone">Time Zone:</td>
	<td>
		<select name="time_zone">
			<option value="UCT_-11" id="manNTPMidIsland">(UTC-11:00) Midway Island, Samoa</option>
			<option value="UCT_-10" id="manNTPHawaii">(UTC-10:00) Hawaii</option>
			<option value="NAS_-09" id="manNTPAlaska">(UTC-09:00) Alaska</option>
			<option value="PST_-08" id="manNTPPacific">(UTC-08:00) Pacific Time</option>
			<option value="MST_-07" id="manNTPMountain">(UTC-07:00) Mountain Time</option>
			<option value="MST_-07" id="manNTPArizona">(UTC-07:00) Arizona</option>
			<option value="CST_-06" id="manNTPCentral">(UTC-06:00) Central Time</option>
			<option value="UCT_-06" id="manNTPMidUS">(UTC-06:00) Middle America</option>
			<option value="UCT_-05" id="manNTPIndianaEast">(UTC-05:00) Indiana East, Colombia</option>
			<option value="EST_-05" id="manNTPEastern">(UTC-05:00) Eastern Time</option>
			<option value="AST_-04" id="manNTPAtlantic">(UTC-04:00) Atlantic Time, Brazil West</option>
			<option value="UCT_-04" id="manNTPBolivia">(UTC-04:00) Bolivia, Venezuela</option>
			<option value="UCT_-03" id="manNTPGuyana">(UTC-03:00) Guyana</option>
			<option value="EBS_-03" id="manNTPBrazilEast">(UTC-03:00) Brazil East, Greenland</option>
			<option value="NOR_-02" id="manNTPMidAtlantic">(UTC-02:00) Mid-Atlantic</option>
			<option value="EUT_-01" id="manNTPAzoresIslands">(UTC-01:00) Azores Islands</option>
			<option value="UCT_000" id="manNTPGambia">(UTC) Gambia, Liberia, Morocco</option>
			<option value="GMT_000" id="manNTPEngland">(UTC) England</option>
			<option value="MET_001" id="manNTPCzechRepublic">(UTC+01:00) Czech Republic, N</option>
			<option value="MEZ_001" id="manNTPGermany">(UTC+01:00) Germany</option>
			<option value="UCT_001" id="manNTPTunisia">(UTC+01:00) Tunisia</option>
			<option value="EET_002" id="manNTPGreece">(UTC+02:00) Greece, Turkey</option>
			<option value="EET-2EEST,M3.5.0/3,M10.5.0/4" id="">(UTC+02:00) Ukraine (Kiev), Latvia (Riga)</option>
			<option value="EET-2EEST,M3.5.0,M10.5.0/3" id="">(UTC+02:00) Belorussia (Minsk)</option>
			<option value="SAS_002" id="manNTPSouthAfrica">(UTC+02:00) South Africa</option>
			<option value="IDDT">(UTC+02:00) Israel (Tel Aviv)</option>
			<option value="IST_003" id="manNTPIraq">(UTC+03:00) Iraq, Jordan, Kuwait</option>
			<option value="EET-2EEST-3" id="">(UTC+03:00) Russia (Kaliningrad)</option>
			<option value="MSK-3MSD-4" id="manNTPMoscowWinter">(UTC+04:00) Russia (Moscow, St.-Petersburg)</option>
			<option value="AMT-4AMST,M3.5.0,M10.5.0/3" id="manNTPArmenia">(UTC+04:00) Armenia (Yerevan)</option>
			<option value="AZT-4AZST,M3.5.0/4,M10.5.0/5">(UTC+04:00) Azebaydzhan (Baku)</option>
			<option value="GET-4">(UTC+04:00) Georgia (Tbilisi)</option>
			<option value="TMT-5">(UTC+05:00) Ashgabad</option>
			<option value="UZT-5">(UTC+05:00) Samarkand, Russia (Tashkent)</option>
			<option value="UCT_005" id="manNTPPakistan">(UTC+05:00) Pakistan</option>
			<option value="YEKT-5YEKST-6">(UTC+06:00) Russia (Yekaterinburg)</option>
			<option value="UCT_006" id="manNTPBangladesh">(UTC+06:00) Bangladesh</option>
			<option value="KGT-6" id="manNTPBangladesh">(UTC+06:00) Kyrgyzstan (Bishkek)</option>
			<option value="NOVT-6NOVST-7">(UTC+07:00) Russia (Novosibirsk)</option>
			<option value="OMST-6OMSST-7">(UTC+07:00) Russia (Omsk)</option>
			<option value="UCT_007" id="manNTPThailand">(UTC+07:00) Thailand</option>
			<option value="KRAT-7KRAST-8" id="manNTPAustraliaWA">(UTC+08:00) Russia (Krasnoyarsk)</option>
			<option value="CST_008" id="manNTPChinaCoast">(UTC+08:00) China Coast, Hong Kong</option>
			<option value="CCT_008" id="manNTPTaipei">(UTC+08:00) Taipei</option>
			<option value="SST_008" id="manNTPSingapore">(UTC+08:00) Singapore</option>
			<option value="AWS_008" id="manNTPAustraliaWA">(UTC+08:00) Australia (WA)</option>
			<option value="ULAT-8">(UTC+08:00) Mongolia (Ulan-Bator)</option>
			<option value="IRKT-8IRKST-9">(UTC+09:00) Russia (Irkutsk)</option>
			<option value="JST_009" id="manNTPJapan">(UTC+09:00) Japan</option>
			<option value="KST_009" id="manNTPKorean">(UTC+09:00) North Korea, South Korea</option>
			<option value="YAKT-9YAKST-10">(UTC+10:00) Russia (Yakutsk)</option>
			<option value="UCT_010" id="manNTPGuam">(UTC+10:00) Guam</option>
			<option value="SAKT-10SAKST-11">(UTC+11:00) Russia (Sakhalin)</option>
			<option value="VLAT-10VLAST-11">(UTC+11:00) Russia (Vladivostok)</option>
			<option value="AES_010" id="manNTPAustraliaQLD">(UTC+10:00) Australia (QLD, TAS,NSW,ACT,VIC)</option>
			<option value="UCT_011" id="manNTPSolomonIslands">(UTC+11:00) Solomon Islands</option>
			<option value="MAGT-11MAGST-12">(UTC+12:00) Russia (Magadan)</option>
			<option value="PETT-12PETST-13">(UTC+12:00) Russia (Kamchatka)</option>
			<option value="UCT_012" id="manNTPFiji">(UTC+12:00) Fiji</option>
			<option value="NZS_012" id="manNTPNewZealand">(UTC+12:00) New Zealand</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="manNTPServer">NTP Server</td>
	<td>
		<input size="32" maxlength="64" name="NTPServerIP" value="<% getCfgGeneral(1, "NTPServerIP"); %>" type="text">
		<br>&nbsp;&nbsp;<font color="#808080">ex:&nbsp;time.nist.gov</font>
		<br>&nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;ntp0.broad.mit.edu</font>
		<br>&nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;time.stdtime.gov.tw</font>
	</td>
</tr>
</tbody>
</table>

<table width="90%" border="0" cellpadding="2" cellspacing="1">
<tr align="center">
	<td>
		<input type="hidden" value="shit" name="stub_shit">
		<input type="submit" style="{width:120px;}" value="Apply"  id="manNTPApply"> &nbsp;&nbsp;
		<input type="reset"  style="{width:120px;}" value="Cancel" id="manNTPCancel" onclick="window.location.reload();">
	</td>
</tr>
</table>
</form>

</td></tr></table>
</body></html>
