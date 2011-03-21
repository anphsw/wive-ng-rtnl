<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Edit User Account</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var index = opener.document.forms[0].selectIndex.value;
var user = opener.document.forms[0].selectUser.value;
var pw = opener.document.forms[0].selectPassword.value;
var ftp = opener.document.forms[0].selectFtp.value;
var maxlogins = opener.document.forms[0].selectMaxLogins.value;
var mode = opener.document.forms[0].selectMode.value;
var smb = opener.document.forms[0].selectSmb.value;
var ftpb = '<% getFtpBuilt(); %>';
var smbb = '<% getSmbBuilt(); %>';

function initTranslation()
{
	var e = document.getElementById("edituserName");
	e.innerHTML = _("adduser name");
	e = document.getElementById("edituserPW");
	e.innerHTML = _("adduser password");
	e = document.getElementById("edituserMulLogins");
	e.innerHTML = _("adduser mullogins");
	e = document.getElementById("edituserMulLoginsEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("edituserMulLoginsDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("edituserMaxLogins");
	e.innerHTML = _("adduser maxlogins");
	e = document.getElementById("edituserDL");
	e.innerHTML = _("adduser download");
	e = document.getElementById("edituserDLEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("edituserDLDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("edituserUL");
	e.innerHTML = _("adduser upload");
	e = document.getElementById("edituserULEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("edituserULDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("edituserOW");
	e.innerHTML = _("adduser overwrite");
	e = document.getElementById("edituserOWEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("edituserOWDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("edituserER");
	e.innerHTML = _("adduser erase");
	e = document.getElementById("edituserEREnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("edituserERDisable");
	e.innerHTML = _("usb disable");

	e = document.getElementById("edituserApply");
	e.value = _("usb apply");
	e = document.getElementById("edituserCancel");
	e.value = _("usb cancel");
}
	
function initValue()
{
	initTranslation();

	document.storage_adduser.adduser_ftp[0].disabled = true;
	document.storage_adduser.adduser_ftp[1].disabled = true;
	document.storage_adduser.adduser_smb[0].disabled = true;
	document.storage_adduser.adduser_smb[1].disabled = true;
	document.storage_edituser.edituser_pw.value = pw;
	if (ftp == "1" && ftpb == "1")
	{
		document.storage_edituser.edituser_ftp[0].disabled = false;
		document.storage_edituser.edituser_ftp[1].disabled = false;
		document.storage_edituser.edituser_ftp[0].checked = true;
		if (mode.indexOf("M") >= 0)
		{
			document.storage_edituser.edituser_mullogins[0].checked = true;
			document.storage_edituser.edituser_maxlogins.disabled = false;
			document.storage_edituser.edituser_maxlogins.value = maxlogins;
		}
		else
		{
			document.storage_edituser.edituser_mullogins[1].checked = true;
			document.storage_edituser.edituser_maxlogins.disabled = true;
		}
		if (mode.indexOf("D") >= 0)
			document.storage_edituser.edituser_download[0].checked = true;
		else
			document.storage_edituser.edituser_download[1].checked = true;
		if (mode.indexOf("U") >= 0)
			document.storage_edituser.edituser_upload[0].checked = true;
		else
			document.storage_edituser.edituser_upload[1].checked = true;
		if (mode.indexOf("O") >= 0)
			document.storage_edituser.edituser_overwrite[0].checked = true;
		else
			document.storage_edituser.edituser_overwrite[1].checked = true;
		if (mode.indexOf("E") >= 0)
			document.storage_edituser.edituser_erase[0].checked = true;
		else
			document.storage_edituser.edituser_erase[1].checked = true;
	}
	else
	{
		document.storage_edituser.edituser_ftp[1].checked = true;
		document.storage_edituser.edituser_mullogins[0].disabled = true;
		document.storage_edituser.edituser_mullogins[1].disabled = true;
		document.storage_edituser.edituser_maxlogins.disabled = true;
		document.storage_edituser.edituser_download[0].disabled = true;
		document.storage_edituser.edituser_download[1].disabled = true;
		document.storage_edituser.edituser_upload[0].disabled = true;
		document.storage_edituser.edituser_upload[1].disabled = true;
		document.storage_edituser.edituser_overwrite[0].disabled = true;
		document.storage_edituser.edituser_overwrite[1].disabled = true;
		document.storage_edituser.edituser_erase[0].disabled = true;
		document.storage_edituser.edituser_erase[1].disabled = true;
	}
	if (smb == "1" && smbb == "1")
	{
		document.storage_edituser.edituser_smb[0].disabled = false;
		document.storage_edituser.edituser_smb[1].disabled = false;
		document.storage_edituser.edituser_smb[0].checked = true;
	}
	else
		document.storage_edituser.edituser_smb[1].checked = true;
}

function checkData()
{
	if (document.storage_edituser.edituser_mullogins[0].checked == true)
	{
		if (document.storage_edituser.edituser_mullogins.value == "")
		{
			alert('Please specify the count of Max. Login');
			document.storage_edituser.edituser_maxlogins.focus();
			document.storage_edituser.edituser_maxlogins.select();
			return false;
		}
		else if (isNaN(document.storage_edituser.edituser_maxlogins.value))
		{
			alert('Please specify valid number');
			document.storage_edituser.edituser_maxlogins.focus();
			document.storage_edituser.edituser_maxlogins.select();
			return false;
		}
	}
	if ((document.storage_edituser.edituser_mullogins[1].checked == true) &&
	    (document.storage_edituser.edituser_download[1].checked == true) &&
	    (document.storage_edituser.edituser_upload[1].checked == true) &&
	    (document.storage_edituser.edituser_overwrite[1].checked == true) &&
	    (document.storage_edituser.edituser_erase[1].checked == true))
	{
		alert('Please Select one capability at least');
		return false;
	}


	return true;
}

function ftp_enable_switch()
{
	if (document.storage_edituser.edituser_ftp[0].checked == true)
	{
		document.storage_edituser.edituser_mullogins[0].disabled = false;
		document.storage_edituser.edituser_mullogins[1].disabled = false;
		document.storage_edituser.edituser_maxlogins.disabled = true;
		document.storage_edituser.edituser_download[0].disabled = false;
		document.storage_edituser.edituser_download[1].disabled = false;
		document.storage_edituser.edituser_upload[0].disabled = false;
		document.storage_edituser.edituser_upload[1].disabled = false;
		document.storage_edituser.edituser_overwrite[0].disabled = false;
		document.storage_edituser.edituser_overwrite[1].disabled = false;
		document.storage_edituser.edituser_erase[0].disabled = false;
		document.storage_edituser.edituser_erase[1].disabled = false;
	}
	else
	{
		document.storage_edituser.edituser_mullogins[0].disabled = true;
		document.storage_edituser.edituser_mullogins[1].disabled = true;
		document.storage_edituser.edituser_maxlogins.disabled = true;
		document.storage_edituser.edituser_download[0].disabled = true;
		document.storage_edituser.edituser_download[1].disabled = true;
		document.storage_edituser.edituser_upload[0].disabled = true;
		document.storage_edituser.edituser_upload[1].disabled = true;
		document.storage_edituser.edituser_overwrite[0].disabled = true;
		document.storage_edituser.edituser_overwrite[1].disabled = true;
		document.storage_edituser.edituser_erase[0].disabled = true;
		document.storage_edituser.edituser_erase[1].disabled = true;
	}
}

function mullogins_enable_switch()
{
	if (document.storage_edituser.edituser_mullogins[0].checked == true)
	{
		document.storage_edituser.edituser_maxlogins.disabled = false;
	}
	else
	{
		document.storage_edituser.edituser_maxlogins.disabled = true;
	}
}

function edituserClose()
{
	opener.location.reload();
}

function submit_apply()
{
	if (checkData() == true)
	{
		document.storage_edituser.hiddenIndex.value = index;
		document.storage_edituser.submit();
		opener.location.reload();
		window.close();
	}
}
</script>
</head>

<body onLoad="initValue()" onUnload="edituserClose()">
<table class="body"><tr><td>

<form method=post name="storage_edituser" action="/goform/StorageEditUser">
<input type=hidden name=hiddenIndex value="">
<table width="540" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="2" id="edituserbasic">Basic Setup</td>
  </tr>
  <tr>
    <td class="head" id="edituserName">User Name</td>
    <td>
    <script language="JavaScript" type="text/javascript">
      document.write(user);
    </script>
    </td>
  </tr>
  <tr>
    <td class="head" id="edituserPW">Password</td>
    <td><input type="password" name="edituser_pw" size="16" maxlength="16" value=""></td>
  </tr>
</table>

<br />

<table width="540" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" id="edituserFtp">Ftp Setup</td>
    <td>
      <input type=radio name=edituser_ftp value="1" onClick="ftp_enable_switch()"><font id="edituserFtpEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_ftp value="0" onClick="ftp_enable_switch()"><font id="edituserFtpDisable">Disable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="edituserMulLogins">Multiple Logins</td>
    <td>
      <input type=radio name=edituser_mullogins value="1" onClick="mullogins_enable_switch()"><font id="edituserMulLoginsEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_mullogins value="0" onClick="mullogins_enable_switch()"><font id="edituserMulLoginsDisable">Disable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="edituserMaxLogins">Max. Login</td>
    <td>
      <input type=text name=edituser_maxlogins size=1 maxlength=1 value="3">
    </td>
  </tr>
  <tr> 
    <td class="head" id="edituserDL">Download Capabled</td>
    <td>
      <input type=radio name=edituser_download value="1"><font id="edituserDLEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_download value="0"><font id="edituserDLDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="edituserUL">Upload Capabled</td>
    <td>
      <input type=radio name=edituser_upload value="1"><font id="edituserULEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_upload value="0"><font id="edituserULDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="edituserOW">Overwrite Capabled</td>
    <td>
      <input type=radio name=edituser_overwrite value="1"><font id="edituserOWEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_overwrite value="0"><font id="edituserOWDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="edituserER">Erase Capabled</td>
    <td>
      <input type=radio name=edituser_erase value="1"><font id="edituserEREnable">Enable</font>&nbsp;
      <input type=radio name=edituser_erase value="0"><font id="edituserERDisable">Disable</font>
    </td>
  </tr>
</table>

<hr />
<br />

<table width="540" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" id="edituserSmb">Samba Setup</td>
    <td>
      <input type=radio name=edituser_smb value="1"><font id="edituserSmbEnable">Enable</font>&nbsp;
      <input type=radio name=edituser_smb value="0"><font id="edituserSmbDisable">Disable</font>
    </td>
  </tr>
</table>

<hr />
<br />

<table width = "540" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type=button style="{width:120px;}" value="Apply" id="edituserApply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" id="edituserCancel" onClick="window.close()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>

