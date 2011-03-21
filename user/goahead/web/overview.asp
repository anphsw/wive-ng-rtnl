<HTML>
<HEAD>
<TITLE>Wive-RTNL - next generation routers firmware for Acorp WiFi CPE</TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<LINK REL="stylesheet" HREF="style/normal_ws.css" TYPE="text/css">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript">
Butterlate.setTextDomain("main");

function initTranslation()
{
	_TR("ovSelectLang", "overview select language");
	_TRV("ovLangApply", "main apply");

	_TR("ovStatus", "overview status link");
	_TR("ovStatistic", "overview statistic link");
	_TR("ovManagement", "overview management link");
}

function initValue()
{
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_zhtw = "<% getLangBuilt("zhtw"); %>";

	initTranslation();
	//lang_element.options.length = 0;
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

		if (s != -1) {
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
</HEAD>

<BODY onLoad="initValue()">
<table class="body"><tr><td>

<H1>Wive-RTNL - firmware for Acorp WiFi CPE</H1>
<p id="ovIntroduction" />

<!-- ----------------- Langauge Settings ----------------- -->
<form method="post" name="Lang" action="/goform/setSysLang">
<blockquote><fieldset>
<legend id="ovSelectLang">Select Language</legend>
<select name="langSelection" id="langSelection">
<!-- added by initValue -->
</select>&nbsp;&nbsp;
<input type="submit" style="{width:50px;}" value="Apply" id="ovLangApply" onClick="return setLanguage()">
</fieldset></blockquote>
</form>

<blockquote><fieldset><p>
<a href="/adm/status.asp" id="ovStatus">Status</a><br />
<a href="/adm/statistic.asp" id="ovStatistic">Statistic</a><br />
<a href="/adm/management.asp" id="ovManagement">Management</a><br />
<br />
</p></fieldset></blockquote>
</td></tr></table>
</BODY>
</HTML>
