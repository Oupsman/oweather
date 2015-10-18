#include <pebble.h>
#include "main_window.h"
#include "graphics.h"
#include "storage.h"
#include "weather.h"
#include "graphics.h"

void main_window_load(Window *window) {
  
  APP_LOG (APP_LOG_LEVEL_INFO, "Creating layers");
  
  s_bt_layer = bitmap_layer_create (GRect (132,1,7,10));
  s_battery_layer = layer_create (GRect (90,1,31,10)); 
  
  s_time_layer = text_layer_create(GRect(0, 10, 110, 44));
  
  s_weekday_layer = text_layer_create (GRect(111,10,33,16));
  s_date_layer = text_layer_create (GRect (111,27,33,34));
  
  s_conditions_layer = bitmap_layer_create(GRect (0,60,72,72));
  s_temperature_layer = text_layer_create(GRect(1, 133, 70, 24));
  
  s_winddir_layer = bitmap_layer_create (GRect(75,60,72,72));  
  s_windforce_layer = text_layer_create (GRect (75,133,50,26));
  
  layer_set_update_proc(s_battery_layer, draw_battery_indicator);
  
  APP_LOG (APP_LOG_LEVEL_INFO, "Done creating layers, starting to set colors");
  
  window_set_background_color(s_main_window, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_background_color(s_weekday_layer, GColorWhite);
  text_layer_set_text_color(s_weekday_layer, GColorBlack);
  text_layer_set_background_color(s_temperature_layer, GColorWhite);
  text_layer_set_text_color(s_temperature_layer, GColorBlack);
  text_layer_set_background_color (s_windforce_layer,GColorWhite);
  text_layer_set_text_color (s_windforce_layer,GColorBlack);
  text_layer_set_background_color (s_date_layer, GColorWhite);
  text_layer_set_text_color (s_date_layer,GColorDarkCandyAppleRed);
  
  APP_LOG (APP_LOG_LEVEL_INFO, "Done setting colors, start to init every layer");

  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_weekday_layer, "Wed");
  text_layer_set_text(s_date_layer,"31");
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentRight);
  
  bitmap_layer_set_bitmap (s_winddir_layer, s_winddir_back);
  
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font (s_temperature_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_temperature_layer,GTextAlignmentLeft);
  text_layer_set_font (s_windforce_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_windforce_layer,GTextAlignmentLeft);
  text_layer_set_font (s_date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment (s_date_layer, GTextAlignmentCenter);
  
  // Add all layers as a child layer of the Window's root layer
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_windforce_layer));       
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_conditions_layer));     
  layer_add_child(window_get_root_layer(window), s_battery_layer);         
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_layer));  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_winddir_layer));
  layer_add_child(window_get_root_layer(window),text_layer_get_layer (s_date_layer));

  APP_LOG (APP_LOG_LEVEL_INFO, "Temperature : %s", temperature);
  
  text_layer_set_text(s_temperature_layer, temperature);

  // Make sure everything is displayed from the start
  update_conditions (conditions);
  update_time();
  init_bt_indicator ();
  layer_mark_dirty(s_battery_layer);
  
  APP_LOG (APP_LOG_LEVEL_INFO, "Done loading the main window");

}

void main_window_unload(Window *window) {
  
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_temperature_layer);
  text_layer_destroy(s_windforce_layer);
  text_layer_destroy(s_date_layer);
  
  bitmap_layer_destroy(s_conditions_layer);
  bitmap_layer_destroy(s_bt_layer);
  bitmap_layer_destroy(s_winddir_layer);
  layer_destroy (s_battery_layer);
  
  destroy_graphics ();

}

void update_conditions (uint8_t weather_code) {

  static char log_buffer[128];
  snprintf (log_buffer, sizeof (log_buffer), "Weather code I've got from the JS : %u", weather_code);
  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);

  set_icon_from_conditions (weather_code);
  
  bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_weather);
  
  conditions = weather_code;

}

void update_time() {

  // Get a tm structure
  time_t temp = time(NULL); 
  
  if ( shift_time > 0 ) {
    APP_LOG (APP_LOG_LEVEL_INFO, "Shifting time %d", shift_time);
    temp += shift_time;
  } 

  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer_weekday [] = "Wed";
  static char buffer_date[] = "31";
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    //strftime(buffer, sizeof("00:00"), "22:22", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  strftime(buffer_weekday, sizeof ("Wed"), "%a", tick_time);
  strftime(buffer_date, sizeof ("31"), "%d", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_weekday_layer, buffer_weekday);
  text_layer_set_text(s_date_layer,buffer_date);
  
}

void main_window_init (void) {
    // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
}