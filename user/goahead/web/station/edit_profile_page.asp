<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Wireless Station Profile Edit Page</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
	_TR("editprofSysConf", "addprof system config");
	_TR("editprofName", "addprof profile name");
	_TR("editprofSSID", "station ssid");
	_TR("editprofNetType", "station network type");
	_TR("editprofAdHoc", "addprof adhoc");
	_TR("editprofInfra", "addprof infrastructure");
	_TR("editprofPWSave", "addprof power save");
	_TR("editprofCAM", "addprof cam");
	_TR("editprofPWSaveMode", "addprof power save");
	_TR("editprofChannel", "station channel");
	_TR("editprofPreamble", "addprof preamble type");
	_TR("editprofPreambleAuto", "wireless auto");
	_TR("editprofPreambleLong", "wireless long");
	_TR("editprofRTS", "adv rts threshold");
	_TR("editprofRTSUsed", "station used");
	_TR("editprofFrag", "adv fragment threshold");
	_TR("editprofFragUsed", "station used");
	_TR("editprofSecurePolicy", "addprof secure policy");
	_TR("editprofSecureMode", "secure security mode");
	_TR("editprofAdHocSecureMode", "secure security mode");
	_TR("editprofWEP", "secure wep");
	_TR("editprofWEPKeyLength", "addprof wep key length");
	_TR("editprofWEPKeyEntryMethod", "addprof wep key entry method");
	_TR("editprofWEPHex", "addprof hex");
	_TR("editprofWEPASCII", "addprof ascii");
	_TR("editprofWEPKey", "secure wep key");
	_TR("editprofWEPKey1", "secure wep key1");
	_TR("editprofWEPKey2", "secure wep key2");
	_TR("editprofWEPKey3", "secure wep key3");
	_TR("editprofWEPKey4", "secure wep key4");
	_TR("editprofDefaultKey", "secure wep default key");
	_TR("editprofDKey1", "secure wep default key1");
	_TR("editprofDKey2", "secure wep default key2");
	_TR("editprofDkey3", "secure wep default key3");
	_TR("editprofDKey4", "secure wep default key4");
	_TR("editprofWPA", "secure wpa");
	_TR("editprofWPAAlg", "secure wpa algorithm");
	_TR("editprofPassPhrase", "secure wpa pass phrase");
	_TR("editprof1XAuthType", "addprof 8021X Auth Type");
	_TR("editprofWPAAuthType", "addprof 8021X Auth Type");
	_TR("editprofPeapTunnelAuth", "addprof tunnel auth");
	_TR("editprofTTLSTunnelAuth", "addprof tunnel auth");
	_TR("editprofIdentity", "addprof identity");
	_TR("editprofPasswd", "addprof passwd");
	_TR("editprofClientCert", "addprof client cert");
	_TR("editprofClientCertUsed", "station used");
	_TR("editprofClientCertPath", "addprof client cert path");
	_TR("editprofPriKeyPath", "addprof private key path");
	_TR("editprofPriKeyPasswd", "addprof private key passwd");
	_TR("editprofCACert", "addprof ca cert");
	_TR("editprofCACertUsed", "station used");
	_TR("editprofCACertPath", "addprof ca cert path");
	
	_TRV("editprofApply", "wireless apply");
	_TRV("editprofCancel", "wireless cancel");
}

function initValue()
{
	initTranslation();
	
	var form = document.profile_page;
	getChannels(form);

	form.profile_name.value = "<% getStaProfileData(1); %>";
	form.Ssid.value = "<% getStaProfileData(2); %>";
	//form.network_type.value = <% getStaProfileData(3); %>;
	form.power_saving_mode['<%getStaProfileData(4);%>'].checked = true;
	form.b_premable_type.value = "<% getStaProfileData(5); %>";
	//if (<% getStaProfileData(6); %> == 1)
	//	form.rts_threshold.checked = true;
	form.rts_thresholdvalue.value = "<% getStaProfileData(7); %>";
	//if (<% getStaProfileData(8); %> == 1)
	//	form.fragment_threshold.checked = true;
	form.fragment_thresholdvalue.value = "<% getStaProfileData(9); %>";

	var encryp = <% getStaProfileData(10); %>;
	var auth = <% getStaProfileData(11); %>;
	
	form.wep_key_1.value = "<% getStaProfileData(12); %>";
	form.wep_key_2.value = "<% getStaProfileData(13); %>";
	form.wep_key_3.value = "<% getStaProfileData(14); %>";
	form.wep_key_4.value = "<% getStaProfileData(15); %>";
	form.wep_key_entry_method.value = <% getStaProfileData(16); %>;
	form.wep_key_entry_method.value = <% getStaProfileData(17); %>;
	form.wep_key_entry_method.value = <% getStaProfileData(18); %>;
	form.wep_key_entry_method.value = <% getStaProfileData(19); %>;
	form.wep_key_length.value = <% getStaProfileData(20); %>;
	form.wep_key_length.value = <% getStaProfileData(21); %>;
	form.wep_key_length.value = <% getStaProfileData(22); %>;
	form.wep_key_length.value = <% getStaProfileData(23); %>;

	var keydefaultid = <% getStaProfileData(24); %>;
	if (keydefaultid == 0)
		form.wep_default_key.options.selectedIndex = 0;
	else
		form.wep_default_key.options.selectedIndex = keydefaultid -1;
	form.passphrase.value = "<% getStaProfileData(25); %>";

	hideElement( [ "div_power_saving_mode", "div_channel", "div_b_premable_type" ] );

	// wpa_supplicant
	var keymgmt = <% getStaProfileData(26); %>;
	var eap = <% getStaProfileData(27); %>;

	if (keymgmt == 1) //Rtwpa_supplicantKeyMgmtWPAEAP,
		form.cert_auth_type_from_wpa.value = eap;
	else if (keymgmt == 2) //Rtwpa_supplicantKeyMgmtIEEE8021X,
		form.cert_auth_type_from_1x.value = eap;

	form.cert_id.value = "<% getStaProfileData(28); %>";
	var cacert = "<% getStaProfileData(29); %>";
	form.cert_ca_cert_path.value = cacert;
	var clientcert = "<% getStaProfileData(30); %>";
	form.cert_client_cert_path.value = clientcert;
	form.cert_private_key_path.value = "<% getStaProfileData(31); %>";
	form.cert_private_key_password.value = "<% getStaProfileData(32); %>";
	form.cert_password.value = "<% getStaProfileData(33); %>";
	var tunnel = <% getStaProfileData(34); %>;

	if (eap == 5 ) // Rtwpa_supplicantEAPPEAP,
		form.cert_tunnel_auth_peap.value = tunnel;
	else if (eap == 6 ) // Rtwpa_supplicantEAPTTLS,
		form.cert_tunnel_auth_ttls.value  = tunnel;

	if (cacert.length > 1) // != '0'
		form.cert_use_ca_cert.checked = true;
	if (clientcert.length > 1) // != '0'
		form.cert_use_client_cert.checked = true;

	networkTypeChange(form, auth, encryp);
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

function networkTypeChange(form, auth, encryp)
{
	var nmode;
	
	hideElement( [ "div_b_premable_type", "div_power_saving_mode", "div_channel", "div_security_infra_mode", "div_security_adhoc_mode" ] );

	nmode = 1*form.network_type.value;
	if (nmode == 1)
	{
		showElement( [ "div_power_saving_mode", "div_security_infra_mode" ] );

		/*if (auth == 0) //open
			form.security_infra_mode.options.selectedIndex = 0 ;
		else if (auth == 1) //shared
			form.security_infra_mode.options.selectedIndex = 1 ;
		else if (auth == 4) //wpa-psk
			form.security_infra_mode.options.selectedIndex = 2 ;
		else if (auth == 7) //wpa2-psk
			form.security_infra_mode.options.selectedIndex = 3 ;*/

		if (!auth )
			auth = form.security_infra_mode.value;

		form.security_infra_mode.value = auth;
		
		if (auth >= 3)
		{
			if (encryp == 4) // tkip
				form.cipher[0].checked = true;
			else if (encryp == 6)
				form.cipher[1].checked = true;
		}
	}
	else
	{
		showElement( [ "div_b_premable_type", "div_channel", "div_security_adhoc_mode" ] );

		/*if (auth == 0) //open
			form.security_adhoc_mode.options.selectedIndex = 0 ;
		else if (auth == 1) //shared
			form.security_adhoc_mode.options.selectedIndex = 1 ;
		else if (auth == 5) //wpa-none
			form.security_adhoc_mode.options.selectedIndex = 2 ;*/

		if (!auth )
			auth = form.security_adhoc_mode.value;

		form.security_adhoc_mode.value = auth;
	}

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
	else if (securitymode  == '0' || securitymode  == '1')
	{
		return check_Wep(form, securitymode);
	}
	else if (securitymode  == '4' || securitymode == '7' || securitymode == '5')
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
		var org_profilename = "<% getStaProfileData(1); %>";
		if (org_profilename != profilename)
		{
			var start=0, i=0, end=0;

			for (end=tmp.indexOf(";"), i=0; i<6 , end>=0; i++, end=tmp.indexOf(";"))
			{

				var subprofilename = tmp.substring(start, end);

				if (subprofilename.indexOf(profilename) >=0 && subprofilename.length == profilename.length)
				{
					alert('Duplicate the Profile Name!');
					return false;
				}

				tmp = tmp.substring(end+1);
				end=tmp.indexOf(";");
			}
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
	document.profile_page.wep_key_1.value = "";
	document.profile_page.wep_key_2.value = "";
	document.profile_page.wep_key_3.value = "";
	document.profile_page.wep_key_4.value = "";

	if (document.profile_page.wep_key_length.options.selectedIndex == 0) {
		<!-- KEY length 64 bits -->
		if (document.profile_page.wep_key_entry_method.options.selectedIndex == 0) {
			<!-- HEX -->
			document.profile_page.wep_key_1.maxLength = 10;
			document.profile_page.wep_key_2.maxLength = 10;
			document.profile_page.wep_key_3.maxLength = 10;
			document.profile_page.wep_key_4.maxLength = 10;
		}
		else {
			<!-- ASCII -->
			document.profile_page.wep_key_1.maxLength = 5;
			document.profile_page.wep_key_2.maxLength = 5;
			document.profile_page.wep_key_3.maxLength = 5;
			document.profile_page.wep_key_4.maxLength = 5;
		}
	}
	else {
		<!-- KEY length 128 bits -->
		if (document.profile_page.wep_key_entry_method.options.selectedIndex == 0) {
			<!-- HEX -->
			document.profile_page.wep_key_1.maxLength = 26;
			document.profile_page.wep_key_2.maxLength = 26;
			document.profile_page.wep_key_3.maxLength = 26;
			document.profile_page.wep_key_4.maxLength = 26;
		}
		else {
			<!-- ASCII -->
			document.profile_page.wep_key_1.maxLength = 13;
			document.profile_page.wep_key_2.maxLength = 13;
			document.profile_page.wep_key_3.maxLength = 13;
			document.profile_page.wep_key_4.maxLength = 13;
		}
	}
}

function securityMode(form)
{
	var security_mode;

	hideElement( [ "div_security_infra_mode", "div_security_adhoc_mode", "div_wpa", "div_wpa_algorithms", "wpa_passphrase", "div_wep" ] );
	hideElement( [ "div_8021x", "div_8021x_cert_from_wpa", "div_8021x_cert_from_1x" ] );

	if (form.network_type.value == '1') //infra
	{
		security_mode = form.security_infra_mode.value;
		showElement("div_security_infra_mode");
	}
	else
	{
		security_mode = form.security_adhoc_mode.value;
		showElement("div_security_adhoc_mode");
	}


	if (security_mode == '0' || security_mode == '1')
		showWep(form);
	else if (security_mode == '4' || security_mode == '7' || security_mode == '5')
	{
		<!-- WPA -->
		showElement( [ "div_wpa", "div_wpa_algorithms", "wpa_passphrase" ] );
	}
	else if (security_mode == '3' || security_mode == '6' || security_mode == '8') //wpa enterprise, 802.1x
	{
		if (security_mode != 8)
			showElement("div_wpa_algorithms");

		<!-- 802.1x -->
		showElement("div_8021x");
		
		if (security_mode != '8') //802.1x
			showElement("div_8021x_cert_from_wpa");
		else
			showElement("div_8021x_cert_from_1x");

		use_ca_cert(form);
		use_client_cert(form);
		certAuthModeChange(form);
	}
}

function use_client_cert()
{
	displayElement( [
			"div_client_cert_path", "div_private_key_path", "div_private_key_password",
		], document.profile_page.cert_use_client_cert.checked);
}

function use_ca_cert(form)
{
	displayElement( [ "div_ca_cert_path" ], document.profile_page.cert_use_ca_cert.checked);
}

function certAuthModeChange(form)
{
	var auth_mode;
	var security_infra_mode = document.profile_page.security_infra_mode.value;

	if (security_infra_mode == '3' || security_infra_mode == '6') //wpa-enterprise
		auth_mode = form.cert_auth_type_from_wpa.value;
	else if (security_infra_mode == 8) // 802.1x
		auth_mode = form.cert_auth_type_from_1x.value;

	hideElement( [ "div_wep", "div_tunnel_auth_peap", "div_tunnel_auth_ttls", "div_password" ] );

	form.cert_id.disabled = true;
	var certpath = form.cert_client_cert_path.value;
	if (certpath.length > 0 )
		form.cert_use_client_cert.checked = true;
	else
		form.cert_use_client_cert.checked = false;

	certpath = form.cert_ca_cert_path.value;
	if (certpath.length > 0 )
		form.cert_use_ca_cert.checked = true;
	else
		form.cert_use_ca_cert.checked = false;
	
	hideElement("div_use_client_cert");

	form.cert_private_key_path.disabled = true;
	form.cert_private_key_password.disabled = true;
	form.cert_client_cert_path.disabled = true;

	if (auth_mode == 5 || auth_mode == 6) // PEAP & TTLS
	{
		if (auth_mode == 5)
			showElement("div_tunnel_auth_peap");
		else
			showElement("div_tunnel_auth_ttls");
		
		showElement( [ "div_password", "div_use_client_cert" ] );
	}
	else if (auth_mode == 4) //TLS
	{
		showElement( [ "div_use_client_cert" ] );
		form.cert_use_client_cert.checked = true;
	}
	else if (auth_mode == 0) //MD5
	{
		form.cert_use_client_cert.checked = false;

		showElement("div_password");
		showWep(form);
	}
	use_ca_cert(form);
	use_client_cert(form);
}

function showWep(form)
{
	// WEP
	showElement("div_wep");

	if (form.wep_key_length.options.selectedIndex == 0)
	{
		// KEY length 64 bits
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			// HEX
			form.wep_key_1.maxLength = 10;
			form.wep_key_2.maxLength = 10;
			form.wep_key_3.maxLength = 10;
			form.wep_key_4.maxLength = 10;
		}
		else
		{
			// ASCII
			form.wep_key_1.maxLength = 5;
			form.wep_key_2.maxLength = 5;
			form.wep_key_3.maxLength = 5;
			form.wep_key_4.maxLength = 5;
		}
	}
	else
	{
		// KEY length 128 bits
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			// HEX
			form.wep_key_1.maxLength = 26;
			form.wep_key_2.maxLength = 26;
			form.wep_key_3.maxLength = 26;
			form.wep_key_4.maxLength = 26;
		}
		else
		{
			// ASCII
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

	if (defaultid == 1 )
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
		{
			// HEX
			keylen = 10;
		}
		else
		{
			// ASCII
			keylen = 5;
		}
	}
	else
	{
		// KEY length 128 bits
		if (form.wep_key_entry_method.options.selectedIndex == 0)
		{
			// HEX
			keylen = 26;
		}
		else
		{
			// ASCII
			keylen = 13;
		}
	}

	if (keyvalue.length == 0 && ( securitymode == 1 || form.cert_auth_type_from_1x.value == '3')) // shared wep  || md5
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

function submit_apply(form)
{
	if (checkData(form))
	{
		form.submit();
		window.close();
		return false;
	}
}
</script>
</head>

<body onLoad="initValue()" onUnload="profileClose()">
<table class="body"><tr><td>


<form method="POST" name="profile_page" action="/goform/editStaProfile">
<table class="form">
<tr>
	<td class="title" colspan="2" id="editprofSysConf">System Configuration</td>
</tr>
<tr>
	<td class="head" id="editprofName">Profile Name</td>
	<td><input type="text" name="profile_name" maxlength="32"></td>
</tr>
<tr>
	<td class="head" id="editprofSSID">SSID</td>
	<td><input type="text" name="Ssid" maxlength="32"></td>
</tr>
<tr style="display:none">
	<td class="head" id="editprofNetType">Network Type</td>
	<td>
		<select name="network_type" size="1" onChange="networkTypeChange(this.form);">
			<!--option value="0" id="editprofAdHoc">802.11 Ad Hoc</option-->
			<option value="1" id="editprofInfra" selected>Infrastructure</option>
		</select>
	</td>
</tr>
<tr id="div_power_saving_mode" name="div_power_saving_mode">
	<td class="head" id="editprofPWSave">Power Saving Mode</td>
	<td>
		<input type="radio" name="power_saving_mode" value="0" checked><font id="editprofCAM">CAM (Constantly Awake Mode)</font>
		<br>
		<input type="radio" name="power_saving_mode" value="1"><font id="editprofPWSaveMode">Power Saving Mode</font>
	</td>
</tr>
<tr id="div_channel" name="div_channel">
	<td class="head" id="editprofChannel"> Channel </td>
	<td>
		<select name="channel" size="1">
		</select>
	</td>
</tr>
<tr id="div_b_premable_type" name="div_b_premable_type">
	<td class="head" id="editprofPreamble">11B Premable Type</td>
	<td>
		<select name="b_premable_type" size="1">
			<option value="0" id="editprofPreambleAuto" selected>Auto</option>
			<option value="1" id="editprofPreambleLong">Long</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="editprofRTS">RTS Threshold</td>
	<td>
		<span style="display: none;"><input type="checkbox" name="rts_threshold" checked><font id="editprofRTSUsed"> Used &nbsp;&nbsp;</font></span>
		<input type="text" name="rts_thresholdvalue" value="2347">
	</td>
</tr>
<tr>
	<td class="head" id="editprofFrag"> Fragement Threshold </td>
	<td>
		<span style="display: none;"><input type="checkbox" name="fragment_threshold" checked><font id="editprofFragUsed"> Used &nbsp;&nbsp;</font></span>
		<input type="text" name="fragment_thresholdvalue" value="2346">
	</td>
</tr>
</table>

<hr width="90%" align="left">

<table class="form">
<tr>
	<td class="title" colspan="2" id="editprofSecurePolicy">Security Policy</td>
</tr>
<tr id="div_security_infra_mode" name="div_security_infra_mode"> 
	<td class="head" id="editprofSecureMode">Security Mode</td>
	<td>
		<select name="security_infra_mode" id="security_infra_mode" size="1" onChange="securityMode(this.form);">
			<option value="0" selected>OPEN</option>
			<option value="1">SHARED</option>
			<option value="4">WPA-Personal</option>
			<option value="7">WPA2-Personal</option>
	<!--		<option value="3">WPA-Enterprise</option>
			<option value="6">WPA2-Enterprise</option>
			<option value="8">802.1x</option>
	-->
		</select>
	</td>
</tr>
<tr id="div_security_adhoc_mode" name="div_security_adhoc_mode"> 
	<td class="head" id="editprofAdHocSecureMode">Security Mode</td>
	<td>
		<select name="security_adhoc_mode" id="security_adhoc_mode" size="1" onChange="securityMode(this.form);">
			<option value="0" selected>OPEN</option>
			<option value="1">SHARED</option>
			<option value="5">WPA-NONE</option>
		</select>
	</td>
	</tr>
</table>

<table id="div_wep" name="div_wep" class="form">
<tr>
	<td class="title" colspan="3" id="editprofWEP">Wire Equivalence Protection (WEP)</td>
</tr>
<tr>
	<td class="head" id="editprofWEPKeyLength" colspan="2">WEP Key Length</td>
	<td>
		<select name="wep_key_length" size="1" onChange="wep_switch_key_length()">
			<option value="0" selected>64 bit (10 hex digits/ 5 ascii keys)</option>
			<option value="1">128 bit (26 hex digits/13 ascii keys)</option>
		</select>
	</td>
</tr>
<tr>
	<td width="45%" colspan="2" class="head" id="editprofWEPKeyEntryMethod">WEP Key Entry Method</td>
	<td>
		<select name="wep_key_entry_method" size="1" onChange="wep_switch_key_length()">
			<option value="0" id="editprofWEPHex" selected>Hexadecimal</option>
			<option value="1" id="editprofWEPASCII">Ascii Text</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head1" rowspan="4" id="editprofWEPKey">WEP Keys</td>
	<td class="head2" id="editprofWEPKey1">WEP Key 1 :</td>
	<td><input type="password" name=wep_key_1 maxlength="26" value=""></td>
</tr>
<tr>
	<td class="head2" id="editprofWEPKey2">WEP Key 2 : </td>
	<td><input type="password" name=wep_key_2 maxlength="26" value=""></td>
</tr>
<tr>
	<td class="head2" id="editprofWEPKey3">WEP Key 3 : </td>
	<td><input type="password" name=wep_key_3 maxlength="26" value=""></td>
</tr>
<tr>
	<td class="head2" id="editprofWEPKey4">WEP Key 4 : </td>
	<td><input type="password" name=wep_key_4 maxlength="26" value=""></td>
</tr>
<tr>
	<td class="head" colspan="2" id="editprofDefaultKey">Default Key</td>
	<td>
		<select name="wep_default_key" size="1">
			<option value="1" id="editprofDKey1" selected>Key 1</option>
			<option value="2" id="editprofDKey2">Key 2</option>
			<option value="3" id="editprofDkey3">Key 3</option>
			<option value="4" id="editprofDKey4">Key 4</option>
		</select>
	</td>
</tr>
</table>

<table id="div_wpa" name="div_wpa" class="form">
<tr>
	<td class="title" colspan="2" id="editprofWPA">WPA</td>
</tr>
<tr id="div_wpa_algorithms" name="div_wpa_algorithms"> 
	<td class="head" id="editprofWPAAlg">WPA Algorithms</td>
	<td>
		<input type="radio" name="cipher" id="cipher" value="0" checked>TKIP &nbsp;
		<input type="radio" name="cipher" id="cipher" value="1">AES &nbsp;
	</td>
</tr>
<tr id="wpa_passphrase" name="wpa_passphrase">
	<td class="head" id="editprofPassPhrase">Pass Phrase</td>
	<td>
		<input type="password" name="passphrase" size="28" maxlength="64" value="">
	</td>
</tr>
</table>

<!-- 802.1x -->
<table id="div_8021x" name="div_8021x" class="form">
<tr>
	<td class="title" colspan="2">802.1x</td>
</tr>
<tr id="div_8021x_cert_from_1x" name="div_8021x_cert_from_1x">
	<td class="head" id="editprof1XAuthType">Authentication Type</td>
	<td>
		<select name="cert_auth_type_from_1x" id="cert_auth_type_from_1x" size="1" onChange="certAuthModeChange(this.form)">
			<option value="5" selected>PEAP</option>
			<option value="6">TTLS</option>
			<option value="4">TLS</option>
			<option value="0">MD5</option>
		</select>
	</td>
</tr>
<tr id="div_8021x_cert_from_wpa" name="div_8021x_cert_from_wpa">
	<td class="head" id="editprofWPAAuthType">Authentication Type</td>
	<td>
		<select name="cert_auth_type_from_wpa" id="cert_auth_type_from_wpa" size="1" onChange="certAuthModeChange(this.form)">
			<option value="5" selected>PEAP</option>
			<option value="6">TTLS</option>
			<option value="4">TLS</option>
		</select>
	</td>
</tr>
<tr id="div_tunnel_auth_peap" name="div_tunnel_auth_peap">
	<td class="head" id="editprofPeapTunnelAuth">Tunnel Authentication</td>
	<td>
		<select name="cert_tunnel_auth_peap" id="cert_tunnel_auth_peap" size="1">
			<option value=1 selected>MSCHAP v2</option>
		</select>
	</td>
</tr>
<tr id="div_tunnel_auth_ttls" name="div_tunnel_auth_ttls">
	<td class="head" id="editprofTTLSTunnelAuth">Tunnel Authentication</td>
	<td>
		<select name="cert_tunnel_auth_ttls" id="cert_tunnel_auth_ttls" size="1">
			<option value="0" selected>MSCHAP</option>
			<option value="1">MSCHAP v2</option>
			<option value="2">PAP</option>
		</select>
	</td>
</tr>
<tr id="div_identity" name="div_identity">
	<td class="head" id="editprofIdentity">Identity</td>
	<td>
		<input type="text" name="cert_id" maxlength="32">
	</td>
</tr>
<tr id="div_password" name="div_password">
	<td class="head" id="editprofPasswd">Password</td>
	<td>
		<input type="password" name="cert_password" maxlength="32">
	</td>
</tr>
<tr id="div_use_client_cert" name="div_use_client_cert">
	<td class="head" id="editprofClientCert">Client Certificate</td>
	<td>
		<input type="checkbox" name="cert_use_client_cert" onClick="use_client_cert()" ><font id="editprofClientCertUsed">Used</font>
	</td>
</tr>
<tr id="div_client_cert_path" name="div_client_cert_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="editprofClientCertPath">Client Certificate Path</font></td>
	<td>
		<input type="text" name="cert_client_cert_path" maxlength="64">
	</td>
</tr>
<tr id="div_private_key_path" name="div_private_key_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="editprofPriKeyPath">Private Key Path</font></td>
	<td>
		<input type="text" name="cert_private_key_path" maxlength="64">
	</td>
</tr>
<tr id="div_private_key_password" name="div_private_key_password">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="editprofPriKeyPasswd">Private Key Password</font></td>
	<td>
		<input type="text" name="cert_private_key_password" maxlength="32">
	</td>
</tr>
<tr id="div_use_ca_cert" name="div_use_ca_cert">
	<td class="head" id="editprofCACert">CA Certificate</td>
	<td>
		<input type="checkbox" name="cert_use_ca_cert" onClick="use_ca_cert()" ><font id="editprofCACertUsed">Used</font>
	</td>
</tr>
<tr id="div_ca_cert_path" name="div_ca_cert_path">
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="editprofCACertPath">CA Certificate Path</font></td>
	<td>
		<input type="text" name="cert_ca_cert_path" maxlength="64">
	</td>
</tr>
</table>

<table class="buttons">
<tr align="center">
	<td>
		<input type="button" style="{width:120px;}" value="Apply" id="editprofApply" onClick="submit_apply(this.form);"> &nbsp; &nbsp;
		<input type="reset"  style="{width:120px;}" value="Cancel" id="editprofCancel" onClick="window.close()">
	</td>
</tr>
</table>
</form>

<div class="whitespace">&nbsp;</div>


</td></tr></table>
</body>
</html>
