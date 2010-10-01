<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Create New User Account</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var ftpb = '<% getFtpBuilt(); %>';
var smbb = '<% getSmbBuilt(); %>';

function initTranslation()
{
	var e = document.getElementById("adduserName");
	e.innerHTML = _("adduser name");
	e = document.getElementById("adduserPW");
	e.innerHTML = _("adduser password");

	e = document.getElementById("adduserFtp");
	e.innerHTML = _("adduser ftp");
	e = document.getElementById("adduserFtpEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserFtpDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("adduserMulLogins");
	e.innerHTML = _("adduser mullogins");
	e = document.getElementById("adduserMulLoginsEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserMulLoginsDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("adduserMaxLogins");
	e.innerHTML = _("adduser maxlogins");
	e = document.getElementById("adduserDL");
	e.innerHTML = _("adduser download");
	e = document.getElementById("adduserDLEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserDLDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("adduserUL");
	e.innerHTML = _("adduser upload");
	e = document.getElementById("adduserULEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserULDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("adduserOW");
	e.innerHTML = _("adduser overwrite");
	e = document.getElementById("adduserOWEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserOWDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("adduserER");
	e.innerHTML = _("adduser erase");
	e = document.getElementById("adduserEREnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserERDisable");
	e.innerHTML = _("usb disable");

	e = document.getElementById("adduserSmb");
	e.innerHTML = _("adduser smb");
	e = document.getElementById("adduserSmbEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("adduserSmbDisable");
	e.innerHTML = _("usb disable");

	e = document.getElementById("adduserApply");
	e.value = _("usb apply");
	e = document.getElementById("adduserCancel");
	e.value = _("usb cancel");
}
	
function initValue()
{
	initTranslation();

	document.storage_adduser.adduser_ftp[0].disabled = true;
	document.storage_adduser.adduser_ftp[1].disabled = true;
	document.storage_adduser.adduser_smb[0].disabled = true;
	document.storage_adduser.adduser_smb[1].disabled = true;
	if (ftpb == "1")
	{
		document.storage_adduser.adduser_ftp[0].disabled = false;
		document.storage_adduser.adduser_ftp[1].disabled = false;
	}
	if (smbb == "1")
	{
		document.storage_adduser.adduser_smb[0].disabled = false;
		document.storage_adduser.adduser_smb[1].disabled = false;
	}

	document.storage_adduser.adduser_mullogins[0].disabled = true;
	document.storage_adduser.adduser_mullogins[1].disabled = true;
	document.storage_adduser.adduser_maxlogins.disabled = true;
	document.storage_adduser.adduser_download[0].disabled = true;
	document.storage_adduser.adduser_download[1].disabled = true;
	document.storage_adduser.adduser_upload[0].disabled = true;
	document.storage_adduser.adduser_upload[1].disabled = true;
	document.storage_adduser.adduser_overwrite[0].disabled = true;
	document.storage_adduser.adduser_overwrite[1].disabled = true;
	document.storage_adduser.adduser_erase[0].disabled = true;
	document.storage_adduser.adduser_erase[1].disabled = true;
}

function checkData()
{
	if (document.storage_adduser.adduser_name.value == "")
	{
		alert('Please specify User Name');
		document.storage_adduser.adduser_name.focus();
		document.storage_adduser.adduser_name.select();
		return false;
	}
	else if (document.storage_adduser.adduser_name.value.match(/[ `~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/))
	{
		alert('Don\'t enter /[ `~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/ in this feild');
		document.storage_adduser.adduser_name.focus();
		document.storage_adduser.adduser_name.select();
		return false;
	}
	else
	{
		var i = 0;
		while(i < 9)
		{
			if (document.storage_adduser.adduser_name.value == eval('opener.document.forms[0].hiddenUser'+i+'.value') || document.storage_adduser.adduser_name.value == "anonymous")
			{
				alert('This user has exited!');
				return false;
			}
			i++;
		}
	}

	if (document.storage_adduser.adduser_ftp[0].checked == true)
	{
		if (document.storage_adduser.adduser_mullogins[0].checked == true)
		{
			if (document.storage_adduser.adduser_maxlogins.value == "")
			{
				alert('Please specify the count of Max. Login');
				document.storage_adduser.adduser_maxlogins.focus();
				document.storage_adduser.adduser_maxlogins.select();
				return false;
			}
			else if (isNaN(document.storage_adduser.adduser_maxlogins.value))
			{
				alert('Please specify valid number');
				document.storage_adduser.adduser_maxlogins.focus();
				document.storage_adduser.adduser_maxlogins.select();
				return false;
			}
		}
		if ((document.storage_adduser.adduser_mullogins[1].checked == true) &&
				(document.storage_adduser.adduser_download[1].checked == true) &&
				(document.storage_adduser.adduser_upload[1].checked == true) &&
				(document.storage_adduser.adduser_overwrite[1].checked == true) &&
				(document.storage_adduser.adduser_erase[1].checked == true))
		{
			alert('Please Select one capability at least');
			return false;
		}
	}

	return true;
}

function ftp_enable_switch()
{
	if (document.storage_adduser.adduser_ftp[0].checked == true)
	{
		document.storage_adduser.adduser_mullogins[0].disabled = false;
		document.storage_adduser.adduser_mullogins[1].disabled = false;
		document.storage_adduser.adduser_download[0].disabled = false;
		document.storage_adduser.adduser_download[1].disabled = false;
		document.storage_adduser.adduser_upload[0].disabled = false;
		document.storage_adduser.adduser_upload[1].disabled = false;
		document.storage_adduser.adduser_overwrite[0].disabled = false;
		document.storage_adduser.adduser_overwrite[1].disabled = false;
		document.storage_adduser.adduser_erase[0].disabled = false;
		document.storage_adduser.adduser_erase[1].disabled = false;
	}
	else
	{
		document.storage_adduser.adduser_mullogins[0].disabled = true;
		document.storage_adduser.adduser_mullogins[1].disabled = true;
		document.storage_adduser.adduser_maxlogins.disabled = true;
		document.storage_adduser.adduser_download[0].disabled = true;
		document.storage_adduser.adduser_download[1].disabled = true;
		document.storage_adduser.adduser_upload[0].disabled = true;
		document.storage_adduser.adduser_upload[1].disabled = true;
		document.storage_adduser.adduser_overwrite[0].disabled = true;
		document.storage_adduser.adduser_overwrite[1].disabled = true;
		document.storage_adduser.adduser_erase[0].disabled = true;
		document.storage_adduser.adduser_erase[1].disabled = true;
	}
}

function mullogins_enable_switch()
{
	if (document.storage_adduser.adduser_mullogins[0].checked == true)
	{
		document.storage_adduser.adduser_maxlogins.disabled = false;
	}
	else
	{
		document.storage_adduser.adduser_maxlogins.disabled = true;
	}
}

function adduserClose()
{
	opener.location.reload();
}

function submit_apply()
{
	if (checkData() == true)
	{
		document.storage_adduser.submit();
		opener.location.reload();
		window.close();
	}
}
</script>
</head>

<body onLoad="initValue()" onUnload="adduserClose()">
<table class="body"><tr><td>

<form method=post name="storage_adduser" action="/goform/StorageAddUser">
<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="2" id="adduserbasic">Basic Setup</td>
  </tr>
  <tr>
    <td class="head" id="adduserName">User Name</td>
    <td>
      <input type=text name=adduser_name size=8 maxlength=8 value="">
    </td>
  </tr>
  <tr>
    <td class="head" id="adduserPW">Password</td>
    <td><input type="password" name="adduser_pw" size="16" maxlength="16" value=""></td>
  </tr>
</table>

<br />

<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" id="adduserFtp">Ftp Setup</td>
    <td>
      <input type=radio name=adduser_ftp value="1" onClick="ftp_enable_switch()"><font id="adduserFtpEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_ftp value="0" onClick="ftp_enable_switch()" checked><font id="adduserFtpDisable">Disable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="adduserMulLogins">Multiple Logins</td>
    <td>
      <input type=radio name=adduser_mullogins value="1" onClick="mullogins_enable_switch()"><font id="adduserMulLoginsEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_mullogins value="0" onClick="mullogins_enable_switch()" checked><font id="adduserMulLoginsDisable">Disable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="adduserMaxLogins">Max. Login</td>
    <td>
      <input type=text name=adduser_maxlogins size=1 maxlength=1 value="3">
    </td>
  </tr>
  <tr> 
    <td class="head" id="adduserDL">Download Capabled</td>
    <td>
      <input type=radio name=adduser_download value="1"><font id="adduserDLEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_download value="0" checked><font id="adduserDLDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="adduserUL">Upload Capabled</td>
    <td>
      <input type=radio name=adduser_upload value="1"><font id="adduserULEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_upload value="0" checked><font id="adduserULDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="adduserOW">Overwrite Capabled</td>
    <td>
      <input type=radio name=adduser_overwrite value="1"><font id="adduserOWEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_overwrite value="0" checked><font id="adduserOWDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="adduserER">Erase Capabled</td>
    <td>
      <input type=radio name=adduser_erase value="1"><font id="adduserEREnable">Enable</font>&nbsp;
      <input type=radio name=adduser_erase value="0" checked><font id="adduserERDisable">Disable</font>
    </td>
  </tr>
</table>

<hr />
<br />

<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" id="adduserSmb">Samba Setup</td>
    <td>
      <input type=radio name=adduser_smb value="1"><font id="adduserSmbEnable">Enable</font>&nbsp;
      <input type=radio name=adduser_smb value="0" checked><font id="adduserSmbDisable">Disable</font>
    </td>
  </tr>
</table>

<hr />
<br />

<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type=button style="{width:120px;}" value="Apply" id="adduserApply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" id="adduserCancel" onClick="window.close()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>

