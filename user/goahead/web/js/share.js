function disableTextField (field)
{
	if (document.all || document.getElementById)
		field.disabled = true;
	else
	{
		field.oldOnFocus = field.onfocus;
		field.onfocus = skip;
	}
}

function enableTextField(field)
{
	if (document.all || document.getElementById)
		field.disabled = false;
	else
		field.onfocus = field.oldOnFocus;
}
