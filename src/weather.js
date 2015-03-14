// JSON Online viewer : http://jsonviewer.stack.hu/

var temp_unit = 0;

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
  
  console.log("Temperature Celcius :" + temp_c);
  
  return temp_c;
  
}


function locationSuccess(pos) {
  // Construct URL
  var url = "https://maps.googleapis.com/maps/api/geocode/json?latlng=" +
      pos.coords.latitude + "," + pos.coords.longitude;
  
  console.log ("Using url " + url);
  
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
             function (responseText){
      //console.log ("JSON " + responseText);
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var town = json.results[2].address_components[0].long_name + "," + json.results[5].address_components[0].short_name;
      console.log("Location :" + town);
      // Now we have geolocalisated the user, let's query the Yahoo weather 
      var yahoo_url = 'https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20%28select%20woeid%20from%20geo.places%281%29%20where%20text%3D%22' + encodeURIComponent(town) + '%22%29&format=json&diagnostics=true&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=';
      console.log ("Yahoo URL : " + yahoo_url);
      xhrRequest(yahoo_url, 'GET', 
      function(responseText) {
        // responseText contains a JSON object with weather info
        //console.log ("Yahoo said " + responseText);
        var json = JSON.parse(responseText);

        // Temperature in Kelvin requires adjustment
        var temperature = json.query.results.channel.item.condition.temp;
      //  console.log("Temperature is " + temperature);

        // Conditions
        var conditions = json.query.results.channel.item.condition.code;      
        if (conditions > 255) {
          conditions = 255;
        }
        console.log("Conditions are " + conditions);
      
        var feelslike = json.query.results.channel.wind.chill;
       // console.log ("Feels like " + feelslike);
        var dictionary;
        console.log ("Unit :" + temp_unit);
        
        // Assemble dictionary using our keys
        if (temp_unit === '0') {
          console.log ("Unit : Celcius");
          dictionary = {
            "KEY_TEMPERATURE": f_to_c(temperature) + "°C",
            "KEY_CONDITIONS": parseInt(conditions),
            "KEY_FEELSLIKE": f_to_c(feelslike) + "°C",
            "KEY_TOWN": town,
            
          };
        } else {
          console.log ("Unit : Farenheit");
          dictionary = {
            "KEY_TEMPERATURE": temperature + "°F",
            "KEY_CONDITIONS": parseInt(conditions),
            "KEY_FEELSLIKE": feelslike  + "°F",
            "KEY_TOWN": town,
            
          };
          
        }
        
        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("Weather info sent to Pebble successfully!");
          },
          function(e) {
            console.log("Error sending weather info to Pebble!");
          }
        );

      }      
    );
           
             
    }
  );

}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    temp_unit = localStorage.getItem("temp_unit");
    if (!temp_unit) {
      temp_unit = 0; // Default: Celcius
    }

    // Get the initial weather
    getWeather();
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
      console.log("showConfiguration Event");

      Pebble.openURL("http://apps.oupsman.fr/oweather_config.php?" +
        "temp_unit=" + temp_unit );
    }
);

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("Configuration window closed");
  console.log(e.type);
  console.log(e.response);
  var configuration = JSON.parse(e.response);
  //Pebble.sendAppMessage(configuration);
  temp_unit = configuration.temp_unit;
  console.log ("Unit : " + temp_unit);
  localStorage.setItem("temp_unit", temp_unit);
  // Force weather refresh when you change the unit
  getWeather();
});
