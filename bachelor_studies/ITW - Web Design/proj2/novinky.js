function hide()
{
	document.getElementById('hidden').style.display="none";
}

function hide_show(which)
{
	if(!document.getElementById)
		return
	if (which.style.display=="block")
		which.style.display="none"
	else
		which.style.display="block"
}
