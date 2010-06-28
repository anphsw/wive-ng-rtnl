function validateNum(str, floating)
{
	var re = (floating) ? /^\d+(?:\.\d+)$/ : /^\d+$/;
	return re.test(str);
}

function validateMAC(mac, info)
{
	var re = /^(?:[A-Fa-f0-9]{2}\:){5}[A-Fa-f0-9]{2}$/;
	
	if (!re.test(mac))
	{
		if (info)
			alert("Please fill the MAC Address in correct format! (XX:XX:XX:XX:XX:XX)");
		return false;
	}
	return true;
}

function validateIP(ip, info)
{
	var re = /^(?:(?:0|1\d{0,2}|2([0-4]\d?|5[0-5]?|[6-9]?)|[3-9]\d?)\.){3}(?:0|1\d{0,2}|2([0-4]\d?|5[0-5]?|[6-9]?)|[3-9]\d?)$/;
	
	if (ip.value == "")
	{
		if (info)
			alert("Error. IP address is empty.");
		return false;
	}
	if (!re.test(ip.value))
	{
		if (info)
			alert("Error. Invalid IP address format! (0-255.0-255.0-255.0-255)");
		return false;
	}
	
	return true;
}

function validateIPMask(ip, info)
{
	var re = /^(?:(?:0|128|192|224|240|248|252|254)\.0+\.0+\.0+|255\.(?:(?:0|128|192|224|240|248|252|254)\.0+\.0+|255\.(?:(?:0|128|192|224|240|248|252|254)\.0+|255\.(?:0|128|192|224|240|248|252|254|255))))$/;
	
	if (ip.value == "")
	{
		if (info)
			alert("Error. Mask is empty.");
		return false;
	}
	if (!re.test(ip.value))
	{
		if (info)
			alert("Error. Invalid IP mask format!");
		return false;
	}
	
	return true;
}

function checkDigitRange(value, start, stop)
{
	var re = /^\d+$/;

	if (re.test(value))
	{
		var val = parseInt(value);
		return (val>=start) && (val<=stop);
	}
	
	return false;
}
