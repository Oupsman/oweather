#include <pebble.h>
#include "graphics.h"
#include "storage.h"
#include "weather.h"
#include "graphics.h"
#include "main_window.h"
#include "comm.h"

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  if ((tick_time->tm_hour < dndperiodstart && tick_time->tm_hour >= dndperiodend) || dnd == 1){
    // Get weather update every ten minutes
    if(tick_time->tm_min % interval == 0) {
   // if(tick_time->tm_min % 1 == 0) {
      APP_LOG (APP_LOG_LEVEL_INFO, "Updating weather");
      // Begin dictionary
      DictionaryIterator *iter;      
      app_message_outbox_begin(&iter);
      // Add a key-value pair
      dict_write_uint8(iter, 0, 0);
      // Send the message!
      app_message_outbox_send();
    }  
  } 
  if ( ( hourlyvibe == 1 ) && (tick_time->tm_min == 0) && (tick_time->tm_sec < 3)) {
    vibes_short_pulse();
  }
}



static void init() {
  
  init_comm ();
  
  main_window_init ();

  load_datas();
  
  load_graphics ();
  
  window_stack_push(s_main_window, true);

  // Register with all services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(&update_battery_indicator);
  bluetooth_connection_service_subscribe(&update_bluetooth_indicator);

}

static void deinit() {
  // Destroy main window
  window_destroy(s_main_window);
  
  // unregister with all services
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe(); 
  
  save_datas();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
