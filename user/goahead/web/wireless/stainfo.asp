<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>Station List</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
	var e = document.getElementById("stalistTitle");
	e.innerHTML = _("stalist title");
	e = document.getElementById("stalistIntroduction");
	e.innerHTML = _("stalist introduction");
	e = document.getElementById("stalistWirelessNet");
	e.innerHTML = _("stalist wireless network");
	e = document.getElementById("stalistMacAddr");
	e.innerHTML = _("stalist macaddr");
}

function PageInit()
{
	initTranslation();
}
</script>
</head>


<body onLoad="PageInit()">
<table class="body"><tr><td>

<h1 id="stalistTitle">Station List</h1>
<p id="stalistIntroduction"> Here you can monitor stations associated with this AP. </p>
<hr />

<table width="90%" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="10" id="stalistWirelessNet">Wireless Network</td>
  </tr>
  <tr>
    <td bgcolor=#E8F8FF id="stalistMacAddr">MAC Address</td>
    <td bgcolor=#E8F8FF>Aid</td>
    <td bgcolor=#E8F8FF>PSM</td>
    <td bgcolor=#E8F8FF>MimoPS</td>
    <td bgcolor=#E8F8FF>MCS</td>
    <td bgcolor=#E8F8FF>BW</td>
    <td bgcolor=#E8F8FF>SGI</td>
    <td bgcolor=#E8F8FF>STBC</td>
    <td bgcolor=#E8F8FF>RSSI</td>
  </tr>
  <% getWlanStaInfo(); %>
</table>

</td></tr></table>
</body>
</html>

