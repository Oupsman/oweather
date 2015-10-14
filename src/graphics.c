/*
  OWeather
  Oupsman Weather 
  A watchface for Pebble, displaying facy icons the current weather of the current location
  Using Yahoo Weather webservice
  
  graphics.c
  
  handling graphics code

*/
#include <pebble.h>
#include "graphics.h"
#include "storage.h"

/*  
static uint8_t BATTERY_ICONS [] = {
  RESOURCE_ID_IMG_BATTERY_0,
  RESOURCE_ID_IMG_BATTERY_20,
  RESOURCE_ID_IMG_BATTERY_40,
  RESOURCE_ID_IMG_BATTERY_60,
  RESOURCE_ID_IMG_BATTERY_80,
  RESOURCE_ID_IMG_BATTERY_100
};
 */ 

 void destroy_graphics (void) {
  
  gbitmap_destroy (s_bitmap_no_image);
  gbitmap_destroy (s_bitmap_sunny);
  gbitmap_destroy (s_bitmap_partly);
  gbitmap_destroy (s_bitmap_partly_night);
  gbitmap_destroy (s_bitmap_fair_night);
  gbitmap_destroy (s_bitmap_cloudy);
  gbitmap_destroy (s_bitmap_foggy);
  gbitmap_destroy (s_bitmap_snowy);
  gbitmap_destroy (s_bitmap_windy);
  gbitmap_destroy (s_battery_level);
  gbitmap_destroy (s_winddir_back);
  
  gbitmap_destroy (s_bluetooth);
  gbitmap_destroy (s_no_bluetooth);
 
}

void load_graphics (void) {

  s_bitmap_no_image = gbitmap_create_with_resource(RESOURCE_ID_IMG_NO_IMAGE);
  s_bitmap_sunny = gbitmap_create_with_resource(RESOURCE_ID_IMG_SUNNY);
  s_bitmap_partly = gbitmap_create_with_resource(RESOURCE_ID_IMG_PARTLY);
  s_bitmap_fair_night = gbitmap_create_with_resource(RESOURCE_ID_IMG_FAIR_NIGHT);
  s_bitmap_partly_night = gbitmap_create_with_resource(RESOURCE_ID_IMG_CLOUD_NIGHT);
  s_bitmap_cloudy = gbitmap_create_with_resource(RESOURCE_ID_IMG_CLOUDY);
  s_bitmap_foggy = gbitmap_create_with_resource(RESOURCE_ID_IMG_FOGGY);
  s_bitmap_windy = gbitmap_create_with_resource(RESOURCE_ID_IMG_WINDY);
  s_bitmap_snowy = gbitmap_create_with_resource(RESOURCE_ID_IMG_SNOWY);
  
  s_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_BTSIGN);
  s_no_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_NO_BT);
  
  s_winddir_back = gbitmap_create_with_resource (RESOURCE_ID_IMG_ROSEDESVENTS);
  
}

void draw_battery_indicator (Layer *me, GContext* ctx ) {
  
  BatteryChargeState charge_state = battery_state_service_peek();
  
  static char log_buffer[128] = "Updating battery indicator" ;
  
  static uint8_t bat_level;
  
  bat_level = charge_state.charge_percent / 4;
  
  APP_LOG (APP_LOG_LEVEL_INFO, "%s %u %u", log_buffer, bat_level, charge_state.charge_percent);

  #ifdef PBL_COLOR
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
  
  #endif  
  
  
}

void update_battery_indicator (BatteryChargeState charge_state) {

  layer_mark_dirty (s_battery_layer);
  
}

#ifndef PBL_COLOR
void update_inversion (uint8_t newvalue) {
  if (newvalue != invert) {
    switch (newvalue) {
      case 0:
        layer_remove_from_parent(inverter_layer_get_layer(inverter_layer));
      break;
      case 1:
        layer_add_child (window_get_root_layer(s_main_window), inverter_layer_get_layer(inverter_layer));
      break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "ARE YOU KIDDING ME ?! : %d", newvalue);
      break;
    }
    invert = newvalue;  
  }
}
#endif