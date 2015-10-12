// JSON Online viewer : http://jsonviewer.stack.hu/

var temp_unit = 0;
var gps = '0';
var town = "Paris, FR";
var owner = "";
var dnd = 1;
var dndperiodstart = 22;
var dndperiodend = 6;
var hourlyvibe = 0;
var interval=30;
var time_shift=0;
var invert=0;
var showseconds=0;
var winddir = 0;
var windforce = 0;

var just_launched = false;

var api_key = "a86e5408cc73fb090d2ed8f4176d0a3b";

var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function f_to_c (temp_f) {
	var temp_c;
	temp_c = Math.round((temp_f-32)/1.8);
	return temp_c;
}

function mph_to_kmh (temp_mph) {
  
  var temp_kmh;
  temp_kmh = Math.round(temp_mph * 1.609);
  return temp_kmh;

}

function ask_Yahoo(where) {
	if (where === null) {
		where ="Paris, FR";
	}
	var yahoo_url = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20" + 
			"where%20woeid%20in%20%28select%20woeid%20from%20geo.places%281%29%20where%20text%3D%22" + 
			encodeURIComponent(where) + '%22%29&format=json&diagnostics=true' +
			'&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=';
  console.log ("Yahoo URL : " + yahoo_url);
	xhrRequest(yahoo_url, 'GET', 
		function(responseText) {
			// responseText contains a JSON object with weather info
      console.log ("Yahoo said : " + responseText);
      var json = JSON.parse(responseText);

			
			// Conditions
			var conditions = json.query.results.channel.item.condition.code;   
			if (conditions > 255) {
				conditions = 255;
				console.log (responseText);
			}

			var feelslike;
			var temperature;
			
			if (temp_unit === 0) {
				feelslike = f_to_c(json.query.results.channel.wind.chill);
				temperature = f_to_c(json.query.results.channel.item.condition.temp);
        windforce = mph_to_kmh(json.query.results.channel.wind.speed);
			} else {

				feelslike = json.query.results.channel.wind.chill;
				temperature = json.query.results.channel.item.condition.temp;        
        windforce = json.query.results.channel.wind.speed;
			}
      
      winddir = json.query.results.channel.wind.direction;

      
			//console.log ("where am i : " + where + " temperature : " + temperature + " Windchill : " + feelslike);
			var dictionary = {
					"KEY_TEMPERATURE": String(temperature),
					"KEY_CONDITIONS": parseInt(conditions),
					"KEY_FEELSLIKE": String(feelslike),
					"KEY_TOWN": where,    
					"KEY_OWNER": owner,
					"KEY_DND": parseInt(dnd),
					"KEY_DNDPERIODSTART": parseInt(dndperiodstart),
					"KEY_DNDPERIODEND": parseInt(dndperiodend),
					"KEY_HOURLYVIBE": parseInt(hourlyvibe),
					"KEY_INTERVAL": parseInt(interval),
					"KEY_SHIFTTIME": parseInt (time_shift),
					"KEY_INVERT": parseInt (invert),
					"KEY_SECONDS": parseInt (showseconds),
					"KEY_UNIT": parseInt (temp_unit),
          "KEY_WINDDIR": parseInt (winddir),
          "KEY_WINDFORCE": String (windforce),        
				};

			// Send to Pebble
			Pebble.sendAppMessage(dictionary,
				function(e) {
					console.log("Weather info sent to Pebble successfully!");
				},
				function(e) {
					console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " for data " + JSON.stringify(dictionary) );
					
				}
			);          
		}
	);
} 


function push_forecast (json,name) {

	var epoch = json.dt;
	
	var date = new Date (epoch * 1000);
	var now = new Date ();
	var tinyicon;
  
//  console.log ("JSON to push to timeline : " + JSON.stringify (json));
	
  switch (json.weather[0].icon) {
		case "01d":
		case "01n":
			tinyicon = "system://images/TIMELINE_SUN";
		break;
		case "02d":
		case "02n":
			tinyicon = "system://images/PARTLY_CLOUDY";
		break;
		case "03d":
		case "03n":
			tinyicon = "system://images/CLOUDY_DAY";
		break;
		case "04d":
		case "04n":
			tinyicon = "system://images/PARTLY_CLOUDY";
		break;
		case "09d":
		case "09n":
				tinyicon = "system://images/LIGHT_RAIN";
		break;
		case "10d":
		case "10n":
				tinyicon = "system://images/LIGHT_RAIN";
		break;
		case "11d":
		case "11n":
				tinyicon = "system://images/HEAVY_RAIN";
		break;
		case "13d":
		case "13n":
				tinyicon = "system://images/HEAVY_SNOW";
		break;
		default:
					tinyicon = "system://images/TIMELINE_WEATHER";
		break;

	}

	var pin = {
		"id": "pin-" + json.dt,
		"time": date.toISOString(),
		"layout": {
			"type": "weatherPin",
			"title": json.weather[0].main,
			"subtitle": Math.round(json.main.temp-273.15)+" C",
			"tinyIcon": tinyicon,
			"largeIcon": tinyicon,
			"locationName": name,
			"body": json.weather[0].description,
			"lastUpdated": now.toISOString()
		}
	};

  insertUserPin(pin, function(responseText) { 
		console.log('Result: ' + responseText);
	});
}




function ask_OpenWeather_forecast (pos) {
	var owurl = 'http://api.openweathermap.org/data/2.5/forecast?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&APPID=' + api_key;
  xhrRequest(owurl, 'GET', 
		function(responseText) { 
			var json;
			var i;
      console.log ("Updating pins");
      if (just_launched === true) just_launched = false;
			json = JSON.parse(responseText);
			for (i=0;i<9;i++) {
				push_forecast (json.list[i],json.city.name);
			}
		}
);
}


function ask_Gisgraphy (pos) {
	var url = 'http://services.gisgraphy.com/street/streetsearch?format=json&lat=' + pos.coords.latitude + '&lng=' + pos.coords.longitude + '&from=1&to=1';
//	console.log ("Gisgraphy Url : " + url);
	xhrRequest (url,'GET',
		function (responseText) {
			// console.log ("In callback" + responseText);
			var json;
			try {
				json = JSON.parse (responseText);
				console.log (JSON.stringify(json));
				if (json.numFound === 1 ) {
					var gpstown = json.result[0].isIn + "," + json.result[0].countryCode;
					console.log("GPS Town : " + gpstown);
					ask_Yahoo (gpstown);   
				} else {
					console.log("Geoloc error from gisgraphy");
					ask_Yahoo (town);
				}
			} catch (e){
				console.log("Geoloc error from gisgraphy : " + e + " Town : " + town);
				console.log ("Gisgraphy response is : " + responseText);
				
				ask_Yahoo (town);
			}
			
		
		}         
	);  
}

function sync_data (pos) {

	var date = new Date();
  var min = date.getMinutes();
  console.log ("sync data : Gisgraphy");
  ask_Gisgraphy (pos);
	if ((min === 0) || (just_launched === true))  {
    console.log ("Openweather");
    ask_OpenWeather_forecast (pos);	
	}

}

function locationError(err) {
	console.log("Error requesting location!");
}

function getWeather() {
	
	if (parseInt(gps) === 1) {
		console.log ("Using GPS");
		navigator.geolocation.getCurrentPosition(
      sync_data,
			locationError,
			{timeout: 15000, maximumAge: 60000}
		);
	} else {
		console.log ("NOT using GPS");
		ask_Yahoo (town);
	}
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log("PebbleKit JS ready!");
		temp_unit = parseInt(localStorage.getItem("temp_unit"));
		
		gps = localStorage.getItem("gps");
		if (gps === null) {
			gps = 1;
		}
		town = localStorage.getItem("town");
		owner = localStorage.getItem("owner");
		hourlyvibe = localStorage.getItem("hourlyvibe");
		dnd = localStorage.getItem ("dnd");
		dndperiodstart = localStorage.getItem ("dndperiodstart");
		dndperiodend = localStorage.getItem ("dndperiodend");
		time_shift = localStorage.getItem ("time_shift");
		invert = localStorage.getItem ("invert");
		winddir = localStorage.getItem ("winddir");
    windforce = localStorage.getItem ("windforce");
    
		if (!temp_unit) {
			temp_unit = 0; // Default: Celcius
		}
		if (!hourlyvibe) {
			hourlyvibe = 0;
		}
		// console.log ("GPS : " + gps);
    just_launched = true;
    getWeather ();
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
		function(e) {
			console.log("AppMessage received!");
			getWeather();
		}                     
);



Pebble.addEventListener("showConfiguration", 
		function(e) {
				var url = "http://apps.oupsman.fr/oweather_config_16.php?" +
						"temp_unit=" + temp_unit +
						"&gps=" + gps +
						"&town=" + encodeURIComponent(town) +
						"&owner=" + encodeURIComponent(owner) +
						"&dnd=" + dnd +
						"&dndperiodstart=" + dndperiodstart +
						"&dndperiodend=" + dndperiodend +
						"&hourlyvibe=" + hourlyvibe +
						"&interval=" + interval + 
						"&invert=" + invert +
						"&showseconds=" + showseconds +
						"&time_shift=" + time_shift ;
			
				console.log (url);

				Pebble.openURL(url);

		}
);

Pebble.addEventListener("webviewclosed", function(e) {
	console.log (e.response);
	var configuration = JSON.parse(e.response);
	
	temp_unit = parseInt(configuration.temp_unit);
	gps = configuration.gps;
	town = configuration.town;
	console.log ("Town :" + town);
	owner = configuration.owner;
	dnd = configuration.dnd;
	dndperiodstart = configuration.dndperiodstart;
	dndperiodend = configuration.dndperiodend;
	hourlyvibe = configuration.hourlyvibe;
	interval = configuration.interval;
	time_shift = configuration.time_shift;
	invert = configuration.invert;
	
	localStorage.setItem("temp_unit", temp_unit);
	localStorage.setItem("gps", gps);
	localStorage.setItem("town", town);
	localStorage.setItem("owner",owner);
	localStorage.setItem("dnd", dnd);
	localStorage.setItem("dndperiodstart",dndperiodstart);
	localStorage.setItem("dndperiodend",dndperiodend);
	localStorage.setItem("hourlyvibe",hourlyvibe);
	localStorage.setItem("interval",interval);
	localStorage.setItem("time_shift",time_shift);
	localStorage.setItem("invert", invert);
	localStorage.setItem("winddir", winddir);
  localStorage.setItem("windforce", windforce);
  
	// Force weather refresh when you change the settings
	getWeather();

});

// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, callback) {
	// User or shared?
	var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;
  console.log ("PIN to send to timeline : " + JSON.stringify(pin));
	// Create XHR
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		console.log('timeline: response received: ' + this.responseText);
		callback(this.responseText);
	};
	xhr.open(type, url);

	// Get token
	Pebble.getTimelineToken(function(token) {
		// Add headers
		xhr.setRequestHeader('Content-Type', 'application/json');
		xhr.setRequestHeader('X-User-Token', '' + token);
  
		// Send
		xhr.send(JSON.stringify(pin));
		console.log('timeline: request sent. Token is ' + token );
	}, function(error) { console.log('timeline: error getting timeline token: ' + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
	timelineRequest(pin, 'PUT', callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
/*
function deleteUserPin(pin, callback) {
	timelineRequest(pin, 'DELETE', callback);
}*/

/***************************** end timeline lib *******************************/