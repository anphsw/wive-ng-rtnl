<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">

<script type="text/javascript">
function delete_sharedfolder_error(error_msg){
	parent.alert_error_msg(error_msg);
}

function delete_sharedfolder_success(){
	parent.refreshpage();
}
</script>
</head>

<body>
<% delete_sharedfolder(); %>
</body>
</html>
