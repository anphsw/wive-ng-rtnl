<html>
<head>
<title>Port Forwarding Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

var MAX_RULES = 32;
var rules_num = <% getPortForwardRuleNumsASP(); %> ;

function deleteClick()
{
	return true;
}

function checkRange(str, num, min, max)
{
	d = atoi(str,num);
	if(d > max || d < min)
		return false;
	return true;
}

function checkIpAddr(field)
{
	if(field.value == "")
	{
		alert("Error. IP address is empty.");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if ( isAllNum(field.value) == 0)
	{
		alert('It should be a [0-9] number.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if( (!checkRange(field.value,1,0,255)) ||
		(!checkRange(field.value,2,0,255)) ||
		(!checkRange(field.value,3,0,255)) ||
		(!checkRange(field.value,4,1,254)) )
	{
		alert('IP format error.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function formCheck()
{
	if(rules_num >= (MAX_RULES-1) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}

	if(!document.portForward.portForwardEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(	document.portForward.ip_address.value == "" &&
		document.portForward.fromPort.value == "" &&
		document.portForward.toPort.value   == "" &&
		document.portForward.comment.value  == "")
		return true;

	// exam IP address
	if (document.portForward.ip_address.value == "")
	{
		alert("Not set a ip address.");
		document.portForward.ip_address.focus();
		return false;
	}

	if(! checkIpAddr(document.portForward.ip_address) )
	{
		alert("IP address format error.");
		document.portForward.ip_address.focus();
		return false;
	}

	if(isAllNum(document.portForward.ip_address.value) == 0)
	{
		alert("Invalid ip address.");
		document.portForward.ip_address.focus();
		return false;
	}

	// exam Port
	if(document.portForward.fromPort.value == ""){
		alert("Not set a port range.");
		document.portForward.fromPort.focus();
		return false;
	}

	if(isAllNum( document.portForward.fromPort.value ) == 0){
		alert("Invalid port number.");
		document.portForward.fromPort.focus();
		return false;
	}

	d1 = atoi(document.portForward.fromPort.value, 1);
	if(d1 > 65535 || d1 < 1){
		alert("Invalid port number!");
		document.portForward.fromPort.focus();
		return false;
	}
	
	if(document.portForward.toPort.value != ""){
		if(isAllNum( document.portForward.toPort.value ) == 0){
			alert("Invalid port number.");
			document.portForward.toPort.focus();
			return false;
		}
		d2 = atoi(document.portForward.toPort.value, 1);
		if(d2 > 65535 || d2 < 1){
			alert("Invalid port number.");
			document.portForward.toPort.focus();
			return false;
		}
		if(d1 > d2){
			alert("Invalid port range setting.");
			document.portForward.fromPort.focus();
			return false;
		}
	}
	return true;
}


function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	_TR("forwardTitle", "forward title");
	_TR("forwardIntroduction", "forward introduction");
	
	_TR("forwardVirtualSrv", "forward virtual server");
	_TR("forwardVirtualSrvSet", "forward virtual server setting");
	_TR("forwardVirtualSrvDisable", "firewall disable");
	_TR("forwardVirtualSrvEnable", "firewall enable");
	_TR("forwardVirtualSrvIPAddr", "forward virtual server ipaddr");
	_TR("forwardVirtualSrvPortRange", "forward virtual server port range");
	_TR("forwardVirtualSrvProtocol", "firewall protocol");
	_TR("forwardVirtualSrvComment", "firewall comment");

	_TRV("forwardVirtualSrvApply", "firewall apply");
	_TRV("forwardVirtualSrvReset", "firewall reset");

	_TR("forwardCurrentVirtualSrv", "forward current virtual server");
	_TR("forwardCurrentVirtualSrvNo", "firewall no");
	_TR("forwardCurrentVirtualSrvIP", "forward virtual server ipaddr");
	_TR("forwardCurrentVirtualSrvPort", "forward virtual server port range");
	_TR("forwardCurrentVirtualSrvProtocol", "firewall protocol");
	_TR("forwardCurrentVirtualSrvComment", "firewall comment");

	_TRV("forwardCurrentVirtualSrvDel", "firewall del select");
	_TRV("forwardCurrentVirtualSrvReset", "firewall reset");
}

function updateState()
{
	initTranslation();
	if(! rules_num )
	{
		disableTextField(document.portForwardDelete.deleteSelPortForward);
		disableTextField(document.portForwardDelete.reset);
	}
	else
	{
		enableTextField(document.portForwardDelete.deleteSelPortForward);
		enableTextField(document.portForwardDelete.reset);
	}

	if(document.portForward.portForwardEnabled.options.selectedIndex == 1)
	{
		enableTextField(document.portForward.ip_address);
		enableTextField(document.portForward.fromPort);
		enableTextField(document.portForward.toPort);
		enableTextField(document.portForward.protocol);
		enableTextField(document.portForward.comment);
	}
	else
	{
		disableTextField(document.portForward.ip_address);
		disableTextField(document.portForward.fromPort);
		disableTextField(document.portForward.toPort);
		disableTextField(document.portForward.protocol);
		disableTextField(document.portForward.comment);
	}
}

</script>
</head>


<!--     body      -->
<body onload="updateState();">
<table class="body"><tr><td>
<h1 id="forwardTitle">Port Forwarding Settings </h1>
<% checkIfUnderBridgeModeASP(); %>
	<p id="forwardIntroduction"> You may setup port forwarding to provide services on Internet.</p>
<hr>

<form method="POST" name="portForward" action="/goform/portForward">
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
	<td class="title" colspan="2" id="forwardVirtualSrv">Port Forwarding Settings</td>
</tr>
<tr>
	<td class="head" id="forwardVirtualSrvSet">
		Port Forwarding Settings
	</td>
	<td>
		<select onChange="updateState();" name="portForwardEnabled" size="1">
			<option value="0" <% getPortForwardEnableASP(0); %> id="forwardVirtualSrvDisable">Disable</option>
			<option value="1" <% getPortForwardEnableASP(1); %> id="forwardVirtualSrvEnable">Enable</option>
		</select>
	</td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvIPAddr">IP Address</td>
	<td><input type="text" size="16" name="ip_address"></td>
</tr>

<tr>
	<td class="head">Network Interface</td>
	<td>
		<select name="fwdIface">
			<option value="LAN" selected="selected">LAN</option>
			<option value="WAN">WAN</option>
			<option value="VPN">VPN</option>
		</select>
	</td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvPortRange">Port Range</td>
	<td><input type="text" size="5" name="fromPort">&nbsp;-&nbsp;<input type="text" size="5" name="toPort"></td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvProtocol">Protocol</td>
	<td>
		<select name="protocol">
			<option value="TCP&amp;UDP" selected="selected">TCP&amp;UDP</option>
			<option value="TCP">TCP</option>
			<option value="UDP">UDP</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="forwardVirtualSrvComment">Comment</td>
	<td><input type="text" name="comment" size="16" maxlength="32"></td>
</tr>

</table>
<script>
	document.write("(The maximum rule count is "+ MAX_RULES +".)");
</script>
<p>
	<input type="submit" value="Apply" id="forwardVirtualSrvApply" name="addFilterPort" onClick="return formCheck();"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="forwardVirtualSrvReset" name="reset">
</p>
</form>

<br>

<hr>

<!--  delete rules -->
<form action="/goform/portForwardDelete" method="POST" name="portForwardDelete">

<table width="400" border="1" cellpadding="2" cellspacing="1">
	<tr>
		<td class="title" colspan="6" id="forwardCurrentVirtualSrv">Current port forwarding setup: </td>
	</tr>
	<tr>
		<td id="forwardCurrentVirtualSrvNo">No.</td>
		<td align="center" id="forwardCurrentVirtualSrvIP">IP Address</td>
		<td align="center" id="forwardCurrentVirtualSrvIP">Interface</td>
		<td align="center" id="forwardCurrentVirtualSrvPort">Port Range</td>
		<td align="center" id="forwardCurrentVirtualSrvProtocol">Protocol</td>
		<td align="center" id="forwardCurrentVirtualSrvComment">Comment</td>
	</tr>

	<% showPortForwardRulesASP(); %>
</table>
<br>

	<input type="submit" value="Delete Selected" id="forwardCurrentVirtualSrvDel" name="deleteSelPortForward" onClick="return deleteClick()">&nbsp;&nbsp;
	<input type="reset" value="Reset" id="forwardCurrentVirtualSrvReset" name="reset">
</form>

</td></tr></table>
</body>
</html>
