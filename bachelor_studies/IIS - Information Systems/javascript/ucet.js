function SidePanelHide1()
{
	document.getElementById('hidden').style.display="none";
}

function SidePanelHide2()
{
	document.getElementById('hidden2').style.display="none";
}

function SidePanelHide3()
{
    document.getElementById('hidden3').style.display="none";
}

function SidePanelHide4()
{
    document.getElementById('hidden4').style.display="none";
}


function SidePanelShow(which)
{
	if(!document.getElementById)
		return
	if (which.style.display=="block")
		which.style.display="none"
	else
		which.style.display="block"
}

function sFocus (field) {
    if(field.value == 'Hledat výraz') 
        field.value = '';
}

function sBlur (field) {
    if (field.value == '') 
        field.value = 'Hledat výraz';
}

function openWin() {
    autori = window.open("http://iis.pesvan.cz/pages/autori.php", "autori", "width=300, height=600");    // Opens a new window
}

function closeWin() {
    autori = window.close("autori");
}

