<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<title>Basic Wireless Settings</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var mbssidapisolated = '<% getCfgZero(1, "NoForwardingBTNBSSID"); %>';
var channel_index  = '<% getWlanChannel(); %>';
var fxtxmode = '<% getCfgGeneral(1, "FixedTxMode"); %>';
var countrycode = '<% getCfgGeneral(1, "CountryCode"); %>';
var ht_mode = '<% getCfgZero(1, "HT_OpMode"); %>';
var ht_gi = '<% getCfgZero(1, "HT_GI"); %>';
var ht_stbc = '<% getCfgZero(1, "HT_STBC"); %>';
var ht_mcs = '<% getCfgZero(1, "HT_MCS"); %>';
var ht_htc = '<% getCfgZero(1, "HT_HTC"); %>';
var ht_rdg = '<% getCfgZero(1, "HT_RDG"); %>';
var ht_extcha = '<% getCfgZero(1, "HT_EXTCHA"); %>';
var ht_amsdu = '<% getCfgZero(1, "HT_AMSDU"); %>';
var ht_autoba = '<% getCfgZero(1, "HT_AutoBA"); %>';
var ht_badecline = '<% getCfgZero(1, "HT_BADecline"); %>';
var ht_f_40mhz = '<% getCfgZero(1, "HT_40MHZ_INTOLERANT"); %>';
//var wifi_optimum = '<!--#include ssi=getWlanWiFiTest()-->';
var apcli_include = '<% getWlanApcliBuilt(); %>';
var tx_stream_idx = '<% getCfgZero(1, "HT_TxStream"); %>';
var rx_stream_idx = '<% getCfgZero(1, "HT_RxStream"); %>';
var is3t3r = '<% is3t3r(); %>';

var ChannelList_24G = 
[
	"2412MHz (Channel 1)",
	"2417MHz (Channel 2)",
	"2422MHz (Channel 3)",
	"2427MHz (Channel 4)",
	"2432MHz (Channel 5)",
	"2437MHz (Channel 6)",
	"2442MHz (Channel 7)",
	"2447MHz (Channel 8)",
	"2452MHz (Channel 9)",
	"2457MHz (Channel 10)",
	"2462MHz (Channel 11)",
	"2467MHz (Channel 12)",
	"2472MHz (Channel 13)",
	"2484MHz (Channel 14)"
];

var ChannelList_5G = new Array(33);
ChannelList_5G[0] = "5180MHz (Channel 36)";
ChannelList_5G[1] = "5200MHz (Channel 40)";
ChannelList_5G[2] = "5220MHz (Channel 44)";
ChannelList_5G[3] = "5240MHz (Channel 48)";
ChannelList_5G[4] = "5260MHz (Channel 52)";
ChannelList_5G[5] = "5280MHz (Channel 56)";
ChannelList_5G[6] = "5300MHz (Channel 60)";
ChannelList_5G[7] = "5320MHz (Channel 64)";
ChannelList_5G[16] = "5500MHz (Channel 100)";
ChannelList_5G[17] = "5520MHz (Channel 104)";
ChannelList_5G[18] = "5540MHz (Channel 108)";
ChannelList_5G[19] = "5560MHz (Channel 112)";
ChannelList_5G[20] = "5580MHz (Channel 116)";
ChannelList_5G[21] = "5600MHz (Channel 120)";
ChannelList_5G[22] = "5620MHz (Channel 124)";
ChannelList_5G[23] = "5640MHz (Channel 128)";
ChannelList_5G[24] = "5660MHz (Channel 132)";
ChannelList_5G[25] = "5680MHz (Channel 136)";
ChannelList_5G[26] = "5700MHz (Channel 140)";
ChannelList_5G[28] = "5745MHz (Channel 149)";
ChannelList_5G[29] = "5765MHz (Channel 153)";
ChannelList_5G[30] = "5785MHz (Channel 157)";
ChannelList_5G[31] = "5805MHz (Channel 161)";
ChannelList_5G[32] = "5825MHz (Channel 165)";

var HT5GExtCh = new Array(22);
HT5GExtCh[0] = new Array(1, "5200MHz (Channel 40)"); // channel 36's extension channel
HT5GExtCh[1] = new Array(0, "5180MHz (Channel 36)"); // channel 40's extension channel
HT5GExtCh[2] = new Array(1, "5240MHz (Channel 48)"); // channel 44's extension channel
HT5GExtCh[3] = new Array(0, "5220MHz (Channel 44)"); // channel 48's extension channel
HT5GExtCh[4] = new Array(1, "5280MHz (Channel 56)"); // channel 52's extension channel
HT5GExtCh[5] = new Array(0, "5260MHz (Channel 52)"); // channel 56's extension channel
HT5GExtCh[6] = new Array(1, "5320MHz (Channel 64)"); // channel 60's extension channel
HT5GExtCh[7] = new Array(0, "5300MHz (Channel 60)"); // channel 64's extension channel
HT5GExtCh[8] = new Array(1, "5520MHz (Channel 104)"); // channel 100's extension channel
HT5GExtCh[9] = new Array(0, "5500MHz (Channel 100)"); // channel 104's extension channel
HT5GExtCh[10] = new Array(1, "5560MHz (Channel 112)"); // channel 108's extension channel
HT5GExtCh[11] = new Array(0, "5540MHz (Channel 108)"); // channel 112's extension channel
HT5GExtCh[12] = new Array(1, "5600MHz (Channel 120)"); // channel 116's extension channel
HT5GExtCh[13] = new Array(0, "5580MHz (Channel 116)"); // channel 120's extension channel
HT5GExtCh[14] = new Array(1, "5640MHz (Channel 128)"); // channel 124's extension channel
HT5GExtCh[15] = new Array(0, "5620MHz (Channel 124)"); // channel 128's extension channel
HT5GExtCh[16] = new Array(1, "5680MHz (Channel 136)"); // channel 132's extension channel
HT5GExtCh[17] = new Array(0, "5660MHz (Channel 132)"); // channel 136's extension channel
HT5GExtCh[18] = new Array(1, "5765MHz (Channel 153)"); // channel 149's extension channel
HT5GExtCh[19] = new Array(0, "5745MHz (Channel 149)"); // channel 153's extension channel
HT5GExtCh[20] = new Array(1, "5805MHz (Channel 161)"); // channel 157's extension channel
HT5GExtCh[21] = new Array(0, "5785MHz (Channel 157)"); // channel 161's extension channel

function insertChannelOption(vChannel, band)
{
	var y = document.createElement('option');

	if (1*band == 24)
	{
		y.text = ChannelList_24G[1*vChannel - 1];
		y.value = 1*vChannel;
	}
	else if (1*band == 5)
	{
		y.value = 1*vChannel;
		if (1*vChannel <= 140)
			y.text = ChannelList_5G[((1*vChannel) - 36) / 4];
		else
			y.text = ChannelList_5G[((1*vChannel) - 36 - 1) / 4];
	}

	if (1*band == 24)
		var x=document.getElementById("sz11gChannel");
	else if (1*band == 5)
		var x=document.getElementById("sz11aChannel");

	try
	{
		x.add(y,null); // standards compliant
	}
	catch(ex)
	{
		x.add(y); // IE only
	}
}

function addOption(list, text, value)
{
	var option = new Option(text, value);
	
	try
	{
		list.add(option, null); // standards compliant
	}
	catch(ex)
	{
		list.add(option); // IE only
	}
}

function insertExtChannelOption()
{
	var wmode = document.wireless_basic.wirelessmode.value * 1;
	var option_length;

	if (wmode >= 5)
	{
		var x = document.getElementById('n_extcha');
		var tmp_value = x.value; // Store value
		x.options.length = 0;

		var CurrentCh = document.wireless_basic.sz11gChannel.value * 1;
		var option_length = document.wireless_basic.sz11gChannel.options.length;

		if ((CurrentCh >=1) && (CurrentCh <= 4))
			addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		else if ((CurrentCh >= 5) && (CurrentCh <= 7))
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
			addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if ((CurrentCh >= 8) && (CurrentCh <= 9))
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);

			if (option_length >= 14)
				addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if (CurrentCh == 10)
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);

			if (option_length > 14)
				addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if (CurrentCh >= 11)
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
		else
			addOption(x, "Auto Select", 0);
		
		x.value = tmp_value;
	}
}

function ChannelOnChange()
{
	if (document.wireless_basic.n_bandwidth[1].checked)
	{
		var w_mode = document.wireless_basic.wirelessmode.value;

		if ((1*w_mode == 6) || (1*w_mode == 7))
			displayElement('extension_channel', document.wireless_basic.n_bandwidth[1].checked);
		else if ((1*w_mode == 3) || (1*w_mode == 4))
		{
			displayElement('extension_channel', document.wireless_basic.n_bandwidth[1].checked);

			if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
				hideElement('extension_channel');
		}
	}

	insertExtChannelOption();
}

function Channel_BandWidth_onClick()
{
	var form_basic = document.wireless_basic;
	var w_mode = form_basic.wirelessmode.value;

	if (form_basic.n_bandwidth[0].checked)
	{
		hideElement("extension_channel");
		form_basic.n_extcha.disabled = true;
		if ((1*w_mode == 6) || (1*w_mode == 7))
			Check5GBandChannelException();
	}
	else
	{
		showElementEx("extension_channel", style_display_on());
		form_basic.n_extcha.disabled = false;

		if ((1*w_mode == 6) || (1*w_mode == 7))
		{
			Check5GBandChannelException();

			if (form_basic.sz11aChannel.options.selectedIndex == 0)
			{
				hideElement("extension_channel");
				form_basic.n_extcha.disabled = true;
			}
		}
	}
}

function Check5GBandChannelException()
{
	var form_basic = document.wireless_basic;
	var w_mode = form_basic.wirelessmode.value;

	if ((1*w_mode == 6) || (1*w_mode == 7))
	{
		var x                   = document.getElementById("sz11aChannel")
		var current_length      = form_basic.sz11aChannel.options.length;
		var current_index       = form_basic.sz11aChannel.options.selectedIndex;
		var current_channel     = form_basic.sz11aChannel.value;
		 
		if (1*current_index == 0)
		{
			if (1*channel_index != 0)
				current_index = 1;
		}

		for (ch_idx = current_length - 1; ch_idx > 0; ch_idx--)
		{
			x.remove(ch_idx);
		}

		if (document.wireless_basic.n_bandwidth[1].checked == true)
		{
			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
			{
				for(ch = 36; ch <= 48; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
			{
				for(ch = 52; ch <= 64; ch+=4)
					insertChannelOption(ch, 5);
			}

			if (countrycode == 'NONE')
			{
				for(ch = 100; ch <= 136; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
				for(ch = 149; ch <= 161; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((1*current_channel == 140) || (1*current_channel == 165))
			{
				form_basic.sz11aChannel.options.selectedIndex = (1*current_index) -1;
			}
			else
			{
				form_basic.sz11aChannel.options.selectedIndex = (1*current_index);
			}
		}
		else
		{
			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
			{
				for(ch = 36; ch <= 48; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
			{
				for(ch = 52; ch <= 64; ch+=4)
					insertChannelOption(ch, 5);
			}

			if (countrycode == 'NONE')
			{
				for(ch = 100; ch <= 140; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
				for(ch = 149; ch <= 161; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
					insertChannelOption(165, 5);
			}

			form_basic.sz11aChannel.options.selectedIndex = (1*current_index);
		}
	}
	else if (1*w_mode == 5)
	{
		var x = document.getElementById("sz11aChannel")
		var current_length = form_basic.sz11aChannel.options.length;
		var current_index = form_basic.sz11aChannel.options.selectedIndex;

		for (ch_idx = current_length - 1; ch_idx > 0; ch_idx--)
		{
			x.remove(ch_idx);
		}

		if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
			(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
		{
			for(ch = 36; ch <= 48; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
			(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
		{
			for(ch = 52; ch <= 64; ch+=4)
				insertChannelOption(ch, 5);
		}

		if (countrycode == 'NONE')
		{
			for(ch = 100; ch <= 140; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
			(countrycode == 'CN') || (countrycode == 'HK'))
		{
			for(ch = 149; ch <= 161; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'US') ||
			(countrycode == 'CN') || (countrycode == 'HK'))
		{
				insertChannelOption(165, 5);
		}

		form_basic.sz11aChannel.options.selectedIndex = (1*current_index);
	}
}

function initTranslation()
{
	_TR("basicTitle", "basic title");
	_TR("basicIntroduction", "basic introduction");

	_TR("basicWirelessNet", "basic wireless network");
	_TR("basicRadioButton", "basic radio button");
	_TR("basicNetMode", "basic network mode");
	_TR("basicSSID", "basic ssid");
	_TR("basicHSSID0", "basic hssid");
	_TR("basicHSSID1", "basic hssid");
	_TR("basicHSSID2", "basic hssid");
	_TR("basicHSSID3", "basic hssid");
	_TR("basicHSSID4", "basic hssid");
	_TR("basicHSSID5", "basic hssid");
	_TR("basicHSSID6", "basic hssid");
	_TR("basicHSSID7", "basic hssid");
	_TR("basicMSSID1", "basic multiple ssid");
	_TR("basicMSSID2", "basic multiple ssid");
	_TR("basicMSSID3", "basic multiple ssid");
	_TR("basicMSSID4", "basic multiple ssid");
	_TR("basicMSSID5", "basic multiple ssid");
	_TR("basicMSSID6", "basic multiple ssid");
	_TR("basicMSSID7", "basic multiple ssid");
	_TR("basicBroadcastSSIDEnable", "wireless enable");
	_TR("basicBroadcastSSIDDisable", "wireless disable");
	_TR("basicBroadcastSSID", "basic broadcast ssid");
	_TR("basicApIsolatedEnable", "wireless enable");
	_TR("basicApIsolatedDisable", "wireless disable");
	_TR("basicApIsolated", "basic apisolated");
	_TR("basicIsolatedSSID0", "basic isolated");
	_TR("basicIsolatedSSID1", "basic isolated");
	_TR("basicIsolatedSSID2", "basic isolated");
	_TR("basicIsolatedSSID3", "basic isolated");
	_TR("basicIsolatedSSID4", "basic isolated");
	_TR("basicIsolatedSSID5", "basic isolated");
	_TR("basicIsolatedSSID6", "basic isolated");
	_TR("basicIsolatedSSID7", "basic isolated");
	_TR("basicMBSSIDApIsolatedEnable", "wireless enable");
	_TR("basicMBSSIDApIsolatedDisable", "wireless disable");
	_TR("basicMBSSIDApIsolated", "basic mbssidapisolated");
	_TR("basicBSSID", "basic bssid");
	_TR("basicFreqA", "basic frequency");
	_TR("basicFreqAAuto", "basic frequency auto");
	_TR("basicFreqB", "basic frequency");
	_TR("basicFreqBAuto", "basic frequency auto");
	_TR("basicFreqG", "basic frequency");
	_TR("basicFreqGAuto", "basic frequency auto");
	_TR("basicRate", "basic rate");

	_TR("basicHTPhyMode", "basic ht phy mode");
	_TR("basicHTOPMode", "basic ht op mode");
	_TR("basicHTMixed", "basic ht op mixed");
	_TR("basicHTChannelBW", "basic ht channel bandwidth");
	_TR("basicHTGI", "basic ht guard interval");
	_TR("basicHTLongGI", "wireless long");
	_TR("basicHTAutoGI", "wireless auto");
	_TR("basicHTAutoMCS", "wireless auto");
	_TR("basicHTRDG", "basic ht rdg");
	_TR("basicHTRDGDisable", "wireless disable");
	_TR("basicHTRDGEnable", "wireless enable");
	_TR("basicHTExtChannel", "basic ht extension channel");
	_TR("basicHTAMSDU", "basic ht amsdu");
	_TR("basicHTAMSDUDisable", "wireless disable");
	_TR("basicHTAMSDUEnable", "wireless enable");
	_TR("basicHTAddBA", "basic ht addba");
	_TR("basicHTAddBADisable", "wireless disable");
	_TR("basicHTAddBAEnable", "wireless enable");
	_TR("basicHTDelBA", "basic ht delba");
	_TR("basicHTDelBADisable", "wireless disable");
	_TR("basicHTDelBAEnable", "wireless enable");

	_TR("basicOther", "basic other");
	_TR("basicHTTxStream", "basic ht txstream");
	_TR("basicHTRxStream", "basic ht rxstream");

	_TRV("basicApply", "wireless apply");
	_TRV("basicCancel", "wireless cancel");
}

var channel_list = [ 'sz11aChannel', 'sz11bChannel', 'sz11gChannel'];
var channels = [];

function saveRadioChannels()
{
	// Store all comboBox options in arrays
	for (var i=0; i<channel_list.length; i++)
	{
		var id = channel_list[i];
		var select = document.getElementById(id);
		var options = [];
		
		if (select != null)
		{
			for (var j=0; j<select.options.length; j++)
				options[j] = select.options[j];
			channels[id] = options;
		}
	}
}

function show14channel(show)
{
	// Display all options except 14 frequency
	for (var i=0; i<channel_list.length; i++)
	{
		var id = channel_list[i];
		var select = document.getElementById(id);
		var options = channels[id];
		
		if ((select != null) && (options != null))
		{
			// Store selected option
			var index = select.selectedIndex;
			
			// remove all options
			select.options.length = 0;
			
			for (var j=0; j<options.length; j++)
			{
				if ((options[j].value != '14') || (show))
					select.options[select.options.length] = options[j];
			}
			
			// Restore selected option
			select.selectedIndex = (index >= select.options.length) ? 0 : index;
		}
	}
}

function initValue()
{
	var ssidArray;
	var HiddenSSIDArray;
	var channel_11a_index;
	var current_channel_length;
	var radio_off   = "<% getCfgZero(1, "RadioOff"); %>";

	saveRadioChannels();
	initTranslation();
	if (countrycode == '')
		countrycode = 'NONE';
	
	var form                  = document.wireless_basic;
	form.wirelessmode.value   = "<% getCfgZero(1, "WirelessMode"); %>";
	form.radioButton.value    = (radio_off == "1") ? "RADIO ON" : "RADIO OFF";

	var wmode = form.wirelessmode.value;

	// Hide & disable elements
	hideElement("div_11a_channel");
	hideElement("div_11b_channel");
	hideElement("div_11g_channel");
	hideElement("div_ht_tx_stream");
	hideElement("div_ht_rx_stream");
	hideElement("div_abg_rate");
	hideElement("div_11n");

	form.sz11aChannel.disabled = true;
	form.sz11bChannel.disabled = true;
	form.sz11gChannel.disabled = true;
	form.abg_rate.disabled = true;
	form.n_mode.disabled = true;
	form.n_bandwidth.disabled = true;
	form.n_rdg.disabled = true;
	form.n_gi.disabled = true;
	form.n_mcs.disabled = true;
	
	hideElement("div_mbssidapisolated");
	form.mbssidapisolated.disabled = true;
	show14channel(true);
	
	// Wireless mode
	if ((wmode*1) >= 6)
	{
		showElement("div_11n");
		form.n_mode.disabled = false;
		form.n_bandwidth.disabled = false;
		form.n_rdg.disabled = false;
		form.n_gi.disabled = false;
		form.n_mcs.disabled = false;
	}
	
	var rfic = '<% getCfgGeneral(1, "RFICType"); %>';
	if ((rfic == "2") || (rfic == "4"))
	{
		index = form.wirelessmode.options.length;
		form.wirelessmode.options[index++] = new Option("11a only", "2");
		form.wirelessmode.options[index++] = new Option("11a/n mixed mode", "8");
		form.wirelessmode.options[index++] = new Option("11n only(5G)", "11");
	}
	
	if ((wmode == "0") || (wmode == "4") || (wmode == "9") || (wmode == "6") || (wmode=="7"))
	{
		form.sz11gChannel.disabled = false;
		showElementEx("div_11g_channel", style_display_on());

		if ((1*wmode) >= 5)
		{
			showElementEx("div_ht_tx_stream", style_display_on());
			showElementEx("div_ht_rx_stream", style_display_on());
			show14channel(false);
		}
	}
	else if (wmode == "1")
	{
		form.sz11bChannel.disabled = false;
		showElementEx("div_11b_channel", style_display_on());
	}
	else if ((wmode == "2") || (wmode == "8") || (wmode == "11"))
	{
		form.sz11aChannel.disabled = false;
		showElementEx("div_11a_channel", style_display_on());
	}
	
	for (i=1; i<8; i++)
	{
		hideElement("div_hssid" + i);
		form.elements["mssid_" + i].disabled = true;
		form.hssid[i].disabled = true;
	}
	
	var mssidb      = "<% getMBSSIDBuilt(); %>";
	if (mssidb == "1")
	{
		for (i=1; i<8; i++)
		{
			showElementEx("div_hssid" + i, style_display_on());
			form.elements["mssid_" + i].disabled = false;
			form.hssid[i].disabled = false;
		}
	}
	
	var HiddenSSID  = '<% getCfgZero(1, "HideSSID"); %>';
	var HiddenSSIDArray = HiddenSSID.split(";");

	form.broadcastssid[ (HiddenSSID.indexOf("0") >= 0) ? 1 : 0 ].checked = true;
	for (i=0; i<8; i++)
		form.hssid[i].checked = (HiddenSSIDArray[i] == "1");

	var APIsolated = '<% getCfgZero(1, "NoForwarding"); %>';
	var APIsolatedArray = APIsolated.split(";");

	form.apisolated[ (APIsolated.indexOf("1") >= 0) ? 1 : 0 ].checked = true;
	for (i=0; i<8; i++)
		form.isolated_ssid[i].checked = (APIsolatedArray[i] == "1");
	
	var ht_bw = '<% getCfgZero(1, "HT_BW"); %>';
	if (1*ht_bw == 0)
	{
		form.n_bandwidth[0].checked = true;
		form.n_extcha.disabled      = true;
		hideElement("extension_channel");
	}
	else
	{
		form.n_bandwidth[1].checked = true;
		form.n_extcha.disabled      = false;
		showElementEx("extension_channel", style_display_on());
	}
	
	channel_index = 1*channel_index;

	if ((wmode == "0") || (wmode == "4") || (wmode == "9") || (wmode == "6"))
	{
		form.sz11gChannel.options.selectedIndex = channel_index;
		current_channel_length = form.sz11gChannel.options.length;

		if ((channel_index + 1) > current_channel_length)
			form.sz11gChannel.options.selectedIndex = 0;
	}
	else if (wmode == "1")
	{
		form.sz11bChannel.options.selectedIndex = channel_index;
		current_channel_length = form.sz11bChannel.options.length;

		if ((channel_index + 1) > current_channel_length)
			form.sz11bChannel.options.selectedIndex = 0;
	}
	else if ((wmode == "2") || (wmode == "8") || (wmode == "11"))
	{
		if (countrycode == 'NONE')
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else if ((channel_index >= 100) && (channel_index <= 140))
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 16;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 17;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if ((countrycode == 'US') || (countrycode == 'HK') || (countrycode == 'FR') || (countrycode == 'IE'))
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 28;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'JP')
		{
			if (channel_index <= 48)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'TW')
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 12;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 32;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'CN')
		{
			if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 36;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else
		{
			channel_11a_index = 0;
		}

		Check5GBandChannelException();

		if (channel_index > 0)
			form.sz11aChannel.options.selectedIndex = channel_11a_index;
		else
			form.sz11aChannel.options.selectedIndex = channel_index;
	}
	
	show_abg_rate(form);

	if (mssidb == "1")
	{
		showElementEx("div_mbssidapisolated", style_display_on());
		form.mbssidapisolated.disabled = false;
		form.mbssidapisolated[ (mbssidapisolated == "1") ? 1 : 0 ].checked = true;
	}
	
	insertExtChannelOption();

	if (ht_mode == "1")
		form.n_mode[1].checked = true;
	else if (ht_mode == "2")
		form.n_mode[2].checked = true;
	else
		form.n_mode[0].checked = true;

	if (ht_gi == "0")
		form.n_gi[0].checked = true;
	else
		form.n_gi[1].checked = true;

	if (is3t3r == "1")
	{
		for (i = 16; i < 24; i++)
			form.n_mcs.options[i] = new Option(i, i);
	}
	
	var mcs_length = form.n_mcs.options.length;
	if (1*is3t3r == 1)
	{
		form.n_mcs.options[mcs_length] = new Option("32", "32");
		mcs_length++;
		form.n_mcs.options[mcs_length] = new Option("Auto", "33");
	}

	if (1*ht_mcs <= mcs_length-1)
		form.n_mcs.options.selectedIndex = ht_mcs;
	else if (1*ht_mcs == 32)
		form.n_mcs.options.selectedIndex = mcs_length-2;
	else if (1*ht_mcs == 33)
		form.n_mcs.options.selectedIndex = mcs_length-1;

	form.n_rdg[(ht_rdg == "0") ? 0 : 1].checked = true;

	var option_length = form.n_extcha.options.length;

	if (1*ht_extcha == 0)
	{
		if (option_length > 1)
			form.n_extcha.options.selectedIndex = 0;
	}
	else if (1*ht_extcha == 1)
	{
		if (option_length > 1)
			form.n_extcha.options.selectedIndex = 1;
	}
	else
	{
		form.n_extcha.options.selectedIndex = 0;
	}
	
	if ((wmode == "8") || (wmode == "11"))
	{
		if (form.sz11aChannel.options.selectedIndex == 0)
		{
			hideElement("extension_channel");
			form.n_extcha.disabled = true;
		}
	}
	else if ((wmode == "9") || (wmode == "6"))
	{
		if (form.sz11gChannel.options.selectedIndex == 0)
		{
			hideElement("extension_channel");
			form.n_extcha.disabled = true;
		}
	}
	
	form.n_amsdu [ (ht_amsdu ==  "0") ? 0 : 1 ].checked = true;
	form.n_autoba[ (ht_autoba == "0") ? 0 : 1 ].checked = true;
	form.n_badecline[ (ht_badecline == "0") ? 0 : 1].checked = true;

	if (apcli_include == "1")
		form.mssid_7.disabled = true;

	if (is3t3r == "1")
	{
		form.rx_stream.options[2] = new Option("3", "3");
		form.tx_stream.options[2] = new Option("3", "3");
	}

	var txpath = '<% getCfgGeneral(1, "TXPath"); %>';
	var rxpath = '<% getCfgGeneral(1, "RXPath"); %>';
	if (txpath == "1")
	{
		hideElement("div_HtTx2Stream");
		tx_stream_idx = 1;
	}
	else
		showElementEx("div_HtTx2Stream", style_display_on());

	if (rxpath == "1")
	{
		hideElement("div_HtRx2Stream");
		rx_stream_idx = 1;
	}
	else
	{
		showElementEx("div_HtRx2Stream", style_display_on());
	}
	form.rx_stream.options.selectedIndex = rx_stream_idx - 1;
	form.tx_stream.options.selectedIndex = tx_stream_idx - 1;
}

function show_abg_rate(form)
{
	var wmode = form.wirelessmode.value;

	hideElement("div_abg_rate");
	form.abg_rate.disabled = true;

	//ABG Rate
	if ((wmode == "0") || (wmode == "2") || (wmode == "4"))
	{
		ht_mcs = 1*ht_mcs;
		var abg_rate = form.abg_rate;
		
		abg_rate.options.length = 0;
		abg_rate.options[0] = new Option("Auto", "0");
		abg_rate.options[1] = new Option("1 Mbps", "1");
		abg_rate.options[2] = new Option("2 Mbps", "2");
		abg_rate.options[3] = new Option("5.5 Mbps", "5");
		abg_rate.options[4] = new Option("6 Mbps", "6");
		abg_rate.options[5] = new Option("9 Mbps", "9");
		abg_rate.options[6] = new Option("11 Mbps", "11");
		abg_rate.options[7] = new Option("12 Mbps", "12");
		abg_rate.options[8] = new Option("18 Mbps", "18");
		abg_rate.options[9] = new Option("24 Mbps", "24");
		abg_rate.options[10] = new Option("36 Mbps", "36");
		abg_rate.options[11] = new Option("48 Mbps", "48");
		abg_rate.options[12] = new Option("54 Mbps", "54");
		
		if (fxtxmode == "CCK" || fxtxmode == "cck")
		{
			if (ht_mcs == 0)
				abg_rate.value = "1";
			else if (ht_mcs == 1)
				abg_rate.value = "2";
			else if (ht_mcs == 2)
				abg_rate.value = "5";
			else if (ht_mcs == 3)
				abg_rate.value = "11";
			else
				abg_rate.value = "0";
		}
		else
		{
			if (ht_mcs == 0)
				abg_rate.value = "6";
			else if (ht_mcs == 1)
				abg_rate.value = "9";
			else if (ht_mcs == 2)
				abg_rate.value = "12";
			else if (ht_mcs == 3)
				abg_rate.value = "18";
			else if (ht_mcs == 4)
				abg_rate.value = "24";
			else if (ht_mcs == 5)
				abg_rate.value = "36";
			else if (ht_mcs == 6)
				abg_rate.value = "48";
			else if (ht_mcs == 7)
				abg_rate.value = "54";
			else
				abg_rate.value = "0";
		}
	}
	else if (wmode == "1")
	{
		ht_mcs = 1*ht_mcs;
		var abg_rate = form.abg_rate;
		
		abg_rate.options.length = 0;
		abg_rate.options[0] = new Option("Auto", "0");
		abg_rate.options[1] = new Option("1 Mbps", "1");
		abg_rate.options[2] = new Option("2 Mbps", "2");
		abg_rate.options[3] = new Option("5.5 Mbps", "5");
		abg_rate.options[4] = new Option("11 Mbps", "11");
		
		if (ht_mcs == 0)
			abg_rate.value = "1";
		else if (ht_mcs == 1)
			abg_rate.value = "2";
		else if (ht_mcs == 2)
			abg_rate.value = "5";
		else if (ht_mcs == 3)
			abg_rate.value = "11";
		else
			abg_rate.value = "0";
	}
	else
	{
		hideElement("div_abg_rate");
		form.abg_rate.disabled = true;
	}
}

function wirelessModeChange(form)
{
	hideElement("div_11a_channel");
	hideElement("div_11b_channel");
	hideElement("div_11g_channel");
	hideElement("div_ht_tx_stream");
	hideElement("div_ht_rx_stream");
	hideElement("div_abg_rate");
	hideElement("div_11n");
	show14channel(true);

	form.sz11aChannel.disabled = true;
	form.sz11bChannel.disabled = true;
	form.sz11gChannel.disabled = true;
	form.abg_rate.disabled = true;
	form.n_mode.disabled = true;
	form.n_bandwidth.disabled = true;
	form.n_rdg.disabled = true;
	form.n_gi.disabled = true;
	form.n_mcs.disabled = true;
	
	// Hide & disable elements
	var wmode = form.wirelessmode.value;

	if ((wmode == "8") || (wmode == "9") || (wmode == "6") || (wmode == "7"))
	{
		showElement("div_11n");
		form.n_mode.disabled = false;
		form.n_bandwidth.disabled = false;
		form.n_rdg.disabled = false;
		form.n_gi.disabled = false;
		form.n_mcs.disabled = false;
	}

	if ((wmode == "0") || (wmode == "4") || (wmode == "9") || (wmode == "6") || (wmode == "7"))
	{
		form.sz11gChannel.disabled = false;
		showElementEx("div_11g_channel", style_display_on());
		
		if ((1*wmode) >= 5)
		{
			showElementEx("div_ht_tx_stream", style_display_on());
			showElementEx("div_ht_rx_stream", style_display_on());
			show14channel(false);
		}
		
		displayElement("extension_channel", form.sz11gChannel.options.selectedIndex != 0);

		insertExtChannelOption();
	}
	else if (wmode == "1")
	{
		form.sz11bChannel.disabled = false;
		showElementEx("div_11b_channel", style_display_on());
	}
	else if ((wmode == "2") || (wmode == "8") || (wmode == "11"))
	{
		form.sz11aChannel.disabled = false;
		showElementEx("div_11a_channel", style_display_on());

		Check5GBandChannelException();
		if (form.sz11aChannel.options.selectedIndex == 0)
		{
			hideElement("extension_channel");
			form.n_extcha.disabled = true;
		}

		insertExtChannelOption();
	}
	
	show_abg_rate(form);
}

function switch_hidden_ssid()
{
	for (i=0; i<8; i++)
		document.wireless_basic.hssid[i].checked = document.wireless_basic.broadcastssid[0].checked;
}

function switch_isolated_ssid()
{
	for (i=0; i<8; i++)
		document.wireless_basic.isolated_ssid[i].checked = document.wireless_basic.apisolated[1].checked;
}

function CheckValue()
{
	var wireless_mode;
	var submit_ssid_num;
	var channel_11a_index;
	var form_basic = document.wireless_basic;

	if (form_basic.ssid.value == "")
	{
		alert("Please enter SSID!");
		form_basic.ssid.focus();
		form_basic.ssid.select();
		return false;
	}

	submit_ssid_num = 1;

	for (i = 1; i < 8; i++)
	{
		//if (eval("document.wireless_basic.mssid_"+i).value != "")
		if (form_basic.elements["mssid_"+i].value != "")
		{
			if (i == 7)
			{
				if (1*apcli_include == 0)
					submit_ssid_num++;
			}
			else
				submit_ssid_num++;
		}
	}

	form_basic.bssid_num.value = submit_ssid_num;
	return true;
}

function doRadioStatusChange(form)
{
	if (form.radioButton.value.indexOf('OFF') >= 0)
	{
		form.radioButton.value = "RADIO OFF";
		form.radiohiddenButton.value = 0;
	}
	else
	{
		form.radioButton.value = "RADIO ON";
		form.radiohiddenButton.value = 1;
	}
	
	form.submit();
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="basicTitle">Basic Wireless Settings </h1>
<p id="basicIntroduction"> Here you can configure the most basic settings of Wireless communication, such as Network Name (SSID) and Channel. These settings are sufficient to have a working Access Point. </p>
<hr>

<form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return CheckValue();">
<table width="600" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr> 
	<td class="title" colspan="2" id="basicWirelessNet">Wireless Network</td>
</tr>
<tr> 
	<td class="head" id="basicRadioButton">Radio On/Off</td>
	<td>
		<input type="button" name="radioButton" style="{width:120px;}" value="RADIO ON" onClick="doRadioStatusChange(this.form);">
		<input type="hidden" name="radiohiddenButton" value="2">
	</td>
</tr>
<tr> 
	<td class="head" id="basicNetMode">Network Mode</td>
	<td>
		<select name="wirelessmode" id="wirelessmode" size="1" onChange="wirelessModeChange(this.form);">
			<option value="0">11b/g mixed mode</option>
			<option value="1">11b only</option>
			<option value="4">11g only</option>
			<option value="7">11g/n mixed mode</option>
			<option value="9">11b/g/n mixed mode</option>
			<option value="6">11n only</option>
		</select>
	<!--
		<select name="wirelessmode" id="wirelessmode" size="1" onChange="wirelessModeChange(this.form);">
			<option value="0">802.11 b/g mixed mode</option>
			<option value="1">802.11 b only</option>
			<option value="2">802.11 a only</option>
			<option value="3">802.11 a/b/g mixed mode</option>
			<option value="4">802.11 g</option>
			<option value="5">802.11 a/b/g/n mixed mode</option>
			<option value="6">802.11 n only</option>
			<option value="7">802.11 g/n mixed mode</option>
			<option value="8">802.11 a/n mixed mode</option>
			<option value="9">802.11 b/g/n mixed mode</option>
		</select>
	-->
	</td>
</tr>
<input type="hidden" name="bssid_num" value="1">

<tr> 
	<td class="head" id="basicSSID">Network Name (SSID)</td>
	<td>
		<input class="normal" type="text" name="ssid" maxlength="32" value="<% getCfgGeneral(1, "SSID1"); %>">
		<font id="basicHSSID0">Hidden</font><input type="checkbox" name="hssid" value="0">&nbsp;
		<font id="basicIsolatedSSID0">Isolated</font><input type="checkbox" name="isolated_ssid" value="0">
	</td>
</tr>
<tr id="div_hssid1"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID1">Multiple SSID</font>1</td>
	<td>
		<input class="normal" name="mssid_1" maxlength="32" value="<% getCfgGeneral(1, "SSID2"); %>">
		<font id="basicHSSID1">Hidden</font><input type=checkbox name=hssid value="1">&nbsp;
		<font id="basicIsolatedSSID1">Isolated</font><input type="checkbox" name="isolated_ssid" value="1">
	</td>
</tr>
<tr id="div_hssid2"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID2">Multiple SSID</font>2</td>
	<td>
		<input class="normal" type="text" name="mssid_2" maxlength="32" value="<% getCfgGeneral(1, "SSID3"); %>">
		<font id="basicHSSID2">Hidden</font><input type="checkbox" name="hssid" value="2">&nbsp;
		<font id="basicIsolatedSSID2">Isolated</font><input type="checkbox" name="isolated_ssid" value="2">
	</td>
</tr>
<tr id="div_hssid3"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID3">Multiple SSID</font>3</td>
	<td>
		<input class="normal" type="text" name="mssid_3" maxlength="32" value="<% getCfgGeneral(1, "SSID4"); %>">
		<font id="basicHSSID3">Hidden</font><input type="checkbox" name="hssid" value="3">&nbsp;
		<font id="basicIsolatedSSID3">Isolated</font><input type="checkbox" name="isolated_ssid" value="3">
	</td>
</tr>
<tr id="div_hssid4"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID4">Multiple SSID</font>4</td>
	<td>
		<input class="normal" type="text" name="mssid_4" maxlength="32" value="<% getCfgGeneral(1, "SSID5"); %>">
		<font id="basicHSSID4">Hidden</font><input type="checkbox" name="hssid" value="4">&nbsp;
		<font id="basicIsolatedSSID4">Isolated</font><input type="checkbox" name="isolated_ssid" value="4">
	</td>
</tr>
<tr id="div_hssid5"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID5">Multiple SSID</font>5</td>
	<td>
		<input class="normal" type="text" name="mssid_5" maxlength="32" value="<% getCfgGeneral(1, "SSID6"); %>">
		<font id="basicHSSID5">Hidden</font><input type="checkbox" name="hssid" value="5">&nbsp;
		<font id="basicIsolatedSSID5">Isolated</font><input type="checkbox" name="isolated_ssid" value="5">
	</td>
</tr>
<tr id="div_hssid6"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID6">Multiple SSID</font>6</td>
	<td>
		<input class="normal" type="text" name="mssid_6" maxlength="32" value="<% getCfgGeneral(1, "SSID7"); %>">
		<font id="basicHSSID6">Hidden</font><input type="checkbox" name="hssid" value="6">&nbsp;
		<font id="basicIsolatedSSID6">Isolated</font><input type=checkbox name=isolated_ssid value="6">
	</td>
</tr>
<tr id="div_hssid7"> 
	<td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID7">Multiple SSID</font>7</td>
	<td>
		<input class="normal" type="text" name="mssid_7" maxlength="32" value="<% getCfgGeneral(1, "SSID8"); %>">
		<font id="basicHSSID7">Hidden</font><input type="checkbox" name="hssid" value="7">&nbsp;
		<font id="basicIsolatedSSID7">Isolated</font><input type="checkbox" name="isolated_ssid" value="7">
	</td>
</tr>
<tr> 
	<td class="head" id="basicBroadcastSSID">Broadcast Network Name (SSID)</td>
	<td>
		<input type="radio" name="broadcastssid" value="0" onClick="switch_hidden_ssid()"><font id="basicBroadcastSSIDDisable">Disable&nbsp;</font>
		<input type="radio" name="broadcastssid" value="1" checked onClick="switch_hidden_ssid()"><font id="basicBroadcastSSIDEnable">Enable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="basicApIsolated">AP Isolation</td>
	<td>
		<input type="radio" name="apisolated" value="0" checked onClick="switch_isolated_ssid()"><font id="basicApIsolatedDisable">Disable&nbsp;</font>
		<input type="radio" name="apisolated" value="1" onClick="switch_isolated_ssid()"><font id="basicApIsolatedEnable">Enable</font>
	</td>
</tr>
<tr id="div_mbssidapisolated"> 
	<td class="head" id="basicMBSSIDApIsolated">MBSSID AP Isolation</td>
	<td>
		<input type="radio" name="mbssidapisolated" value="0" checked><font id="basicMBSSIDApIsolatedDisable">Disable&nbsp;</font>
		<input type="radio" name="mbssidapisolated" value="1"><font id="basicMBSSIDApIsolatedEnable">Enable</font>
	</td>
</tr>
<tr> 
	<td class="head" id="basicBSSID">BSSID</td>
	<td>&nbsp;&nbsp;<% getWlanCurrentMac(); %></td>
</tr>
<tr id="div_11a_channel" name="div_11a_channel">
	<td class="head"><font id="basicFreqA">Frequency (Channel)</font></td>
	<td>
		<select id="sz11aChannel" name="sz11aChannel" size="1" onChange="ChannelOnChange()">
			<option value="0" id="basicFreqAAuto">AutoSelect</option>
			<% getWlan11aChannels(); %>
		</select>
	</td>
</tr>
<tr id="div_11b_channel" name="div_11b_channel">
	<td class="head"><font id="basicFreqB">Frequency (Channel)</font></td>
	<td>
		<select id="sz11bChannel" name="sz11bChannel" size="1" onChange="ChannelOnChange()">
			<option value="0" id="basicFreqBAuto">AutoSelect</option>
			<% getWlan11bChannels(); %>
		</select>
	</td>
</tr>
<tr id="div_11g_channel" name="div_11g_channel">
	<td class="head"><font id="basicFreqG">Frequency (Channel)</font></td>
	<td>
		<select id="sz11gChannel" name="sz11gChannel" size="1" onChange="ChannelOnChange()">
			<option value="0" id="basicFreqGAuto">AutoSelect</option>
			<% getWlan11gChannels(); %>
		</select>
	</td>
</tr>
<tr id="div_abg_rate">
	<td class="head"><font id="basicRate">Rate</font></td>
	<td>
		<select name="abg_rate" size="1">
		</select>
	</td>
</tr>
<tr id="div_ht_tx_stream">
	<td class="head" id="basicHTTxStream">HT TxStream</td>
	<td>
		<select name="tx_stream" size="1">
			<option value="1">1</option>
			<option value="2" id="div_HtTx2Stream">2</option>
		</select>
	</td>
</tr>
<tr id="div_ht_rx_stream">
	<td class="head" id="basicHTRxStream">HT RxStream</td>
	<td>
		<select name="rx_stream" size="1">
			<option value="1">1</option>
			<option value="2" id="div_HtRx2Stream">2</option>
		</select>
	</td>
</tr>
</table>

<table id="div_11n" name="div_11n" width="600" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD" style="display:none">
<tr> 
	<td class="title" colspan="2" id="basicHTPhyMode">HT Physical Mode</td>
</tr>
<tr>
	<td class="head" id="basicHTOPMode">Operating Mode</td>
	<td>
		<input type="radio" name="n_mode" value="0" checked><font id="basicHTMixed">Mixed Mode&nbsp;</font>
		<input type="radio" name="n_mode" value="1">Green Field
	</td>
</tr>
<tr>
	<td class="head" id="basicHTChannelBW">Channel BandWidth</td>
	<td>
		<input type="radio" name="n_bandwidth" value="0" onClick="Channel_BandWidth_onClick()" checked>20&nbsp;
		<input type="radio" name="n_bandwidth" value="1" onClick="Channel_BandWidth_onClick()">20/40
	</td>
</tr>
<tr>
	<td class="head" id="basicHTGI">Guard Interval</td>
	<td>
		<input type="radio" name="n_gi" value="0" checked><font id="basicHTLongGI">Long&nbsp;</font>
		<input type="radio" name="n_gi" value="1"><font id="basicHTAutoGI">Auto</font>
	</td>
</tr>
<tr>
	<td class="head">MCS</td>
	<td>
		<select name="n_mcs" size="1">
			<option value = "0">0</option>
			<option value = "1">1</option>
			<option value = "2">2</option>
			<option value = "3">3</option>
			<option value = "4">4</option>
			<option value = "5">5</option>
			<option value = "6">6</option>
			<option value = "7">7</option>
			<option value = "8">8</option>
			<option value = "9">9</option>
			<option value = "10">10</option>
			<option value = "11">11</option>
			<option value = "12">12</option>
			<option value = "13">13</option>
			<option value = "14">14</option>
			<option value = "15">15</option>
			<option value = "32">32</option>
			<option value = "33" selected id="basicHTAutoMCS">Auto</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="basicHTRDG">Reverse Direction Grant(RDG)</td>
	<td>
		<input type="radio" name="n_rdg" value="0" checked><font id="basicHTRDGDisable">Disable&nbsp;</font>
		<input type="radio" name="n_rdg" value="1"><font id="basicHTRDGEnable">Enable</font>
	</td>
</tr>
<tr name="extension_channel" id="extension_channel">
	<td class="head" id="basicHTExtChannel">Extension Channel</td>
	<td>
		<select id="n_extcha" name="n_extcha" size="1">
			<option value="1" selected>2412MHz (Channel 1)</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="basicHTAMSDU">Aggregation MSDU (A-MSDU)</td>
	<td>
		<input type="radio" name="n_amsdu" value="0" checked><font id="basicHTAMSDUDisable">Disable&nbsp;</font>
		<input type="radio" name="n_amsdu" value="1"><font id="basicHTAMSDUEnable">Enable</font>
	</td>
</tr>
<tr>
	<td class="head" id="basicHTAddBA">Auto Block ACK</td>
	<td>
		<input type="radio" name="n_autoba" value="0" checked><font id="basicHTAddBADisable">Disable&nbsp;</font>
		<input type="radio" name="n_autoba" value="1"><font id="basicHTAddBAEnable">Enable</font>
	</td>
</tr>
<tr>
	<td class="head" id="basicHTDelBA">Decline BA Request</td>
	<td>
		<input type="radio" name="n_badecline" value="0" checked><font id="basicHTDelBADisable">Disable&nbsp;</font>
		<input type="radio" name="n_badecline" value="1"><font id="basicHTDelBAEnable">Enable</font>
	</td>
</tr>
</table>

<br>

<table width = "600" border = "0" cellpadding = "2" cellspacing = "1">
<tr align="center">
	<td>
		<input type="submit" class="half" value="Apply" id="basicApply">&nbsp;&nbsp;
		<input type="button" class="half" value="Cancel" id="basicCancel" onClick="window.location.reload();">
	</td>
</tr>
</table>

</form>

</td></tr></table>
</body>
</html>
