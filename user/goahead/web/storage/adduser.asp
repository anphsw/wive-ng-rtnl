<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
<META HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Create New User Account</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("storage");
var user0 = '<% getCfgGeneral(1, "Login"); %>';
var guest = '<% getCfgGeneral(1, "FtpAnonymous"); %>';
if (guest = "1")
{
	var user1 = 'anonymous';
}
else
{
	var user1 = "";
}
var user2 = '<% getCfgGeneral(1, "User1"); %>';
var user3 = '<% getCfgGeneral(1, "User2"); %>';
var user4 = '<% getCfgGeneral(1, "User3"); %>';
var user5 = '<% getCfgGeneral(1, "User4"); %>';
var user6 = '<% getCfgGeneral(1, "User5"); %>';
var user7 = '<% getCfgGeneral(1, "User6"); %>';
var user8 = '<% getCfgGeneral(1, "User7"); %>';
var user9 = '<% getCfgGeneral(1, "User8"); %>';
var count = 0;


function initTranslation()
{
	var e = document.getElementById("adduserName");
	e.innerHTML = _("adduser name");
	e = document.getElementById("adduserPW");
	e.innerHTML = _("adduser password");
	e = document.getElementById("adduserMulLogins");
	e.innerHTML = _("adduser mullogins");
	e = document.getElementById("adduserMulLoginsEnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("adduserMulLoginsDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("adduserMaxLogins");
	e.innerHTML = _("adduser maxlogins");
	e = document.getElementById("adduserDL");
	e.innerHTML = _("adduser download");
	e = document.getElementById("adduserDLEnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("adduserDLDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("adduserUL");
	e.innerHTML = _("adduser upload");
	e = document.getElementById("adduserULEnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("adduserULDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("adduserOW");
	e.innerHTML = _("adduser overwrite");
	e = document.getElementById("adduserOWEnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("adduserOWDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("adduserER");
	e.innerHTML = _("adduser erase");
	e = document.getElementById("adduserEREnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("adduserERDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("adduserApply");
	e.value = _("storage apply");
	e = document.getElementById("adduserCancel");
	e.value = _("storage cancel");
}
	
function initValue()
{
	initTranslation();

	document.storage_adduser.adduser_maxlogins.disabled = true;
	for (i=2;i<10;i++)
	{
		if (eval('user'+i) != "")
			count++;
	}
}

function checkData()
{
	if(count >= 8)
	{
		alert('User counts have exceeded Maximum');
		return false;
	}
	else if (document.storage_adduser.adduser_name.value == "")
	{
		alert('Please specify User Name');
		document.storage_adduser.adduser_name.focus();
		document.storage_adduser.adduser_name.select();
		return false;
	}
	else if (document.storage_adduser.adduser_name.value.indexOf(" ") >= 0)
	{
		alert('Don\'t enter Blank Space in this feild');
		document.storage_adduser.adduser_name.focus();
		document.storage_adduser.adduser_name.select();
		return false;
	}
	else
	{
		var i = 0;
		while(i < 10)
		{
			if (document.storage_adduser.adduser_name.value == eval("user"+i))
			{
				alert('This user has exited!');
				return false;
			}
			i++;
		}
	}

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

	return true;
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
    <td class="head" id="adduserName">User Name</td>
    <td>
      <input type=text name=adduser_name size=8 maxlength=8 value="">
    </td>
  </tr>
  <tr>
    <td class="head" id="adduserPW">Password</td>
    <td><input type="password" name="adduser_pw" size="16" maxlength="16" value=""></td>
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
</body>
</html>
