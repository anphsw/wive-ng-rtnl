<html><head><title>Upload Firmware</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
<META HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">
<script type="text/javascript" src="/lang/b28n.js"></script>
<style type="text/css">
<!--
#loading {
       width: 250px;
       height: 200px;
       background-color: #3399ff;
       position: absolute;
       left: 50%;
       top: 50%;
       margin-top: -150px;
       margin-left: -250px;
       text-align: center;
}
-->
</style>

<script language="JavaScript" type="text/javascript">
document.write('<div id="loading" style="display: none;"><br><br><br>Uploading firmware <br><br> Please be patient...</div>');
Butterlate.setTextDomain("admin");

var _singleton = 0;
function formCheck()
{
	if(_singleton)
		return false;
	if(document.UploadFirmware.filename.value == ""){
		alert("Firmware Upgrade: Please specify a file.");
		return false;
	}

	document.UploadBootloader.UploadBootloaderSubmit.disabled = true;
	document.UploadBootloader.reset.disabled = true;

    document.getElementById("loading").style.display="block";
	parent.menu.setUnderFirmwareUpload(1);
	_singleton = 1;
	return true;
}

function formBootloaderCheck()
{
	ret = confirm("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\nThis is for engineer only. Are u sure?\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	if(!ret)
		return false;

	if(_singleton)
		return false;
	if(document.UploadBootloader.filename.value == ""){
		alert("Bootloader Upgrade: Please specify a file.");
		return false;
	}

	document.UploadFirmware.UploadFirmwareSubmit.disabled = true;
	document.UploadFirmware.reset.disabled = true;

	document.getElementById("loading").style.display="block";
	parent.menu.setUnderFirmwareUpload(1);
	_singleton = 1;
	return true;
}

function initTranslation()
{
	var e = document.getElementById("uploadTitle");
	e.innerHTML = _("upload title");
	e = document.getElementById("uploadIntroduction1");
	e.innerHTML = _("upload introduction1");
	e = document.getElementById("uploadIntroduction2");
	e.innerHTML = _("upload introduction2");

	e = document.getElementById("uploadFW");
	e.innerHTML = _("upload firmware");
	e = document.getElementById("uploadFWLocation");
	e.innerHTML = _("upload firmware location");
	e = document.getElementById("uploadFWApply");
	e.value = _("admin apply");
	e = document.getElementById("uploadFWReset");
	e.value = _("admin reset");

	e = document.getElementById("uploadBoot");
	e.innerHTML = _("upload bootloader");
	e = document.getElementById("uploadBootLocation");
	e.innerHTML = _("upload bootloader location");
	e = document.getElementById("uploadBootApply");
	e.value = _("admin apply");
	e = document.getElementById("uploadBootReset");
	e.value = _("admin reset");
}

function pageInit(){
	initTranslation();
    document.UploadFirmware.UploadFirmwareSubmit.disabled = false;
    document.UploadFirmware.reset.disabled = false;
    document.UploadBootloader.UploadBootloaderSubmit.disabled = false;
    document.UploadBootloader.reset.disabled = false;

//	document.UploadFirmware.filename.disabled = false;
	document.getElementById("loading").style.display="none";
}
</script></head><body onLoad="pageInit()">
<table class="body"><tbody><tr><td>
<h1 id="uploadTitle">Upgrade Firmware</h1>
<p><font id="uploadIntroduction1">Upgrade the Acorp WR-NL firmware to obtain new functionality. </font><font id="uploadIntroduction2" color="#ff0000">It takes about 1 minute to upload &amp; upgrade flash and be patient please. Caution! A corrupted image will hang up the system.</font> </p>

<!-- ----------------- Upload firmware Settings ----------------- -->
<form method="post" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
<table border="1" cellpadding="2" cellspacing="1" width="95%">
<tbody><tr>
  <td class="title" colspan="2" id="uploadFW">Update Firmware</td>
</tr>
<tr>
  <td class="head" id="uploadFWLocation">Location:</td>
	<td> <input name="filename" size="20" maxlength="256" type="file"> </td>
</tr>
</tbody></table>

<input value="Apply" id="uploadFWApply" name="UploadFirmwareSubmit" onclick="return formCheck()" type="submit"> &nbsp;&nbsp;
<input value="Reset" id="uploadFWReset" name="reset" type="reset">
</form>

<form method="post" name="UploadBootloader" action="/cgi-bin/upload_bootloader.cgi" enctype="multipart/form-data">
<table border="1" cellpadding="2" cellspacing="1" width="95%">
<tbody><tr>
  <td class="title" colspan="2" id="uploadBoot">Update Bootloader</td>
</tr>
<tr>
  <td class="head"i id="uploadBootLocation">Location:</td>
	<td> <input name="filename" size="20" maxlength="256" type="file"> </td>
</tr>
</tbody></table>
<input value="Apply" id="uploadBootApply" name="UploadBootloaderSubmit" onclick="return formBootloaderCheck()" type="submit"> &nbsp;&nbsp;
<input value="Reset" id="uploadBootReset" name="reset" type="reset">
</form>

<br>
</td></tr></tbody></table>
</body></html>
