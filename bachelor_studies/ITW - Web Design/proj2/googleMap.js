function initialize()
{
	var myLatlng = new google.maps.LatLng(48.2055198,18.613761);
	var map_canvas = document.getElementById('map_canvas');
	var mapOptions = 
	{
	  center: myLatlng,
	  zoom: 17,
	  mapTypeId: google.maps.MapTypeId.HYBRID
	}
	var map = new google.maps.Map(map_canvas, mapOptions);

	var marker = new google.maps.Marker({
		position: myLatlng,
		map: map,
		title: 'Lekáreň Dobrá Bylinka'
	});
}

function initialize2()
{
	var myLatlng = new google.maps.LatLng(48.2055198,18.613761);
	var map_canvas = document.getElementById('map_canvas2');
	var mapOptions = 
	{
	  center: myLatlng,
	  zoom: 15,
	  mapTypeId: google.maps.MapTypeId.TERRAIN
	}
	var map = new google.maps.Map(map_canvas, mapOptions);

	var marker = new google.maps.Marker({
		position: myLatlng,
		map: map,
		title: 'Lekáreň Dobrá Bylinka'
	});
}