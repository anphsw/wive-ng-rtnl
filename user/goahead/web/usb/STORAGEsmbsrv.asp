<!DOCTYPE html>
<html>
<head>
<title>SAMBA Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">
var dir_count = 0;


function initValue(form)
{
	var smbEnabled = defaultNumber("<% getCfgZero(1, "SmbEnabled"); %>", '1');
	var smbTimeserver = defaultNumber("<% getCfgZero(1, "SmbTimeserver"); %>", '0');
	
	form.SmbEnabled.value = (smbEnabled != '1') ? '0' : '1';
	form.SmbTimeserver.value = (smbTimeserver != '1') ? '0' : '1';
	smbEnabledSwitch(form);
}
function smbEnabledSwitch(form)
{
	disableElement( [ form.WorkGroup, form.SmbNetBIOS, form.SmbString, form.SmbOsLevel, form.SmbTimeserver  ] , form.SmbEnabled.value != '1');
}

function checkForm(form)
{
	if (form.SmbEnabled.value == '1')
	{
		if (form.WorkGroup.value == '')
		{
			alert('Workgroup not specified.');
			form.WorkGroup.focus();
			return false;
		}
		
		if (form.SmbNetBIOS.value == '')
		{
			alert('NetBIOS name not specified.');
			form.SmbNetBIOS.focus();
			return false;
		}
		
		// Check OS level
		var os_level = -1;
		if (validateNum(form.SmbOsLevel.value, false))
			os_level = 1*form.SmbOsLevel.value;
		
		if ((os_level < 0) || (os_level > 255))
		{
			alert('Invalid OS level value [0-255].');
			form.SmbOsLevel.focus();
			return false;
		}
	}
	return true;
}


</script>
</head>

<body onLoad="initValue(document.storageSmbSrv)">
<table class="body">
  <tr>
    <td><h1 id="smbTitle">Samba/CIFS Setup</h1>
      <p id="smbIntroduction">Here you can configure Samba/CIFS service.</p>
      <hr />
      <form action="/goform/storageSmbSrv" method=POST name=storageSmbSrv  onsubmit="return checkForm(this);" >
        <table class="form">
          <tr>
            <td class="title" colspan="2">Samba/CIFS Settings</td>
          </tr>
          <tr>
            <td class="head">Enable Samba</td>
            <td><select name="SmbEnabled" class="half" onChange="smbEnabledSwitch(this.form);">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head">Workgroup</td>
            <td><input name="WorkGroup" class="mid" value="<% getCfgGeneral(1, "WorkGroup"); %>" ></td>
          </tr>
          <tr>
            <td class="head">Netbios name</td>
            <td><input name="SmbNetBIOS" class="mid" value="<% getCfgGeneral(1, "SmbNetBIOS"); %>" ></td>
          </tr>
          <tr>
            <td class="head">Server string</td>
            <td><input name="SmbString" class="mid" value="<% getCfgGeneral(1, "SmbString"); %>" ></td>
          </tr>
          <tr>
            <td class="head">OS level</td>
            <td><input name="SmbOsLevel" class="half" value="<% getCfgGeneral(1, "SmbOsLevel"); %>" ></td>
          </tr>
          <tr>
            <td class="head">Enable time server</td>
            <td><select name="SmbTimeserver" class="half">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select></td>
          </tr>
        </table>
        <table class="buttons">
          <tr align="center">
            <td><input type="hidden" value="/usb/STORAGEsmbsrv.asp" name="submit-url">
              <input type=submit class="normal" value="Apply" id="smbApply">
              <input type=button class="normal" value="Cancel" id="smbCancel" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
