// JSON Online viewer : http://jsonviewer.stack.hu/

var temp_unit = 0;
var gps = 1;
var town = "";
var owner = "";
var dnd = 1;
var dndperiodstart = 22;
var dndperiodend = 6;
var hourlyvibe = 0;
var interval=30;
var time_shift=0;
var invert=0;

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

function ask_Yahoo(where) {
  
  var yahoo_url = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20" + 
      "where%20woeid%20in%20%28select%20woeid%20from%20geo.places%281%29%20where%20text%3D%22" + 
      encodeURIComponent(where) + '%22%29&format=json&diagnostics=true' +
      '&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=';
      
  console.log ("Yahoo URL : " + yahoo_url);
               
  xhrRequest(yahoo_url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      // console.log ("Yahoo said " + responseText);
      var json = JSON.parse(responseText);

      
      // Conditions
      var conditions = json.query.results.channel.item.condition.code;   
      if (conditions > 255) {
        conditions = 255;
        console.log (responseText);
      }
      /*
      var conditions1 = json.query.results.channel.item.forecast[0].code;
      var conditions2 = json.query.results.channel.item.forecast[1].code;
      var conditions3 = json.query.results.channel.item.forecast[2].code;
      */
      var feelslike;
      var temperature;
      
      if (temp_unit === 0) {
        console.log ("°C");
        feelslike = f_to_c(json.query.results.channel.wind.chill) + "°C";
        temperature = f_to_c(json.query.results.channel.item.condition.temp) + "°C";
      } else {
        console.log ("°F");
        feelslike = json.query.results.channel.wind.chill + "°F";
        temperature = json.query.results.channel.item.condition.temp + "°F";        
      }
       
      
      
      var dictionary = {
          "KEY_TEMPERATURE": temperature,
          "KEY_CONDITIONS": parseInt(conditions),
          "KEY_FEELSLIKE": feelslike,
          "KEY_TOWN": where,    
          "KEY_OWNER": owner,
          "KEY_DND": parseInt(dnd),
          "KEY_DNDPERIODSTART": parseInt(dndperiodstart),
          "KEY_DNDPERIODEND": parseInt(dndperiodend),
          "KEY_HOURLYVIBE": parseInt(hourlyvibe),
          "KEY_INTERVAL": parseInt(interval),
          "KEY_SHIFTTIME": parseInt (time_shift),
          "KEY_INVERT": parseInt (invert),
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

function ask_Google(pos) {
  // console.log ("GPS value" + gps);
  if (gps === '1') {
  // console.log ("Using GPS");  
  // Construct URL
  var url = "https://maps.googleapis.com/maps/api/geocode/json?latlng=" +
      pos.coords.latitude + "," + pos.coords.longitude;
  
   console.log ("Asking Google for your position, using url " + url);
  // Send request to Google WS
  xhrRequest(url, 'GET', 
    function (responseText){
      var i;
      var townelement=0;
      var countryelement=0;
      // var gpstown = "unknow";
      
      // responseText contains a JSON object with localisation info
      var json = JSON.parse(responseText);
       if (json.status !== "OK" ){
         console.log ("JSON " + responseText);
         console.log("No valid answer from Google, fallback to town");
         ask_Yahoo (town);
       } else {
         var nbelements = (json.results[0].address_components).length;
         // console.log ("Nb elements " + nbelements);
         for (i=0; i<nbelements;i++) {
           // console.log ("Type : " + json.results[0].address_components[i].types[0]);
           if (json.results[0].address_components[i].types[0] == "locality") {
             townelement = i;
           }
             if (json.results[0].address_components[i].types[0] == "country") {
               countryelement = i;
             }        
         }
         console.log ("Town element " + townelement + " Country element " + countryelement);
      
         /*      var nbelements_country = (json.results[0].address_components).length;
         for (i=0;i<nbelements_country;i++) {      

         }*/
         var gpstown = json.results[0].address_components[townelement].long_name + "," + json.results[0].address_components[countryelement].short_name;
         // console.log("Location :" + gpstown);
         // Now we have geolocalisated the user, let's query the Yahoo weather 
         ask_Yahoo (gpstown); 
       }
    }      
  );      
  } else {
    // console.log ("NOT using GPS, Town is " + town);
    ask_Yahoo (town);
  }
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  if (gps === '1') {
    navigator.geolocation.getCurrentPosition(
      ask_Google,
      locationError,
      {timeout: 15000, maximumAge: 60000}
    );
  }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    temp_unit = parseInt(localStorage.getItem("temp_unit"));
    console.log ("Temp_unit : " + temp_unit);
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
    
    if (!temp_unit) {
      temp_unit = 0; // Default: Celcius
    }
    if (!hourlyvibe) {
      hourlyvibe = 0;
    }
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
        console.log("showConfiguration Event");
        var url = "http://apps.oupsman.fr/oweather_config_15.php?" +
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
            "&time_shift=" + time_shift ;
        console.log (url);

        Pebble.openURL(url);

    }
);

Pebble.addEventListener("webviewclosed", function(e) {
  
  var configuration = JSON.parse(e.response);
  
  temp_unit = parseInt(configuration.temp_unit);
  gps = configuration.gps;
  town = configuration.town;
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
  console.log ("Time shift : " + time_shift);
  
  // Force weather refresh when you change the settings
  getWeather();

});
