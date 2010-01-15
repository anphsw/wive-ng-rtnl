<html>
<head>
<title>IGD Device Description</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Expires" content="Fri, Jun 12 1981 00:00:00 GMT">
<meta http-equiv="Cache-control" content="no-cache,must-revalidate">
<meta http-equiv="Pragma" content="no-cache">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<!--
<link href="select_style.css" rel="stylesheet" type="text/css">
-->
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">
function change_lang(f)
{
	if (	(f.first_time.value != '1') || 
		(f.preferred_lang_menu.value != 'fake' && f.preferred_lang.value != f.preferred_lang_menu.value)
	)
	{
		f.submit();
		return true;
	}
}
/*
isMenuopen = 0;
function menutrigger(){
	if(!isMenuopen){
		document.getElementById('selectMenucontent').style.display = 'block';
		isMenuopen = 1;
	}
	else{
		document.getElementById('selectMenucontent').style.display = 'none'
		isMenuopen = 0;
	}
}

function selectit(lang, f){  // when select the option, do below aciton.
		if (lang=="EN")
			document.getElementById('selectMenumain').innerHTML = '<img src=\'graph/english_1.gif\'>';
		else if (lang=="TW")
			document.getElementById('selectMenumain').innerHTML = '<img src=\'graph/chinese_1.gif\'>';
		else if (lang=="CN")
			document.getElementById('selectMenumain').innerHTML = '<img src=\'graph/schinese_1.gif\'>';
		document.form.preferred_lang_menu.value = lang;
		document.getElementById('selectMenucontent').style.display = 'none'
		isMenuopen = 0;

		if (f.first_time.value != '1' || document.form.preferred_lang.value != lang)
		{
			f.submit();
			return true;
		}
}
*/
</script>
</head>
<body>
<form method="GET" name="form" action="change_lang.cgi">
<input type="hidden" name="current_page" value="Title.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get_x("IPConnection","wan_nat_x"); %>">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">
<input type="hidden" name="support_cdma" value="<% nvram_get_x("IPConnection","support_cdma"); %>">
<input type="hidden" name="first_time" value="<% nvram_get_x("General","x_Setting"); %>">
<input type="hidden" name="preferred_lang" value="<% nvram_get_x("","preferred_lang"); %>">
<!--
<input type="hidden" name="preferred_lang_menu" value="fake">
-->
<table background="./graph/iBox_title_all.jpg" style="width: 663; height: 81;" border="0" cellspacing="0" cellpadding="0">
<tr>
<td style="text-align: right; vertical-align: bottom;">
<select name="preferred_lang_menu" class="content_input_fd" value="fake" onChange="change_lang(this.form);">
<option class="content_input_fd" value="fake" selected > <#LANG_select#> </option>
<option class="content_input_fd" value="EN" > English </option>
<option class="content_input_fd" value="TW" > 繁體中文 </option>
<option class="content_input_fd" value="CN" > 简体中文 </option>
</select>
<!--
<div id="selectMenumain" onclick="menutrigger();">
<script>
if (document.form.preferred_lang.value=="EN")
	document.write('<img src=\'graph/english_1.gif\'>');
else if (document.form.preferred_lang.value=="TW")
	document.write('<img src=\'graph/chinese_1.gif\'>');
else if (document.form.preferred_lang.value=="CN")
	document.write('<img src=\'graph/schinese_1.gif\'>');
</script>
</div>
<div id="selectMenucontent">
	<div id="En" onClick="selectit('EN', this.form);"><a href="#" class="En"></a></div>
	<div id="Tc" onClick="selectit('TW', this.form);"><a href="#" class="Tc"></a></div>
	<div id="Sc" onClick="selectit('CN', this.form);"><a href="#" class="Sc"></a></div>
</div>
</td>
</tr>
-->
</table>
</form>
</body>
</html>
