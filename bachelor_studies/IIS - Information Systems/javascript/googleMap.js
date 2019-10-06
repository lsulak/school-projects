function initialize()
{
	var myLatlng = new google.maps.LatLng(49.834547, 18.292108);
	var map_canvas = document.getElementById('map_canvas');
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
		title: 'Knihovna ITI'
	});
}
