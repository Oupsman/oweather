// Yahoo Weather https://developer.yahoo.com/weather/
// https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20%28select%20woeid%20from%20geo.places%281%29%20where%20text%3D%22Orl%C3%A9ans%2C%20FR%22%29&format=json&diagnostics=true&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=

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
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude;
  
  console.log ("Using url " + url);
  
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
             function (responseText){
      console.log ("JSON " + responseText);
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var town = json.name + "," + json.sys.country;
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
        console.log("Conditions are " + conditions);
      
        var feelslike = json.query.results.channel.wind.chill;
       // console.log ("Feels like " + feelslike);
        
        // Assemble dictionary using our keys
        var dictionary = {
          "KEY_TEMPERATURE": f_to_c(temperature),
          "KEY_CONDITIONS": parseInt(conditions),
          "KEY_FEELSLIKE": f_to_c(feelslike),
          "KEY_TOWN": town
        };

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