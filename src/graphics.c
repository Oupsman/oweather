#include <pebble.h>
#include "graphics.h"

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
  
  gbitmap_destroy (s_bluetooth);
  gbitmap_destroy (s_no_bluetooth);
  
  gbitmap_destroy (s_battery_level_100);
  gbitmap_destroy (s_battery_level_80);
  gbitmap_destroy (s_battery_level_60);
  gbitmap_destroy (s_battery_level_40);
  gbitmap_destroy (s_battery_level_20);
  gbitmap_destroy (s_battery_level_0);
  
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
  s_battery_level_100 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_100);
  s_battery_level_80 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_80);
  s_battery_level_60 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_60);
  s_battery_level_40 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_40);
  s_battery_level_20 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_20);
  s_battery_level_0 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_0);
  
  
}