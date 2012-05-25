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

		mtuChange(form);
}

function initializeForm(form)
{
var wmenabled     	 = '<% getCfgGeneral(1, "MODEMENABLED"); %>';
form.modem_type.value    = '<% getCfgGeneral(1, "MODEMTYPE"); %>';
form.modem_port.value    = '<% getCfgGeneral(1, "WMODEMPORT"); %>';
form.modem_speed.value   = '<% getCfgGeneral(1, "MODEMSPEED"); %>';
form.modem_mtu.value     = '<% getCfgGeneral(1, "MODEMMTU"); %>';
form.modem_user.value    = '<% getCfgGeneral(1, "MODEMUSERNAME"); %>';
form.modem_pass.value    = '<% getCfgGeneral(1, "MODEMPASSWORD"); %>';
form.modem_dialn.value	 = '<% getCfgGeneral(1, "MODEMDIALNUMBER"); %>';
form.modem_apn.value     = '<% getCfgGeneral(1, "APN"); %>';

	if (wmenabled == "1")
	{
		document.usbmodem.modem_enabled[0].checked = true;
	}
	else
	{
		document.usbmodem.modem_enabled[1].checked = true;
	}
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
	    <td class="head">USB Modem</td>
    <td>
      <input type="radio" name="modem_enabled" value="1"><font id="Enable">Enable</font>
      <input type="radio" name="modem_enabled" value="0"><font id="Disable">Disable</font>
    </td>
	</tr>
	<tr id="modem_type_row">
  		<td class="head">Modem type:</td>
		<td>
			<select name="modem_type" class="mid" >
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
	<tr id="modem_dialn_row">
		<td class="head">Dial number:</td>
		<td><input name="modem_dialn" class="mid" size="25" maxlength="60" type="text"></td>
	</tr>
	<tr id="modem_apn_row">
		<td class="head">Access Point Name (APN):</td>
		<td><input name="modem_apn" class="mid" size="25" maxlength="60" type="text"></td>
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
