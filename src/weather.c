/*
  OWeather
  Oupsman Weather 
  A watchface for Pebble, displaying facy icons the current weather of the current location
  Using Yahoo Weather webservice
  
  weather.c
  
  Handling the weather 

*/

#include <pebble.h>
#include "weather.h"

static uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMG_WINDY,         //0
  RESOURCE_ID_IMG_WINDY,         //1
  RESOURCE_ID_IMG_WINDY,         //2
  RESOURCE_ID_IMG_THUNDER,       //3
  RESOURCE_ID_IMG_THUNDER,       //4
  RESOURCE_ID_IMG_SNOWY,         //5
  RESOURCE_ID_IMG_SNOWY,         //6
  RESOURCE_ID_IMG_SNOWY,         //7
  RESOURCE_ID_IMG_RAINY,         //8
  RESOURCE_ID_IMG_RAINY,         //9
  RESOURCE_ID_IMG_RAINY,         //10
  RESOURCE_ID_IMG_RAINY,         //11
  RESOURCE_ID_IMG_RAINY,         //12
  RESOURCE_ID_IMG_SNOWY,         //13
  RESOURCE_ID_IMG_SNOWY,         //14
  RESOURCE_ID_IMG_SNOWY,         //15
  RESOURCE_ID_IMG_SNOWY,         //16
  RESOURCE_ID_IMG_HAIL,          //17
  RESOURCE_ID_IMG_SNOWY,         //18
  RESOURCE_ID_IMG_SNOWY,         //19
  RESOURCE_ID_IMG_FOGGY,         //20
  RESOURCE_ID_IMG_FOGGY,         //21
  RESOURCE_ID_IMG_FOGGY,         //22
  RESOURCE_ID_IMG_WINDY,         //23
  RESOURCE_ID_IMG_WINDY,         //24
  RESOURCE_ID_IMG_WINDY,         //25
  RESOURCE_ID_IMG_CLOUDY,        //26
  RESOURCE_ID_IMG_CLOUDY,        //27 //NIGHT //MOSTLY 
  RESOURCE_ID_IMG_CLOUDY,        //28 //DAY //MOSTLY
  RESOURCE_ID_IMG_PARTLY,        //29 //NIGHT //PARTLY
  RESOURCE_ID_IMG_PARTLY,        //30 // DAY // PARTLY
  RESOURCE_ID_IMG_FAIR_NIGHT,    //31 // NIGHT
  RESOURCE_ID_IMG_SUNNY,         //32 // DAY
  RESOURCE_ID_IMG_FAIR_NIGHT,    //33 //NIGHT
  RESOURCE_ID_IMG_SUNNY,         //34 //FAIR DAY
  RESOURCE_ID_IMG_HAIL,          //35 // HAIL AND RAIN
  RESOURCE_ID_IMG_SUNNY,         //36 //HOT WEATHER
  RESOURCE_ID_IMG_THUNDER,       //37
  RESOURCE_ID_IMG_THUNDER,       //38
  RESOURCE_ID_IMG_THUNDER,       //39
  RESOURCE_ID_IMG_RAINY,         //40
  RESOURCE_ID_IMG_SNOWY,         //41
  RESOURCE_ID_IMG_SNOWY,         //42
  RESOURCE_ID_IMG_SNOWY,         //43
  RESOURCE_ID_IMG_PARTLY,        //44
  RESOURCE_ID_IMG_THUNDER,       //45
  RESOURCE_ID_IMG_SNOWY,         //46
  RESOURCE_ID_IMG_THUNDER        //47

};

#define MAX_CONDITIONS 47
  
void set_icon_from_conditions (uint8_t conditions) {

  gbitmap_destroy (s_bitmap_weather);

  if (conditions <= MAX_CONDITIONS && conditions != 255) {
    s_bitmap_weather = gbitmap_create_with_resource (WEATHER_ICONS[conditions]);
  } else {
    s_bitmap_weather = gbitmap_create_with_resource (RESOURCE_ID_IMG_NO_IMAGE);
  }  
}