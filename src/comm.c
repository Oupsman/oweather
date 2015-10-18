#include <pebble.h>
#include "comm.h"
#include "graphics.h"
#include "storage.h"
#include "weather.h"
#include "graphics.h"
#include "main_window.h"

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  static unsigned int weather_code;
  // Store incoming information

  time_t temp = time(NULL); 
  
  struct tm *tick_time = localtime(&temp);
  static char unit_text [] = "° C";
  static char speed_unit [] = "km/h";
  Tuple *t = dict_read_first(iterator);
  
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case KEY_TEMPERATURE:
        APP_LOG (APP_LOG_LEVEL_INFO,"Raw value : %s", t->value->cstring);
        strcpy(temperature,t->value->cstring);
        APP_LOG (APP_LOG_LEVEL_INFO,"Temperature : %s", temperature);

      break;
      case KEY_CONDITIONS:
        weather_code = (unsigned int)t->value->uint8;
        update_conditions (weather_code);      
        strftime(time_update, sizeof("00:00"), "%H:%M", tick_time);
      break;
      case KEY_INTERVAL: 
        APP_LOG(APP_LOG_LEVEL_INFO, "Interval : %d", t->value->uint8);
        interval = t->value->uint8;
      break;
      case KEY_DND: 
        APP_LOG(APP_LOG_LEVEL_INFO, "DND : %d", t->value->uint8);
        dnd = t->value->uint8;
      break;
      case KEY_DNDPERIODSTART: 
        APP_LOG(APP_LOG_LEVEL_INFO, "DND Period Start: %d", t->value->uint8);
        dndperiodstart = t->value->uint8;
      break;
      case KEY_DNDPERIODEND: 
        APP_LOG(APP_LOG_LEVEL_INFO, "DND : %d", t->value->uint8);
        dndperiodend = t->value->uint8;
      break;
      case KEY_OWNER:
        APP_LOG(APP_LOG_LEVEL_INFO, "Owner : %s", t->value->cstring);
        strcpy (owner,t->value->cstring);
      break;
      case KEY_HOURLYVIBE: 
        APP_LOG(APP_LOG_LEVEL_INFO, "HOURLYVIBE : %d", t->value->uint8);
        hourlyvibe = t->value->uint8;
      break;
      case KEY_INVERT:
        APP_LOG(APP_LOG_LEVEL_INFO, "INVERT : %d", t->value->uint8);
      break;
      case KEY_SHIFTTIME:
        APP_LOG(APP_LOG_LEVEL_INFO, "SHIFT TIME : %d", t->value->uint16);
        shift_time = t->value->uint16;    
        update_time();      
      break;
      case KEY_UNIT: 
        switch (t->value->uint8) {
          case 0:
            strcpy (unit_text, "°C");
            strcpy (speed_unit,"km/h");
          break;
          case 1:
            strcpy (unit_text, "°F");
            strcpy (speed_unit,"mp/h");
          break;
        }
      break;
      case KEY_WINDDIR:
        APP_LOG (APP_LOG_LEVEL_INFO, "WINDDIR : %d", t->value->uint16);   
      break;
      case KEY_WINDFORCE:
        strcpy (windforce,t->value->cstring);
        APP_LOG (APP_LOG_LEVEL_INFO,"Wind Speed : %s", windforce);
      break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  strcat (temperature, unit_text);
  strcat (windforce, speed_unit);
  
  text_layer_set_text(s_temperature_layer, temperature);
  text_layer_set_text(s_windforce_layer,windforce);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {

  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
  
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");

}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {

  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");

}

void init_comm(void) {
  
    // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
}