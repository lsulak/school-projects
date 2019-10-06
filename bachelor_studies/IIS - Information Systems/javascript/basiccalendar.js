
var themonths=['Leden  /  January','Unor  /  February',
 				'Brezen  /  March','Duben  /  April',
 				'Kveten  /  May','Cerven  /  June', 
 				'Cervenec  /  July','Srpen  /  August',
 				'Zari  /  September','Rijen  /  October',
 				'Listopad  /  November','Prosinec  /  December']

var todaydate=new Date()
var curmonth=todaydate.getMonth()+1 //get current month (1-12)
var curyear=todaydate.getFullYear() //get current year

function buildCal(m, y, cM, cH, cDW, cD, brdr)
{
	var dim=[31,0,31,30,31,30,31,31,30,31,30,31];

	var oD = new Date(y, m-1, 1); //DD replaced line to fix date bug when current day is 31st
	oD.od=oD.getDay()+1; //DD replaced line to fix date bug when current day is 31st

	var scanfortoday=(y==todaydate.getFullYear() &&	
	m == todaydate.getMonth()+1)? todaydate.getDate() : 0 //DD added

	dim[1]=(((oD.getFullYear()%100!=0)&&(oD.getFullYear()%4==0))
		||(oD.getFullYear()%400==0))?29:28;

	var t='<div class="'+cM+'"><table class="'+cM+
	'" cols="7" cellpadding="0" border="'+brdr+
	'" cellspacing="0"><tr align="center">';

	t += '<td colspan="7" align="center" class="'+cH+'">'+themonths[m-1]+' - '+y+
	'</td></tr><tr align="center">';


	t+='<td class="'+cDW+'">'+"Po"+'</td>';
	t+='<td class="'+cDW+'">'+"Ut"+'</td>';
	t+='<td class="'+cDW+'">'+"St"+'</td>';
	t+='<td class="'+cDW+'">'+"Čt"+'</td>';
	t+='<td class="'+cDW+'">'+"Pá"+'</td>';
	t+='<td class="'+cDW+'">'+"So"+'</td>';
	t+='<td class="'+cDW+'">'+"Ne"+'</td>';


	t+='</tr><tr align="center">';
	
	for(i = 2; i <= 36; i++)
	{
		var x=((i-oD.od >= 0) && (i-oD.od < dim[m-1])) ? i-oD.od+1 : '&nbsp;';
		
		if (x == scanfortoday) //DD added
			x = '<span id="today">'+x+'</span>' //DD added
		
		t += '<td class="'+cD+'">'+x+'</td>';
	
		if(((i) % 7 == 1) && (i < 37))
			t+='</tr><tr align="center">';
	}
	return t+='</tr></table></div>';
}

function updatecalendar(theselection)
{
	var themonth=parseInt(theselection[theselection.selectedIndex].value)+1;
	var calendarstr=buildCal(themonth, curyear, "main", "month", "daysofweek", "days", 1);

	if (document.getElementById)
	  document.getElementById("calendarspace").innerHTML=calendarstr
}
