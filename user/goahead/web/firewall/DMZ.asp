<html>
<head>
<title>MAC Filtering Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

function deleteClick()
{
    return true;
}


function checkRange(str, num, min, max)
{
	d = atoi(str,num);
	if(d > max || d < min)
		return false;
	return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}


function checkIPAddr(field)
{
    if(field.value == ""){
        alert("IP address cannot empty.");
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        alert('It should be a [0-9] number.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        alert('Invalid IP address.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

   return true;
}


function formCheck()
{
	if(!document.DMZ.DMZEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if ( !checkIPAddr(document.DMZ.DMZIPAddress) ) {
		document.DMZ.DMZIPAddress.focus();
		return false;
	}

	return true;
}


function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	var e = document.getElementById("dmzTitle");
	e.innerHTML = _("dmz title");
	e = document.getElementById("dmzIntroduction");
	e.innerHTML = _("dmz introduction");

	e = document.getElementById("dmzSetting");
	e.innerHTML = _("dmz setting");
	e = document.getElementById("dmzSet");
	e.innerHTML = _("dmz setting");
	e = document.getElementById("dmzDisable");
	e.innerHTML = _("firewall disable");
	e = document.getElementById("dmzEnable");
	e.innerHTML = _("firewall enable");
	e = document.getElementById("dmzIPAddr");
	e.innerHTML = _("dmz ipaddr");
	e = document.getElementById("dmzApply");
	e.value = _("firewall apply");
	e = document.getElementById("dmzReset");
	e.value = _("firewall reset");
}

function updateState()
{
	initTranslation();
	if(document.DMZ.DMZEnabled.options.selectedIndex == 1){
		enableTextField(document.DMZ.DMZIPAddress);
	}else{
		disableTextField(document.DMZ.DMZIPAddress);
	}
}

</script>
</head>


<!--     body      -->
<body onload="updateState()">
<table class="body"><tr><td>
<h1 id="dmzTitle"> DMZ Settings </h1>
<% checkIfUnderBridgeModeASP(); %>
<p id="dmzIntroduction"> Here you can setup the De-Militarized Zone (DMZ) to separate your external services from the rest of LAN.</p>
<hr />

<form method="POST" name="DMZ" action="/goform/DMZ">
<table class="form">
<tr>
	<td class="title" colspan="2" id="dmzSetting">DMZ Settings</td>
</tr>
<tr>
	<td class="head" id="dmzSet">
		DMZ Settings
	</td>
	<td>
	<select onChange="updateState()" name="DMZEnabled" class="half">
		<option value=0 <% getDMZEnableASP(0); %> id="dmzDisable">Disable</option>
		<option value=1 <% getDMZEnableASP(1); %> id="dmzEnable">Enable</option>
	</select>
	</td>
</tr>

<tr>
	<td class="head" id="dmzIPAddr">
		DMZ IP Address
	</td>
	<td>
  		<input type="text" size="24" name="DMZIPAddress" value=<% showDMZIPAddressASP(); %> >
	</td>
</tr>
</table>

<table class="buttons">
<tr><td>
	<input type="submit" value="Apply" id="dmzApply" name="addDMZ" onClick="return formCheck()"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="dmzReset" name="reset">
	<input type="hidden" name="submit-url" value="/firewall/DMZ.asp" >
</td></tr>
</table>

</form>

<div class="whitespace">&nbsp;</div>

</td></tr></table>
</body>
</html>
