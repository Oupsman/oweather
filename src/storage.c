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
  conditions = persist_exists(KEY_CONDITIONS) ? persist_read_int(KEY_CONDITIONS) : 0;
  hourlyvibe = persist_exists(KEY_HOURLYVIBE) ? persist_read_int(KEY_HOURLYVIBE) : 0;
  dnd = persist_exists(KEY_DND) ? persist_read_int(KEY_DND) : 1;
  dndperiodstart = persist_exists(KEY_DNDPERIODSTART) ? persist_read_int(KEY_DNDPERIODSTART) : 22;
  dndperiodend = persist_exists(KEY_DNDPERIODEND) ? persist_read_int(KEY_DNDPERIODEND) : 6;

  if (persist_exists (KEY_UPDATETIME)) {
    persist_read_string (KEY_UPDATETIME,time_update,sizeof(time_update));
  }
  if (persist_exists (KEY_TOWN)){
    persist_read_string (KEY_TOWN,town,sizeof(town));
  } 
  
  if (persist_exists (KEY_OWNER)) {
    persist_read_string (KEY_OWNER, owner, sizeof(owner));    
  }
  shift_time = persist_exists(KEY_SHIFTTIME) ? persist_read_int(KEY_SHIFTTIME): 0;
  invert = persist_exists(KEY_INVERT) ? persist_read_int(KEY_INVERT): 0;

    APP_LOG (APP_LOG_LEVEL_INFO,"Loading datas from storage %s %u %s %s %u",temperature,conditions,time_update,town, hourlyvibe);

}

void save_datas(void) {
  
  APP_LOG (APP_LOG_LEVEL_INFO,"Writing datas to storage %s %u %s %s %u",temperature,conditions,time_update,town, hourlyvibe);

  persist_write_string (KEY_TEMPERATURE, temperature);
  persist_write_int (KEY_CONDITIONS, conditions);  
  
  persist_write_int (KEY_DND, dnd);
  persist_write_int (KEY_DNDPERIODSTART, dndperiodstart);
  persist_write_int (KEY_DNDPERIODEND, dndperiodend);
  persist_write_int (KEY_HOURLYVIBE, hourlyvibe);
  persist_write_int (KEY_SHIFTTIME, shift_time);
  persist_write_int (KEY_INVERT,invert);
  
  
  persist_write_string (KEY_UPDATETIME, time_update);
  persist_write_string (KEY_TOWN,town);
  persist_write_string (KEY_OWNER,owner);
}
