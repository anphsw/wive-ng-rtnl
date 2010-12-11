<html><head><title>Statistic</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

function initTranslation()
{
	_TR("statisticTitle", "statistic title");
	_TR("statisticIntroduction", "statistic introduction");

	_TR("statisticMM", "statistic memory");
	_TR("statisticMMTotal", "statistic memory total");
	_TR("statisticMMLeft", "statistic memory left");

	_TR("statisticWANLAN", "statistic wanlan");
	_TR("statisticWANRxPkt", "statistic wan rx pkt");
	_TR("statisticWANRxBytes", "statistic wan rx bytes");
	_TR("statisticWANTxPkt", "statistic wan tx pkt");
	_TR("statisticWANTxBytes", "statistic wan tx bytes");
	_TR("statisticLANRxPkt", "statistic lan rx pkt");
	_TR("statisticLANRxBytes", "statistic lan rx bytes");
	_TR("statisticLANTxPkt", "statistic lan tx pkt");
	_TR("statisticLANTxBytes", "statistic lan tx bytes");

	_TR("statisticAllIF", "statistic all interface");
}

function PageInit()
{
	initTranslation();
	
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';
	var dpbsta = '<% getDpbSta(); %>';
	var ethconv = '<% getCfgZero(1, "ethConvert"); %>';
	
	if (((opmode == '0') && (dpbsta == '1') && (ethconv == '1')) || (opmode == '2'))
	{
		showElement("wirelessAbout");
		showElement("wirelessDriverVersion");
		showElement("wirelessMacAddr");
	}
}

function formCheck()
{
	if( document.SystemCommand.command.value == "")
	{
		alert("Please specify a command.");
		return false;
	}

	return true;
}

</script>

</head>
<body onload="PageInit()">
<table class="body"><tr><td>

<h1 id="statisticTitle">Statistic</h1>
<p id="statisticIntroduction"> Take a look at the CPE statistics </p>


<table border="1" cellpadding="2" cellspacing="1" width="95%">
<tbody>

<tr id="wirelessAbout" style="display:none;">
	<td class="title" colspan="2">Wireless About</td>
</tr>
<tr id="wirelessDriverVersion" style="display:none;">
	<td class="head">Driver Version</td>
	<td><% getStaDriverVer(); %></td>
</tr>
<tr id="wirelessMacAddr" style="display:none;">
	<td class="head">Mac Address</td>
	<td><% getStaMacAddr(); %></td>
</tr>

<!-- =================  MEMORY  ================= -->
<tr>
  <td class="title" colspan="2" id="statisticMM">Memory</td>
</tr>
<tr>
  <td class="head" id="statisticMMTotal">Memory total: </td>
  <td> <% getMemTotalASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticMMLeft">Memory left: </td>
  <td> <% getMemLeftASP(); %></td>
</tr>


<!-- =================  WAN/LAN  ================== -->
<tr>
  <td class="title" colspan="2" id="statisticWANLAN">WAN/LAN</td>
</tr>
<tr>
  <td class="head" id="statisticWANRxPkt">WAN Rx packets: </td>
  <td> <% getWANRxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANRxBytes">WAN Rx bytes: </td>
  <td> <% getWANRxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANTxPkt">WAN Tx packets: </td>
  <td> <% getWANTxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANTxBytes">WAN Tx bytes: </td>
  <td> <% getWANTxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANRxPkt">LAN Rx packets: &nbsp; &nbsp; &nbsp; &nbsp;</td>
  <td> <% getLANRxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANRxBytes">LAN Rx bytes: </td>
  <td> <% getLANRxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANTxPkt">LAN Tx packets: </td>
  <td> <% getLANTxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANTxBytes">LAN Tx bytes: </td>
  <td> <% getLANTxByteASP(); %></td>
</tr>

<!-- =================  ALL  ================= -->
<tr>
  <td class="title" colspan="2" id="statisticAllIF">All interfaces</td>
<tr>

<script type="text/javascript">
var i;
var a = [<% getAllNICStatisticASP(); %>];
for(i=0; i<a.length; i+=5)
{
	// name
	document.write("<tr> <td class=head> Name </td><td class=head>");
	document.write(a[i]);
	document.write("</td></tr>");

	// Order is important! rxpacket->rxbyte->txpacket->txbyte
	// rxpacket
	document.write("<tr> <td class=head> Rx Packet </td><td>");
	document.write(a[i+1]);
	document.write("</td></tr>");

	// rxbyte
	document.write("<tr> <td class=head> Rx Byte </td><td>");
	document.write(a[i+2]);
	document.write("</td></tr>");

	// txpacket
	document.write("<tr> <td class=head> Tx Packet </td><td>");
	document.write(a[i+3]);
	document.write("</td></tr>");

	// txbyte
	document.write("<tr> <td class=head> Tx Byte </td><td>");
	document.write(a[i+4]);
	document.write("</td></tr>");
}
</script>

</tbody>
</table>

</td></tr></table>
</body></html>

