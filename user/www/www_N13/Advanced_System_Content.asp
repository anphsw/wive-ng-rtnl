<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_6_2#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" language="JavaScript" src="/help.js"></script>
<script type="text/javascript" language="JavaScript" src="/detect.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function initial(){
	show_banner(1);
	
	if(sw_mode == '1')
		show_menu(5,6,1);
	else if(sw_mode == '2')
		show_menu(5,6,1);
	else if(sw_mode == '3')
		show_menu(5,6,1);	
	
	show_footer();
	
	enable_auto_hint(11, 3);

	//load_body();
	corrected_timezone();
	document.form.http_passwd2.value = "";
}

function applyRule(){
	if(validForm()){
		showLoading();
		
		if(document.form.http_passwd2.value.length > 0)
			document.form.http_passwd.value = document.form.http_passwd2.value;
		document.form.action_mode.value = " Apply ";
		document.form.next_page.value = "";
		
		document.form.submit();
	}
}

function validForm(){
	if(!validate_string(document.form.http_passwd2) || !validate_string(document.form.v_password2))
		return false;
	
	if(document.form.http_passwd2.value != document.form.v_password2.value){
		showtext($("alert_msg"),"*<#File_Pop_content_alert_desc7#>");
		
		document.form.http_passwd2.focus();
		document.form.http_passwd2.select();
		
		return false;
	}
	
	if(!validate_ipaddr(document.form.log_ipaddr, 'log_ipaddr')
			|| !validate_string(document.form.ntp_server0)
			)
		return false;
	
	if(document.form.http_passwd2.value.length > 0)
		alert("<#File_Pop_content_alert_desc10#>");
	
	return true;
}

function done_validating(action){
	refreshpage();
}

function corrected_timezone(){
	var today = new Date();
	var StrIndex;	
	
	if(today.toString().lastIndexOf("-") > 0)
		StrIndex = today.toString().lastIndexOf("-");
	else if(today.toString().lastIndexOf("+") > 0)
		StrIndex = today.toString().lastIndexOf("+");

	if(StrIndex > 0){		
		if(timezone != today.toString().substring(StrIndex, StrIndex+5)){
			$("timezone_hint").style.display = "block";
			$("timezone_hint").innerHTML = "<#LANHostConfig_x_TimeZone_itemhint#>";
		}
		else
			return;			
	}
	else
		return;	
}

</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(11, 3);return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">

<input type="hidden" name="current_page" value="Advanced_System_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="LANHostConfig;General;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<input type="hidden" name="http_passwd" value="<% nvram_get_x("General", "http_passwd"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="23">&nbsp;</td>
	
	<!--=====Beginning of Main Menu=====-->
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
		
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div><br />
		<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td valign="top" >
<table width="98%" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle" table>
	<thead>
	<tr>
		<td><#menu5_6#> - <#menu5_6_2#></td>
	</tr>
	</thead>
	<tbody>
	<tr>
		<td bgcolor="#FFFFFF"></td>
	</tr>
	</tbody>	
	<tr>
	  <td bgcolor="#FFFFFF">
	  <table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
	  	<thead>
	  	<tr>
          <td colspan="2"><#PASS_changepasswd#></td>
        </tr>
    	</thead>
        <tr>
          <th width="40%"><#PASS_new#></th>
          <td>
            <input type="password" name="http_passwd2" onKeyPress="return is_string(this);" class="input" size="16" maxlength="16" />
          </td>
        </tr>
        <tr>
          <th valign="top"><#PASS_retype#></th>
          <td>
            <input type="password" name="v_password2" onKeyPress="return is_string(this);" class="input" size="16" maxlength="16" /><br/><span id="alert_msg"></span>
          </td>
        </tr>
      </table>
	</td>
  </tr>
  <tr>
	  <td bgcolor="#FFFFFF">
      <table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
      	<thead>
	  	<tr>
          <td colspan="2"><#t2Misc#></td>
        </tr>
    	</thead>
        <tr>
          <th width="40%"><a class="hintstyle"  href="javascript:void(0);" onClick="openHint(11,1)"><#LANHostConfig_x_ServerLogEnable_itemname#></a></th>
          <td><input type="text" maxlength="15" class="input" size="15" name="log_ipaddr" value="<% nvram_get_x("LANHostConfig","log_ipaddr"); %>" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td>
        </tr>
        <tr>
          <th><a class="hintstyle"  href="javascript:void(0);" onClick="openHint(11,2)"><#LANHostConfig_x_TimeZone_itemname#></a></th>
          <td>
            <select name="time_zone" class="input" onchange="return change_common(this, 'LANHostConfig', 'time_zone')">						
							<option value="UCT12" <% nvram_match_x("LANHostConfig","time_zone", "UCT12","selected"); %>			>(GMT-12:00) <#TZ01#></option>
							<option value="UCT11" <% nvram_match_x("LANHostConfig","time_zone", "UCT11","selected"); %>			>(GMT-11:00) <#TZ02#></option>
							<option value="UCT10" <% nvram_match_x("LANHostConfig","time_zone", "UCT10","selected"); %>			>(GMT-10:00) <#TZ03#></option>
							<option value="NAST9NADT" <% nvram_match_x("LANHostConfig","time_zone", "NAST9NADT","selected"); %>		>(GMT-09:00) <#TZ04#></option>
							<option value="PST8PDT" <% nvram_match_x("LANHostConfig","time_zone", "PST8PDT","selected"); %>		>(GMT-08:00) <#TZ05#></option>
							<option value="MST7_1" <% nvram_match_x("LANHostConfig","time_zone", "MST7_1","selected"); %>		>(GMT-07:00) <#TZ06#></option>
							<option value="MST7" <% nvram_match_x("LANHostConfig","time_zone", "MST7","selected"); %>			>(GMT-07:00) <#TZ07#></option>
							<option value="MST7MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST7MDT","selected"); %>		>(GMT-07:00) <#TZ08#></option>
							<option value="CST6CDT_1" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_1","selected"); %>		>(GMT-06:00) <#TZ09#></option>
							<option value="CST6CDT_2" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_2","selected"); %>		>(GMT-06:00) <#TZ10#></option>
							<option value="CST6CDT_3" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_3","selected"); %>		>(GMT-06:00) <#TZ11#></option>
							<option value="CST6CDT_3_1" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT_3_1","selected"); %>	>(GMT-06:00) <#TZ12#></option>
							<option value="UCT6" <% nvram_match_x("LANHostConfig","time_zone", "UCT6","selected"); %>			>(GMT-06:00) <#TZ13#></option>
							<option value="EST5EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST5EDT","selected"); %>		>(GMT-05:00) <#TZ14#></option>
							<option value="UCT5_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT5_1","selected"); %>			>(GMT-05:00) <#TZ15#></option>
							<option value="UCT5_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT5_2","selected"); %>			>(GMT-05:00) <#TZ16#></option>
							<option value="AST4ADT" <% nvram_match_x("LANHostConfig","time_zone", "AST4ADT","selected"); %>		>(GMT-04:00) <#TZ17#></option>
							<option value="UCT4_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT4_1","selected"); %>			>(GMT-04:00) <#TZ18#></option>
							<option value="UCT4_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT4_2","selected"); %>			>(GMT-04:00) <#TZ19#></option>
							<option value="NST3.30" <% nvram_match_x("LANHostConfig","time_zone", "NST3.30","selected"); %>		>(GMT-03:30) <#TZ20#></option>
							<option value="EBST3EBDT_1" <% nvram_match_x("LANHostConfig","time_zone", "EBST3EBDT_1","selected"); %>	>(GMT-03:00) <#TZ21#></option>
							<option value="UCT3" <% nvram_match_x("LANHostConfig","time_zone", "UCT3","selected"); %>			>(GMT-03:00) <#TZ22#></option>
							<option value="EBST3EBDT_2" <% nvram_match_x("LANHostConfig","time_zone", "EBST3EBDT_2","selected"); %>	>(GMT-03:00) <#TZ23#></option>
							<option value="NORO2" <% nvram_match_x("LANHostConfig","time_zone", "NORO2","selected"); %>			>(GMT-02:00) <#TZ24#></option>
							<option value="EUT1EUTDST" <% nvram_match_x("LANHostConfig","time_zone", "EUT1EUTDST","selected"); %>		>(GMT-01:00) <#TZ25#></option>
							<option value="UCT1" <% nvram_match_x("LANHostConfig","time_zone", "UCT1","selected"); %>			>(GMT-01:00) <#TZ26#></option>
							<option value="GMT0BST_1" <% nvram_match_x("LANHostConfig","time_zone", "GMT0BST_1","selected"); %>		>(GMT) <#TZ27#></option>
							<option value="GMT0BST_2" <% nvram_match_x("LANHostConfig","time_zone", "GMT0BST_2","selected"); %>		>(GMT) <#TZ28#></option>
							<option value="UCT-1_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_1","selected"); %>		>(GMT+01:00) <#TZ29#></option>
							<option value="UCT-1_1_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_1_1","selected"); %>		>(GMT+01:00) <#TZ30#></option>
							<option value="UCT-1_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_2","selected"); %>		>(GMT+01:00) <#TZ31#></option>
							<option value="UCT-1_2_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_2_1","selected"); %>		>(GMT+01:00) <#TZ32#></option>
							<option value="MET-1METDST" <% nvram_match_x("LANHostConfig","time_zone", "MET-1METDST","selected"); %>	>(GMT+01:00) <#TZ33#></option>
							<option value="MET-1METDST_1" <% nvram_match_x("LANHostConfig","time_zone", "MET-1METDST_1","selected"); %>	>(GMT+01:00) <#TZ34#></option>
							<option value="MEZ-1MESZ" <% nvram_match_x("LANHostConfig","time_zone", "MEZ-1MESZ","selected"); %>		>(GMT+01:00) <#TZ35#></option>
							<option value="MEZ-1MESZ_1" <% nvram_match_x("LANHostConfig","time_zone", "MEZ-1MESZ_1","selected"); %>	>(GMT+01:00) <#TZ36#></option>
							<option value="UCT-1_3" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1_3","selected"); %>		>(GMT+01:00) <#TZ37#></option>
							<option value="UCT-2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-2","selected"); %>		>(GMT+02:00) <#TZ38#></option>
							<option value="EST-2EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST-2EDT","selected"); %>		>(GMT+02:00) <#TZ39#></option>
							<option value="UCT-2_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-2_1","selected"); %>		>(GMT+02:00) <#TZ40#></option>
							<option value="UCT-2_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-2_2","selected"); %>		>(GMT+02:00) <#TZ41#></option>
							<option value="IST-2IDT" <% nvram_match_x("LANHostConfig","time_zone", "IST-2IDT","selected"); %>		>(GMT+02:00) <#TZ42#></option>
							<option value="SAST-2" <% nvram_match_x("LANHostConfig","time_zone", "SAST-2","selected"); %>			>(GMT+02:00) <#TZ43#></option>
							<option value="EET-2EETDST" <% nvram_match_x("LANHostConfig","time_zone", "EET-2EETDST","selected"); %>		>(GMT+02:00) <#TZ43_2#></option>
							<option value="MST-3MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST-3MDT","selected"); %>		>(GMT+03:00) <#TZ44#></option>
							<option value="MST-3MDT_1" <% nvram_match_x("LANHostConfig","time_zone", "MST-3MDT_1","selected"); %>		>(GMT+03:00) <#TZ45#></option>
							<option value="UCT-3_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3_1","selected"); %>		>(GMT+03:00) <#TZ46#></option>
							<option value="UCT-3_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3_2","selected"); %>		>(GMT+03:00) <#TZ47#></option>
							<option value="IST-3IDT" <% nvram_match_x("LANHostConfig","time_zone", "IST-3IDT","selected"); %>		>(GMT+03:00) <#TZ48#></option>
							<option value="UCT-3.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-3.30","selected"); %>		>(GMT+03:30) <#TZ49#></option>
							<option value="UCT-4_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4_1","selected"); %>		>(GMT+04:00) <#TZ50#></option>
							<option value="UCT-4_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4_2","selected"); %>		>(GMT+04:00) <#TZ51#></option>
							<option value="UCT-4.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4.30","selected"); %>		>(GMT+04:30) <#TZ52#></option>
							<option value="RFT-5RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-5RFTDST","selected"); %>	>(GMT+05:00) <#TZ53#></option>
							<option value="UCT-5" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5","selected"); %>			>(GMT+05:00) <#TZ54#></option>
							<option value="UCT-5.30_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30_2","selected"); %>		>(GMT+05:30) <#TZ55#></option>
							<option value="UCT-5.30_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30_1","selected"); %>		>(GMT+05:30) <#TZ56#></option>
							<option value="UCT-6" <% nvram_match_x("LANHostConfig","time_zone", "UCT-6","selected"); %>			>(GMT+05:30) <#TZ58#></option>
							<option value="UCT-5.45" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.45","selected"); %>		>(GMT+05:45) <#TZ57#></option>
							<option value="UCT-5.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5.30","selected"); %>		>(GMT+06:00) <#TZ59#></option>
							<option value="RFT-6RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-6RFTDST","selected"); %>	>(GMT+06:00) <#TZ60#></option>
							<option value="UCT-6.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-6.30","selected"); %>		>(GMT+06:30) <#TZ61#></option>
							<option value="UCT-7" <% nvram_match_x("LANHostConfig","time_zone", "UCT-7","selected"); %>			>(GMT+07:00) <#TZ62#></option>
							<option value="RFT-7RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-7RFTDST","selected"); %>	>(GMT+07:00) <#TZ63#></option>
							<option value="CST-8" <% nvram_match_x("LANHostConfig","time_zone", "CST-8","selected"); %>			>(GMT+08:00) <#TZ64#></option>
							<option value="CST-8_1" <% nvram_match_x("LANHostConfig","time_zone", "CST-8_1","selected"); %>		>(GMT+08:00) <#TZ65#></option>
							<option value="SST-8" <% nvram_match_x("LANHostConfig","time_zone", "SST-8","selected"); %>			>(GMT+08:00) <#TZ66#></option>
							<option value="CCT-8" <% nvram_match_x("LANHostConfig","time_zone", "CCT-8","selected"); %>			>(GMT+08:00) <#TZ67#></option>
							<option value="WAS-8WAD" <% nvram_match_x("LANHostConfig","time_zone", "WAS-8WAD","selected"); %>		>(GMT+08:00) <#TZ68#></option>
							<option value="UCT-8" <% nvram_match_x("LANHostConfig","time_zone", "UCT-8","selected"); %>			>(GMT+08:00) <#TZ69#></option>
							<option value="UCT-9_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9_1","selected"); %>		>(GMT+09:00) <#TZ70#></option>
							<option value="JST" <% nvram_match_x("LANHostConfig","time_zone", "JST","selected"); %>			>(GMT+09:00) <#TZ71#></option>
							<option value="UCT-9_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9_2","selected"); %>		>(GMT+09:00) <#TZ72#></option>
							<option value="CST-9.30CDT" <% nvram_match_x("LANHostConfig","time_zone", "CST-9.30CDT","selected"); %>	>(GMT+09:30) <#TZ73#></option>
							<option value="UCT-9.30" <% nvram_match_x("LANHostConfig","time_zone", "UCT-9.30","selected"); %>		>(GMT+09:30) <#TZ74#></option>
							<option value="UCT-10_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_1","selected"); %>		>(GMT+10:00) <#TZ75#></option>
							<option value="UCT-10_2" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_2","selected"); %>		>(GMT+10:00) <#TZ76#></option>
							<option value="TST-10TDT" <% nvram_match_x("LANHostConfig","time_zone", "TST-10TDT","selected"); %>		>(GMT+10:00) <#TZ77#></option>
							<option value="RFT-10RFTDST" <% nvram_match_x("LANHostConfig","time_zone", "RFT-10RFTDST","selected"); %>	>(GMT+10:00) <#TZ78#></option>
							<option value="UCT-10_5" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10_5","selected"); %>		>(GMT+10:00) <#TZ79#></option>
							<option value="UCT-11" <% nvram_match_x("LANHostConfig","time_zone", "UCT-11","selected"); %>			>(GMT+11:00) <#TZ80#></option>
							<option value="UCT-11_1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-11_1","selected"); %>		>(GMT+11:00) <#TZ81#></option>
							<option value="UCT-12" <% nvram_match_x("LANHostConfig","time_zone", "UCT-12","selected"); %>			>(GMT+12:00) <#TZ82#></option>
							<option value="NZST-12NZDT" <% nvram_match_x("LANHostConfig","time_zone", "NZST-12NZDT","selected"); %>	>(GMT+12:00) <#TZ83#></option>
							<option value="UCT-13" <% nvram_match_x("LANHostConfig","time_zone", "UCT-13","selected"); %>			>(GMT+13:00) <#TZ84#></option>
            </select>
            <span id="timezone_hint" style="display:none;"></span>
            </td>
        </tr>
        <tr>
          <th><a class="hintstyle"  href="javascript:void(0);" onClick="openHint(11,3)"><#LANHostConfig_x_NTPServer1_itemname#></a></th>
          <td><input type="text" maxlength="256" class="input" size="32" name="ntp_server0" value="<% nvram_get_x("LANHostConfig","ntp_server0"); %>" onKeyPress="return is_string(this);">
          <a href="javascript:openLink('x_NTPServer1')" class="content_input_link" name="x_NTPServer1_link">
		  <#LANHostConfig_x_NTPServer1_linkname#></td>
        </tr>
          <tr>
            <td colspan="2" align="right"><input name="button" type="button" class="button" onclick="applyRule();" value="<#CTL_apply#>"/></td>
          </tr>		
      </table></td></tr>
</table></td>
</form>

          <td id="help_td" style="width:15px;" valign="top">
<form name="hint_form"></form>
            <div id="helpicon" onClick="openHint(0,0);" title="<#Help_button_default_hint#>"><img src="images/help.gif" /></div>
            <div id="hintofPM" style="display:none;">
              <table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
			  	<thead>
                <tr>
                  <td><div id="helpname" class="AiHintTitle"></div><a href="javascript:void(0);" onclick="closeHint()" ><img src="images/button-close.gif" class="closebutton" /></a></td>
                </tr>
				</thead>
                <tr>				
                  <td valign="top" >
  				    <div class="hint_body2" id="hint_body"></div>
  				    <iframe id="statusframe" name="statusframe" class="statusframe" src="" frameborder="0"></iframe>
				</td>
                </tr>
              </table>
          </div>
		  </td>
        </tr>
      </table>				
		<!--===================================Ending of Main Content===========================================-->		
	</td>
		
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</body>
</html>
