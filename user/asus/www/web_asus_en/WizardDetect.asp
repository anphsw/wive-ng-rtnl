<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="javascript">

function redirect()
{  
   window.location.href="index.asp"
   
}

function buttonOver(o) 
{
    o.style.backgroundColor = "#FFFFCC";
}

function buttonOut(o) {
    o.style.backgroundColor = "#C0C0C0";
}

function blocking(nr,show)
{
        if (document.getElementById)
        {
                var vista = (show) ? 'block' : 'none';
                document.getElementById(nr).style.display = vista;
        }
                                                                                                                                               
        else if (document.layers)
        {
                var current = (show) ? 'block' : 'none';
                document.layers[nr].display = current;
        }
        else if (document.all)
        {
                var current = (show) ? 'block' : 'none';
                document.all[nr].style.display = current;
        }
}

function check_wanlink(){
        if ("<% nvram_get_x("", "wan_route_X"); %>" == "IP_Bridged"){
                blocking('wan_link',false);
        }
        else{
                blocking('wan_link',true);
        }
}

</script>

</head>
<body bgcolor="#FFFFFF" onload="check_wanlink();">
<form name="formname" method="POST">
<table width="420" border="0" cellpadding="0" cellspacing="0">
  <tr bgcolor="#FFFFFF"> 
    <td width="420"> 
      <table width="420" border="0" cellpadding="0" cellspacing="0" border="0">
        <tr> 
          <td bgcolor="#FFFFFF" height="50" colspan="2"><b><font size="5" face="Arial" color="#FF0000"><#STS_title#></font></b></td>
        </tr>

        <tr>
	   <td width="70" height="30" class="content_header_td_less_sp"><font face="Arial" color="#000000"><#STS_type#></font></td>
	   <td class="content_input_td">
	   	<input type="text" class="content_input_fd" size="36" name="PPPConnection_x_WANType" value="<% nvram_get_x("Layer3Forwarding","wan_proto_t"); %>" readonly="1">
	   </td>
	</tr>
      </table>

	<div id="wan_link" style="display:none">
      <table width="420" border="0" cellpadding="0" cellspacing="0" border="0">
	<tr>
	   <td width="70" height="30" class="content_header_td_less_sp"><font face="Arial" color="#000000"><#STS_status#></font></td>
	   <td class="content_input_td">
	   	<input type="text" class="content_input_fd" size="36" name="PPPConnection_x_WANLink" value="<% nvram_get_f("wanstatusshow.log","wan_status_t"); %>" readonly="1">
	   </td>
	</tr>

	<tr>
	   <td width="70" height="30" class="content_header_td_less_sp"><font face="Arial" color="#000000"><#STS_reason#></font></td>
	   <td class="content_input_td">
	   	<input type="text" class="content_input_fd" size="36" name="PPPConnection_x_WANStatus" value="<% nvram_get_f("wanstatusshow.log","wan_reason_t"); %>" readonly="1">
	   	<input type="hidden" class="content_input_fd" size="36" name="PPPConnection_x_WANStatusCode" value="<% nvram_get_f("wanstatusshow.log","wan_reasoncode_t"); %>" readonly="1">
	   </td>
	</tr>
       </table>
        </div>

      <table width="420" height="125" border="0" cellpadding="0" cellspacing="0" border="0">
	<tr> 
          <td width="420" height="30" bgcolor="#FFFFFF" colspan="2">          
          </td>
        </tr>

        <tr>           
          <td align="right" width="420" height="60" bgcolor="#FFFFFF" colspan="2">
          	<input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver=buttonOver(this) type="button" onClick="redirect()" value="<#CTL_login#>" name="action">
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
</form>
</body>
</html>
