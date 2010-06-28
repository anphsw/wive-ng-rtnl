<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<title>Settings Management</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<script language="JavaScript" type="text/javascript">

function SubmitForm(message, form)
{
	if (confirm(message))
		form.submit();
}

</script>

</head>
<body>
<table class="body"><tr><td>
<h1 id="setmanTitle">Settings Management</h1>
<p id="setmanIntroduction">You might save system settings by exporting them to a configuration file, restore them by importing the file, or reset them to factory default.</p>
<hr />

<table width="100%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
	<td class="title" colspan="2">Router Settings Management</td>
</tr>
<tr>
	<td class="head" id="setmanExpSetButton">Export Settings to file</td>
	<td>
		<form method="POST" name="ExportSettings" action="/cgi-bin/ExportSettings.sh">
			<input type="button" value="Export" id="setmanExpSetExport" name="Export" class="half" onClick="return SubmitForm('Do you want to export settings to file?', this.form);">
		</form>
	</td>
</tr>
<tr>
	<td class="head" id="setmanImpSetFileLocation">Import settings from file</td>
	<td>
		<form method="POST" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
			<input type="file" name="filename" maxlength="256">
			<input type="button" value="Import" id="setmanImpSetImport" class="half" onClick="return SubmitForm('Proceed importing settings?', this.form);"> &nbsp; &nbsp;
		</form>
	</td>
</tr>
<tr>
	<td class="head" id="setmanImpSetFileLocation">Reset to factory defaults</td>
	<td>
		<form method="POST" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings">
			<input type="button" value="Reset" id="setmanLoadDefault" name="LoadDefault" class="half" onClick="return SubmitForm('All settings will be reset to factory defaults. Really proceed?', this.form);">
		</form>
	</td>
</tr>
</table>

</td></tr></table>
</body></html>
