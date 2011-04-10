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

function disableElement(element, disabled)
{
	if (typeof(element) == 'string')
	{
		element = document.getElementById(element);
		if (element != null)
			element.disabled = disabled;
	}
	else if (element instanceof Array)
	{
		for (var i=0; i<element.length; i++)
			disableElement(element[i], disabled);
	}
	else if (typeof(element) == 'object')
		element.disabled = disabled;
}

self.getElementValue = function(element)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.value;
}

self.setElementValue = function(element, value)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.value = value;
}

self.getElementChecked = function(element)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.checked;
}

self.setElementChecked = function(element, checked)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.checked = checked;
}

self.enableElements = function(elements, enable)
{
	if (elements == null)
		return;
	if (enable == null)
		enable = true;
	if (typeof(elements) == 'string')
	{
		elements = document.getElementById(elements);
		if (elements == null)
			return;
	}

	if (elements instanceof Array)
	{
		for (var i=0; i<elements.length; i++)
			enableElements(elements[i], enable);
	}
	else // Object
		elements.disabled = (enable) ? false : true;
}
