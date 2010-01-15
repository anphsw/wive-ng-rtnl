<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>Untitled Document</title>
<link href="../NM_style.css" rel="stylesheet" type="text/css">
<link rel="stylesheet" type="text/css" href="../form_style.css">

<script type="text/javascript" src="../state.js"></script>
<script>

function initial(){

}

function refreshpage(){
	location.href=location.href;
}
</script>
</head>

<body class="statusbody" onload="initial();">

<form method="post" name="form" action="start_apply.htm">
<input type="hidden" name="current_page" value="">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="sid_list" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="productid" value="<% nvram_get_x("",  "productid"); %>">
<input type="hidden" name="load_script" value="<% load_script("wan.sh"); %>">

<table width="97%" border="1" align="center" bordercolor="#6b8fa3" cellpadding="4" cellspacing="0" class="table1px">
	<tr>
		<th width="125"><#HSDPAConfig_ConnStatus_itemname#>:</th>
		<td width="140">
			<% nvram_get_f("wan.log","wan2_status_t"); %>
			<input class="button" type="submit" onClick="return onSubmitApply('dhcpc_release')" value="<#Disconnect#>"/>
			<input class="button" type="submit" onClick="return onSubmitApply('dhcpc_renew')" value="<#Connect#>"/>
		</td>
  </tr>
  <tr>
    <th><#HSDPAConfig_hsdpa_mode_itemname#>:</th>
    <td><% nvram_get_f("wan.log","hsdpa_mode_t"); %></td>
  </tr>
  <tr>
    <th><#WAN_IP#>:</th>
    <td><% nvram_get_f("wan.log","wan2_ipaddr_t"); %></td>
  </tr>
  <tr>
  <tr>
    <th><#HSDPAConfig_Subnetmask_itemname#>:</th>
    <td><% nvram_get_f("wan.log","wan2_netmask_t"); %></td>
  </tr>  
  <tr>
    <th><#HSDPAConfig_DefGateway_itemname#>:</th>
    <td><% nvram_get_f("wan.log","wan2_gateway_t"); %></td>
  </tr>
  <tr>
  <tr>
    <th><#HSDPAConfig_DNSServers_itemname#>:</th>
    <td><% nvram_get_f("wan.log","wan2_dns_t"); %></td>
  </tr>
  <tr>
    <th><#HSDPAConfig_BaseStationType_itemname#>:</th>
    <td><% nvram_get_f("wan.log","basestation_type"); %></td>
  </tr>
  <tr>
    <th><#HSDPAConfig_ConnectionTime_itemname#>:</th>
    <td><% nvram_get_f("wan.log","connection_time"); %></td>
  </tr>
  <tr>
    <th><#HSDPAConfig_TransDataSize_itemname#>:</th>
    <td><% nvram_get_f("wan.log","transdata_size"); %> <#Bytes#></td>
  </tr>
  <tr>
    <th><#HSDPAConfig_ReceviedDataSize_itemname#>:</th>
    <td><% nvram_get_f("wan.log","recevdata_size1"); %> <#Bytes#></td>
  </tr>
  <tr>
    <th><#HSDPAConfig_TotalDataSize_itemname#>:</th>
    <td><% nvram_get_f("wan.log","totaldata_size1"); %> <#Bytes#></td>
  </tr>   
	<tr align="right">
	  <td colspan="2"><input type="button" class="button" onclick="refreshpage();" value="<#CTL_refresh#>"></td>
	</tr>
</table>
</form>
</body>
</html>
