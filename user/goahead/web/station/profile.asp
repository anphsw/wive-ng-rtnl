<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<title>Station Profile</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function open_profile_page() {
	window.open("add_profile_page.asp","profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function edit_profile_page(){
	document.sta_profile.hiddenButton.value = 'edit';
	document.sta_profile.submit();
	window.open("edit_profile_page.asp","profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function selectedProfileChange()
{
	document.sta_profile.deleteProfileButton.disabled=false;
	document.sta_profile.editProfileButton.disabled=false;
	document.sta_profile.activateProfileButton.disabled=false;
}

function submit_apply(parm)
{
	document.sta_profile.hiddenButton.value = parm;
	document.sta_profile.submit();
}

function initTranslation()
{
	_TR("profTitle", "prof title");
	_TR("profIntroduction", "prof introduction");

	_TR("profList", "prof list");
	_TR("profSelect", "station select");
	_TR("profProfile", "prof profile");
	_TR("profSSID", "station ssid");
	_TR("profChannel", "station channel");
	_TR("profAuth", "station auth");
	_TR("staproEncryp", "station encryp");
	_TR("staproNetType", "station network type");
	
	_TRV("profAdd", "station add");
	_TRV("profDel", "station del");
	_TRV("profEdit", "station edit");
	_TRV("profActive", "station active");
}

function PageInit()
{
	initTranslation();
}
</script>
</head>


<body onload="PageInit()">
<table class="body"><tr><td>

<h1 id="profTitle">Station Profile</h1>
<p id="profIntroduction">The Status page shows the settings and current operation status of the Station.</p>
<hr>

<form method="post" name="sta_profile" action="/goform/setStaProfile">
<table class="form">
  <tr> 
    <td class="title" colspan="7" id="profList">Pofile List</td>
  </tr>
  <tr>
    <th width=15px id="profSelect">&nbsp;</th>
    <th id="profProfile">Profile</th>
    <th id="profSSID">SSID</th>
    <th id="profChannel">Channel</th>
    <th id="profAuth">Authentication</th>
    <th id="staproEncryp">Encryption</th>
    <th id="staproNetType">Network Type</th>
  </tr>
  <% getStaProfile(); %>
</table>
<br />

<table width="90%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type="button" name="addProfileButton" id="profAdd" class="normal" value="Add" onClick="open_profile_page()"> &nbsp; &nbsp;
    <input type="button" name="deleteProfileButton" id="profDel" class="normal" value="Delete" disabled onClick="submit_apply('delete')"> &nbsp; &nbsp;
    <input type="button" name="editProfileButton" id="profEdit" class="normal" value="Edit" disabled onClick="edit_profile_page()"> &nbsp; &nbsp;
    <input type="button" name="activateProfileButton" id="profActive" class="normal" value="Activate" disabled onClick="submit_apply('activate')"> &nbsp; &nbsp;
  </td>
</tr>
</table>
<input type=hidden name=hiddenButton value="">
</form>

<div class="whitespace">&nbsp;</div>

</td></tr></table>
</body>
</html>

