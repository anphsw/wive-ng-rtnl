<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>FTP Settings</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var ftpenabled = '<% getCfgZero(1, "FtpEnabled"); %>';
var anonymous = '<% getCfgZero(1, "FtpAnonymous"); %>';
var port = '<% getCfgGeneral(1, "FtpPort"); %>';
var maxusers = '<% getCfgGeneral(1, "FtpMaxUsers"); %>';
var logintimeout = '<% getCfgGeneral(1, "FtpLoginTimeout"); %>';
var staytimeout = '<% getCfgGeneral(1, "FtpStayTimeout"); %>';

function initTranslation()
{
	var e = document.getElementById("ftpTitle");
	e.innerHTML = _("ftp title");
	e = document.getElementById("ftpIntroduction");
	e.innerHTML = _("ftp introduction");

	e = document.getElementById("ftpSrvSet");
	e.innerHTML = _("ftp server setup");
	e = document.getElementById("ftpSrv");
	e.innerHTML = _("ftp server enable");
	e = document.getElementById("ftpSrvEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("ftpSrvDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("ftpSrvAnonymous");
	e.innerHTML = _("ftp server anonymous");
	e = document.getElementById("ftpSrvAnonymousEnable");
	e.innerHTML = _("usb enable");
	e = document.getElementById("ftpSrvAnonymousDisable");
	e.innerHTML = _("usb disable");
	e = document.getElementById("ftpSrvPort");
	e.innerHTML = _("ftp server port");
	e = document.getElementById("ftpSrvMaxUsers");
	e.innerHTML = _("ftp server maxusers");
	e = document.getElementById("ftpSrvLoginTimeout");
	e.innerHTML = _("ftp server logintimeout");
	e = document.getElementById("ftpSrvStayTimeout");
	e.innerHTML = _("ftp server staytimeout");

	e = document.getElementById("ftpApply");
	e.value = _("usb apply");
	e = document.getElementById("ftpReset");
	e.value = _("usb reset");
}

function initValue()
{
	initTranslation();

	/*
	alert(ftpenabled);
	alert(anonymous);
	alert(port);
	alert(maxusers);
	alert(logintimeout);
	alert(staytimeout);
	*/

	document.storage_ftp.ftp_anonymous[0].disabled = true;
	document.storage_ftp.ftp_anonymous[1].disabled = true;
	document.storage_ftp.ftp_port.disabled = true;
	document.storage_ftp.ftp_max_users.disabled = true;
	document.storage_ftp.ftp_login_timeout.disabled = true;
	document.storage_ftp.ftp_stay_timeout.disabled = true;

	if (ftpenabled == "1")
	{
		// alert("FTP E");
		document.storage_ftp.ftp_enabled[0].checked = true;
		document.storage_ftp.ftp_anonymous[0].disabled = false;
		document.storage_ftp.ftp_anonymous[1].disabled = false;
		if (anonymous == 1)
		{
			document.storage_ftp.ftp_anonymous[0].checked = true;
		}
		else
		{
			document.storage_ftp.ftp_anonymous[1].checked = true;
		}

		document.storage_ftp.ftp_port.disabled = false;
		document.storage_ftp.ftp_port.value = port;

		document.storage_ftp.ftp_max_users.disabled = false;
		document.storage_ftp.ftp_max_users.value = maxusers;

		document.storage_ftp.ftp_login_timeout.disabled = false;
		document.storage_ftp.ftp_login_timeout.value = logintimeout;

		document.storage_ftp.ftp_stay_timeout.disabled = false;
		document.storage_ftp.ftp_stay_timeout.value = staytimeout;
	}
	else
	{
		// alert("FTP D");
		document.storage_ftp.ftp_enabled[1].checked = true;
	}
}

function CheckValue()
{
	if (document.storage_ftp.ftp_enabled[0].checked == true)
	{
		if (document.storage_ftp.ftp_port.value == "")
		{
			alert('Please specify FTP Port');
			document.storage_ftp.ftp_port.focus();
			document.storage_ftp.ftp_port.select();
			return false;
		}
		else if (isNaN(document.storage_ftp.ftp_port.value) ||
			 parseInt(document.storage_ftp.ftp_port.value,10) > 65535)
		{
			alert('Please specify valid number');
			document.storage_ftp.ftp_port.focus();
			document.storage_ftp.ftp_port.select();
			return false;
		}

		if (document.storage_ftp.ftp_max_users.value == "")
		{
			alert('Please specify FTP Max. Users');
			document.storage_ftp.ftp_max_users.focus();
			document.storage_ftp.ftp_max_users.select();
			return false;
		}
		else if (isNaN(document.storage_ftp.ftp_max_users.value))
		{
			alert('Please specify valid number');
			document.storage_ftp.ftp_max_users.focus();
			document.storage_ftp.ftp_max_users.select();
			return false;
		}
		
		if (document.storage_ftp.ftp_login_timeout.value == "")
		{
			alert('Please specify FTP Login Timeout');
			document.storage_ftp.ftp_login_timeout.focus();
			document.storage_ftp.ftp_login_timeout.select();
			return false;
		}
		else if (isNaN(document.storage_ftp.ftp_login_timeout.value))
		{
			alert('Please specify valid number');
			document.storage_ftp.ftp_login_timeout.focus();
			document.storage_ftp.ftp_login_timeout.select();
			return false;
		}

		if (document.storage_ftp.ftp_stay_timeout.value == "")
		{
			alert('Please specify FTP Stay Timeout');
			document.storage_ftp.ftp_stay_timeout.focus();
			document.storage_ftp.ftp_stay_timeout.select();
			return false;
		}
		else if (isNaN(document.storage_ftp.ftp_stay_timeout.value))
		{
			alert('Please specify valid number');
			document.storage_ftp.ftp_stay_timeout.focus();
			document.storage_ftp.ftp_stay_timeout.select();
			return false;
		}
	}

	return true;
}

function ftp_enable_switch()
{
	if (document.storage_ftp.ftp_enabled[1].checked == true)
	{
		document.storage_ftp.ftp_anonymous[0].disabled = true;
		document.storage_ftp.ftp_anonymous[1].disabled = true;
		document.storage_ftp.ftp_port.disabled = true;
		document.storage_ftp.ftp_max_users.disabled = true;
		document.storage_ftp.ftp_login_timeout.disabled = true;
		document.storage_ftp.ftp_stay_timeout.disabled = true;
	}
	else
	{
		document.storage_ftp.ftp_anonymous[0].disabled = false;
		document.storage_ftp.ftp_anonymous[1].disabled = false;
		document.storage_ftp.ftp_port.disabled = false;
		document.storage_ftp.ftp_max_users.disabled = false;
		document.storage_ftp.ftp_login_timeout.disabled = false;
		document.storage_ftp.ftp_stay_timeout.disabled = false;
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>


<h1 id="ftpTitle">FTP Settings </h1>
<p id="ftpIntroduction"></p>
<hr />

<form method=post name=storage_ftp action="/goform/storageFtpSrv" onSubmit="return CheckValue()">
<table width="540" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="2" id="ftpSrvSet">FTP Server Setup</td>
  </tr>
  <tr> 
    <td class="head" id="ftpSrv">FTP Server</td>
    <td>
      <input type=radio name=ftp_enabled value="1" onClick="ftp_enable_switch()"><font id="ftpSrvEnable">Enable</font>&nbsp;
      <input type=radio name=ftp_enabled value="0" onClick="ftp_enable_switch()" checked><font id="ftpSrvDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="ftpSrvAnonymous">Anonymous Login</td>
    <td>
      <input type=radio name=ftp_anonymous value="1"><font id="ftpSrvAnonymousEnable">Enable</font>&nbsp;
      <input type=radio name=ftp_anonymous value="0" checked><font id="ftpSrvAnonymousDisable">Disable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="ftpSrvPort">FTP Port</td>
    <td>
      <input type=text name=ftp_port size=5 maxlength=5 value="21">
    </td>
  </tr>
  <tr>
    <td class="head" id="ftpSrvMaxUsers">Max. Users</td>
    <td>
      <input type=text name=ftp_max_users size=2 maxlength=2 value="10">
    </td>
  </tr>
  <tr>
    <td class="head" id="ftpSrvLoginTimeout">Login Timeout</td>
    <td>
      <input type=text name=ftp_login_timeout size=4 maxlength=4 value="120">
    </td>
  </tr>
  <tr>
    <td class="head" id="ftpSrvStayTimeout">Stay Timeout</td>
    <td>
      <input type=text name=ftp_stay_timeout size=4 maxlength=4 value="240">
    </td>
  </tr>
</table>
<hr />
<br />
<table width = "540" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type=submit style="{width:120px;}" value="Apply" id="ftpApply"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Reset" id="ftpReset" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>

