#include <pebble.h>
#include "graphics.h"
#include "storage.h"
#include "weather.h"
#include "graphics.h"
  
  

static TextLayer *s_time_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_temperature_layer;
static TextLayer *s_windchill_layer;
static TextLayer *s_town_layer;
static TextLayer *s_update_layer;
static TextLayer *s_windforce_layer;


static void update_bluetooth_indicator (bool connected) {
  
  if (connected) {
    vibes_short_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_bluetooth);  
    text_layer_set_text(s_update_layer, time_update);
  } else {    
    vibes_double_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_no_bluetooth);
    text_layer_set_text(s_update_layer, owner);
  } 
  
}

static void init_bt_indicator (void) {
  
  static char log_buffer[128] = "Initializing BT indicator" ;

  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);
  
  bitmap_layer_set_bitmap (s_bt_layer, s_bluetooth);
  
}

static void update_conditions (uint8_t weather_code) {

  static char log_buffer[128];
  snprintf (log_buffer, sizeof (log_buffer), "Weather code I've got from the JS : %u", weather_code);
  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);

  set_icon_from_conditions (weather_code);
  
  bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_weather);
  
  conditions = weather_code;

}

static void update_time() {

  // Get a tm structure
  time_t temp = time(NULL); 
  
  if ( shift_time > 0 ) {
    APP_LOG (APP_LOG_LEVEL_INFO, "Shifting time %d", shift_time);
    temp += shift_time;
  } 

  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer_weekday [] = "Wednesday 31";
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  strftime(buffer_weekday, sizeof ("Wednesday 31"), "%A %d", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_weekday_layer, buffer_weekday);
}

static void main_window_load(Window *window) {
  
  s_time_layer = text_layer_create(GRect(0, 10, 144, 50));
  s_weekday_layer = text_layer_create (GRect(0,0,100,16));
  s_temperature_layer = text_layer_create(GRect(10, 124, 46, 26));
  s_windchill_layer = text_layer_create(GRect(10, 145, 46, 26));
  s_town_layer = text_layer_create (GRect(0,134,144,16));
  s_update_layer = text_layer_create (GRect(0,150,144,16));
  s_windforce_layer = text_layer_create (GRect (75,124,50,26));
  s_conditions_layer = bitmap_layer_create(GRect (0,54,72,72));
  // s_battery_layer  = bitmap_layer_create (GRect (100,0,25,10));
  
  s_battery_layer = layer_create (GRect (90,0,31,10));
  layer_set_update_proc(s_battery_layer, draw_battery_indicator);
  
  s_bt_layer = bitmap_layer_create (GRect (132,0,7,10));
  s_winddir_layer = bitmap_layer_create (GRect(75,54,72,72));
  
#ifdef PBL_COLOR
  window_set_background_color(s_main_window, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_background_color(s_weekday_layer, GColorWhite);
  text_layer_set_text_color(s_weekday_layer, GColorBlack);
  text_layer_set_background_color(s_temperature_layer, GColorWhite);
  text_layer_set_text_color(s_temperature_layer, GColorBlack);
  text_layer_set_background_color(s_windchill_layer, GColorWhite);
  text_layer_set_text_color(s_windchill_layer, GColorBlack);
  text_layer_set_background_color (s_town_layer,GColorWhite);
  text_layer_set_text_color (s_town_layer,GColorBlack);
  text_layer_set_background_color (s_update_layer,GColorWhite);
  text_layer_set_text_color (s_update_layer,GColorBlack);
  text_layer_set_background_color (s_windforce_layer,GColorWhite);
  text_layer_set_text_color (s_windforce_layer,GColorBlack);
#else 
  window_set_background_color(s_main_window, GColorBlack);
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_weekday_layer, GColorBlack);
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_background_color(s_temperature_layer, GColorBlack);
  text_layer_set_text_color(s_temperature_layer, GColorWhite);
  text_layer_set_background_color(s_windchill_layer, GColorBlack);
  text_layer_set_text_color(s_windchill_layer, GColorWhite);
  text_layer_set_background_color (s_town_layer,GColorBlack);
  text_layer_set_text_color (s_town_layer,GColorWhite);
  text_layer_set_background_color (s_update_layer,GColorBlack);
  text_layer_set_text_color (s_update_layer,GColorWhite); 
  
#endif
  
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_weekday_layer, "Wednesday 31");
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentRight);
  //text_layer_set_text(s_temperature_layer, "-xx °C");
  text_layer_set_text_alignment(s_windchill_layer, GTextAlignmentRight);
  //text_layer_set_text(s_windchill_layer, "-xx °C");  
  text_layer_set_text_alignment(s_town_layer, GTextAlignmentCenter);
  text_layer_set_text(s_town_layer, town);
  text_layer_set_text_alignment(s_update_layer, GTextAlignmentRight);
  text_layer_set_text(s_update_layer, time_update);  
  bitmap_layer_set_bitmap (s_conditions_layer, s_bitmap_no_image);  
  bitmap_layer_set_bitmap (s_winddir_layer, s_winddir_back);
  // bitmap_layer_set_compositing_mode (s_conditions_layer,GCompOpSet);

  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentLeft);

  text_layer_set_font (s_temperature_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_temperature_layer,GTextAlignmentLeft);
                       
  text_layer_set_font (s_windchill_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_windchill_layer,GTextAlignmentLeft);
  
  text_layer_set_font (s_windforce_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_windforce_layer,GTextAlignmentLeft);
  
  text_layer_set_font(s_town_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_town_layer, GTextAlignmentLeft);
  
  text_layer_set_font(s_update_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_update_layer, GTextAlignmentLeft);
  
  
  //text_layer_set_text_alignment(s_static_temp_layer, GTextAlignmentLeft);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_windchill_layer));       
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_windforce_layer));       
  // layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_town_layer));     
  // layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_update_layer));    
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_conditions_layer));     
  
  layer_add_child(window_get_root_layer(window), s_battery_layer);         
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_layer));  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_winddir_layer));
  
#ifndef PBL_COLOR  
  inverter_layer = inverter_layer_create(GRect (0, 0, 144, 168));
  if (invert == 1) {
    layer_add_child (window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
  }
#endif  
  APP_LOG (APP_LOG_LEVEL_INFO, "Temperature : %s", temperature);
  text_layer_set_text(s_temperature_layer, temperature);

  APP_LOG (APP_LOG_LEVEL_INFO, "Windchill : %s", windchill);
  text_layer_set_text(s_windchill_layer, windchill);
  
  

  
  // Make sure everything is displayed from the start
  update_conditions (conditions);
  update_time();
  init_bt_indicator ();
  layer_mark_dirty(s_battery_layer);
//  update_battery_indicator(charge_state);
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_windchill_layer);
  text_layer_destroy(s_temperature_layer);
  text_layer_destroy(s_town_layer);
  text_layer_destroy(s_update_layer);
  text_layer_destroy(s_windforce_layer);
  
  bitmap_layer_destroy(s_conditions_layer);
//  bitmap_layer_destroy(s_battery_layer);
  bitmap_layer_destroy(s_bt_layer);
  bitmap_layer_destroy(s_winddir_layer);
  
  destroy_graphics ();

}

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

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  static unsigned int weather_code;
  // Store incoming information

  static char town_buffer[64];
  time_t temp = time(NULL); 
  static char buffer_windforce[15];
  
  struct tm *tick_time = localtime(&temp);
  //static char conditions_buffer[32];
  //static char weather_layer_buffer[32];
  uint8_t newvalue;
  static char unit_text [] = "° C";
  static char speed_unit [] = "km/h";
  // Read first item
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
        text_layer_set_text(s_update_layer, time_update);
      break;
      case KEY_WINDCHILL:
        strcpy (windchill, t->value->cstring);
        APP_LOG (APP_LOG_LEVEL_INFO,"Windchill : %s", windchill);
      break;
      case KEY_TOWN:
        APP_LOG(APP_LOG_LEVEL_INFO, "Town : %s", t->value->cstring);
        snprintf(town_buffer, sizeof(town_buffer), "%s", t->value->cstring);
        text_layer_set_text (s_town_layer,town_buffer);
        strcpy (town,t->value->cstring);
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
#ifndef PBL_COLOR
        newvalue = t->value->uint8;    
        update_inversion (newvalue);  
#endif
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
  strcat (windchill, unit_text);
  strcat (windforce, speed_unit);
  
  text_layer_set_text(s_temperature_layer, temperature);
  text_layer_set_text(s_windchill_layer, windchill);

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

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Load saved datas 
  
  load_datas();
  
  // Load all images

  load_graphics ();
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register battery state change service
  
  battery_state_service_subscribe(&update_battery_indicator);
  
  // Register BT state change service
  bluetooth_connection_service_subscribe(&update_bluetooth_indicator);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  
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
