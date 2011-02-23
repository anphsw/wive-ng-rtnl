<html><head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-cache"><meta http-equiv="PRAGMA" content="NO-CACHE">
<title>PPTP tunnel setup</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>

<script language="javascript">
var pptpResetRoutingTable =
[
	<% vpnInitRoutingTable(); %>
];

var pptpType     = '<% getCfgGeneral(1, "vpnType"); %>';
var pptpServerIP = (pptpType != '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';
var pptpACName   = (pptpType == '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';
var pptpRoutingTable = [];
var currentRoute = undefined;

function rememberRoutingTable(form)
{
	var table = "";
	for (var i=0; i<pptpRoutingTable.length; i++)
		table += pptpRoutingTable[i].join(" ") + "\n";
	
	form.vpn_routing_table.value = table;
}

function resetRoutingTable(form)
{
	pptpRoutingTable = [];
	for (var i=0; i< pptpResetRoutingTable.length; i++)
		pptpRoutingTable[i] = pptpResetRoutingTable[i];
}

function showHint(key)
{
	var row = document.getElementById("vpn_hint_row");
	var form = document.formVPNSetup;
	var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
	var show = true;
	
	if (key=='vpn_enabled')
		text += 'Enable Virtual Private Network support.';
	else if (key=='vpn_vpn_status')
		text += 'This indicator shows current state of VPN connection.';
	else if (form.vpn_enabled.checked)
	{
		if (key=='vpn_user')
			text += 'Specify user name given by your network provider.';
		else if (key=='vpn_password')
			text += 'Specify password given by your network provider.';
		else if (key=='vpn_auth_type')
			text += 'Select VPN authentication protocol type.';
		else if (key=='vpn_pppoe_iface')
			text += 'Select available interface for PPPoE.';
		else if (key=='vpn_server')
		{
			if (form.vpn_type.value == '0') // PPPoE
				text += 'Specify Access Concentrator name for PPPoE connection. If no Access Concentrator name is set system will try to connect to first available Access Concentrator.';
			else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
				text += 'Specify host address (IP address or domain name) of VPN server.';
			else if (form.vpn_type.value == '3') // L2TP server
				text += 'Use the following IP as VPN server own IP address in your VPN network.';
			else if ((form.vpn_type.value == '4') || (form.vpn_type.value == '5')) // GSM/CDMA
				text += 'Specify Access Point Name given by network provider. If no Access Point Name is specified it will be taken from modem default settings.';
		}
		else if (key=='vpn_range')
			text += 'Specify range of IP addresses given to clients by VPN server in <b>L2TP server</b> mode.';
		else if (key=='vpn_mtu')
			text += 'Specify Maximum Transfer Unit/Maximum Recieve Unit size in octets.';
		else if (key=='vpn_mppe')
			text += 'Enable automatic Microsoft Point-to-Point Encryption (MPPE) mode for VPN.';
		else if (key=='vpn_dgw')
		{
			text += 'Manage default gateway replacing in routing table.</p><p class="val">';
			if (form.vpn_dgw.value == '0')
				text += '<b>Disabled</b> means that no default gateway will be written to routing table.';
			else if (form.vpn_dgw.value == '1')
				text += '<b>Enabled</b> means that default gateway will be replaced by gateway given by network provider.';
			else if (form.vpn_dgw.value == '2')
				text += '<b>Multiple</b> means that default gateway will be added to existing gateway but with metric 10.';
			text += '</p>';
		}
		else if (key=='vpn_peerdns')
			text += 'Allow to get DNS adress from VPN server and write to /etv/ppp/resolv.conf.';
		else if (key=='vpn_debug')
			text += 'Allow debug mode for VPN connections.';
		else if (key=='vpn_nat')
			text += 'Add Network Address Translation to new VPN connection.';
		else if (key=='vpn_lcp')
			text += 'Enable adaptive LCP echo interval.';
		else if (key=='vpn_type')
		{
			text += 'Specify VPN access mode.<p class="val">';
			
			if (form.vpn_type.value == "0")
				text += '<b>PPPoE</b> (see RFC #2516) means encapsulating Point-to-Point Protocol (PPP) frames ' +
					'inside Ethernet frames. It is used mainly with DSL services where individual ' +
					'users connect to the DSL modem over Ethernet and in plain Metro Ethernet networks.';
			else if (form.vpn_type.value == "1")
				text += '<b>PPTP</b> (see RFC #2637) means a method for implementing virtual private networks. ' +
					'PPTP uses a control channel over TCP and a GRE tunnel operating to encapsulate ' +
					'PPP packets.';
			else if ((form.vpn_type.value == "2") || (form.vpn_type.value == "3"))
			{
				text += '<b>L2TP</b> (see RFC #2661) means a tunneling protocol used to support virtual private networks. ' +
					'It does not provide any encryption or confidentiality by itself; it relies on an ' +
					'encryption protocol that it passes within the tunnel to provide privacy.</p><p class="val">';
				if (form.vpn_type.value == "2")
					text += '<b>L2TP client</b> means a connection to remote L2TP server.';
				else
				text += '<b>L2TP server</b> means a connection from remote machines to L2TP server on this router.';
			}
			else if (form.vpn_type.value == "4")
				text += '<b>Modem GPRS</b> means connection to Internet via GPRS/EDGE in GSM mobile networks.';
			else if (form.vpn_type.value == "5")
				text += '<b>Modem CDMA</b> means connection to Internet via UMTS in CDMA mobile networks.';
			else if (form.vpn_type.value == "6")
				text += '<b>Kabinet Authorization</b> means connection to KABINET provider network.';
			text += '</p>';
		}
		else if (key=='vpn_routing')
			text += 'Enable this option to add additional routes when VPN connection is established.';
		else if (key=='vpn_lanauth_access')
			text += 'Specify access mode to KABINET provider network.';
		else if (form.vpn_routing_enabled.checked)
		{
			if (key=='vpn_route_attrs')
				text += "Route attributes: \n" +
					'<p class="val"><b>Network</b> means network address to add to routing table</p>' +
					'<p class="val"><b>Netmask</b> means network mask to add to routing table</p>' +
					'<p class="val"><b>Metric</b> means a metric (priority) assigned to route</p>' +
					'<p class="val"><b>Interface</b> describes interface to pass packets for specified network</p>' +
					'<p class="val"><b>Gateway</b> can declare IP address of gateway to pass packets</p>';
			else if (key=='vpn_route_net')
				text += 'Specify network address.';
			else if (key=='vpn_route_mask')
				text += 'Specify network mask.';
			else if (key=='vpn_route_via')
				text += 'Specify gateway IP address to use (optional feature).';
			else if (key=='vpn_route_metric')
				text += 'Specify metric for route.';
			else if (key=='vpn_route_iface')
				text += 'Specify interface to associate with network.';
			else if (key=='vpn_add_route')
				text += 'Add new route to routing table.';
			else if (key=='vpn_del_route')
				text += "Remove route from list: \n<p class=\"val\">\"" + getRouteName(currentRoute) + '"</p>';
			else if (key=='vpn_show_route')
				text += "Add route on success VPN connection: \n<p class=\"val\">\"" + getRouteName(currentRoute) + '"</p>';
			else
				show = false;
		}
		else
			show = false;
	}
	else
		show = false;
	
	if (show)
	{
		text += '</div>';
		row.innerHTML = text;
	}
}

function showRoutingHint(key, route)
{
	currentRoute = route;
	showHint(key);
}

function hideHint(ctl)
{
	var row = document.getElementById("vpn_hint_row");
	row.innerHTML = '';
}

function vpnSwitchClick(form)
{
	var dis = !form.vpn_enabled.checked;

	form.vpn_server.disabled       = dis;
	form.vpn_range.disabled        = dis;
	form.vpn_auth_type.disabled    = dis;
	form.vpn_user.disabled         = dis;
	form.vpn_pass.disabled         = dis;
	form.vpn_mtu.disabled          = dis;
	form.vpn_mppe.disabled         = dis;
	form.vpn_peerdns.disabled      = dis;
	form.vpn_debug.disabled        = dis;
	form.vpn_nat.disabled          = dis;
	form.vpn_dgw.disabled          = dis;
	form.vpn_mtu_type.disabled     = dis;
	form.vpn_pppoe_iface.disabled  = dis;
	form.vpn_type.disabled         = dis;
	form.vpn_lcp.disabled          = dis;
	form.vpn_routing_enabled.disabled = dis;
	form.lanauth_access.disabled   = dis;
	
	routingSwitchClick(form);
}

function addRouteClick(form)
{
	var via = (form.vpn_route_via.value.match(/^(\s|\*)*$/)) ? '*' : form.vpn_route_via.value;
	var row = [ form.vpn_route_net.value, form.vpn_route_mask.value, form.vpn_route_metric.value,
			form.vpn_route_iface.value, via ];
	
	if (!validateIP(form.vpn_route_net, 1))
		return;
	if (!validateIPMask(form.vpn_route_mask, 1))
		return;
	if ((via!='*') && (!validateIP(form.vpn_route_via, 1)))
		return;
	
	if (!checkDigitRange(form.vpn_route_metric.value, 0, 1500))
	{
		alert("Metric must be between 0 and 1500");
		return;
	}

	pptpRoutingTable[pptpRoutingTable.length] = row;
	
	form.vpn_route_net.value = '';
	form.vpn_route_mask.value = '';
	form.vpn_route_iface.value = '';
	form.vpn_route_metric.value = '0';
	form.vpn_route_via.value = '';

	routingSwitchClick(form);
}

function getRouteName(route)
{
	var row = pptpRoutingTable[route];
	var name = 'network ' + row[0] + ', netmask ' + row[1] + ', metric ' + row[2];
	if (row[3] != '*')
		name += ' via interface ' + row[3];
	if (row[4] != '*')
		name += ' via gateway ' + row[4];
	return name;
}

function delRouteClick(form, route)
{
	if (form.vpn_enabled.checked)
	{
		if (confirm("Do you want to delete: \n \"" + getRouteName(route) + "\"?"))
		{
			pptpRoutingTable.splice(route, 1);
			routingSwitchClick(form);
		}
	}
}

function genRoutingTable(form)
{
	var vpn_routing_table = document.getElementById("vpn_routing_table");

	var table = '<table class="small"><tr onmouseover="showHint(\'vpn_route_attrs\');" onmouseout="hideHint(\'vpn_route_attrs\');" >' +
		'<th>Destination IP</th><th>Mask</th><th style="text-align: center;">Metric</th>' + 
		'<th style="text-align: center;">Interface</th><th>Gateway</th><th style="text-align: center;">Actions</th>' +
		'</tr>';
	
	var disabled = (form.vpn_enabled.checked) ? '' : 'disabled="disabled"';
	
	for (var i=0; i < pptpRoutingTable.length; i++)
	{
		var row = pptpRoutingTable[i];
		table += '<tr>';
		for (var j=0; j<row.length; j++)
		{
			var align = ((j==2) || (j==3) || (j==5)) ? 'align="center"' : '';
			table += '<td ' + align + ' onmouseover="showRoutingHint(\'vpn_show_route\',' + i + ');" '
			table += 'onmouseout="hideHint(\'vpn_show_route\');" >' + row[j] + '</td>';
		}
		table += '<td align="center"><a onclick="delRouteClick(' + form.name +',' + i + ');" style="cursor: pointer; color: red;" ';
		table += 'onmouseover="showRoutingHint(\'vpn_del_route\',' + i + ');" onmouseout="hideHint(\'vpn_del_route\');">[X]</a></td></tr>';
	}
	
	// Add new route row
	table += '<tr>';
	table += '<td align="center"><input name="vpn_route_net" style="width: 110px;" onmouseover="showHint(\'vpn_route_net\');" onmouseout="hideHint(\'vpn_route_net\');" ' + disabled +'></td>';
	table += '<td align="center"><input name="vpn_route_mask" style="width: 110px;" onmouseover="showHint(\'vpn_route_mask\');" onmouseout="hideHint(\'vpn_route_mask\');" ' + disabled +'></td>';
	table += '<td align="center"><input name="vpn_route_metric" style="width: 50px;" value="0" onmouseover="showHint(\'vpn_route_metric\');" onmouseout="hideHint(\'vpn_route_metric\');" ' + disabled +'></td>';
	table += '<td align="center"><select name="vpn_route_iface" class="half" onmouseover="showHint(\'vpn_route_iface\');" onmouseout="hideHint(\'vpn_route_iface\');" ' + disabled + '>'+
		'<% vpnRouteIfaceList(); %>';
	table += '<td align="center"><input name="vpn_route_via" style="width: 110px;" onmouseover="showHint(\'vpn_route_via\');" onmouseout="hideHint(\'vpn_route_via\');" ' + disabled +'></td>';
	table += '<td align="center"><input type="button" value="Add" onclick="addRouteClick(this.form);" onmouseover="showHint(\'vpn_add_route\');" onmouseout="hideHint(\'vpn_add_route\');" ' + disabled +'></td>';
	table += '<tr>';

	table += '</table>';
	vpn_routing_table.innerHTML = table;
}

function routingSwitchClick(form)
{
	var vpn_routing_row = document.getElementById("vpn_routing_row");
	if (form.vpn_routing_enabled.checked)
	{
		genRoutingTable(form);
		vpn_routing_row.style.display='';
		return;
	}
	
	vpn_routing_row.style.display='none';
}

function mtuChange(form)
{
	var vpn_mtu_select = document.getElementById("vpn_mtu_select");
	var vpn_mtu_field  = document.getElementById("vpn_mtu_field");
	
	if (form.vpn_mtu_type.value == '1')
	{
		vpn_mtu_field.style.display = '';
		vpn_mtu_select.setAttribute("class", "half");
		vpn_mtu_field.setAttribute("class", "half");
	}
	else
	{
		vpn_mtu_select.setAttribute("class", "mid");
		vpn_mtu_field.style.display = 'none';
		form.vpn_mtu.value = form.vpn_mtu_type.value;
	}
}

function bodyOnLoad(form)
{
	initializeForm(form);
	resetRoutingTable(form);
	
	/* Check if option was set */
	var vpn_mtu_select = document.getElementById('vpn_mtu_select');
	for (var i=0; i < vpn_mtu_select.options.length; i++)
		if (form.vpn_mtu_type.options[i].value == form.vpn_mtu.value)
		{
			form.vpn_mtu_type.value = form.vpn_mtu_select.options[i].value;
			break;
		}

	vpnSwitchClick(form);
	selectType(form);
	mtuChange(form);
	
	showVPNStatus();
}

function selectType(form)
{
	var vpn_server_col = document.getElementById("vpn_server_col");

	var pppoe_on = form.vpn_type.value == '0';
	var l2tp_server_on = form.vpn_type.value == '3';
	var kabinet_on = form.vpn_type.value == '6';

	// Display mode-dependent elements
	displayElement('vpn_pppoe_iface_row', !kabinet_on);
	displayElement('vpn_server_row', !kabinet_on);
	displayElement('vpn_auth_type_row', !kabinet_on);
	displayElement('vpn_user_row', !kabinet_on);
	displayElement('vpn_mtu_row', !kabinet_on);
	displayElement('vpn_dgw_row', !kabinet_on);
	displayElement('vpn_lanauth_lvl_row', kabinet_on);
	displayElement('table_vpn_params', !kabinet_on);

	displayElement('vpn_pppoe_row', pppoe_on);
	displayElement('vpn_mppe_row', !l2tp_server_on);
	displayElement('vpn_l2tp_range', l2tp_server_on);

	var vpn_server = 'Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name';
	if (form.vpn_type.value == '0') // PPPoE
		vpn_server = '<acronym title="Access Concentrator">AC</acronym> name';
	else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
		vpn_server = 'Host, <acronym title="Internet Protocol">IP</acronym> or <acronym title="Domain Name System">DNS</acronym> name';
	else if (form.vpn_type.value == '3') // L2TP server
		vpn_server = 'VPN Local <acronym title="Internet Protocol">IP</acronym>';
	else if ((form.vpn_type.value == '4') || (form.vpn_type.value == '5'))
		vpn_server = '<acronym title="Access Point Name">APN</acronym> name'; // GSM/CDMA
	
	if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2') || (form.vpn_type.value == '3'))
		form.vpn_server.value = pptpServerIP;
	else
		form.vpn_server.value = pptpACName;
	
	vpn_server_col.innerHTML = '<b>' + vpn_server + ':</b>';
}

function resetClick(form)
{
	form.reset();
	bodyOnLoad(form);
	return true;
}

function submitClick(form)
{
	if (form.vpn_pass.value.match(/[\s\$]/))
	{
		alert("Password can not contain spaces or dollar ('$') sign!");
		form.vpn_pass.focus();
		return false;
	}
	
	if ((form.vpn_type.value != "0") && (form.vpn_type.value != "6"))
	{
		if (form.vpn_user.value.match(/[\s\$]/))
		{
			alert("User name can not contain spaces or dollar ('$') sign!");
			form.vpn_user.focus();
			return false;
		}
		
		if ((!validateIP(form.vpn_server, false)) && (!validateDNS(form.vpn_server, false)))
		{
			alert("Invalid IP address or domain name!");
			form.vpn_type.focus();
			return false;
		}
	}

	rememberRoutingTable(form); // Remember routing table

	return true;
}

function initializeForm(form)
{
	var vpnEnabled = '<% getCfgGeneral(1, "vpnEnabled"); %>';
	var pptpType   = '<% getCfgGeneral(1, "vpnType"); %>';
	var routingOn  = '<% getCfgGeneral(1, "vpnRoutingEnabled"); %>';
	var mppe       = '<% getCfgGeneral(1, "vpnMPPE"); %>';
	var peerdns    = '<% getCfgGeneral(1, "vpnPeerDNS"); %>';
	var debug      = '<% getCfgGeneral(1, "vpnDebug"); %>';
	var nat        = '<% getCfgGeneral(1, "vpnNAT"); %>';
	var dgw        = '<% getCfgGeneral(1, "vpnDGW"); %>';
	var vpn_auth   = '<% getCfgGeneral(1, "vpnAuthProtocol"); %>';
	var lcp        = '<% getCfgGeneral(1, "vpnEnableLCP"); %>';

	var kabinet_built = '<% getLANAUTHBuilt(); %>';

	if (kabinet_built != '0')
	{
		var lanauth_access   = '<% getCfgGeneral(1, "LANAUTH_LVL"); %>';
		
		// Add specific option
		form.vpn_type.options[form.vpn_type.options.length] = new Option('KABINET Authorization', '6');
		form.lanauth_access.value      = lanauth_access;
	}

	form.vpn_enabled.checked = (vpnEnabled == 'on');
	form.vpn_routing_enabled.checked = (routingOn == 'on');
	form.vpn_mppe.checked    = (mppe == 'on');
	form.vpn_peerdns.checked = (peerdns == 'on');
	form.vpn_debug.checked   = (debug == 'on');
	form.vpn_nat.checked     = (nat == 'on');
	form.vpn_type.value      = pptpType;
	form.vpn_dgw.value       = dgw;
	form.vpn_lcp.checked     = (lcp == 'on');
	form.vpn_auth_type.value = vpn_auth;
	
	selectType(form);
}

function showVPNStatus()
{
	var reset_timer = function() { setTimeout('showVPNStatus();', 5000); };

	ajaxLoadElement(
		'vpn_status_col',
		'/internet/vpn_status.asp',
		reset_timer
	);
}

</script>
</head>

<body onload="bodyOnLoad(document.formVPNSetup)">
<table class="body">
<tr><td>
<h1>Virtual Private Network setup</h1>
<p>
This page is used to configure the <acronym title="Virtual Private Network">VPN</acronym>
tunnel on your Router.
</p>

<form action="/goform/formVPNSetup" method="POST" name="formVPNSetup">
<table width="500" border="0" cellpadding="0" cellspacing="4">
	<tr>
		<td colspan="2"><hr></td>
	</tr>
	<tr>
		<td onMouseOver="showHint('vpn_enabled')" onMouseOut="hideHint('vpn_enabled')" >
			<input name="vpn_enabled" onclick="vpnSwitchClick(this.form)" type="checkbox">
			<b>Enable <acronym title="Virtual Private Network">VPN</acronym></b>
		</td>
		<td onMouseOver="showHint('vpn_vpn_status')" onMouseOut="hideHint('vpn_vpn_status')" id="vpn_status_col">
		</td>
	</tr>
	<tr onMouseOver="showHint('vpn_type')" onMouseOut="hideHint('vpn_type')" >
		<td width="50%">
			<b><acronym title="Virtual Private Network">VPN</acronym> Mode:</b>
		</td>
		<td width="50%">
			<select disabled="disabled" name="vpn_type" onChange="selectType(this.form);" class="mid" >
				<option value="0" selected="selected">PPPoE client</option>
				<option value="1">PPTP  client</option>
				<option value="2">L2TP  client</option>
				<!-- No L2TP support now
				<option value="3">L2TP  server</option> -->
			</select>
		</td>
	</tr>
	<tr id="vpn_pppoe_row" style="display: none;" onMouseOver="showHint('vpn_pppoe_iface')" onMouseOut="hideHint('vpn_pppoe_iface')">
		<td width="50%"><b>PPPoE interface:</b></td>
		<td width="50%">
			<select disabled="disabled" name="vpn_pppoe_iface" class="mid" >
				<% vpnIfaceList(); %>
			</select>
		</td>
	</tr>
	<tr id="vpn_server_row" onMouseOver="showHint('vpn_server')" onMouseOut="hideHint('vpn_server')">
		<td width="50%" id="vpn_server_col">
			<b>Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name:</b>
		</td>
		<td width="50%"><input name="vpn_server" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnServer"); %>" disabled="disabled" type="text"></td>
	</tr>
	<tr id="vpn_l2tp_range" onMouseOver="showHint('vpn_range')" onMouseOut="hideHint('vpn_range')" style="display: none;" >
		<td width="50%"><b><acronym title="Virtual Private Network">VPN</acronym> range <acronym title="Internet Protocol">IP</acronym> adresses:</b></td>
		<td width="50%"><input name="vpn_range" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnRange"); %>" disabled="disabled" type="text"></td>
	</tr>
	<tr id="vpn_auth_type_row" onMouseOver="showHint('vpn_auth_type')" onMouseOut="hideHint('vpn_auth_type')" >
		<td width="50%"><b>Authentication method:</b></td>
		<td width="50%">
			<select id="vpn_auth_type_select" disabled="disabled" name="vpn_auth_type" class="mid">
				<option value="0" selected="selected">AUTO</option>
				<option value="1">PAP</option>
				<option value="2">CHAP</option>
				<option value="3">MSCHAP</option>
			</select>
		</td>
	</tr>
	<tr id="vpn_lanauth_lvl_row" onMouseOver="showHint('vpn_lanauth_access')" onMouseOut="hideHint('vpn_lanauth_access')">
		<td width="50%"><b>KABINET access level:</b></td>
		<td width="50%">
			<select name="lanauth_access" class="mid" disabled="disabled">
				<option value="0">offline</option>
				<option value="1">kabinet</option>
				<option value="2">full</option>
			</select>
		</td>
	</tr>
	<tr id="vpn_user_row" onMouseOver="showHint('vpn_user')" onMouseOut="hideHint('vpn_user')" >
		<td width="50%"><b>User name:</b></td>
		<td width="50%"><input name="vpn_user" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnUser"); %>" disabled="disabled" type="text"></td>
	</tr>
	<tr onMouseOver="showHint('vpn_password')" onMouseOut="hideHint('vpn_password')" >
		<td width="50%"><b>Password:</b></td>
		<td width="50%"><input name="vpn_pass" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnPassword"); %>" disabled="disabled" type="password"></td>
	</tr>
	<tr id="vpn_mtu_row" onMouseOver="showHint('vpn_mtu')" onMouseOut="hideHint('vpn_mtu')" >
		<td width="50%"><b><acronym title="Maximum Transfer Unit">MTU</acronym>/<acronym title="Maximum Recieve Unit">MRU:</acronym></b></td>
		<td width="50%">
			<input id="vpn_mtu_field" name="vpn_mtu" maxlength="4" disabled="disabled" type="text" class="half" style="display:none; " value="<% getCfgGeneral(1, "vpnMTU"); %>" >
			<select id="vpn_mtu_select" disabled="disabled" name="vpn_mtu_type" onChange="mtuChange(this.form);" class="mid" >
				<option value="AUTO">AUTO</option>
				<option value="1" selected="selected">Custom</option>
				<option value="1500">1500</option>
				<option value="1492">1492</option>
				<option value="1440">1440</option>
				<option value="1400">1400</option>
				<option value="1300">1300</option>
				<option value="1200">1200</option>
				<option value="1100">1100</option>
				<option value="1000">1000</option>
			</select>
		</td>
	</tr>
	<tr id="vpn_dgw_row" onMouseOver="showHint('vpn_dgw')" onMouseOut="hideHint('vpn_dgw')" >
		<td width="50%">
			<b>Default gateway:</b>
		</td>
		<td width="50%">
			<select disabled="disabled" name="vpn_dgw" class="mid" class="mid" >
				<option value="0" selected="selected">Disabled</option>
				<option value="1">Enabled</option>
			</select>
		</td>
	</tr>

</table>

<table id="table_vpn_routing" width="500" border="0" cellpadding="0" cellspacing="4" style="display: none" >
	<tr onmouseover="showHint('vpn_routing')" onmouseout="hideHint('vpn_routing')">
		<td colspan="2">
			<input name="vpn_routing_enabled" onclick="routingSwitchClick(this.form);" type="checkbox">
			<b>Enable routing</b>
		</td>
	</tr>
	<tr id="vpn_routing_row" style="display: none;">
		<td colspan="2" id="vpn_routing_table">
		</td>
	</tr>
</table>

<table id="table_vpn_params" width="500" border="0" cellpadding="0" cellspacing="4" style="display: none">
	<tr id="vpn_mppe_row">
		<td width="50%" onMouseOver="showHint('vpn_mppe')" onMouseOut="hideHint('vpn_mppe')" >
			<input disabled="disabled" name="vpn_mppe" type="checkbox">
			<b>Allow <acronym title="Microsoft Point-to-Point Encryption">MPPE</acronym></b>
		</td>
		<td width="50%" onMouseOver="showHint('vpn_peerdns')" onMouseOut="hideHint('vpn_peerdns')" >
			<input disabled="disabled" name="vpn_peerdns" type="checkbox">
			<b>Peer <acronym title="Domain Name Server">DNS</acronym></b>
		</td>
	</tr>
	<tr>
		<td width="50%" onMouseOver="showHint('vpn_debug')" onMouseOut="hideHint('vpn_debug')" >
			<input disabled="disabled" name="vpn_debug" type="checkbox">
			<b>Allow debug</b>
		</td>
		<td width="50%" onMouseOver="showHint('vpn_nat')" onMouseOut="hideHint('vpn_nat')" >
			<input disabled="disabled" name="vpn_nat" type="checkbox">
			<b>Enable <acronym title="Network Address Translation">NAT</acronym></b>
		</td>
	</tr>
	<tr>
		<td width="50%" onMouseOver="showHint('vpn_lcp')" onMouseOut="hideHint('vpn_lcp')" >
			<input disabled="disabled" name="vpn_lcp" type="checkbox">
			<b>Adaptive LCP</b>
		</td>
		<td></td>
	</tr>
</table>

<table width="500" border="0" cellpadding="0" cellspacing="4">
	<tr height="32px"><td colspan="2"></td></tr>
	<tr>
		<td colspan="2">
			<input name="vpn_routing_table" type="hidden">
			<input name="lanauth_pass_changed" type="hidden">
			<input value="/internet/vpn.asp" name="submit-url" type="hidden">
			<input style="none" value="Apply and connect" name="save" type="submit" onclick="return submitClick(this.form);" >&nbsp;&nbsp;
			<input style="none" value="Reset" name="reset_button" onclick="resetClick(this.form);" type="button">
		</td>
	</tr>
</table>
</form>

<div id="vpn_hint_row">
</div>

</td>
</tr>
</table>

</body></html>
