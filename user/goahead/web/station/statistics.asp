<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Wireless Station Statistics</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
	var e = document.getElementById("statisticTitle");
	e.innerHTML = _("statistic title");
	e = document.getElementById("statisticIntroduction");
	e.innerHTML = _("statistic introduction");

	e = document.getElementById("statisticTx");
	e.innerHTML = _("statistic tx");
	e = document.getElementById("statisticRx");
	e.innerHTML = _("statistic rx");
	e = document.getElementById("statisticResetCounter");
	e.value = _("statistic reset counter");
}

function PageInit()
{
	initTranslation();
}
</script>
</head>


<body onload="PageInit()">
<table class="body"><tr><td>

<h1 id="statisticTitle">Station Statistics</h1>
<p id="statisticIntroduction">The Status page shows the settings and current operation status of the Station.</p>
<hr />

<form method=post name="sta_statistics" action="/goform/resetStaCounters">
<table width="90%" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td class="title" colspan="2" id="statisticTx">Transmit Statistics</td>
  </tr>
  <% getStaStatsTx(); %>
  <!--
  <tr>
    <td class="title" colspan="2" id="statisticRx">Receive Statistics</td>
  </tr>
  <tr>
    <td class="head">Frames Received Successfully</td>
    <td><% getStaStatsRxOk(); %></td>
  </tr>
  -->

  <tr>
    <td width="65%" bgcolor="#E8F8FF" >Frames Received With CRC Error </td>
    <td><% getStaStatsRxCRCErr(); %></td>
  </tr>

  <tr>
    <td width="65%" bgcolor="#E8F8FF" >Frames Dropped Due To Out-of-Resource</td>
    <td><% getStaStatsRxNoBuf(); %></td>
  </tr>

  <tr>
    <td width="65%" bgcolor="#E8F8FF" >Duplicate Frames Received </td>
    <td><% getStaStatsRxDup(); %></td>
  </tr>
</table>
<input type=hidden name=dummyData value="1">
<br />

<table width="90%" border="0" cellpadding="2" cellspacing="1">
  <tr align="center">
    <td>	
      <input type="submit" style="{width:120px;}" value="Reset Counters" id="statisticResetCounter">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>


