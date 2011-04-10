<html>
<head>
<title>MAC Filtering Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<script type="text/javascript" src="/js/controls.js"></script>

<script language="JavaScript" type="text/javascript">
function updateState()
{
	var st = {
		'ftp': '<% getCfgZero(1, "fwAlgFTP"); %>',
		'gre': '<% getCfgZero(1, "fwAlgGRE"); %>',
		'h323': '<% getCfgZero(1, "fwAlgH323"); %>',
		'ipsec': '<% getCfgZero(1, "fwAlgIPSec"); %>',
		'l2tp': '<% getCfgZero(1, "fwAlgL2TP"); %>',
		'pptp': '<% getCfgZero(1, "fwAlgPPTP"); %>',
		'sip': '<% getCfgZero(1, "fwAlgSIP"); %>',
		'tftp': '<% getCfgZero(1, "fwAlgTFTP"); %>'
	};

	for (var field in st)
		setElementChecked('alg_' + field, st[field] == '1');
}
</script>

</head>
<!--     body      -->
<body onload="updateState();">

<table class="body"><tr><td>
<h1 id="dmzTitle">ALG Settings</h1>
<p>On this page you can enable/disable ALG services.</p>
<hr>

<form method="post" name="alg" action="/goform/setFirewallAlg">
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2">ALG Settings</td>
</tr>
<tr>
	<td class="head">FTP</td>
	<td><input type="checkbox" id="alg_ftp" name="alg_ftp"></td>
</tr>
<tr>
	<td class="head">GRE</td>
	<td><input type="checkbox" id="alg_gre" name="alg_gre"></td>
</tr>
<tr>
	<td class="head">H.323</td>
	<td><input type="checkbox" id="alg_h323" name="alg_h323"></td>
</tr>
<tr>
	<td class="head">PPTP</td>
	<td><input type="checkbox" id="alg_pptp" name="alg_pptp"></td>
</tr>
<tr>
	<td class="head">SIP</td>
	<td><input type="checkbox" id="alg_sip" name="alg_sip"></td>
</tr>
<tr>
	<td class="head">TFTP</td>
	<td><input type="checkbox" id="alg_tftp" name="alg_tftp"></td>
</tr>

</table>

<p>
	<input type="hidden" name="submit-url" value="/firewall/alg.asp">
	<input type="submit" value="Apply"> &nbsp;&nbsp;
</p>
</form>

<br><br>

</td></tr></table>
</body>
</html>
