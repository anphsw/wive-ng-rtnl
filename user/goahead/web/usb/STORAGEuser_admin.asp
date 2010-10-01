<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>Administration</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var user0 = '<% getCfgGeneral(1, "Login"); %>';

var guest = '<% getCfgZero(1, "FtpAnonymous"); %>';
if (guest == "1")
{
	var user9 = 'anonymous';
}

var user1 = '<% getCfgGeneral(1, "User1"); %>';
var upw1 = '<% getCfgGeneral(1, "UserPasswd1"); %>';
var ftpuser1 = '<% getCfgZero(1, "FtpUser1"); %>';
var ftpmaxlogins1 = '<% getCfgGeneral(1, "FtpMaxLogins1"); %>';
var ftpmode1 = '<% getCfgGeneral(1, "FtpMode1"); %>';
var smbuser1 = '<% getCfgZero(1, "SmbUser1"); %>';

var user2 = '<% getCfgGeneral(1, "User2"); %>';
var upw2 = '<% getCfgGeneral(1, "UserPasswd2"); %>';
var ftpuser2 = '<% getCfgZero(1, "FtpUser2"); %>';
var ftpmaxlogins2 = '<% getCfgGeneral(1, "FtpMaxLogins2"); %>';
var ftpmode2 = '<% getCfgGeneral(1, "FtpMode2"); %>';
var smbuser2 = '<% getCfgZero(1, "SmbUser2"); %>';

var user3 = '<% getCfgGeneral(1, "User3"); %>';
var upw3 = '<% getCfgGeneral(1, "UserPasswd3"); %>';
var ftpuser3 = '<% getCfgZero(1, "FtpUser3"); %>';
var ftpmaxlogins3 = '<% getCfgGeneral(1, "FtpMaxLogins3"); %>';
var ftpmode3 = '<% getCfgGeneral(1, "FtpMode3"); %>';
var smbuser3 = '<% getCfgZero(1, "SmbUser3"); %>';

var user4 = '<% getCfgGeneral(1, "User4"); %>';
var upw4 = '<% getCfgGeneral(1, "UserPasswd4"); %>';
var ftpuser4 = '<% getCfgZero(1, "FtpUser4"); %>';
var ftpmaxlogins4 = '<% getCfgGeneral(1, "FtpMaxLogins4"); %>';
var ftpmode4 = '<% getCfgGeneral(1, "FtpMode4"); %>';
var smbuser4 = '<% getCfgZero(1, "SmbUser4"); %>';

var user5 = '<% getCfgGeneral(1, "User5"); %>';
var upw5 = '<% getCfgGeneral(1, "UserPasswd5"); %>';
var ftpuser5 = '<% getCfgZero(1, "FtpUser5"); %>';
var ftpmaxlogins5 = '<% getCfgGeneral(1, "FtpMaxLogins5"); %>';
var ftpmode5 = '<% getCfgGeneral(1, "FtpMode5"); %>';
var smbuser5 = '<% getCfgZero(1, "SmbUser5"); %>';

var user6 = '<% getCfgGeneral(1, "User6"); %>';
var upw6 = '<% getCfgGeneral(1, "UserPasswd6"); %>';
var ftpuser6 = '<% getCfgZero(1, "FtpUser6"); %>';
var ftpmaxlogins6 = '<% getCfgGeneral(1, "FtpMaxLogins6"); %>';
var ftpmode6 = '<% getCfgGeneral(1, "FtpMode6"); %>';
var smbuser6 = '<% getCfgZero(1, "SmbUser6"); %>';

var user7 = '<% getCfgGeneral(1, "User7"); %>';
var upw7 = '<% getCfgGeneral(1, "UserPasswd7"); %>';
var ftpuser7 = '<% getCfgZero(1, "FtpUser7"); %>';
var ftpmaxlogins7 = '<% getCfgGeneral(1, "FtpMaxLogins7"); %>';
var ftpmode7 = '<% getCfgGeneral(1, "FtpMode7"); %>';
var smbuser7 = '<% getCfgZero(1, "SmbUser7"); %>';

var user8 = '<% getCfgGeneral(1, "User8"); %>';
var upw8 = '<% getCfgGeneral(1, "UserPasswd8"); %>';
var ftpuser8 = '<% getCfgZero(1, "FtpUser8"); %>';
var ftpmaxlogins8 = '<% getCfgGeneral(1, "FtpMaxLogins8"); %>';
var ftpmode8 = '<% getCfgGeneral(1, "FtpMode8"); %>';
var smbuser8 = '<% getCfgZero(1, "SmbUser8"); %>';

var ftpenabled = '<% getCfgZero(1, "FtpEnabled"); %>';
var smbenabled = '<% getCfgZero(1, "SmbEnabled"); %>';
var count = 0;

/*
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
*/

function initTranslation()
{
	var e = document.getElementById("storageAdmTitle");
	e.innerHTML = _("storage adm title");
	e = document.getElementById("storageAdmIntroduction");
	e.innerHTML = _("storage adm introduction");

	e = document.getElementById("storageAdmUser");
	e.innerHTML = _("storage adm user");
	e = document.getElementById("storageAdmUserName");
	e.innerHTML = _("storage user name");
	e = document.getElementById("storageAdmUserFtp");
	e.innerHTML = _("storage user ftp");
	e = document.getElementById("storageAdmUserSmb");
	e.innerHTML = _("storage user smb");

	e = document.getElementById("storageAdmUserAdd");
	e.value = _("usb add");
	e = document.getElementById("storageAdmUserEdit");
	e.value = _("usb edit");
	e = document.getElementById("storageAdmUserDel");
	e.value = _("usb del");
	e = document.getElementById("storageAdmApply");
	e.value = _("usb apply");
}

function initValue()
{
	var tmp;

	initTranslation();
	/*
	alert(ftpenabled);
	alert(anonymous);
	alert(port);
	alert(maxusers);
	alert(logintimeout);
	alert(staytimeout);
	*/
	for (i=1;i<9;i++)
	{
		if (eval('user'+i) != "")
			count++;
	}
}

/*
function CheckValue()
{

	return true;
}
*/

function submit_apply(parm)
{
	document.storage_user_adm.hiddenButton.value = parm;
	document.storage_user_adm.submit();
}

function open_useradd_window()
{
	document.storage_user_adm.hiddenUser0.value = user0;
	document.storage_user_adm.hiddenUser1.value = user1;
	document.storage_user_adm.hiddenUser2.value = user2;
	document.storage_user_adm.hiddenUser3.value = user3;
	document.storage_user_adm.hiddenUser4.value = user4;
	document.storage_user_adm.hiddenUser5.value = user5;
	document.storage_user_adm.hiddenUser6.value = user6;
	document.storage_user_adm.hiddenUser7.value = user7;
	document.storage_user_adm.hiddenUser8.value = user8;
	if(count >= 8)
		alert("User Accounts have exceeded Maximun!");
	else
		window.open("STORAGEadduser.asp","Storage_User_Add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=640")
}

function open_useredit_window()
{
	var i = 0;
	var index = 0;

	if (count == 0)
	{
		alert("No any user can be choosed!");
	}
	else
	{
		while (i < count)
		{
			if (count == 1)
			{
				if (document.storage_user_adm.storage_user_select.checked == true)
				{
					index = document.storage_user_adm.storage_user_select.value;
					break;
				}
			}
			else if (count > 1)
			{
				if (document.storage_user_adm.storage_user_select[i].checked == true)
				{
					index = document.storage_user_adm.storage_user_select[i].value;
					break;
				}
			}
			i++;
		}
		// alert("user: "+index);
		if (index > 0)
		{
			document.storage_user_adm.hiddenUser0.value = user0;
			document.storage_user_adm.hiddenUser1.value = user1;
			document.storage_user_adm.hiddenUser2.value = user2;
			document.storage_user_adm.hiddenUser3.value = user3;
			document.storage_user_adm.hiddenUser4.value = user4;
			document.storage_user_adm.hiddenUser5.value = user5;
			document.storage_user_adm.hiddenUser6.value = user6;
			document.storage_user_adm.hiddenUser7.value = user7;
			document.storage_user_adm.hiddenUser8.value = user8;
			document.storage_user_adm.selectIndex.value = index;
			document.storage_user_adm.selectUser.value = eval('user'+index);
			document.storage_user_adm.selectPassword.value = eval('upw'+index);
			document.storage_user_adm.selectFtp.value = eval('ftpuser'+index);
			document.storage_user_adm.selectMaxLogins.value = eval('ftpmaxlogins'+index);
			document.storage_user_adm.selectMode.value = eval('ftpmode'+index);
			document.storage_user_adm.selectSmb.value = eval('smbuser'+index);
			window.open("STORAGEedituser.asp","Storage_User_Edit","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=640")
		}
		else
		{
			alert("please select one user account!");
		}
	}
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>
<h1 id="storageAdmTitle">Administration</h1>
<p id="storageAdmIntroduction"></p>
<hr />
<form method=post name=storage_user_adm action="/goform/storageAdm">
<input type=hidden name=hiddenButton value="">
<input type=hidden name=hiddenUser0 value="">
<input type=hidden name=hiddenUser1 value="">
<input type=hidden name=hiddenUser2 value="">
<input type=hidden name=hiddenUser3 value="">
<input type=hidden name=hiddenUser4 value="">
<input type=hidden name=hiddenUser5 value="">
<input type=hidden name=hiddenUser6 value="">
<input type=hidden name=hiddenUser7 value="">
<input type=hidden name=hiddenUser8 value="">
<input type=hidden name=selectIndex value="">
<input type=hidden name=selectUser value="">
<input type=hidden name=selectPassword value="">
<input type=hidden name=selectFtp value="">
<input type=hidden name=selectMaxLogins value="">
<input type=hidden name=selectMode value="">
<input type=hidden name=selectSmb value="">
<table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="4"><font id="storageAdmUser">User Management</font>
      </td>
  </tr>
  <tr align=center> 
    <td bgcolor="#E8F8FF" width=15px id="storageAdmUserSelect">&nbsp;</td>
    <td bgcolor="#E8F8FF" id="storageAdmUserName">User Name</td>
    <td bgcolor="#E8F8FF" id="storageAdmUserFtp">FTP used</td>
    <td bgcolor="#E8F8FF" id="storageAdmUserSmb">Samba used</td>
  </tr>
<script language="JavaScript" type="text/javascript">
var TRDHeader = "<tr align=center><td>";
var TDConneter = "</td><td>";
var TDFooter = "</td><tr>";

document.write(TRDHeader+"--"+TDConneter+user0+TDConneter);
if (ftpenabled == "1")
	document.write("E"+TDConneter);
else
	document.write(_("usb disable")+TDConneter);

if (smbenabled == "1")
	document.write(_("usb enable")+TDFooter);
else
	document.write(_("usb disable")+TDFooter);

if (guest == "1")
	document.write(TRDHeader+"--"+TDConneter+user9+TDConneter+"MD"+TDConneter+_("storage disable")+TDFooter);
for (var i=1;i<9;i++)
{
	if (eval("user"+i) != "")
	{
		var item = TRDHeader;
		item += "<input type=radio name=storage_user_select value="+i+">";
		item += TDConneter;
		item += eval("user"+i);
		item += TDConneter;
		if (eval("ftpuser"+i) == "1" && ftpenabled == "1")
		{
			item += eval("ftpmode"+i);
			item += TDConneter;
		}
		else
		{
			item += _("usb disable");
			item += TDConneter;
		}
		if (eval("smbuser"+i) == "1" && smbenabled == "1")
			item += _("usb enable");	
		else
			item += _("usb disable");
		item += TDFooter;		
		document.write(item);	
	}
}
</script>
</table>
<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type="button" style="{width:80px;}" name=storage_user_add value="Add" id="storageAdmUserAdd" onClick="open_useradd_window()">&nbsp;&nbsp;
      <input type="button" style="{width:80px;}" name=storage_user_edit value="edit" id="storageAdmUserEdit" onClick="open_useredit_window()">&nbsp;&nbsp;
      <input type="button" style="{width:80px;}" value="Delete" id="storageAdmUserDel" onClick="submit_apply('delete')"> &nbsp; &nbsp;
</table>
<hr />
<br>
<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type="button" style="{width:80px;}" value="Apply" id="storageAdmApply" onClick="submit_apply('apply')"> 
      <input type="button" style="{width:80px;}" value="Cancle" id="storageCancel" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>

