﻿<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<script>
var action = '<% get_parameter("wanaction"); %>';
var restart_time;

function restart_needed_time(second){
	restart_time = second;
}

function Callback(){
	parent.showLoading(restart_time);
	setTimeout("document.redirectForm.submit();", restart_time*1000)
	return;
}

</script>
</head>

<body onLoad="Callback();">
<% wan_action(); %>
<form method="post" name="redirectForm" action="/" target="_parent">
	<input type="hidden" name="flag" value="Internet">
</form>
</body>
</html>
