<html>
<head>
<title>Accounting</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">

<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>

<script language="Javascript" type="text/javascript">
function resetClick(form)
{
	form.reset.value = 1;
	form.submit();
}

function initValue()
{
	var form = document.formIptAccounting;
	
	form.iptEnable.value = defaultNumber("<% getCfgZero(1, "ipt_account"); %>", '0');
	var nat_fastpath = defaultNumber("<% getCfgGeneral(1, "natFastpath"); %>", "1");
	displayElement('fastpath_warning', nat_fastpath != '0');
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1>Accounting</h1>
<p>This page contains accounting control management</p>
<hr>

<!-- IP Accounting -->
<h2>IP Accounting</h2>

<p style="display:none;" id="fastpath_warning"><span style="color: #ff0000;"><b>CAUTION!&nbsp;</b></span>NAT fastpath option is turned on.
To gather correct statistics you need to shut down <b>NAT fastpath</b> option on <a href="/services/misc.asp#nat_fastpath_ref">MISC Services</a>
configuration page. Disabling <b>NAT Fastpath</b> will grow CPU load up to 50%.</p>

<form action="/goform/formIptAccounting" method="POST" name="formIptAccounting" >
<table width="95%" border="1" cellspacing="1" cellpadding="2">
<tr>
	<td class="title" colspan="2">IP Accounting Settings</td>
</tr>
<tr>
	<td class="head">IPT accounting</td>
	<td>
		<select name="iptEnable" class="half">
			<option value="0">Disable</option>
			<option value="1">Enable</option>
		</select>
	</td>
</tr>
<tr>
	<td colspan="2">
		<input type="submit" value="Apply">
	</td>
</tr>
</table>

<table width="95%" border="1" cellspacing="1" cellpadding="2">
	<% iptStatList(); %>
</table>

<br>
<input type="hidden" value="/services/account.asp" name="submit-url">
<input type="button" value="Refresh" onClick="window.location.reload();">
<input type="hidden" value="0" name="reset">
<input type="button" onClick="resetClick(this.form);" value="Reset Statistics">
</form>

</td></tr></table>
</body>
</html>
