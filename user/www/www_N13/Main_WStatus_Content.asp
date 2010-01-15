<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_7_4#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">

<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';
function initial(){
	show_banner(1);
	
	if(sw_mode == "1")
		show_menu(5,7,3);
	else
		show_menu(5,7,2);
	
	show_footer();
	
	enable_auto_hint(0, 11);
	
	//load_body();
}
</script>
</head>

<body onload="initial();" >
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" action="apply.cgi" >
<input type="hidden" name="current_page" value="Main_WStatus_Content.asp">
<input type="hidden" name="next_page" value="Main_WStatus_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="23">&nbsp;</td>
    <td valign="top" width="202">
      <div id="mainMenu"></div>
      <div id="subMenu"></div></td>
    <td valign="top">
	  <div id="tabMenu" class="submenuBlock"></div><br/>
      
      <!--===================================Beginning of Main Content===========================================-->
      <table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
          <td valign="top" >           
            <table width="500" border="0" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle">
              <thead>
                <tr>
                  <td colspan="2"><#menu5_7#> - <#menu5_7_4#></td>
                </tr>
              </thead>
              
              <tr>
                <td colspan="2" align="center">
                  <textarea  cols="63" rows="10" style="font-family:'Courier New', Courier, mono; font-size:13px;" readonly="readonly" wrap=VIRTUAL><% nvram_dump("wlan11b.log","wlan11b.sh"); %>
                  </textarea><!--==magic 2008.11 del name ,if there are name, when the form was sent, the textarea also will be sent==-->
								</td>
              </tr>
              <tr>
			  				<td colspan="2" align="right">
						  		<input type="submit" class="button" onclick="onSubmitCtrl(this, ' Refresh ')" value="<#CTL_refresh#>" name="action" />
								</td>
              </tr>
            </table>
			</form>
			</td>
			<td id="help_td" style="width:15px;" valign="top">
			<form name="hint_form"></form>
			<div id="helpicon" onClick="openHint(0, 0);" title="<#Help_button_default_hint#>">
				<img src="images/help.gif">
			</div>
	  
			<div id="hintofPM" style="display:none;">
			<table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
			  <thead>
			  <tr>
				<td>
				  <div id="helpname" class="AiHintTitle"></div>
				  <a href="javascript:closeHint();"><img src="images/button-close.gif" class="closebutton" /></a>
				</td>
			  </tr>
			  </thead>
		  
			  <tbody>
			  <tr>
				<td valign="top">
				  <div id="hint_body" class="hint_body2"></div>
				  <iframe id="statusframe" name="statusframe" class="statusframe" src="" frameborder="0"></iframe>
				</td>
			  </tr>
			  </tbody>
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
