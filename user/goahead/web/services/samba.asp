<html>
<head>
<title>Samba/CIFS setup</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">

<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>

<script language="Javascript" type="text/javascript">
function initValue()
{
	var form = document.formWins;
	
	/*
	form.iptEnable.value = defaultNumber("<% getCfgZero(1, "ipt_account"); %>", '0');
	var nat_fastpath = defaultNumber("<% getCfgGeneral(1, "natFastpath"); %>", "1");
	displayElement('fastpath_warning', nat_fastpath != '0');
	*/
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1>Samba/CIFS setup</h1>
<p>This page contains Samba/CIFS service configuration setup.</p>
<hr>

<!-- IP Accounting -->
<h2>WINS</h2>

<p>Here you can configure WINS (Windows Internet Name Service).</p>

<form action="/goform/formWins" method="POST" name="formWins" >
<table width="95%" border="1" cellspacing="1" cellpadding="2">
<tr>
	<td class="title" colspan="2">WINS Settings</td>
</tr>
<tr>
	<td class="head">Enable WINS</td>
	<td>
		<select name="winsEnable" class="half">
			<option value="0">Disable</option>
			<option value="1">Enable</option>
		</select>
	</td>
</tr>
</table>

<br>
<input type="hidden" value="/services/samba.asp" name="submit-url">
<input type="submit" value="Apply">
</form>

</td></tr></table>
</body>
</html>
