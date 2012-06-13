<html><head>
<title>3G modem setup</title>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">


<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>

<script language="javascript">

function mtuChange(form)
{
	var modem_mtu_select = document.getElementById("modem_mtu_select");
	var modem_mtu_field  = document.getElementById("modem_mtu_field");

	if (form.modem_mtu_type.value == '1')
	{
		modem_mtu_field.style.display = '';
		modem_mtu_select.setAttribute("class", "half");
		modem_mtu_field.setAttribute("class", "half");
	}
	else
	{
		modem_mtu_select.setAttribute("class", "mid");
		modem_mtu_field.style.display = 'none';
		form.modem_mtu.value = form.modem_mtu_type.value;
	}
}


function submitClick(form)
{
	if (form.modem_pass.value.match(/[\s\$]/))
	{
		alert("Password can not contain spaces or dollar ('$') sign!");
		form.modem_pass.focus();
		return false;
	}
		if (form.modem_user.value.match(/[\s\$]/))
		{
			alert("User name can not contain spaces or dollar ('$') sign!");
			form.modem_user.focus();
			return false;
		}

		return true;
}

function selectModemType(form)
{
	var gsm_on = form.modem_type.value == '0';
	var cdma_on = form.modem_type.value == '1';
	
	// Display mode-dependent elements
	displayElement( 'modem_dialn_row', gsm_on);
	
}	

function bodyOnLoad(form)
{
	initializeForm(form);

	/* Check if option was set */
	var modem_mtu_select = document.getElementById('modem_mtu_select');
	for (var i=0; i < modem_mtu_select.options.length; i++)
		if (form.modem_mtu_type.options[i].value == form.modem_mtu.value)
		{
			form.modem_mtu_type.value = form.modem_mtu_select.options[i].value;
			break;
		}
		
		modemSwitchClick(form);
		selectModemType(form);
		mtuChange(form);
		
}
  
function modemSwitchClick(form)
{
	enableElements( [
		form.modem_type, form.modem_port, form.modem_speed, form.modem_mtu, form.modem_mtu_type, form.modem_user,
        form.modem_pass, form.modem_dialn, form.modem_apn, form.at_enabled, form.mdebug_enabled, form.modem_at1, form.modem_at2,
		form.modem_at3 ], form.modem_enabled.checked );
}
function initializeForm(form)
{
var wmenabled     	     = '<% getCfgGeneral(1, "MODEMENABLED"); %>';
var watmenabled			 = '<% getCfgGeneral(1, "MODEMATENABLED"); %>';
var wmdebug				 = '<% getCfgGeneral(1, "MODEMDEBUG"); %>';
form.modem_type.value    = '<% getCfgGeneral(1, "MODEMTYPE"); %>';
form.modem_port.value    = '<% getCfgGeneral(1, "WMODEMPORT"); %>';
form.modem_speed.value   = '<% getCfgGeneral(1, "MODEMSPEED"); %>';
form.modem_mtu.value     = '<% getCfgGeneral(1, "MODEMMTU"); %>';
form.modem_user.value    = '<% getCfgGeneral(1, "MODEMUSERNAME"); %>';
form.modem_pass.value    = '<% getCfgGeneral(1, "MODEMPASSWORD"); %>';
form.modem_dialn.value	 = '<% getCfgGeneral(1, "MODEMDIALNUMBER"); %>';
form.modem_apn.value     = '<% getCfgGeneral(1, "APN"); %>';
form.modem_at1.value     = '<% getCfgGeneral(1, "MODEMAT1"); %>';
form.modem_at2.value     = '<% getCfgGeneral(1, "MODEMAT2"); %>';
form.modem_at3.value     = '<% getCfgGeneral(1, "MODEMAT3"); %>';

	form.modem_enabled.checked = (wmenabled == '1');
	form.at_enabled.checked = (watmenabled == '1');
	form.mdebug_enabled.checked = (wmdebug == '1');
	
	selectModemType(form);
}
</script>
</head>

<body onload="bodyOnLoad(document.usbmodem);">
<table class="body">
<tr><td>
<h1>USB Modem setup</h1>
<p>This page is used to configure the USB Modem settings</p>
<hr>

<form action="/goform/usbmodem" method="POST" name="usbmodem">
<table class="form">
	<tr>
		<td class="title" colspan="2">USB Modem configuration</td>
	</tr>
	<tr id="modem_enable_row">
	    <td class="head">
		<input name="modem_enabled" onclick="modemSwitchClick(this.form)" type="checkbox">&nbsp;Enable USB Modem</td>
        </tr>
	<tr id="modem_type_row">
  		<td class="head">Modem type:</td>
		<td>
			<select name="modem_type" onChange="selectModemType(this.form);"class="mid">
				<option value="0">WCDMA/UMTS/GPRS</option>
				<option value="1">CDMA/EVDO</option>
				</select>
		</td>
	</tr>
	<tr id="modem_port_row">
	        <td class="head">Modem port:</td>
		<td>
		<select name="modem_port" class="mid">
				<option value="AUTO">AUTO</option>
				<option value="ttyUSB0">ttyUSB0</option>
				<option value="ttyUSB1">ttyUSB1</option>
				<option value="ttyUSB2">ttyUSB2</option>
				<option value="ttyUSB3">ttyUSB3</option>
				<option value="ttyUSB4">ttyUSB4</option>
				<option value="ttyUSB5">ttyUSB5</option>
				<option value="ttyUSB6">ttyUSB6</option>
				<option value="ttyUSB7">ttyUSB7</option>
				<option value="ttyUSB8">ttyUSB8</option>
				<option value="ttyUSB9">ttyUSB9</option>
				<option value="ttyACM0">ttyACM0</option>
				</select>
		</td>
	</tr>
	<tr id ="modem_speed_row">
	        <td class="head">Modem port speed:</td>
	        <td><select name="modem_speed" class="mid">
				<option value="AUTO">AUTO</option>
				<option value="57600">57600</option>
				<option value="115200">115200</option>
				<option value="230400">230400</option>
				</select>
	        </td>
	</tr>
	<tr id="modem_mtu_row">
		 <td class="head">Modem MTU/MRU:</td>
		 <td>
			<input id="modem_mtu_field" name="modem_mtu" maxlength="4" type="text" class="half" style="display:none; ">
			<select id="modem_mtu_select" name="modem_mtu_type" onChange="mtuChange(this.form);" class="mid" >
				<option value="AUTO">AUTO</option>
				<option value="1" selected="selected" >Custom</option>
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
	<tr id="modem_user_row">
		<td class="head">User name:</td>
		<td><input name="modem_user" class="mid" size="25" maxlength="60" type="text"></td>
	</tr>
	<tr id="modem_pass_row">
		<td class="head">Password:</td>
		<td><input name="modem_pass" class="mid" size="25" maxlength="60" type="password"></td>
	</tr>
	<tr id="modem_dialn_row"  style="display: none;">
		<td class="head">Dial number:</td>
		<td><input name="modem_dialn" class="mid" size="25" maxlength="60" type="text"></td>
	</tr>
	<tr id="modem_apn_row">
		<td class="head">Access Point Name (APN):</td>
		<td><input name="modem_apn" class="mid" size="25" maxlength="60" type="text"></td>
	</tr>
</table>
	
<table class="form">
	<tr>
		<td colspan="2" class="title">Additional options</td>
	</tr>	
	<tr id="at_anable_row">
		<td class="head" width="50%">
		<input name="mdebug_enabled" type="checkbox">&nbsp;Allow debug</td>
		<td class="head" width="50%">
		<input name="at_enabled" type="checkbox">&nbsp;Enable AT commands</td>
	</tr>
	<tr id="modem AT1_row">
		<td class="head">Modem AT commands</td>
		<td><input name="modem_at1" size="40" maxlength="60" type="text"></td>
	</tr>
	<tr id="modem AT2_row">
		<td class="head"</td>
		<td><input name="modem_at2" size="40" maxlength="60" type="text"></td>
	</tr>
	<tr id="modem AT3_row">
		<td class="head"></td>
		<td><input name="modem_at3" size="40" maxlength="60" type="text"></td>
	</tr>
</table>

<table class="buttons">
	<tr>
		<td>
			<input value="/usb/USBmodem.asp" name="submit-url" type="hidden">
			<input class="normal" value="Apply and connect" name="Apply" type="submit" onclick="return submitClick(this.form);" >&nbsp;&nbsp;
			<input class="normal" value="Cancel" name="Cancel" onClick="window.location.reload()" type="button">
		</td>
	</tr>

</table>
</form>
</td></tr>
</body></html>
