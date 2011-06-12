<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Wireless Station Site Survey</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var bssid_list = [ <% getStaBSSIDList(); %> ];
var cwin = null;
var pcwin = null;

var colors = [
	'ff0000', '00ff00', '0000ff', 'ffff00', '00ffff', 'ff00ff',
	'c00000', '00c000', '0000c0', 'c0ff00', 'ffc000', 'c000ff', 'ff00c0', '00c0ff', '00ffc0', 'c0c000', 'c000c0', '00c0c0',
	'800000', '008000', '000080', '808000', '800080', '008080', '80ff00', 'ff8000', '8000ff', 'ff8000', '0080ff', '00ff80', '80c000', 'c08000', '8000c0', 'c00080', '0080c0', '00c080'
];

function countTime()
{
	//var connectstatus = '<!--#include ssi=getStaConnectionStatus() -->';

	//if (connectstatus == 1)  // 0 is NdisMediaStateConnected, 1 is NdisMediaStateDisconnected
		self.setTimeout("window.location.reload();", 1000*4);
}

function open_connection_page()
{
	cwin = window.open("site_survey_connection.asp","sta_site_survey_connection","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function open_profile_page()
{
	pcwin = window.open("add_profile_page.asp","add_profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function genStaTable()
{
	var html = '<table border="1" cellpadding="2" cellspacing="1" width="600">';
	
	html += '<tr><td class="title" colspan="8" id="scanSiteSurvey">Site Survey</td></tr>'; // Header
	html += '<tr><th>Sel</th>' +
		'<th>SSID</th>' +
		'<th>BSSID</th>' +
		'<th>RSSI</th>' +
		'<th>Channel</th>' +
		'<th>Encryption</th>' +
		'<th>Authentication</th>' +
		'<th>Network Type</th></td>';
	
	for (var i=0; i<bssid_list.length; i++)
	{
		var row = bssid_list[i];
		var color = colors[i % colors.length];
		var style = (row[0] == 1) ? ' style="color: #ffffff; background-color: #008000;">' : '>'; // Category
		var sel = (row[0] == 1) ? ' checked' : '';

		html += '<tr' + style;
		html += '<td style="background-color: #' + color + '">' +
				'<input name="bssid_sel" type="radio"' + sel + ' value="' + (i+1) +
				'" onchange="sta_select_change(this.form);"></td>' + // rownum
			'<td' + style + row[1] + '</td>' + // SSID
			'<td' + style + row[2] + '</td>' + // BSSID
			'<td' + style + row[4] + '</td>' + // RSSI
			'<td' + style + row[5] + '</td>' + // Channel
			'<td' + style + row[6] + '</td>' + // Encryption
			'<td' + style + row[7] + '</td>' + // Authentication
			'<td' + style + row[8] + '</td>'; // Network Type
		
		html += '</tr>';
	}
	
	html += '</table>';
	
	setInnerHTML('ajxCtxStaTable', html);
}

function current_bssid()
{
	var form = document.sta_site_survey;
	for (var i=0; i<form.bssid_sel.length; i++)
		if (form.bssid_sel[i].checked)
			return bssid_list[i];
	return null;
}

function sta_select_change(form)
{
	var bssid = current_bssid();
	var dis = bssid[0] == 1;
	disableElement(form.connectionButton, dis);
	disableElement(form.addProfileButton, dis);
}

function drawSVG(target, name, attributes, html)
{
	var item = document.createElementNS('http://www.w3.org/2000/svg', name);
	for (var key in attributes)
		item.setAttribute(key, attributes[key]);
	if (html != null)
	{
		if (name == 'text')
			item.textContent = html;
		else
			item.innerHTML = html;
	}
	if (target != null)
		target.appendChild(item);
	return item;
}

function showGraph(svg)
{
	var w = svg.getAttribute('width');
	var h = svg.getAttribute('height');
	var pad = [60, 10, 30, 50 ]; // Left, right, top, bottom
	var s_width = 5;
	var r_power = [-90, 0, 10]; // Min, Max, Step
	var chans = [ 2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462, 2467, 2472, 2484 ];
	var chan_sh = [ 10, 10 ]; // Left, right
	
	// Background
	drawSVG(svg, 'rect',
		{
			'fill': '#000000',
			'fill-opacity:': '1',
			'fill-rule': 'evenodd',
			'stroke': 'none',
			'x': '0',
			'y': '0',
			'width': w,
			'height': h
		}
	);
	
	// Show channels
	for (var chan = 0; chan<chans.length; chan++)
	{
		var dist = chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
		var x = Math.round(pad[0] + (w - pad[0] - pad[1]) * ((chans[chan] - chans[0] + chan_sh[0])/dist));
		drawSVG(svg, 'line',
			{
				'x1': x,
				'y1': pad[2],
				'x2': x,
				'y2': h - pad[3] + s_width + (chan % 2)*15,
				'style': 'stroke:#c0c0c0;',
				'fill': 'none',
				'stroke-dasharray': '2,2'
			}
		);
		
		// Channel
		drawSVG(svg, 'text',
			{
				'fill': "#ffffff",
				'x': x,
				'y': pad[2]-5,
				'text-anchor': 'middle',
				'dominant-baseline' : 'text-after-edge',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			chan + 1
		);
		
		// Frequence
		drawSVG(svg, 'text',
			{
				'fill': "#ffffff",
				'x': x,
				'y': h - pad[3] + s_width + 5 + (chan % 2)*15,
				'dominant-baseline' : 'text-before-edge',
				'text-anchor': 'middle',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			chans[chan]
		);
	}
	
	// Draw horizontal grid
	for (var power = r_power[0]; power <= r_power[1]; power += r_power[2])
	{
		var dist = r_power[0]-r_power[1];
		var red = Math.round(255*(power/dist));
		var green = Math.round(255*((dist - power)/dist));
		var y = Math.round(pad[2] + (h - pad[2] - pad[3]) * ((-power)/(r_power[2]-dist)));
		drawSVG(svg, 'line',
			{
				'x1': pad[0] - s_width,
				'y1': y,
				'x2': w - pad[1],
				'y2': y,
				'style': 'stroke:rgb(' + red + ',' + green +',0);',
				'stroke-dasharray': '2,2'
			}
		);
		
		// DBm
		drawSVG(svg, 'text',
			{
				'fill': 'rgb(' + red + ',' + green +',0)',
				'x': pad[0] - s_width - 5,
				'y': y,
				'text-anchor': 'end',
				'dominant-baseline' : 'central',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			power
		);
	}

	// Now draw stations
	for (var i=0; i<bssid_list.length; i++)
	{
		var sta = bssid_list[i];
		var dist = chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
		var dx = w - pad[0] - pad[1];
		var dy = h - pad[2] - pad[3];
		var x1 = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0] - 10)/dist));
		var x2 = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0] + 10)/dist));
		var y  = Math.round(pad[2] + dy * ((-sta[3])/(r_power[2]-r_power[0]+r_power[1])));
		var col= colors[i % bssid_list.length];
		
		drawSVG(svg, 'rect',
			{
				'fill': '#' + col,
				'stroke': '#' + col,
				'x': x1,
				'y': y,
				'width': x2-x1,
				'height': h - pad[3] - y,
				'stroke-dasharray': '2,2',
				'style': 'fill-opacity: 0.45; stroke-opacity: 1;'
			}
		);
	}
	
	// Now draw text for stations
	for (var i=0; i<bssid_list.length; i++)
	{
		var sta = bssid_list[i];
		var x = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0])/dist));
		var y = Math.round(pad[2] + dy * ((-sta[3])/(r_power[2]-r_power[0]+r_power[1])));
		var col= colors[i % bssid_list.length];

		drawSVG(svg, 'text',
			{
				'fill': '#' + col,
				'x': x,
				'y': y-5,
				'text-anchor': 'middle',
				'dominant-baseline' : 'text-after-edge',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			sta[1]
		);
	}
	
	// Vertical line
	drawSVG(svg, 'line', { 'x1': pad[0], 'y1': pad[2], 'x2': pad[0], 'y2': h - pad[3] + s_width, 'style': 'stroke:#ffffff;stroke-width:2px;', 'fill': 'none' } );
	// Horizontal line
	drawSVG(svg, 'line', { 'x1': pad[0] - s_width, 'y1': h - pad[3], 'x2': w - pad[1], 'y2': h - pad[3], 'style': 'stroke:#ffffff;stroke-width:2px;', 'fill': 'none' } );
	
	// Draw text
	drawSVG(svg, 'text',
		{
			'fill': 'rgb(' + red + ',' + green +',0)',
			'x': 0,
			'y': 0,
			'text-anchor': 'middle',
			'dominant-baseline' : 'central',
			'transform' : 'rotate(-90),translate(' + (-h/2) + ',15)',
			'text-rendering': "optimizeLegibility",
			'font-size': "10pt", 'font-family': "Tahoma",
			'fill-rule': "evenodd"
		},
		'Amplitude [dB]'
	);
}

function PageInit()
{
	genStaTable();
	
	var div = document.getElementById('ajxCtxStaGraph');
	var el = drawSVG(div, 'svg', { 'width': 600, 'height': 300});
	showGraph(el);
}

function showConnectionSsid()
{
	var form = cwin.document.forms["sta_site_survey_connection"];
	var bssid = current_bssid();
	var g_auth = bssid[7];
	var g_encry = bssid[6];
	
	form.Ssid.value = bssid[1];
	form.bssid.value = bssid[2];

	if (bssid[9] == 1)
	{
		var infra = form.security_infra_mode;
		if (g_auth.indexOf("WPA2-PSK") >= 0)
			infra.value = 7;
		else if (g_auth.indexOf("WPA-PSK") >= 0)
			infra.value = 4;
		else if (g_auth.indexOf("WPA2") >= 0)
			infra.value = 6;
		else if (g_auth.indexOf("WPA") >= 0)
			infra.value = 3;
		else
			infra.value = 0;
	}
	else
	{
		var adhoc = form.security_adhoc_mode;
		if ( g_auth.indexOf("WPA-NONE") >= 0 || g_auth.indexOf("WPA2-NONE") >= 0)
			adhoc.value = 5;
		else
			adhoc.value = 0;
	}

	//encry
	if (g_encry.indexOf("Not Use") >= 0)
		form.openmode.value = 1;
	else if (g_encry.indexOf("AES") >= 0)
		form.cipher[1].checked = true;
	else if (g_encry.indexOf("TKIP") >= 0)
		form.cipher[0].checked = true;
	else
		form.openmode.value = 0;

	form.network_type.value = bssid[9];
}

function showProfileSsid()
{
	var form = pcwin.document.forms["profile_page"];
	var bssid = current_bssid();
	var g_auth = bssid[7];
	var g_encry = bssid[6];
	
	form.Ssid.value = bssid[1];

	if (bssid[9] == 1)
	{
		var infra = form.security_infra_mode;
		if (g_auth.indexOf("WPA2-PSK") >= 0)
			infra.value = 7;
		else if (g_auth.indexOf("WPA-PSK") >= 0)
			infra.value = 4;
		else if (g_auth.indexOf("WPA2") >= 0)
			infra.value = 6;
		else if (g_auth.indexOf("WPA") >= 0)
			infra.value = 3;
		else
			infra.value = 0;
	}
	else
	{
		var adhoc = form.security_adhoc_mode;
		if ( g_auth.indexOf("WPA-NONE") >= 0 || g_auth.indexOf("WPA2-NONE") >= 0)
			adhoc.value = 5;
		else
			adhoc.value = 0;
	}

	//encry
	if (g_encry.indexOf("TKIP") >= 0)
		form.cipher[0].checked = true;
	else if (g_encry.indexOf("AES") >= 0)
		form.cipher[1].checked = true;

	form.network_type.value = bssid[9];
	form.channel.value = bssid[5];
}
</script>
</head>

<body onload="PageInit();">
<table class="body"><tr><td>

<h1 id="scanTitle">Station Site Survey</h1>
<p id="scanIntroduction">Site survey page shows information of APs nearby. You may choose one of these APs connecting or adding it to profile.</p>
<hr>

<form method="post" name="sta_site_survey">
<div id="ajxCtxStaTable"></div>
<div id="ajxCtxStaGraph"></div>

<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
<tr>
	<td>
		<input type=text name="connectedssid" size=28 value="<% getStaConnectionSSID(); %>" disabled>
	</td>
</tr>
<tr>
	<td>
		<input type="button" style="{width:100px;}" name="connectionButton" value="Connect" id="scanConnect" disabled onClick="open_connection_page()">&nbsp;
		<input type="button" style="{width:100px;}" value="Rescan" id="scanRescan" onClick="location.href=location.href;">&nbsp;
		<input type="button" style="{width:100px;}" name="addProfileButton" value="Add Profile" id="scanAddProfile" disabled onClick="open_profile_page()">
	</td>
</tr>
</table>

</form>

</td></tr></table>
</body>
</html>

