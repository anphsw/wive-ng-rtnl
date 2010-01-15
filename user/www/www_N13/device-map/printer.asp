<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>Untitled Document</title>
<link href="../NM_style.css" rel="stylesheet" type="text/css">
<link rel="stylesheet" type="text/css" href="../form_style.css">

<script type="text/javascript" src="/state.js"></script>
<script>
var printer_manufacturer_array = parent.printer_manufacturers();
var printer_model_array = parent.printer_models(); 
var printer_pool_array = parent.printer_pool();

function initial(){
	if(printer_model_array.length > 0 ) {
		showtext($("printerModel"), printer_manufacturer_array[0]+" "+printer_model_array[0]);
		
		if(printer_pool_array[0] != "")
			showtext($("printerStatus"), '<#CTL_Enabled#>');
		else
			showtext($("printerStatus"), '<#CTL_Disabled#>');
	}
	else
		showtext($("printerStatus"), '<% translate_x("System_Internet_Details_Item5_desc2"); %>');
}

function cleanTask(){
	parent.showLoading(3);
	
	document.form.action_mode.value = "Update";
	document.form.action_script.value = "mfp_monopolize";
	document.form.current_page.value = "";
	document.form.next_page.value = "index.asp";
	
	document.form.submit();
}
</script>
</head>

<body class="statusbody" onload="initial();">

<form method="post" name="form" action="/start_apply.htm">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">
<input type="hidden" name="current_page" value="Main_GStatus_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">

<table width="95%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="table1px">
	<tr>
		<th width="50%"><span class="top-messgae"><#PrinterStatus_x_PrinterModel_itemname#></span></th>
		<td width="130"><span id="printerModel"></span></td>
	</tr>
	<tr>
		<th><span class="top-messgae"><#Printing_status#></span></th>
		<td><span id="printerStatus"></span></td>
	</tr>
	<tr>
		<th><#Printing_button_item#></th>
		<td><input type="button" class="button" value="<#btn_Enable#>" onclick="cleanTask();"></td>
	</tr>
</table>
</form>
</body>
</html>
