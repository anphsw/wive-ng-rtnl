/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function createXMLHttp()
{
	var xmlHttp;

	try
	{	
		xmlHttp = new XMLHttpRequest();// Firefox, Opera 8.0+, Safari
	}
	catch (e)
	{
		try
		{
			xmlHttp = new ActiveXObject("Msxml2.XMLHTTP"); // Internet Explorer
		}
		catch (e)
		{
			try
			{
				xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch (e)
			{
				alert("No AJAX support!");
				return null;
			}
		}
	}
	
	return xmlHttp;
}

function ajaxPerformRequest(uri)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;
	
	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("GET", uri, true);
	xmlHttp.send(null);
}

function ajaxPostRequest(uri, content, refresh)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;
	
	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			// Free resources
			if (refresh)
				window.location.reload();
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("POST", uri, true);
	xmlHttp.send(content);
}

function ajaxLoadElement(elementID, url)
{
	var element = document.getElementById(elementID);
	if (element == null)
		return;

	// Create XMLHttpRequest
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (xmlHttp.status == 200)
				element.innerHTML = xmlHttp.responseText;
			else
				element.innerHTML = '<b>' + xmlhttp.statusText + '</b>';
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("GET", url, true);
	xmlHttp.send(null);
}

function ajaxLoadScript(scriptFile)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (xmlHttp.status == 200)
			{
				try
				{
					eval(xmlHttp.responseText);
				}
				catch (e)
				{
					alert("Error evaluating " + scriptFile + ": " + e);
				}
			}
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("GET", scriptFile, true);
	xmlHttp.send(null);
}
