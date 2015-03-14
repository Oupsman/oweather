/*
  OWeather
  Oupsman Weather 
  A watchface for Pebble, displaying facy icons the current weather of the current location
  Using Yahoo Weather webservice
  
  Storage.c
  
  handling permanent storage functions

*/


#include <pebble.h>
#include "storage.h"

void load_datas (void) {
  if ( persist_exists(KEY_TEMPERATURE)) {
     persist_read_string (KEY_TEMPERATURE, temperature, sizeof (temperature)); 
  }
  if (persist_exists(KEY_WINDCHILL)) {
    persist_read_string (KEY_WINDCHILL, windchill, sizeof(windchill));
    
  } 
  conditions = persist_exists(KEY_CONDITIONS) ? persist_read_int(KEY_CONDITIONS) : 0;
  if (persist_exists (KEY_UPDATETIME)) {
    persist_read_string (KEY_UPDATETIME,time_update,sizeof(time_update));
  }
  if (persist_exists (KEY_TOWN)){
    persist_read_string (KEY_TOWN,town,sizeof(town));
  } 
  APP_LOG (APP_LOG_LEVEL_INFO,"Loading datas from storage %s %s %ul %s %s",temperature,windchill,conditions,time_update,town);
}

void save_datas(void) {
  APP_LOG (APP_LOG_LEVEL_INFO,"Writing datas to storage %s %s %u %s %s",temperature,windchill,conditions,time_update,town);
  persist_write_string (KEY_TEMPERATURE, temperature);
  persist_write_string (KEY_WINDCHILL, windchill);
  persist_write_int (KEY_CONDITIONS, conditions);
  
  persist_write_string (KEY_UPDATETIME, time_update);
  persist_write_string (KEY_TOWN,town);
}
