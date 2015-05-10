#include <pebble.h>
#include "graphics.h"
#include "storage.h"
  
static uint8_t BATTERY_ICONS [] = {
  RESOURCE_ID_IMG_BATTERY_0,
  RESOURCE_ID_IMG_BATTERY_20,
  RESOURCE_ID_IMG_BATTERY_40,
  RESOURCE_ID_IMG_BATTERY_60,
  RESOURCE_ID_IMG_BATTERY_80,
  RESOURCE_ID_IMG_BATTERY_100
};
  
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
  
}

void update_battery_indicator (BatteryChargeState charge_state) {
  
  static char log_buffer[128] = "Updating battery indicator" ;
  static uint8_t bat_level;
  
 // BatteryChargeState charge_state = battery_state_service_peek();
  bat_level = charge_state.charge_percent / 20;
  
  APP_LOG (APP_LOG_LEVEL_INFO, "%s %u %u", log_buffer, bat_level, charge_state.charge_percent);
  
  gbitmap_destroy (s_battery_level);
  
  s_battery_level = gbitmap_create_with_resource (BATTERY_ICONS[bat_level]);
  
  bitmap_layer_set_bitmap (s_battery_layer, s_battery_level);  

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