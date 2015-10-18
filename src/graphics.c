/*
 * OWeather
 * Oupsman Weather 
 * A watchface for Pebble, displaying facy icons the current weather of the current location
 * Using Yahoo Weather webservice
 * 
 * graphics.c
 * 
 * handling graphics code
 *
 */

#include <pebble.h>
#include "graphics.h"
#include "storage.h"
#include "weather.h"

void destroy_graphics (void) {
  
  gbitmap_destroy (s_winddir_back);
  
  gbitmap_destroy (s_bluetooth);
  gbitmap_destroy (s_no_bluetooth);
 
  gbitmap_destroy (s_bitmap_weather);
}

void load_graphics (void) {

  s_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_BTSIGN);
  s_no_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_NO_BT);
  
  s_winddir_back = gbitmap_create_with_resource (RESOURCE_ID_IMG_ROSEDESVENTS);
  
}

void draw_battery_indicator (Layer *me, GContext* ctx ) {
  
  BatteryChargeState charge_state = battery_state_service_peek();
  static uint8_t bat_level;
  
  bat_level = charge_state.charge_percent / 4;
  
  APP_LOG (APP_LOG_LEVEL_INFO, "Updating battery indicator, battery level : %u charge percent : %u", bat_level, charge_state.charge_percent);

  graphics_context_set_stroke_color (ctx,GColorBlack);
  graphics_draw_rect (ctx,GRect (0,0,27,10));
  graphics_draw_rect (ctx,GRect(26,3,4,4));
  
  if (!charge_state.is_charging) {
    if (charge_state.charge_percent < 21) {
      graphics_context_set_fill_color(ctx, GColorRed);
    } else {
      graphics_context_set_fill_color(ctx, GColorGreen);
    }
    graphics_fill_rect(ctx, GRect(1, 1, bat_level, 8), 0, GCornerNone);
  } else {
    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_rect(ctx, GRect(1, 1, 25, 8), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(27,4,2,2),0,GCornerNone);
  }      

}

void update_battery_indicator (BatteryChargeState charge_state) {

  layer_mark_dirty (s_battery_layer);
  
}

void update_bluetooth_indicator (bool connected) {
  
  if (connected) {
    vibes_short_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_bluetooth);  
  } else {    
    vibes_double_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_no_bluetooth);
  } 
  
}

void init_bt_indicator (void) {
  
  static char log_buffer[128] = "Initializing BT indicator" ;

  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);
  
  bitmap_layer_set_bitmap (s_bt_layer, s_bluetooth);
  
}
