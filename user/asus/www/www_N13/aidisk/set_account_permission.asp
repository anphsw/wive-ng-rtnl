<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">

<script type="text/javascript">
var protocol = '<% get_parameter("protocol"); %>';
var flag = '<% get_parameter("flag"); %>';

function set_account_permission_error(error_msg){
	if(flag == "aidisk_wizard")
		parent.alert_error_msg("<#AiDisk_Wizard_failedreson1#>");
	else
		parent.alert_error_msg(error_msg);
}

function set_account_permission_success(){
	parent.submitChangePermission(protocol);
}
</script>
</head>

<body>

<% set_account_permission(); %>

</body>
</html>
