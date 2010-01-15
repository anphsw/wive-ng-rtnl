﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#AP_survey#></title>

<link rel="stylesheet" type="text/css" href="qis/qis_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<link rel="stylesheet" type="text/css" href="survey.css">
<link rel="stylesheet" type="text/css" href="index_style.css">
<style type="text/css">
</style>
<script type="text/JavaScript" src="/jquery.js"></script>
<script type="text/javascript" src="/alttxt.js"></script>
<script type="text/JavaScript" src="/validation.js"></script>
<script type="text/javascript" src="/tablesorter.js"></script>
<!--script type="text/javascript" src="/ui.core.js"></script>
<script type="text/javascript" src="/ui.draggable.js"></script-->
<script>

var ureIP = <% nvram_dump("urelease",""); %>;
var router_ip = "";

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];

function initial(){

	if(isWLclient()){
		$("#LoadingBox").hide();
		$("#warningBox").show();
		return false;
	}
	show_prev_btn();
	$("#LoadingBox").show();
	setTimeout("ajaxGetData();",1000)
	//loadDragPanel("wl_key_table");
	//loadDragPanel("connecting_table");
}

function isWLclient(){  //detect login client is by wireless or wired
	if(wireless.length > 0){
		for(var i=0; i < wireless.length; i++){
			if(wireless[i][0] == login_mac_str())
				return true;  //wireless
		}
	}
	return false; //wired
}

function ajaxGetData(e) { //Use AJAX to get real-time AP status from aplist.asp

	$("#content_mask").show();
	$("#LoadingBox_mask").show();
	$("#LoadingBox").show();
	
	document.form.connect_btn.disabled = true;
	document.form.refresh_btn.disabled = true;

  $.ajax({
    url: 'aplist.asp',
    dataType: 'script', 
	
    error: function(xhr) {
      alert('<#ajax_disconnect#>');
    },
    success: function(response) {
      
      showList();
	  	bind_event();

 			//Load completed, hide mask and loadingbar
			$("#content_mask").hide();
			$("#LoadingBox_mask").hide();
			$("#LoadingBox").hide();
			document.form.refresh_btn.disabled = false;

			$("#aplist_table").tablesorter();
    }
  });
}

var retry = 4;

function ajaxGetStatus(e) { //Use AJAX to get connecting status from aplist.asp( apscan2.log hook ).

  $.ajax({
    url: 'aplist.asp', // Get status from aplist again.
    dataType: 'script', 
	
    error: function(xhr) {
      alert('<#ajax_disconnect#>');
    },
    success: function(response) {
			var ConnectingStatus = getConnectingStatus();

			showList();
	  	bind_event();
			//alert("ConnectingStatus: "+ConnectingStatus +" \nretry: "+ retry);
				  	  	
			if(retry > 0 && (ConnectingStatus==1||ConnectingStatus==0)){
				ajaxGetStatus();
				retry--;
			}
			else if(ConnectingStatus == 2 || ConnectingStatus == 4){
				//Status(2): 連線正確，等待重新取得連線...
				SuccessMsg();
			}
			else if(updateIdx(document.form.sta_bssid.value)){ //先確定基地台是否存在，且更新idx。
				if(ConnectingStatus == 1){
					//Status(1): WEP金鑰輸入有誤。");
					closePopup('connecting_table');
					showKeyPanel();
					$("#wl_key_table tbody td:first").html("<span style='color:red;'><#APSurvey_action_ConnectingStatus1#></span>");
					retry = 3;
				}
				else if(ConnectingStatus == 3 || ConnectingStatus == 5){
					//Status(3): WPA金鑰認證失敗，請重新輸入;
					closePopup("connecting_table");
					showKeyPanel();
					$("#wl_key_table tbody td:first").html("<span style='color:red;'><#APSurvey_action_ConnectingStatus35#></span>");
					retry = 3;
				}
			}
			else{ // 基地台不存在。 Maybe case 0;
					closePopup("connecting_table");
					alert("<#APSurvey_action_ConnectingStatus0#>");
					//closePopup('connecting_table');
					ajaxGetData();
					retry = 3;
			}
    }
  });
}

function updateIdx(selectedAP_bssid){
	for(var i=0; i<aplist.length ;i++){
		if(aplist[i][5] == selectedAP_bssid){
			idx = i;
			return true;
		}
		else{
			continue;
		}
	}
	return false;
}

function getConnectingStatus(){

	for(var i=0; i<aplist.length ;i++){
		//alert(aplist[i][0] + ": "+aplist[i][8]);
		if(aplist[i][8] != 0){
			return aplist[i][8]; //1: disconnected(including wep key error).  2:connected.  3:connecting(including psk key error).			
		}
		else{ //0: not in profile & not connected.
			continue;
		}
	}
	return 0;
}

var times = 5; // times for retry request;
var secs = 80;

function ajaxGetUREIP(e) { //Use AJAX to get New IP of RT-N13U from ureip.asp( urelease hook ).
  $.ajax({
    url: 'ureip.asp', // Get status from aplist again.
    dataType: 'script', 
	
    error: function(xhr) {
      //alert('<#ajax_disconnect#>');
      setTimeout("ajaxGetUREIP();", 3000);
    },
    success: function(response) {
    	//alert("urelease: [" + ureIP + "]");
    	//ajaxLogout();
    	
    	if(times >= 0){
	    	if(ureIP[0] == ""){  //未取得新IP，則每隔五秒重新request;
					setTimeout("ajaxGetUREIP();", 3000);
					times--;
				}
				else{
					$('#connecting_table tbody td').eq(1).html('<br/><#APSurvey_action_renew_IP#><strong>'+ ureIP[0] + '</strong>。<p><#APSurvey_action_renewing#></p>');
					setTimeout("$('#connecting_table tbody td').eq(1).prepend('<#APSurvey_msg_renew_fail_hint0#><br/><br/>');", 15000);
					ureLogout();
					countdown();
				}
			}
			else{
				$('#connecting_table tbody td').eq(1).html('<#APSurvey_msg_renew_fail#>');
				$('#connecting_table tbody td img').attr('src','images/survey/warning.gif');
				setTimeout("closePopup('connecting_table');", 2000);
				setTimeout("ajaxGetData();", 2000);
			}
    }
  });
}

function ureLogout(){
	document.getElementById("hidden_frame").src = "Logout.asp";
}

function ajaxLogout(e){
	//$.get("Logout.asp");
	//$.ajax({type: "GET", url: "Logout.asp", dataType: "html"});
	alert("進入logout");
  $.ajax({
    url: 'Logout.asp',
    dataType: 'html',
    error: function(xhr) {
      alert('logout失敗');
    },
    success: function(response) {
    	alert("logout成功");
    }
  });    
}

function countdown(){
	
	var remote_url = "http://"+ ureIP[0] +"/remote.asp";
	if(secs > 0){
		id_countdown = setTimeout("countdown();", 5000);
	}
	secs = secs-5;
	
  if(secs <= 5){
  	$('#connecting_table tbody td img').attr('src','/images/survey/warning.gif');
  	$('#connecting_table tbody td').eq(1).html('<br/><#APSurvey_msg_renew_fail_hint1#> <a href="http://'+ ureIP[0] +'/ure_success.htm"><#APSurvey_msg_renew_fail_hint2#></a>');
  }
  $.ajax({
    url: remote_url,
    dataType: 'script',
    error: function(xhr) { 	
    },
    success: function(response) {
    	//alert('router_ip:'+router_ip);
    	clearTimeout(id_countdown);
			if(navigator.appName.indexOf("Microsoft") >= 0){
				setTimeout("$('#connecting_table tbody td').eq(1).html('<br/><#APSurvey_action_renew_success#>');", 15000);
				setTimeout("location.href = 'http://' + ureIP[0] + '/ure_success.htm';", 20000);
			}
			else{
				$('#connecting_table tbody td').eq(1).html('<br/><#APSurvey_action_renew_success#>');
				setTimeout("location.href = 'http://' + ureIP[0] + '/ure_success.htm';", 3000);
			}			
    }
  });
}


/*function loadDragPanel(obj){
	$("#"+obj+" thead").mousedown(  
  	function () {
			$("#"+obj).draggable();
	  }
	)
	$("#"+obj+" thead").mouseup(
  	function () {
			$("#"+obj).draggable("destroy");
	  }
	)
}*/

function showMask(){
	var bodyH = $("body").height();  //遮罩高度以JQuery取得
	var docuH = $(document).height();	
	bodyH = (bodyH > docuH)?bodyH:docuH;
	
	$("#Survey_mask").css("height", bodyH);
	$("#LoadingBox_mask").css("height", bodyH);

	$("#Survey_mask").show();
	//$("#LoadingBox_mask").show();
}

function tune_list(arr){ // Shift connected ap to 1st position.

	var temp_obj = arr[0];
	for(var i=0; i<arr.length ;i++){
		//alert(arr[i][0]+", "+arr[i][8]);
		if(arr[i][8] == 2 || arr[i][8] == 4){
			arr[0] = arr[i];
			arr[i] = temp_obj;
			break;
		}
	}
	return arr;
}

function showList(){
	var html_code = "";
	var wl_RSSI;		

	document.form.sta_ssid.value = profile_ssid; // initial the sta_ssid;	

	if(aplist.length == 0){
		html_code += "<tr><th colspan='4' style='text-align:center; color:red;'><#APSurvey_action_searching_noresult#></th></tr>";
	}
	
	aplist = tune_list(aplist);
	
	for(var i=0; i<aplist.length ;i++){
	
		if(Math.abs(aplist[i][4]) <= 20)     // 訊號：Excellent
			wl_RSSI = 5;
		else if(Math.abs(aplist[i][4]) <= 40)//       Very good
			wl_RSSI = 4;
		else if(Math.abs(aplist[i][4]) <= 60)//       Good
			wl_RSSI = 3;
		else if(Math.abs(aplist[i][4]) <= 80)//       Fairly
			wl_RSSI = 2;
		else                                 //       Poor
			wl_RSSI = 1;
	
		//aplist[i][0] = decodeURIComponent(aplist[i][0]); // decode SSID to char;
		var decodeSSID = decodeURIComponent(aplist[i][0]);
		
		//use abbr to store BSSID, Orig ssid;
		if(aplist[i][8] != 0 && aplist[i][0] == ""){
			abbrtext = "<strong>SSID: </strong>"+ document.form.sta_ssid.value + "<br/><strong>MAC: </strong>" + aplist[i][5];
		}
		else		
			abbrtext = "<strong>SSID: </strong>"+ aplist[i][0] + "<br/><strong>MAC: </strong>" + aplist[i][5];
		
		
		//Shortening the SSID which the length is over 18 characters.
		var ellipsis_ssid = (decodeSSID.length > 18)?decodeSSID.substring(0,18)+"...":decodeSSID;
	
		html_code += "<tr>\n";
		html_code += "<th abbr='"+ abbrtext +"'>"; //tooltips of orig SSID & MAC
		//html_code += "<th>"; //tooltips of orig SSID & MAC
		
		if(aplist[i][8] != 0 && aplist[i][0] == ""){ //Hidden SSID but connected.
			abbrtext = "<strong>SSID: </strong>"+ document.form.sta_ssid.value + "<br/><strong>MAC: </strong>" + aplist[i][5];
			html_code += "<span class='ssid' style='color:#999;'>"+ document.form.sta_ssid.value +"</span>"; 
		}
		else
			html_code += "<span class='ssid'>"+ ellipsis_ssid+ "</span>";
			
		html_code += (aplist[i][8]==2 || aplist[i][8]==4)?"<span class='connected' title='<#CTL_Connect#>'></span>\n":"<span class='blank_block'>&nbsp;</span>";  // isConnected		
		//html_code += (aplist[i][8]==1)?"<span class='star'></span>\n":"<span class='blank_block'>&nbsp;</span>";  // isProfile
		html_code += (aplist[i][7])?"<span class='wl_mode'>" +aplist[i][7]+ "</span>":"";  //bgn
		html_code += "<input type='hidden' value='"+ i +"'>"; // index for get value from Array
		html_code += "</th>\n";
		
		html_code += "<td>"+ aplist[i][1] +"</td>\n";  //Channel
		html_code += "<td class='auth'>";  //Authentication (encryption)
		html_code += aplist[i][2] +" (" +aplist[i][3]+ ")";
		
		if(aplist[i][2] != "Open System" && aplist[i][3] != "NONE"){
			html_code += "<img src='images/survey/security.gif' /></td>\n";
		}
			
		html_code += "<td><img src='images/survey/radio"+ wl_RSSI +".gif' title='<#Radio#>：'/></td>\n"; //RSSI
		html_code += "</tr>\n";
	}
	$("#aplist_table thead").html('<tr><th width="45%" style="text-align:left;"><#Wireless_name#></th><th width="10%"><#WLANConfig11b_Channel_itemname#></th><th><#Security#></th><th width="11%"><#Radio#></th></tr>');
	$("#aplist_table tbody").html(html_code);																												
	$("#aplist_table tbody tr:odd").css("background", "#F4F4F4");
	$("#aplist_table tbody tr:even").css("background", "#FFFFFF");	
}

var lastClickTr = "";  //for click event and style change.
var lastClickTrBg = "";
var idx = "1";

function bind_event(){ //綁定每個<TR>的click 及 hover event
	
	$("#aplist_table tbody tr").bind("click", function(e){  // bind click event to <tr> in <tbody>
		if(lastClickTr){
			lastClickTr.css("background",lastClickTrBg);
		}
		lastClickTr = $(this);
		lastClickTrBg = $(this).css("background");
		
		$(this).css("background", "url(images/survey/select_tr_bg.gif) repeat-x scroll 0px bottom");
		//$(this).css("background", "#004080 repeat-x scroll 0px bottom");
		
		idx = $(this).children().eq(0).find("input").val();		//Get index value
		//將選定的AP資訊填入 form value 中等待送出
		document.form.sta_ssid.value = decodeURIComponent(aplist[idx][0]);
		document.form.sta_bssid.value = aplist[idx][5];
		document.form.sta_encryption.value = aplist[idx][3];
		document.form.connect_btn.disabled = false;

		//若選定的AP是Hidden SSID，則將sta_check_ha設為1
		document.form.sta_check_ha.value = (document.form.sta_ssid.value == "")?"1":"0";
		
	}),
	$(".ssid").hover(   //Show tooltips of SSID & MAC
		 function(){
		 	var tooltiptext = decodeURIComponent($(this).parent().attr('abbr'));
		 	//alert($(this).parent().children().eq(0).find("input").eq(1).val());
		 	//var tooltiptext = $(this).parent().children().eq(0).find("input").value;
			writetxt(tooltiptext);
		 },
		 function(){
			writetxt(0);
		 }
	),
	$("#connect_btn").click(
		function(){
			if(document.form.sta_encryption.value == "NONE"){
				if(confirm("<#APSurvey_msg_connect_open_wlan#>")){
					if(document.form.sta_check_ha.value == "1")
						setTimeout("showKeyPanel();",800);
					else
						submitForm();
				}
				else{
					return false;
				}	
			}
			else{
				showKeyPanel();
			}
			return true;
		}
	);
	
	for(var i=0; i<aplist.length ;i++){
		if(aplist[i][8] == 2 || aplist[i][8] == 4){  //將已連線的TR列為last click
			lastClickTr = $("#aplist_table tbody tr").eq(i);
			lastClickTrBg = lastClickTr.css("background");
			$("#aplist_table tbody tr").eq(i).css("background", "url(images/survey/select_tr_bg.gif) repeat-x scroll 0px bottom");
		}
	}
}
function showKeyPanel(){

	document.form.connect_btn.disabled = true;
	document.form.refresh_btn.disabled = true;

	$("#content_mask").show();
	$("#LoadingBox_mask").show();
	$("#wl_key_table").slideDown(500);

	$("#ssid_input_field").css("display", "");
	$("#password_input_field").css("display", "");
	$("#pwdcheck_input_field").css("display", "");
	
	$("#wl_key_table tbody td:first").html("<strong><span></span></strong>&nbsp;<#APSurvey_msg_ask_to_input1#> <strong><span></span></strong>&nbsp;<#Network_key#>");
	
	$("#wl_key_table tbody td:first span").eq(0).text($("#sta_ssid").val()); //SSID + 說明
	$("#wl_key_table tbody td:first span").eq(1).text($("#sta_encryption").val());

	if(document.form.sta_check_ha.value == "1"){  // Show ssid input field when ssid is null;
		$("#wl_key_table tbody td:first span").eq(1).prepend("<#APSurvey_msg_ask_to_input2#>&nbsp;");
		
		if($("#sta_encryption").val() == "NONE"){
			$("#password_input_field").css("display", "none");
			$("#pwdcheck_input_field").css("display", "none");
			$("#wl_key_table tbody td:first").html("<#APSurvey_msg_ask_to_input1#> <strong><span><#Wireless_name#></span></strong>");
		}
				
		if(document.form.ha_ssid.value == "")
			setTimeout("document.form.ha_ssid.focus();", 800);
		else
			setTimeout("document.form.password.focus();", 800);	
	}
	else{
		$("#ssid_input_field").css("display", "none");
		setTimeout("document.form.password.focus();", 800);
	}
	
	if($("#sta_encryption").val() == "WEP")
		$("#keyindex_input_field").css("display", "");
	else
		$("#keyindex_input_field").css("display", "none");
	
	
	$("#alert_msg").html(""); //清空錯誤訊息方塊。
	$("#ha_ssid").val("");
	$("#password").val("");
	//$("#password_confirm").val("");
	
}
function closePopup(obj){
	$("#"+obj).slideUp();
	$("#content_mask").hide();
	$("#LoadingBox_mask").hide();
	
	document.form.connect_btn.disabled = false;
	document.form.refresh_btn.disabled = false;	
}

function validateKey(){
		
		//if(document.form.sta_ssid.value == "" && document.form.password.value == ""){
		if(document.form.sta_ssid.value == "" && document.form.sta_check_ha.value == "1"){
			document.getElementById("alert_msg").innerHTML = "<#APSurvey_Validate_ssid_blank#>";
			document.form.ha_ssid.focus();
			return false;
		}
		else if(document.form.password.value == ""){
			//$("#alert_msg").html("<#APSurvey_Validate_key_blank#>");
			document.getElementById("alert_msg").innerHTML = "<#APSurvey_Validate_key_blank#>";
			document.form.password.focus();
			return false;
		}
		
		if(aplist[idx][2] == "WPA-Personal" || aplist[idx][2] == "WPA2-Personal"){
			if(!validate_psk(document.form.password)){  //Lock Add 2009.02.13
				return false;
			}
			else
				return true;
		}
		else if(aplist[idx][2] == "Unknown" || aplist[idx][3] == "WEP"){
			if(!validate_wlkey(document.form.password)){  //Lock Add 2009.02.13
				return false;
			}
			else
				return true;
		}
		else
			return true;
}

function submitForm(){
	
	if($("#sta_check_ha").val() == 1){
		$("#sta_ssid").val($("#ha_ssid").val());
	}
	
	if(document.form.sta_encryption.value == "NONE"){
		if(document.form.sta_check_ha.value == "0"){
			$("#content_mask").show();
			$("#LoadingBox_mask").show();
		}
		else{
			if(document.form.sta_ssid.value == ""){
				document.getElementById("alert_msg").innerHTML = "<#APSurvey_Validate_ssid_blank#>";
				document.form.ha_ssid.focus();
				return false;
			}
			else
				$("#wl_key_table").hide();
		}
	}
	else if(validateKey()){
		$("#wl_key_table").hide();
	}
	else{
		return false;
	}
	document.form.sta_key.disabled = 1;
	document.form.sta_key1.disabled = 1;
	document.form.sta_key2.disabled = 1;
	document.form.sta_key3.disabled = 1;
	document.form.sta_key4.disabled = 1;

	document.form.sta_crypto.disabled = 1;
	document.form.sta_wpa_mode.disabled = 1;
	document.form.sta_wpa_psk.disabled = 1;
	
	//alert("idx:"+ idx + "aplist[idx]\n" +aplist[idx]+"\naplist[idx][3]:" + aplist[idx][3]);
	
	if (aplist[idx][2] == "Open System")
	{
		document.form.sta_auth_mode.value = "open";
		document.form.sta_wep_x.value = "0";
	}
	else if (aplist[idx][2] == "Unknown" && aplist[idx][3] == "WEP")
	{
		//document.form.sta_auth_mode.value = "open";
		//document.form.sta_auth_mode.value = "shared";
		
		if (document.form.password.value.length == 5 || document.form.password.value.length == 10)
			document.form.sta_wep_x.value = "1";
		else if (document.form.password.value.length == 13 || document.form.password.value.length == 26)
			document.form.sta_wep_x.value = "2";
		else
			document.form.sta_wep_x.value = "0";
			
		document.form.sta_key.disabled = 0;
		document.form.sta_key.value = document.form.sta_key_index.options[document.form.sta_key_index.selectedIndex].value;
		if (document.form.sta_key.value == "1")
		{
			document.form.sta_key1.disabled = 0;
			document.form.sta_key1.value = document.form.password.value;
		}
		else if (document.form.sta_key.value == "2")
		{
			document.form.sta_key2.disabled = 0;
			document.form.sta_key2.value = document.form.password.value;
		}
		else if (document.form.sta_key.value == "3")
		{
			document.form.sta_key3.disabled = 0;
			document.form.sta_key3.value = document.form.password.value;
		}
		else if (document.form.sta_key.value == "4")
		{
			document.form.sta_key4.disabled = 0;
			document.form.sta_key4.value = document.form.password.value;
		}
	}
	else if (aplist[idx][2] == "WPA-Personal")
	{

		document.form.sta_auth_mode.value = "psk";
		document.form.sta_wep_x.value = "0";
		document.form.sta_wpa_mode.disabled = 0;
		document.form.sta_wpa_mode.value = "1";

		document.form.sta_crypto.disabled = 0;
		if (document.form.sta_encryption.value == "TKIP")
			document.form.sta_crypto.value = "tkip";
		else if (document.form.sta_encryption.value == "AES")
			document.form.sta_crypto.value = "aes";

		document.form.sta_wpa_psk.disabled = 0;
		document.form.sta_wpa_psk.value = document.form.password.value;
	}
	else if (aplist[idx][2] == "WPA2-Personal")
	{
		
		document.form.sta_auth_mode.value = "psk";
		document.form.sta_wep_x.value = "0";
		document.form.sta_wpa_mode.disabled = 0;
		document.form.sta_wpa_mode.value = "2";

		document.form.sta_crypto.disabled = 0;
		if (document.form.sta_encryption.value == "TKIP")
			document.form.sta_crypto.value = "tkip";
		else if (document.form.sta_encryption.value == "AES")
			document.form.sta_crypto.value = "aes";

		document.form.sta_wpa_psk.disabled = 0;
		document.form.sta_wpa_psk.value = document.form.password.value;
	}
	else	// WPA-Enterprise & WPA2-Enterprise: not supported
	{	
		document.form.sta_auth_mode.value = "open";
		document.form.sta_wep_x.value = "0";
	}

	document.form.sta_encryption.disabled = 1;
	//  document.form.ha_ssid.disabled = 1;
	//	document.form.password.disabled = 1;
	
	$('#connecting_table tbody td').eq(1).html('<#APSurvey_msg_waiting_to_connect#> <span></span>');
	$("#connecting_table tbody td span").text($("#sta_ssid").val());
	$('#connecting_table tbody td img').attr('src','images/survey/connecting.gif');	
	$('#connecting_table').show();
	// 正在連線...
	document.form.action_mode.value = " Apply ";
	document.form.r_Setting.value = "1";
	document.form.submit();	
}

function SuccessMsg(){
			$('#connecting_table tbody td').eq(1).text('<#APSurvey_action_ConnectingStatus24#>');
			$('#connecting_table tbody td img').attr('src','images/survey/accepted.gif');

			setTimeout("$('#connecting_table tbody td').eq(1).text('<#APSurvey_msg_renew_IP#>')", 2000);
			setTimeout("$('#connecting_table tbody td img').attr('src','images/survey/connecting.gif')", 2000);
			//setTimeout("ajaxGetUREIP();", 2000);
			ajaxGetUREIP();
			
			//After success;
			// 1. Send http request until N12 response(remind user keep waiting) and refresh the page.
			// 2. Send http request but N12 did not response (remind user change static IP) and refresh the page.
			
}

function showPassword(obj){
	
	var pw_val = document.form.password.value;
	
	if(obj.checked){
		$("#password").replaceWith("<input type='password' value='"+ pw_val +"' class='input' size='30' maxlength='64' id='password' />");
	}
	else
		$("#password").replaceWith("<input name='password' type='text' value='"+ pw_val +"' class='input' size='30' maxlength='64' id='password' />");
}
function show_prev_btn(){
	if(history.length == 1)	{
		$("#back_btn").val("<#CTL_Setting#>");
		$("#back_btn").bind("click", function(e){
			location.href = "/";
		})
	}
}
</script>
</head>

<body class="LevelQIS">
<script>
var sw_mode = '<% nvram_get_x("IPConnection",  "sw_mode"); %>';

	if(sw_mode != 2){
		alert("<#page_not_support_mode_hint#>");
		location.href = "/";
	}

</script>
<noscript>
	<div class="popup_bg"  style="display:block; visibility:visible; z-index:999;" >
		<div style="margin:200px auto; width:300px; background-color:#006699; color:#FFFFFF; line-height:150%; border:3px solid #FFF; padding:5px;"><#not_support_script#></p></div>
	</div>
</noscript>
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>
<form name="form" method="POST" action="wlconn_apply.htm" target="hidden_frame">

<input type="hidden" name="current_page" value="survey.asp">
<input type="hidden" name="next_page" value="survey.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">

<input type="hidden" name="sta_ssid" id="sta_ssid" value="<% nvram_char_to_ascii("",  "sta_ssid"); %>">
<input type="hidden" name="sta_bssid" id="sta_bssid" value="">
<input type="hidden" name="sta_encryption" id="sta_encryption" value="">
<input type="hidden" name="sta_check_ha" id="sta_check_ha" value="0">
<input type="hidden" name="sta_auth_mode" id="sta_auth_mode" value="open">
<input type="hidden" name="sta_wep_x" id="sta_wep_x" value="0">
<input type="hidden" name="sta_key" id="sta_key" value="1">
<input type="hidden" name="sta_key1" id="sta_key1" value="">
<input type="hidden" name="sta_key2" id="sta_key2" value="">
<input type="hidden" name="sta_key3" id="sta_key3" value="">
<input type="hidden" name="sta_key4" id="sta_key4" value="">
<input type="hidden" name="sta_crypto" id="sta_crypto" value="tkip">
<input type="hidden" name="sta_wpa_mode" id="sta_wpa_mode" value="1">
<input type="hidden" name="sta_wpa_psk" id="sta_wpa_psk" value="12345678">
<input type="hidden" name="r_Setting" id="r_Setting" value="<% nvram_get_x("",  "r_Setting"); %>">


<table width="705"  border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
  	<td  colspan="3" height="55" valign="top" background="images/qis_title.gif"><div class="modelName" style="margin:28px 0px 0px 180px;"><#Web_Title#></div></td>
  </tr>
</table>
<table width="705"  border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="187" height="350" style="background:url(images/qis_banner.gif) 0px bottom no-repeat">&nbsp;</td>
    <td width="510" height="380" valign="top" >
	<div id="content_mask" class="content_mask"><!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]--></div>
	<div class="LoadingBox_mask" id="LoadingBox_mask">
		<table align="center" id="LoadingBox" style="display:block;">
			<tr>
				<td>
					<img src="/images/load.gif" align="absmiddle" hspace="2" vspace="3">
				</td>
				<td valign="middle">
					<#APSurvey_action_searching_AP#>
				</td>
			</tr>
		</table>
		<table align="center" class="popup_table" width="400" border="0" id="wl_key_table" style="display:none">
			<thead>
				<tr><td colspan="2"><H2><#Input_some_value#> <#Network_key#></H2><div class="popup_close_x"><a href="javascript:closePopup('wl_key_table')"></a></div></td></tr>
			</thead>
			<tbody>
				<tr>
					<td colspan="2" style="padding:10px;">
						<span></span> <#APSurvey_msg_ask_to_input1#><strong><span></span></strong> <#Network_key#>
					</td>
				</tr>
				<tr id="ssid_input_field">
					<th><#Wireless_name#></th>
					<td>
						<input type="text" id="ha_ssid" name="ha_ssid" value="" class="input" size="30" maxlength="32"/>
					</td>
				</tr>
				<tr id="keyindex_input_field">
					<th><#WLANConfig11b_WEPDefaultKey_itemname#></th>
					<td>
      			<select name="sta_key_index">
        			<option value="1" selected>1</option>
        			<option value="2">2</option>
        			<option value="3">3</option>
        			<option value="4">4</option>
      			</select>					
					</td>
				</tr>				
				<tr id="password_input_field">
					<th><#Network_key#></th>
					<td>
						<input type="password" id="password" name="password" value="" class="input" size="30" maxlength="64"/>
					</td>
				</tr>
				<tr id="pwdcheck_input_field">
					<th>&nbsp;</th>	
					<td><input style="vertical-align:middle;line-height:100%;" type="checkbox" onclick="showPassword(this);" checked="checked" /><#APSurvey_action_hidden_key#></td>
				</tr>		
			</tbody>
			<tfoot>
				<tr>
					<td colspan="2" valign="bottom">
						<div id="alert_msg"></div>
						<input type="button" value="<#CTL_Add_enrollee#>" onclick="submitForm();" style="margin-left:200px;"/>
						<input type="button" value="<#CTL_Cancel#>" onclick="closePopup('wl_key_table');" style="margin-left:5px;"/>
					</td>
				</tr>
			</tfoot>			
		</table>
		<table align="center" class="popup_table" width="400" border="0" id="connecting_table" style="display:none">
			<thead>
				<tr><td colspan="2"><H2><#menu5_1#></H2><div class="popup_close_x"><a href="javascript:closePopup('connecting_table')"></a></div></td></tr>
			</thead>
			<tbody>
				<tr>
					<td width="25%" align="center"><img src="images/survey/connecting.gif" /></td>
					<td width="75%" height="60" valign="center">
						<#APSurvey_msg_waiting_to_connect#>
					</td>
				</tr>
			</tbody>
			<tfoot>
				<tr>
					<td height="40" colspan="2">
						<input type="button" value="<#CTL_ok#>" onclick="closePopup('connecting_table');" style="display:none;"/>
					</td>
				</tr>
			</tfoot>
		</table>
		<table align="center" class="popup_table" width="400" border="0" id="warningBox" style="display:none">
			<thead>
				<tr><td><H2><#CTL_help#></H2><div class="popup_close_x"><a href="javascript:closePopup('warningBox');ajaxGetData();"></a></div></td></tr>
			</thead>
			<tbody>
				<tr>
					<td height="200" valign="center">
						<div style="background:url('/images/survey/wllogin_hint.gif') no-repeat center center; height:72px;width:370px;"></div>
						<#APSurvey_msg_connect_suggest1#>
						<br/><br/><#APSurvey_msg_connect_suggest2#>
						<br/><br/><#APSurvey_msg_connect_suggest3#>
					</td>
				</tr>
			</tbody>
			<tfoot>
				<tr>
					<td height="40" align="center" style="background:'#CCCCCC';">
						<input type="button" value="<#CTL_ok#>" onclick="closePopup('warningBox');ajaxGetData();" />
						<input type="button" value="<#t1Logout#>" onclick="javascript:location.href='Logout.asp';" style="margin-left:20px;"/>
					</td>
				</tr>
			</tfoot>
		</table>		
	</div>
	
	<div class="survey_main">
		<div class="description_down"><#APSurvey_main_title#></div>
		<table border="0" cellpadding="2" id="aplist_table">
			<thead>
				<tr>
				  <th width="45%" style="text-align:left;"><#Wireless_name#></th>
				  <th width="10%"><#WLANConfig11b_Channel_itemname#></th>
				  <th><#Security#></th>
				  <th width="10%"><#Radio#></th>
			    </tr>
			</thead>
			<tbody>

			</tbody>
		</table>
	</div><!--end of survey_main-->
	<div class="survey_foot">
		<input id="back_btn" type="button" value="<#btn_pre#>" onclick="javascript:history.back();" class="button" />
	 <input id="connect_btn" name="connect_btn" type="button" value="<#CTL_Add_enrollee#>" class="sbtn" disabled=true />
 	 <input id="refresh_btn" name="refresh_btn" type="button" value="<#CTL_refresh#>"  onclick="ajaxGetData();" class="sbtn" disabled=disabled />
	</div>
    </td>
    <td width="8" height="380"valign="top" background="images/qis_right.gif" ></td>
  </tr>
	<tr>
		<td height="12" colspan="3" style="background: url('images/qis_bottom.gif') no-repeat">&nbsp;</td>
	</tr>
</table>
<!--Tooltip Box-->
<div id="navtxt" class="navtext" style="position:absolute; top:50px; left:-100px; visibility:hidden; font-family:Arial, Verdana"></div>
	
<!--Full screen mask-->
<div id="Survey_mask" class="mask_bg2"><!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]--></div>

</form>
<script>
  initial();

  document.form.connect_btn.disabled = true;
  document.form.refresh_btn.disabled = true;
  
</script>

</body>
</html>
