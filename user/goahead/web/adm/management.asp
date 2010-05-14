<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>System Management</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

function AdmFormCheck()
{
	if (document.Adm.admuser.value == "")
	{
		alert("Please specify the administrator account.");
		return false;
	}
	if (document.Adm.admpass.value == "")
	{
		alert("Please specify the administrator password.");
		return false;
	}
	return true;
}

function initTranslation()
{
	var e = document.getElementById("manTitle");
	e.innerHTML = _("man title");
	e = document.getElementById("manIntroduction");
	e.innerHTML = _("man introduction");

	e = document.getElementById("manLangApply");
	e.value = _("admin apply");
	e = document.getElementById("manLangCancel");
	e.value = _("admin cancel");

	e = document.getElementById("manLangSet");
	e.innerHTML = _("man language setting");
	e = document.getElementById("manSelectLang");
	e.innerHTML = _("man select language");
	e = document.getElementById("manAdmSet");
	e.innerHTML = _("man admin setting");
	e = document.getElementById("manAdmAccount");
	e.innerHTML = _("man admin account");
	e = document.getElementById("manAdmPasswd");
	e.innerHTML = _("man admin passwd");

	e = document.getElementById("manAdmApply");
	e.value = _("admin apply");
	e = document.getElementById("manAdmCancel");
	e.value = _("admin cancel");
}

function initValue()
{
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_zhtw = "<% getLangBuilt("zhtw"); %>";
	var lang_zhcn = "<% getLangBuilt("zhcn"); %>";

	initTranslation();
	lang_element.options.length = 0;
	if (lang_en == "1")
		lang_element.options[lang_element.length] = new Option('English', 'en');
	if (lang_zhtw == "1")
		lang_element.options[lang_element.length] = new Option('Traditional Chinese', 'zhtw');
	if (lang_zhcn == "1")
		lang_element.options[lang_element.length] = new Option('Simple Chinese', 'zhcn');

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
}

function setLanguage()
{
	document.cookie="language="+document.Lang.langSelection.value+"; path=/";
	parent.menu.location.reload();
	return true;
}

</script>

</head>
<body onload="initValue()">
<table class="body"><tr><td>
<h1 id="manTitle">System Management</h1>
<p id="manIntroduction">You may configure administrator account and password, NTP settings, and Dynamic DNS settings here.</p>
<hr />

<!-- ================= Langauge Settings ================= -->
<form method="post" name="Lang" action="/goform/setSysLang">
<table width="90%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr>
    <td class="title" colspan="2" id="manLangSet">Language Settings</td>
  </tr>
  <tr>
    <td class="head" id="manSelectLang">Select Language</td>
    <td>
      <select name="langSelection" id="langSelection">
        <!-- added by initValue -->
      </select>
    </td>
  </tr>
</table>
<table width="90%" border="0" cellpadding="2" cellspacing="1">
  <tr align="center">
    <td>
      <input type=submit style="{width:120px;}" value="Apply" id="manLangApply" onClick="return setLanguage()"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" id="manLangCancel" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>

<!-- ================= Adm Settings ================= -->
<form method="post" name="Adm" action="/goform/setSysAdm">
<table width="90%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr>
    <td class="title" colspan="2" id="manAdmSet">Adminstrator Settings</td>
  </tr>
  <tr>
    <td class="head" id="manAdmAccount">Account</td>
    <td><input type="text" name="admuser" size="16" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>"></td>
  </tr>
  <tr>
    <td class="head" id="manAdmPasswd">Password</td>
    <td><input type="password" name="admpass" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
  </tr>
</table>
<table width="90%" border="0" cellpadding="2" cellspacing="1">
  <tr align="center">
    <td>
      <input type=submit style="{width:120px;}" value="Apply" id="manAdmApply" onClick="return AdmFormCheck()"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" id="manAdmCancel" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
