<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<title>System Management</title>

<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");

function SubmitForm(message, form)
{
	if (confirm(message))
		form.submit();
}

function AdmFormCheck(form)
{
	var re_login = /^[a-zA-Z0-9_]+$/;
	if (!re_login.test(form.admuser.value))
	{
		alert("Please specify correct administrator login (a-z, A-Z, 0-9, _).");
		form.admuser.focus();
		return false;
	}
	
	var re_pass = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
	if (!re_pass.test(form.admpass.value))
	{
		alert("Please specify correct administrator password (spaces and dollar symbol are disallowed).");
		form.admpass.focus();
		return false;
	}
	return true;
}

function initValue()
{
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_zhtw = "<% getLangBuilt("zhtw"); %>";

	lang_element.options.length = 0;
	if (lang_en == "1")
		lang_element.options[lang_element.length] = new Option('English', 'en');
	if (lang_zhtw == "1")
		lang_element.options[lang_element.length] = new Option('Traditional Chinese', 'zhtw');

	if (document.cookie.length > 0)
	{
		var s = document.cookie.indexOf("language=");
		var e = document.cookie.indexOf(";", s);
		var lang = "en";
		var i;

		if (s != -1)
		{
			if (e == -1)
				lang = document.cookie.substring(s+9);
			else
				lang = document.cookie.substring(s+9, e);
		}
		for (i=0; i<lang_element.options.length; i++) {
			if (lang == lang_element.options[i].value) {
				lang_element.options.selectedIndex = i;
				break;
			}
		}
	}
	
	// Firmware
	document.getElementById("loading").style.display="none";
}

function setLanguage()
{
	document.cookie="language="+document.Lang.langSelection.value+"; path=/";
	parent.menu.location.reload();
	return true;
}

function onUploadFirmwareSubmit(form)
{
	if (checkFilePresent(form.filename))
		ajaxPostForm(
			'Do not power off the device while upgrading firmware! ' + 
			'Doing so can result in permanent damage to the device. ' + 
			'Do you really want to proceed?',
			form,
			'firmwareReloader',
			'/messages/wait_firmware.asp',
			ajaxShowProgress);
}

function onImportSettings(form)
{
	if (checkFilePresent(form.filename))
		ajaxPostForm(
			'Proceed uploading settings?',
			form,
			'setmanReloader',
			'/messages/wait_config.asp',
			ajaxShowProgress);
}

</script>

</head>
<body onload="initValue();">
<table class="body" style="width:600px;"><tr><td>
<h1>System Management</h1>
<div id="staticText">
<p>You can select language and set administrator login and password here.</p>
<p>You can also upgrade the Wive-NG-RTNL firmware to obtain new functionality.
It takes about 1 minute to upload firmware &amp; upgrade flash. Please be patient.</p>
<p style="color: #ff0000;">Caution! A corrupted image will hang up the system.</p>
</div>

<p id="loading" style="display: none; color: #ff0000; font-size: 16px;">
	Uploading firmware <br><br> Please be patient and don't remove USB device if present...
</p>

<div id="staticControls">
<hr>

<!-- ================= Langauge Settings ================= -->
<table width="100%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2" id="manLangSet">Language Settings</td>
</tr>
<tr>
	<td class="head" id="manSelectLang">Select Language</td><td>
		<form method="POST" name="Lang" action="/goform/setSysLang">
			<select name="langSelection" id="langSelection">
			<!-- added by initValue -->
			</select>
			<input type="hidden" name="submit-url" value="/adm/management.asp" >
			<input type="submit" class="half" value="Apply" id="manLangApply" onClick="return setLanguage();"> &nbsp; &nbsp;
		</form>
	</td>
</tr>
</table>

<!-- ================= Adm Settings ================= -->
<form method="POST" name="Adm" action="/goform/setSysAdm" onsubmit="return AdmFormCheck(this);">
<table width="100%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2" id="manAdmSet">Administrator Settings</td>
</tr>
<tr>
	<td class="head">Login</td>
	<td><input type="text" name="admuser" size="16" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>"></td>
</tr>
<tr>
	<td class="head" id="manAdmPasswd">Password</td>
	<td><input type="password" name="admpass" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
</tr>
<tr>
	<td class="head">Apply new login/password</td>
	<td>
		<input type="hidden" name="submit-url" value="/adm/management.asp" >
		<input type="submit" class="half" value="Apply" id="manAdmApply">
	</td>
</tr>
</table>
</form>

<!-- ================= Firmware ================= -->
<table width="100%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
	<td colspan="2" class="title">Firmware update</td>
</tr>
<tr>
	<td class="head" id="uploadFWLocation">Firmware update:</td>
	<td class="value">
	<form method="POST" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data" onSubmit="return uploadFirmwareCheck();" >
		<input type="checkbox" name="reset_rwfs" checked="checked">Reset RWFS on update
		<br>
		<input name="filename" size="20" maxlength="256" type="file">
		<input type="button" value="Update" id="uploadFWApply" class="half" name="UploadFirmwareSubmit" onclick="onUploadFirmwareSubmit(this.form);">
		<br>
		<iframe id="firmwareReloader" name="firmwareReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
	</form>
	</td>
</tr>
</table>

<!-- ----------------- Settings management ----------------- -->
<table width="100%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2">Router Settings Management</td>
</tr>
<tr>
	<td class="head" id="setmanExpSetButton">Backup Settings to file</td>
	<td>
		<form method="GET" name="ExportSettings" action="/cgi-bin/ExportSettings.sh" 
			onsubmit="return confirm('Do you want to export settings to file?');" >
			<input type="submit" value="Backup" id="setmanExpSetExport" name="Export" class="half">
		</form>
	</td>
</tr>
<tr>
	<td class="head" id="setmanImpSetFileLocation">Load settings from file</td>
	<td>
		<form method="POST" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
			<input type="file" name="filename" maxlength="256">
			<input type="button" value="Load" id="setmanImpSetImport" class="half" onclick="onImportSettings(this.form);">
			<iframe id="setmanReloader" name="setmanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
		</form>
	</td>
</tr>
<tr>
	<td class="head">Reset to factory defaults</td>
	<td>
		<form method="POST" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings"
			onsubmit="return confirm('All settings will be reset to factory defaults. Really proceed?');">
			<input type="submit" value="Reset" id="setmanLoadDefault" name="LoadDefault" class="half">
			<input type="hidden" value="stub" >
		</form>
	</td>
</tr>
</table>
<div>

</td></tr></table>
</body></html>
