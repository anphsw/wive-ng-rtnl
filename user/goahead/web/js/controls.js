/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function showElement(elementID)
{
	var item = (typeof(elementID)=='object') ? 
		elementID : document.getElementById(elementID);
	if (item != null)
	{
		item.style.display = "";
		item.style.visibility = "visible";
	}
}

function showElementEx(elementID, display)
{
	var item = (typeof(elementID)=='object') ? 
		elementID : document.getElementById(elementID);
	if (item != null)
	{
		item.style.display = display;
		item.style.visibility = "visible";
	}
}

function hideElement(elementID)
{
	var item = (typeof(elementID)=='object') ? 
		elementID : document.getElementById(elementID);
	if (item != null)
	{
		item.style.display = "none";
		item.style.visibility = "hidden";
	}
}

function displayElement(elementID, visible)
{
	if (visible)
		showElement(elementID);
	else
		hideElement(elementID);
}
