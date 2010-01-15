<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>EVE Wireless Router <#Web_Title#> - <#menu5_3_4#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">

<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script type="text/javascript" language="JavaScript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/client_function.js"></script>
<script language="JavaScript" type="text/javascript" src="/detect.js"></script>
<script>
var wItem = new Array(new Array("", "", "TCP"),
											new Array("FTP", "20:21", "TCP"),
											new Array("TELNET", "23", "TCP"),
											new Array("SMTP", "25", "TCP"),
											new Array("DNS", "53", "UDP"),
											new Array("FINGER", "79", "TCP"),
											new Array("HTTP", "80", "TCP"),
											new Array("POP3", "110", "TCP"),
											new Array("SNMP", "161", "UDP"),
											new Array("SNMP TRAP", "162", "UDP"));

var wItem2 = new Array(new Array("", "", "TCP"),
											 new Array("Age of Empires", "2302:2400,6073", "BOTH"),
											 new Array("BitTorrent", "6881:6889", "TCP"),
											 new Array("Counter Strike(TCP)", "27030:27039", "TCP"),
											 new Array("Counter Strike(UDP)", "1200,27000:27015", "UDP"),	
											 new Array("PlayStation2", "4658,4659", "BOTH"),
											 new Array("Warcraft III", "6112:6119,4000", "BOTH"),
											 new Array("WOW", "3724", "BOTH"),
											 new Array("Xbox Live", "3074", "BOTH"));

<% login_state_hook(); %>

wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

var client_ip = login_ip_str();
var client_mac = login_mac_str();

var leases = [<% dhcp_leases(); %>];	// [[hostname, MAC, ip, lefttime], ...]
var arps = [<% get_arp_table(); %>];		// [[ip, x, x, MAC, x, type], ...]
var arls = [<% get_arl_table(); %>];		// [[MAC, port, x, x], ...]
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]
var ipmonitor = [<% get_static_client(); %>];	// [[IP, MAC, DeviceName, Type, http, printer, iTune], ...]
var networkmap_fullscan = '<% nvram_match_x("","networkmap_fullscan", "0", "done"); %>'; //2008.07.24 Add.  1 stands for complete, 0 stands for scanning.;

var clients_info = getclients();

var VSList = [<% get_nvram_list("IPConnection", "VSList"); %>];

function initial(){
	show_banner(1);
	show_menu(5,3,4);
	show_footer();
	
	loadAppOptions();
	loadGameOptions();
	
	showLANIPList();	
	showVSList();
}

function applyRule(){
	showLoading();
	
	document.form.action_mode.value = " Restart ";
	document.form.next_page.value = "";

	document.form.submit();
}

function done_validating(action){
	if(action == " Add ")
		split_vts_rule();
	else
		refreshpage();
}

function loadAppOptions(){
	free_options(document.form.KnownApps);
	add_option(document.form.KnownApps, "<#Select_menu_default#>", 0, 1);
	for(var i = 1; i < wItem.length; i++)
		add_option(document.form.KnownApps, wItem[i][0], i, 0);
}

function loadGameOptions(){
	free_options(document.form.KnownGames);
	add_option(document.form.KnownGames, "<#Select_menu_default#>", 0, 1);
	for(var i = 1; i < wItem2.length; i++)
		add_option(document.form.KnownGames, wItem2[i][0], i, 0);
}

function change_wizard(o, id){
	if(id == "KnownApps"){
		$("KnownGames").value = 0;
		
		for(var i = 0; i < wItem.length; ++i){
			if(wItem[i][0] != null && o.value == i){
				if(wItem[i][2] == "TCP")
					document.form.vts_proto_x_0.options[0].selected = 1;
				else if(wItem[i][2] == "UDP")
					document.form.vts_proto_x_0.options[1].selected = 1;
				else if(wItem[i][2] == "BOTH")
					document.form.vts_proto_x_0.options[2].selected = 1;
				else
					document.form.vts_proto_x_0.options[3].selected = 1;
				
				document.form.vts_ipaddr_x_0.value = client_ip;
				document.form.vts_port_x_0.value = wItem[i][1];
				document.form.vts_desc_x_0.value = wItem[i][0]+" Server";				
				break;
			}
		}
	}
	else if(id == "KnownGames"){
		$("KnownApps").value = 0;
		
		for(var i = 0; i < wItem2.length; ++i){
			if(wItem2[i][0] != null && o.value == i){
				if(wItem2[i][2] == "TCP")
					document.form.vts_proto_x_0.options[0].selected = 1;
				else if(wItem2[i][2] == "UDP")
					document.form.vts_proto_x_0.options[1].selected = 1;
				else if(wItem2[i][2] == "BOTH")
					document.form.vts_proto_x_0.options[2].selected = 1;
				else
					document.form.vts_proto_x_0.options[3].selected = 1;
				
				document.form.vts_ipaddr_x_0.value = client_ip;
				document.form.vts_port_x_0.value = wItem2[i][1];
				document.form.vts_desc_x_0.value = wItem2[i][0];
				
				break;
			}
		}
	}
}

/*-----------------------------------------------------------------
Old markGroup in general.js, change to single page at 2008/04/10
------------------------------------------------------------------*/
function markGroup2(o, s, c, b) {	
	document.form.group_id.value = s;	

	if(b == " Add "){		
		if (document.form.vts_num_x_0.value >= c)  //vts_num_x_0: number of virtual server
			alert("<#JS_itemlimit1#> " + c + " <#JS_itemlimit2#>");
		else if (document.form.vts_ipaddr_x_0.value==""){
			alert("\"<#IPConnection_VServerIP_itemname#>\" <#JS_fieldblank#>");					
			document.form.vts_ipaddr_x_0.focus();
			return false;
		}
		else if (document.form.vts_proto_x_0.value == "OTHER"){
			if (!validate_ipaddr(document.form.vts_ipaddr_x_0, "") ||
					!validate_portrange(document.form.vts_port_x_0, "") ||
					!validate_range(document.form.vts_protono_x_0, 0, 255)) return false;
			else if (document.form.vts_protono_x_0.value==""){
				alert("<#JS_fieldblank#>");
				document.form.vts_protono_x_0.focus();
				return false;
			}	
			document.form.vts_port_x_0.value = "";
		}
		else{				
			if (!validate_ipaddr(document.form.vts_ipaddr_x_0, "") ||
					!validate_portrange(document.form.vts_port_x_0, "") ||
					!validate_range_sp(document.form.vts_lport_x_0, 1, 65535)) return false;							
			else if (document.form.vts_port_x_0.value==""){ 
				alert("\"<#IPConnection_VServerPort_itemname#>\" <#JS_fieldblank#>");	
				document.form.vts_port_x_0.focus();
				return false;
			}
			else{					
				for(i=0; i< VSList.length; i++){
				if(entry_cmp(VSList[i][3].toLowerCase(), document.form.vts_proto_x_0.value.toLowerCase(), 5)==0){
					if(!(portrange_min(document.form.vts_port_x_0.value, 11) > portrange_max(VSList[i][0], 11) ||
						portrange_max(document.form.vts_port_x_0.value, 11) < portrange_min(VSList[i][0], 11))){
						alert('<#JS_duplicate#>');
						return false;
					}
					
					if(entry_cmp(VSList[i][1], document.form.vts_ipaddr_x_0.value.toLowerCase(), 15)==0){
						if(document.form.vts_lport_x_0.value.length!=0){
							if(entry_cmp(VSList[i][2], "", 5)==0){
								if(!(portrange_min(document.form.vts_lport_x_0.value, 5) > portrange_max(VSList[i][0], 11) ||portrange_max(document.form.vts_lport_x_0.value, 5) < portrange_min(VSList[i][0], 11))){
									alert('<#JS_duplicate#>');
									return false;
								}
							}
							else{
								if(portrange_min(document.form.vts_lport_x_0.value,5) == portrange_min(VSList[i][2], 5)){
									alert('<#JS_duplicate#>');
									return false;
								}
							}
						}
						else{
							if(entry_cmp(VSList[i][2], "", 5)==0){
								if(!(portrange_min(document.form.vts_port_x_0.value, 11) > portrange_max(VSList[i][0], 11) ||
									 portrange_max(document.form.vts_port_x_0.value, 11) < portrange_min(VSList[i][0], 11))){
										alert('<#JS_duplicate#>');
										return false;
								}
							}
							else{
								if(!(portrange_min(document.form.vts_port_x_0.value, 11) > portrange_min(VSList[i][2], 5) ||
									portrange_max(document.form.vts_port_x_0.value, 11) < portrange_min(VSList[i][2], 5))){
										alert('<#JS_duplicate#>');
										return false;
									}
								}
							}
						}
					}
				}
			}				
			var vts_port_array = new Array();				
			document.form.vts_protono_x_0.value = "";
		}
	}
	pageChanged = 0;
	pageChangedCount = 0;	
	document.form.action_mode.value = b;
	return true;
}

var vts_rule_array = new Array();
var count = 0;
function split_vts_rule(s){
	if(typeof(s) != "undefined"){
		this.vts_rule_array = s;
	}
	
	if(this.vts_rule_array.length <= 0){
		refreshpage();
		return;
	}
	else{
		document.form.vts_port_x_0.value = this.vts_rule_array[0];
		this.vts_rule_array.shift();
	}
	
	document.form.action = "/start_apply.htm";
	document.form.target = "hidden_frame";
	document.form.action_mode.value = " Add ";
	document.form.current_page.value = "";
	document.form.next_page.value = "";
	/*alert(document.form.vts_port_x_0.value+"\n"+
				document.form.vts_ipaddr_x_0.value+"\n"+
				document.form.vts_lport_x_0.value+"\n"+
				document.form.vts_proto_x_0.value+"\n"+
				document.form.vts_protono_x_0.value+"\n"+
				document.form.vts_desc_x_0.value);//*/
	document.form.submit();
}

function setClientIP(num){
	document.form.vts_ipaddr_x_0.value = clients_info[num][1];
	hideClients_Block();
	over_var = 0;
}

function showLANIPList(){
	var code = "";
	var show_name = "";
	
	for(var i = 0; i < clients_info.length ; i++){
		if(clients_info[i][0] && clients_info[i][0].length > 12)
			show_name = clients_info[i][0].substring(0, 10) + "..";
		else
			show_name = clients_info[i][0];
		
		if(clients_info[i][1]){
			code += '<a href="#"><div onmouseover="over_var=1;" onmouseout="over_var=0;" onclick="setClientIP('+i+');"><strong>'+clients_info[i][1]+'</strong> ';
			if(show_name && show_name.length > 0)
				code += '( '+show_name+')';
			code += ' </div></a>';
		}
	}
	code +='<!--[if lte IE 6.5]><iframe class="hackiframe2"></iframe><![endif]-->';	
	$("ClientList_Block").innerHTML = code;
}

/*------------ Mouse event of fake LAN IP select menu {-----------------*/
function pullLANIPList(obj){
	
	if(isMenuopen == 0){		
		obj.src = "/images/arrow-top.gif"
		$("ClientList_Block").style.display = 'block';		
		document.form.vts_ipaddr_x_0.focus();		
		isMenuopen = 1;
	}
	else
		hideClients_Block();
}
var over_var = 0;
var isMenuopen = 0;

function hideClients_Block(){
	$("pull_arrow").src = "/images/arrow-down.gif";
	$('ClientList_Block').style.display='none';
	isMenuopen = 0;
}
/*----------} Mouse event of fake LAN IP select menu-----------------*/

function showVSList(){
	var code = "";
	code +='<table width="100%" border="1" cellspacing="0" cellpadding="3" align="center" class="list_table">';
	if(VSList.length == 0)
		code +='<tr><td style="color:#CC0000;"><#IPConnection_VSList_Norule#></td></tr>';
	else{
		for(var i = 0; i < VSList.length; i++){
		code +='<tr id="row'+i+'">';
		code +='<td width="">'+ VSList[i][5] +'</td>';			//desp		
		code +='<td width="115">'+ VSList[i][0] +'</td>';		//Port  range
		code +='<td width="125">'+ VSList[i][1] +'</td>';		//local IP
		code +='<td width="55">'+ VSList[i][2] +'&nbsp;</td>';	//local port
		code +='<td width="75">'+ VSList[i][3] +'</td>';		//proto
		code +='<td width="33">'+ VSList[i][4] +'&nbsp;</td>';	//proto no
		code +='<td width=\"27\"><input type=\"checkbox\" name=\"VSList_s\" value='+ i +' onClick="changeBgColor(this,'+i+');" id=\"check'+ i +'\"></td>';
		if(i == 0)
			code +="<td width='80' style='background:#C0DAE4;' rowspan=" + VSList.length + "><input style=\"padding:2px 2px 0px 2px\" class=\"button\" type=\"submit\" onclick=\"markGroup2(this, 'VSList', 24, ' Del ');\" name=\"VSList\" value=\"<#CTL_del#>\"/></td>";
		
	    code +='</tr>';
		}
	}
	code +='<tfoot><tr align="right">';
	code +='<td colspan="8"><input name="button" type="button" class="button" onclick="applyRule();" value="<#CTL_apply#>"/></td>';
	code +='</tr></tfoot>';
    code +='</table>';
	
	$("VSList_Block").innerHTML = code;
}

function changeBgColor(obj, num){
	if(obj.checked)
 		$("row" + num).style.background='#FF9';
	else
 		$("row" + num).style.background='#FFF';
}
</script>
</head>

<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" action="/start_apply.htm" target="hidden_frame" >
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">

<input type="hidden" name="current_page" value="Advanced_VirtualServer_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="IPConnection;">
<input type="hidden" name="group_id" value="VSList">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0" >
	<tr>
		<td width="23">&nbsp;</td>		
		<td valign="top" width="202">				
		<div  id="mainMenu"></div>	
		<div  id="subMenu"></div>		
		</td>				
		
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div><br />
		<!--===================================Beginning of Main Content===========================================-->		
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td valign="top" >
		
<table width="100%" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle">
	<thead>
	<tr>
		<td><#t1NAT#> - <#menu5_3_4#></td>
	</tr>
	</thead>
	
	<tr>
	  <td bgcolor="#FFFFFF"><#IPConnection_VServerEnable_sectiondesc#>
		<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" class="FormTable">
          <tr>
            <th colspan="3"><#IPConnection_VServerEnable_itemname#><input type="hidden" name="vts_num_x_0" value="<% nvram_get_x("IPConnection", "vts_num_x"); %>" readonly="1" /></th>
            <td colspan="3">
			  <input type="radio" value="1" name="vts_enable_x" class="content_input_fd" onclick="return change_common_radio(this, 'IPConnection', 'vts_enable_x', '1')" <% nvram_match_x("IPConnection","vts_enable_x", "1", "checked"); %> />Yes
			  <input type="radio" value="0" name="vts_enable_x" class="content_input_fd" onclick="return change_common_radio(this, 'IPConnection', 'vts_enable_x', '0')" <% nvram_match_x("IPConnection","vts_enable_x", "0", "checked"); %> />No
            </td>
		  </tr>
		  
		  <tr>
				<th colspan="3" align="right"><#IPConnection_VSList_groupitemdesc#></th>
				<td colspan="3" bgcolor="#a7d2e2" id="VSList">
			  	<select name="KnownApps" id="KnownApps" class="input" onchange="change_wizard(this, 'KnownApps');"></select>
				</td>
		  </tr>
		  
			<tr>
				<th colspan="3" align="right"><#IPConnection_VSList_gameitemdesc#></th>
				<td colspan="3" bgcolor="#a7d2e2" id="VSGameList">
					<select name="KnownGames" id="KnownGames" class="input" onchange="change_wizard(this, 'KnownGames');"></select>
				</td>
			</tr>
      </table>			
		<table width="100%" border="1" align="center" cellpadding="3" cellspacing="0" class="FormTable">
		  <thead>
			<tr>
              <td colspan="7"><#IPConnection_VSList_title#></td>
            </tr>
 		  </thead>
          <tr>
            <th width="" style="text-align:center;"><#IPConnection_VServerDescript_itemname#></th>
            <th width="90" style="text-align:center;"><#IPConnection_VServerPort_itemname#></th>
            <th width="145" style="text-align:center;"><#IPConnection_VServerIP_itemname#></th>
            <th width="55" style="text-align:center;"><#IPConnection_VServerLPort_itemname#></th>
            <th width="75" style="text-align:center;"><#IPConnection_VServerProto_itemname#></th>
            <th width="67" style="text-align:center;"><#IPConnection_VServerPNo_itemname#></th>
						<th width="50">&nbsp;</th>
          </tr>
          
          <tr align="center">
  			<td align="center">
  				<input type="text" maxlength="30" size="15" name="vts_desc_x_0" class="input" onkeypress="return is_string(this)" /></td>
        <td align="center">
				<input type="text" size="15" class="input" name="vts_port_x_0" onkeypress="return is_portrange(this)" />
			</td>
			<td align="center">
				<input type="text" maxlength="15" size="14" class="input" name="vts_ipaddr_x_0" onkeypress="return is_ipaddr(this)" onkeyup="change_ipaddr(this)" style="float:left;"/ autocomplete="off" onblur="if(!over_var){hideClients_Block();}" onClick="hideClients_Block();">
				<img id="pull_arrow" src="images/arrow-down.gif" onclick="pullLANIPList(this);" title="Select the IP of DHCP clients." onmouseover="over_var=1;" onmouseout="over_var=0;">			</td>
			<td align="center"><input type="text" maxlength="5" size="3" class="input" name="vts_lport_x_0" onkeypress="return is_number(this)" /></td>
			<td align="center">
				<select name="vts_proto_x_0" class="input">
					<option value="TCP">TCP</option>
					<option value="UDP">UDP</option>
					<option value="BOTH">BOTH</option>
					<option value="OTHER">OTHER</option>
				</select>			</td>
			<td align="left"><input type="text" class="input" maxlength="3"  size="1" name="vts_protono_x_0" onkeypress="return is_number(this)" /></td>
			<td align="center" valign="bottom" width="50">			
				<input class="button" type="submit" onclick="return markGroup2(this, 'VSList', 24, ' Add ');" name="VSList2" value="<#CTL_add#>"/>
		  </td></tr>
        </table>
		<div id="ClientList_Block" class="ClientList_Block"></div>
		<div id=VSList_Block></div>
		</td>
		</tr>
	</table>
</td>
          <td style="width:15px;" valign="top">
		  </td>
        </tr>
      </table>				
		<!--===================================Ending of Main Content===========================================-->		
	</td>
		
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</form>
<script>

</script>
</body>
</html>
