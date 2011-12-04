<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Wireless Station Add Profile</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
	_TR("addprofSysConf", "addprof system config");
	_TR("addprofProfName", "addprof profile name");
	_TR("addprofSSID", "station ssid");
	_TR("addprofNetType", "station network type");
	_TR("addprofAdHoc", "addprof adhoc");
	_TR("addprofInfra", "addprof infrastructure");
	_TR("addprofPWSave", "addprof power save");
	_TR("addprofCAM", "addprof cam");
	_TR("addprofPWSaveMode", "addprof power save");
	_TR("addprofChannel", "station channel");
	_TR("addprofPreambleType", "addprof preamble type");
	_TR("addprofPremableAuto", "wireless auto");
	_TR("addprofPremableLong", "wireless long");
	_TR("addprofRTS", "adv rts threshold");
	_TR("addprofRTSUsed", "station used");
	_TR("addprofFrag", "adv fragment threshold");
	_TR("addprofFragUsed", "station used");
	_TR("addprofSecurePolicy", "addprof secure policy");
	_TR("addprofSecureMode", "secure security mode");
	_TR("addprofAdHocSecure", "secure security mode");
	_TR("addprofWEP", "secure wep");
	_TR("addprofWEPKeyLength", "addprof wep key length");
	_TR("addprofWEPKeyEntryMethod", "addprof wep key entry method");
	_TR("addprofHex", "addprof hex");
	_TR("addprofASCII", "addprof ascii");
	_TR("addprofWEPKey", "secure wep key");
	_TR("addprofWEPKey1", "secure wep key1");
	_TR("addprofWEPKey2", "secure wep key2");
	_TR("addprofWEPKey3", "secure wep key3");
	_TR("addprofWEPKey4", "secure wep key4");
	_TR("addprofDefaultKey", "secure wep default key");
	_TR("addprofDKey1", "secure wep default key1");
	_TR("addprofDKey2", "secure wep default key2");
	_TR("addprofDKey3", "secure wep default key3");
	_TR("addprofDKey4", "secure wep default key4");
	_TR("addprofWPA", "secure wpa");
	_TR("addprofWPAAlg", "secure wpa algorithm");
	_TR("addprofPassPhrase", "secure wpa pass phrase");
	_TR("addprof1XAuthType", "addprof 8021X Auth Type");
	_TR("addprofWPAAuthType", "addprof 8021X Auth Type");
	_TR("addprofPEAPTunnelAtuth", "addprof tunnel auth");
	_TR("addprofTTLSTunnelAuth", "addprof tunnel auth");
	_TR("addprofIdentity", "addprof identity");
	_TR("addprofPasswd", "addprof passwd");
	_TR("addprofClientCert", "addprof client cert");
	_TR("addprofClientCertUsed", "station used");
	_TR("addprofClientCertPath", "addprof client cert path");
	_TR("addprofPrivateKeyPath", "addprof private key path");
	_TR("addprofPrivateKeyPasswd", "addprof private key passwd");
	_TR("addprofCACert", "addprof ca cert");
	_TR("addprofCACertUsed", "station used");
	_TR("addprofCACertPath", "addprof ca cert path");

	_TRV("addprofApply", "wireless apply");
	_TRV("addprofCancel", "wireless cancel");
}

function initValue()
{
	var form = document.profile_page;
	
	initTranslation();
	getChannels(form);

	var profName = "<% getStaNewProfileName(); %>";
	form.profile_name.value = profName;

	if (opener.showProfileSsid)
		opener.showProfileSsid();

	networkTypeChange(form);
}

function getChannels(form)
{
	var channel = "<% getStaAdhocChannel(); %>";
	var wireless_mode = "<% getCfgZero(1, "WirelessMode"); %>";

	var bg_channel = channel & 0xFF;
	var a_channel = (channel >> 8) & 0xFF;
	var channels = [];

	switch (wireless_mode)
	{
		case 0: // B/G mixed
		case 1: // B only
			channels = getBGChannel(bg_channel);
			break;
		case 2: // A only
			channels = getAChannel(a_channel);
			break;
		case 3: // A/B/G mixed
		default:
			channels = getBGChannel(bg_channel);
			channels = channels.concat(getAChannel(a_channel));
			break;
	}
	
	// Add options
	for (var i=0; i<channels.length; i++)
		form.channel.options.add(new Option(channels[i], channels[i]));
}

function getBGChannel( channel )
{
	switch (channel)
	{
		case 0: return [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11];
		case 1: return [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ];
		case 2: return [ 10, 11 ];
		case 3: return [ 10, 11, 12, 13 ];
		case 4: return [ 14 ];
		case 5: return [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 ];
		case 6: return [ 3, 4, 5, 6, 7, 8, 9 ];
		case 7: return [ 5, 6, 7, 8, 9, 10, 11, 12, 13 ];
	}
	
	return [];
}

function getAChannel( channel )
{
	switch (channel)
	{
		case 0: return [ 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165 ];
		case 1: return [ 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 ];
		case 2: return [ 36, 40, 44, 48, 52, 56, 60, 64 ];
		case 3: return [ 52, 56, 60, 64, 149, 153, 157, 161 ];
		case 4: return [ 149, 153, 157, 161, 165 ];
		case 5: return [ 149, 153, 157, 161 ];
		case 6: return [ 36, 40, 44, 48 ];
		case 7: return [ 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165 ];
		case 8: return [ 52, 56, 60, 64 ];
		case 9: return [ 34, 38, 42, 46 ];
		case 10: return [ 34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64 ];
	}
	
	return [];
}

function networkTypeChange(form)
{
	var nmode = form.network_type.value;

	// Show/hide elements
	displayElement( [ "div_power_saving_mode", "div_security_infra_mode" ], nmode == '1');
	displayElement( [ "div_b_premable_type", "div_channel", "div_security_adhoc_mode" ], nmode != '1');

	securityMode(form);
}

function checkData(form)
{
	var securitymode;
	var profilename = form.profile_name.value;
	var ssid = form.Ssid.value;
	
	if (form.network_type.value == '1') //infra
		securitymode = form.security_infra_mode.value;
	else
		securitymode = form.security_adhoc_mode.value;
	
	if (profilename.length <=0)
	{
		alert('Pleaes input the Profile Name!');
		return false;
	}
	else if (ssid.length <= 0)
	{
		alert('Pleaes input the SSID!');
		return false;
	}
	else if ((securitymode  == '0') || (securitymode  == '1'))
		return check_Wep(form, securitymode);
	else if ((securitymode  == '4') || (securitymode == '7') || (securitymode == '5'))
	{
		var keyvalue = form.passphrase.value;

		if (keyvalue.length == 0)
		{
			alert('Please input wpapsk key!');
			return false;
		}

		if (keyvalue.length < 8)
		{
			alert('Please input at least 8 character of wpapsk key !');
			return false;
		}
	}
	//802.1x
	else if (securitymode == '3' || securitymode == '6' || securitymode == '8') //wpa enterprise, 802.1x
	{
		var certid = form.cert_id.value;
		if (certid.length == 0)
		{
			alert('Please input the 802.1x identity !');
			return false;
		}

		if (form.cert_password.disabled == false)
		{
			var certpassword = form.cert_password.value;
			if (certpassword.length == 0)
			{
				alert('Please input the 802.1x password !');
				return false;
			}
		}

		if (form.cert_use_client_cert.checked == true)
		{
			//alert("client cert check");
			var client_cert = form.cert_client_cert_path.value;
			var private_key = form.cert_private_key_path.value;
			var private_key_password = form.cert_private_key_password.value;

			if (client_cert.length == 0)
			{
				alert('Please input the 802.1x Client Certificate Path !');
				return false;
			}

			if (private_key.length == 0)
			{
				alert('Please input the 802.1x Private Key Path !');
				return false;
			}

			if (private_key_password.length == 0)
			{
				alert('Please input the 802.1x Private Key Password !');
				return false;
			}
		}

		if (form.cert_use_ca_cert.checked == true)
		{
			//alert("CA cert check");
			var ca_cert_path = form.cert_ca_cert_path.value;

			if (ca_cert_path.length == 0)
			{
				alert('Please input the 802.1x CA Certificate Path !');
				return false;
			}
		}

		if (form.cert_auth_type_from_1x.value == 0) //md5
			return check_Wep(form, securitymode);

	}
	else
	{
		var tmp = "<% getStaAllProfileName(); %>";
		if (tmp.indexOf(profilename) >= 0 && (tmp.length == profilename.length)) {
			alert('Duplicate Profile Name!');
			return false;
		}
	}
	return true;
}

function profileClose()
{
	opener.location.reload();
}

function wep_switch_key_length()
{
	var form = document.profile_page;
	
	form.wep_key_1.value = "";
	form.wep_key_2.value = "";
	form.wep_key_3.value = "";
	form.wep_key_4.value = "";

	if (form.wep_key_length.options.selectedIndex == 0)
	{
		<!-- KEY length 64 bits -->
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			<!-- HEX -->
			form.wep_key_1.maxLength = 10;
			form.wep_key_2.maxLength = 10;
			form.wep_key_3.maxLength = 10;
			form.wep_key_4.maxLength = 10;
		}
		else
		{
			<!-- ASCII -->
			form.wep_key_1.maxLength = 5;
			form.wep_key_2.maxLength = 5;
			form.wep_key_3.maxLength = 5;
			form.wep_key_4.maxLength = 5;
		}
	}
	else
	{
		<!-- KEY length 128 bits -->
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			<!-- HEX -->
			form.wep_key_1.maxLength = 26;
			form.wep_key_2.maxLength = 26;
			form.wep_key_3.maxLength = 26;
			form.wep_key_4.maxLength = 26;
		}
		else
		{
			<!-- ASCII -->
			form.wep_key_1.maxLength = 13;
			form.wep_key_2.maxLength = 13;
			form.wep_key_3.maxLength = 13;
			form.wep_key_4.maxLength = 13;
		}
	}
}

function securityMode(form)
{
	var security_mode;

	hideElement( [ "div_security_infra_mode", "div_security_adhoc_mode" ] );

	hideWep(form);

	hideElement( [ "div_wpa", "div_wpa_algorithms", "wpa_passphrase" ] );
	form.cipher[0].disabled = true;
	form.cipher[1].disabled = true;
	form.passphrase.disabled = true;

	// 802.1x
	hideElement( [ "div_8021x", "div_8021x_cert_from_wpa", "div_8021x_cert_from_1x" ] );
	form.cert_auth_type_from_wpa.disabled = true;
	form.cert_auth_type_from_1x.disabled = true;

	form.cert_tunnel_auth_peap.disabled = true;
	form.cert_tunnel_auth_ttls.disabled = true;
	form.cert_id.disabled = true;
	form.cert_password.disabled = true;
	form.cert_client_cert_path.disabled = true;
	form.cert_private_key_path.disabled = true;
	form.cert_private_key_password.disabled = true;
	form.cert_ca_cert_path.disabled = true;
	
	if (form.network_type.value == '1') //infra
	{
		security_mode = form.security_infra_mode.value;
		showElement("div_security_infra_mode");
		form.security_infra_mode.disabled = false;
	}
	else
	{
		security_mode = form.security_adhoc_mode.value;
		showElement("div_security_adhoc_mode");
		form.security_adhoc_mode.disabled = false;
	}

	if ((security_mode == '0') || (security_mode == '1'))
		showWep(form);
	else if ((security_mode == '4') || (security_mode == '7') || (security_mode == '5'))
	{
		<!-- WPA -->
		showElement( [ "div_wpa", "div_wpa_algorithms", "wpa_passphrase" ] );

		form.cipher[0].disabled = false;
		form.cipher[1].disabled = false;
		form.passphrase.disabled = false;
	}
	else if ((security_mode == '3') || (security_mode == '6') || (security_mode == '8')) //wpa enterprise, 802.1x
	{
		if (security_mode != '8')
		{
			showElement( [ "div_wpa", "div_wpa_algorithms" ] );
			form.cipher[0].disabled = false;
			form.cipher[1].disabled = false;
		}

		showElement("div_8021x");
		
		if (security_mode != '8') //802.1x
			showElement("div_8021x_cert_from_wpa");
		else
			showElement("div_8021x_cert_from_1x");

		form.cert_tunnel_auth_peap.disabled = false;
		form.cert_tunnel_auth_ttls.disabled = false;
		form.cert_id.disabled = false;
		form.cert_password.disabled = false;

		use_ca_cert(form);
		use_client_cert(form);
		certAuthModeChange(form);
	}
}

function use_client_cert(form)
{
	displayElement( [ "div_client_cert_path", "div_private_key_path", "div_private_key_password" ],
		form.cert_use_client_cert.checked);
}

function use_ca_cert(form)
{
	displayElement( [ "div_ca_cert_path" ], form.cert_use_ca_cert.checked);
}

function certAuthModeChange(form)
{
	var auth_mode;
	var security_infra_mode = form.security_infra_mode.value;

	if (security_infra_mode == 3 || security_infra_mode == 6) //wpa-enterprise
		auth_mode = form.cert_auth_type_from_wpa.value;
	else if (security_infra_mode == 8) // 802.1x
		auth_mode = form.cert_auth_type_from_1x.value;

	hideWep(form);

	hideElement( [ "div_tunnel_auth_peap", "div_tunnel_auth_ttls", "div_password", "div_use_client_cert" ]);

	if ((auth_mode == 5) || (auth_mode == 6)) // PEAP & TTLS
	{
		if (auth_mode == 5)
			showElement("div_tunnel_auth_peap");
		else
			showElement("div_tunnel_auth_ttls");
		
		showElement( [ "div_password", "div_use_client_cert" ] );
	}
	else if (auth_mode == 4) //TLS
	{
		showElement( "div_use_client_cert" );
		form.cert_use_client_cert.checked = true;
	}
	else if ( auth_mode == 0) //MD5
	{
		showElement( "div_password" );
		showWep(form);
	}
	use_ca_cert(form);
	use_client_cert(form);
}

function hideWep(form)
{
	hideElement("div_wep");

	form.wep_key_length.disabled = true;
	form.wep_key_entry_method.disabled = true;
	form.wep_key_1.disabled = true;
	form.wep_key_2.disabled = true;
	form.wep_key_3.disabled = true;
	form.wep_key_4.disabled = true;
	form.wep_default_key.disabled = true;
}

function showWep(form)
{
	<!-- WEP -->
	showElement("div_wep");

	form.wep_key_length.disabled = false;
	form.wep_key_entry_method.disabled = false;
	form.wep_key_1.disabled = false;
	form.wep_key_2.disabled = false;
	form.wep_key_3.disabled = false;
	form.wep_key_4.disabled = false;
	form.wep_default_key.disabled = false;

	if (form.wep_key_length.options.selectedIndex == 0)
	{
		<!-- KEY length 64 bits -->
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			<!-- HEX -->
			form.wep_key_1.maxLength = 10;
			form.wep_key_2.maxLength = 10;
			form.wep_key_3.maxLength = 10;
			form.wep_key_4.maxLength = 10;
		}
		else
		{
			<!-- ASCII -->
			form.wep_key_1.maxLength = 5;
			form.wep_key_2.maxLength = 5;
			form.wep_key_3.maxLength = 5;
			form.wep_key_4.maxLength = 5;
		}
	}
	else {
		<!-- KEY length 128 bits -->
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			<!-- HEX -->
			form.wep_key_1.maxLength = 26;
			form.wep_key_2.maxLength = 26;
			form.wep_key_3.maxLength = 26;
			form.wep_key_4.maxLength = 26;
		}
		else
		{
			<!-- ASCII -->
			form.wep_key_1.maxLength = 13;
			form.wep_key_2.maxLength = 13;
			form.wep_key_3.maxLength = 13;
			form.wep_key_4.maxLength = 13;
		}
	}
}

function check_Wep(form, securitymode)
{
	var defaultid = form.wep_default_key.value;
	var keylen = 0 ;

	if ( defaultid == 1 )
		var keyvalue = form.wep_key_1.value;
	else if (defaultid == 2)
		var keyvalue = form.wep_key_2.value;
	else if (defaultid == 3)
		var keyvalue = form.wep_key_3.value;
	else if (defaultid == 4)
		var keyvalue = form.wep_key_4.value;

	if (form.wep_key_length.options.selectedIndex == 0)
	{
		// KEY length 64 bits
		if (form.wep_key_entry_method.options.selectedIndex == 0)
			keylen = 10; // HEX
		else
			keylen = 5; // ASCII
	}
	else
	{
		// KEY length 128 bits
		if (form.wep_key_entry_method.options.selectedIndex == 0)
			keylen = 26; // HEX
		else
			keylen = 13; // ASCII
	}

	if (keyvalue.length == 0 && ( securitymode == '1' || form.cert_auth_type_from_1x.value == '3')) // shared wep  || md5
	{
		alert('Please input wep key'+defaultid+' !');
		return false;
	}

	if (keyvalue.length != 0)
	{
		if (keyvalue.length != keylen)
		{
			alert('Please input '+keylen+' character of wep key !');
			return false;
		}
	}
	return true;
}
</script>

<script type="text/javascript">
function applyForm(form)
{
	if (checkData(form))
	{
		form.submit();
		window.opener.location.reload();
	}
}
</script>
</head>

<body onLoad="initValue();" onUnload="profileClose();">
<table class="body"><tr><td>


<form method="POST" name="profile_page" action="/goform/addStaProfile">

<table class="form">
<tr>
	<td class="title" colspan="2" id="addprofSysConf">System Configuration</td>
</tr>
<tr>
	<td class="head" id="addprofProfName">Profile Name</td>
	<td><input type=text name="profile_name" maxlength=32></td>
</tr>
<tr>
	<td class="head" id="addprofSSID">SSID</td>
	<td><input type=text name="Ssid" maxlength=32></td>
</tr>
<tr style="display:none">
	<td class="head" id="addprofNetType">Network Type</td>
	<td>
		<select name="network_type">
			<!--option value="0" id="addprofAdHoc">802.11 Ad Hoc</option -->
			<option value="1" id="addprofInfra" selected>Infrastructure</option>
		</select>
	</td>
</tr>
<tr id="div_power_saving_mode" style="display: none;">
	<td class="head" id="addprofPWSave">Power Saving Mode</td>
	<td>
		<input type="radio" name="power_saving_mode" value="0" checked><font id="addprofCAM">CAM (Constantly Awake Mode)</font>
		<br>
		<input type="radio" name="power_saving_mode" value="1"><font id="addprofPWSaveMode">Power Saving Mode</font>
	</td>
</tr>
<tr id="div_channel">
	<td class="head" id="addprofChannel">Channel</td>
	<td>
		<select name="channel" size="1">
		</select>
	</td>
</tr>
<tr id="div_b_premable_type">
	<td class="head" id="addprofPreambleType">11B Premable Type</td>
	<td>
		<select name="b_premable_type" size="1">
			<option value="0" id="addprofPremableAuto" selected>Auto</option>
			<option value="1" id="addprofPremableLong">Long</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="addprofRTS"> RTS Threshold </td>
	<td>
		<span style="display: none;"><input type="checkbox" name="rts_threshold" checked></span>
		<input type="text" name="rts_thresholdvalue" value="2347">
	</td>
</tr>
<tr>
	<td class="head" id="addprofFrag"> Fragement Threshold </td>
	<td>
		<span style="display:none;"><input type="checkbox" name="fragment_threshold" checked></span>
		<input type="text" name="fragment_thresholdvalue" value="2346">
	</td>
</tr>
</table>

<hr width="90%" align="left">

<table class="form">
<tr>
	<td class="title" colspan="2" id="addprofSecurePolicy">Security Policy</td>
</tr>
<tr id="div_security_infra_mode"> 
	<td class="head" id="addprofSecureMode">Security Mode</td>
	<td>
		<select name="security_infra_mode" id="security_infra_mode" size="1" onChange="securityMode(this.form);">
			<option value="0" selected>OPEN</option>
			<option value="1">SHARED</option>
			<option value="4">WPA-Personal</option>
			<option value="7">WPA2-Personal</option>
<!--			<option value="3">WPA-Enterprise</option>
			<option value="6">WPA2-Enterprise</option>
			<option value="8">802.1x</option>
-->
		</select>
	</td>
</tr>
<tr id="div_security_adhoc_mode"> 
	<td class="head" id="addprofAdHocSecure">Security Mode</td>
	<td>
		<select name="security_adhoc_mode" id="security_adhoc_mode" size="1" onChange="securityMode(this.form);">
			<option value="0" id="addprofAdHocOpen" selected>OPEN</option>
			<option value="1" id="addprofAdHocShared">SHARED</option>
			<option value="5" id="addprofAdHocWPA-None">WPA-NONE</option>
		</select>
	</td>
</tr>
</table>

<!-- WEP -->
<table id="div_wep" name="div_wep" class="form">
<tr>
	<td class="title" colspan="3" id="addprofWEP">Wire Equivalence Protection (WEP)</td>
</tr>
<tr> 
	<td class="head" colspan="2" id="addprofWEPKeyLength">WEP Key Length</td>
	<td>
		<select name="wep_key_length" size="1" onChange="wep_switch_key_length()">
			<option value="0" selected>64 bit (10 hex digits / 5 ascii keys)</option>
			<option value="1">128 bit (26 hex digits / 13 ascii keys)</option>
		</select>
	</td>
</tr>
<tr> 
	<td class="head" colspan="2" id="addprofWEPKeyEntryMethod">WEP Key Entry Method</td>
	<td>
		<select name="wep_key_entry_method" size="1" onChange="wep_switch_key_length()">
			<option value="0" id="addprofHex" selected >Hexadecimal</option>
			<option value="1" id="addprofASCII">Ascii Text</option>
		</select>
	</td>
</tr>
<tr> 
	<td class="head1" rowspan="4" id="addprofWEPKey">WEP Keys</td>
	<td class="head2" id="addprofWEPKey1">WEP Key 1 :</td>
	<td><input type="password" name="wep_key_1" maxlength="26" value=""></td>
</tr>
<tr> 
	<td class="head2" id="addprofWEPKey2">WEP Key 2 : </td>
	<td><input type="password" name="wep_key_2" maxlength="26" value=""></td>
</tr>
<tr> 
	<td class="head2" id="addprofWEPKey3">WEP Key 3 : </td>
	<td><input type="password" name="wep_key_3" maxlength="26" value=""></td>
</tr>
<tr> 
	<td class="head2" id="addprofWEPKey4">WEP Key 4 : </td>
	<td><input type="password" name="wep_key_4" maxlength="26" value=""></td>
</tr>
<tr> 
	<td class="head" colspan="2" id="addprofDefaultKey">Default Key</td>
	<td>
		<select name="wep_default_key" size="1">
			<option value="1" selected id="addprofDKey1">Key 1</option>
			<option value="2" id="addprofDKey2">Key 2</option>
			<option value="3" id="addprofDKey3">Key 3</option>
			<option value="4" id="addprofDKey4">Key 4</option>
		</select>
	</td>
</tr>
</table>

<!-- WPA -->
<table id="div_wpa" class="form">
<tr>
	<td class="title" colspan="2" id="addprofWPA">WPA</td>
</tr>
<tr id="div_wpa_algorithms"> 
	<td class="head" id="addprofWPAAlg">WPA Algorithms</td>
	<td>
		<input type="radio" name="cipher" value="0" checked>TKIP &nbsp;
		<input type="radio" name="cipher" value="1">AES &nbsp;
	</td>
</tr>
<tr id="wpa_passphrase">
	<td class="head" id="addprofPassPhrase">Pass Phrase</td>
	<td>
		<input type="password" name="passphrase" size="28" maxlength="64" value="">
	</td>
</tr>
</table>

<!-- 802.1x -->
<table id="div_8021x" class="form">
<tr>
	<td class="title" colspan="2" >802.1x</td>
</tr>
<tr id="div_8021x_cert_from_1x">
	<td class="head" id="addprof1XAuthType">Authentication Type</td>
	<td>
		<select name="cert_auth_type_from_1x" id="cert_auth_type_from_1x" size="1" onChange="certAuthModeChange(form)">
			<option value="5" selected>PEAP</option>
			<option value="6">TTLS</option>
			<option value="4">TLS</option>
			<option value="0">MD5</option>
		</select>
	</td>
</tr>
<tr id="div_8021x_cert_from_wpa">
	<td class="head" id="addprofWPAAuthType">Authentication Type</td>
	<td>
		<select name="cert_auth_type_from_wpa" id="cert_auth_type_from_wpa" size="1" onChange="certAuthModeChange(form)">
			<option value="5" selected>PEAP</option>
			<option value="6">TTLS</option>
			<option value="4">TLS</option>
		</select>
	</td>
</tr>
<tr id="div_tunnel_auth_peap">
	<td class="head" id="addprofPEAPTunnelAtuth">Tunnel Authentication</td>
	<td>
		<select name="cert_tunnel_auth_peap" id="cert_tunnel_auth_peap" size="1">
			<option value="1" selected>MSCHAP v2</option>
		</select>
	</td>
</tr>
<tr id="div_tunnel_auth_ttls">
	<td class="head" id="addprofTTLSTunnelAuth">Tunnel Authentication</td>
	<td>
		<select name="cert_tunnel_auth_ttls" id="cert_tunnel_auth_ttls" size="1">
			<option value="0" selected>MSCHAP</option>
			<option value="1">MSCHAP v2</option>
			<option value="2">PAP</option>
		</select>
	</td>
</tr>
<tr id="div_identity">
	<td class="head" id="addprofIdentity">Identity</td>
	<td>
		<input type="text" name="cert_id" maxlength="32">
	</td>
</tr>
<tr id="div_password">
	<td class="head" id="addprofPasswd">Password</td>
	<td>
		<input type="password" name="cert_password" maxlength="32">
	</td>
</tr>
<tr id="div_use_client_cert">
	<td class="head" id="addprofClientCert">Client Certificate</td>
	<td>
		<input type="checkbox" name="cert_use_client_cert" onClick="use_client_cert(this.form);"><font id="addprofClientCertUsed">Used</font>
	</td>
</tr>
<tr id="div_client_cert_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="addprofClientCertPath">Client Certificate Path</font></td>
	<td>
		<input type="text" name="cert_client_cert_path" maxlength="64">
	</td>
</tr>
<tr id="div_private_key_path" name="div_private_key_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="addprofPrivateKeyPath">Private Key Path</font></td>
	<td>
		<input type="text" name="cert_private_key_path" maxlength="64">
	</td>
</tr>
<tr id="div_private_key_password">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="addprofPrivateKeyPasswd">Private Key Password</font></td>
	<td>
		<input type="text" name="cert_private_key_password" maxlength="32">
	</td>
</tr>
<tr id="div_use_ca_cert">
	<td class="head" id="addprofCACert">CA Certificate</td>
	<td>
		<input type="checkbox" name="cert_use_ca_cert" onClick="use_ca_cert(this.form);"><font id="addprofCACertUsed">Used</font>
	</td>
</tr>
<tr id="div_ca_cert_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="addprofCACertPath">CA Certificate Path</font></td>
	<td>
		<input type="text" name="cert_ca_cert_path" maxlength="64">
	</td>
</tr>
</table>

<br>

<table class="buttons">
<tr>
	<td>
		<input type="button" name="applyButton" class="normal" value="Apply" id="addprofApply" onClick="applyForm(this.form);"> &nbsp; &nbsp;
		<input type="button" name="resetButton" class="normal" value="Cancel" id="addprofCancel" onClick="window.close();">
	</td>
</tr>
</table>
</form>

<div class="whitespace">&nbsp;</div>

</td></tr>
</table>
</body>
</html>
