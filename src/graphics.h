#include <pebble.h>
#pragma once

Window *s_main_window;

GBitmap *s_bitmap_no_image;
GBitmap *s_bitmap_sunny;
GBitmap *s_bitmap_cloudy;
GBitmap *s_bitmap_foggy;
GBitmap *s_bitmap_rainy;
GBitmap *s_bitmap_snowy;
GBitmap *s_bitmap_windy;
GBitmap *s_bitmap_partly;
GBitmap *s_bitmap_fair_night;
GBitmap *s_bitmap_partly_night;

GBitmap *s_bluetooth;
GBitmap *s_no_bluetooth;

GBitmap *s_battery_level;

GBitmap *s_winddir_back;


BitmapLayer *s_bt_layer;

Layer *s_battery_layer;

BitmapLayer *s_winddir_layer;


void load_graphics (void);

void destroy_graphics (void);

void draw_battery_indicator (Layer *, GContext* );

void update_battery_indicator (BatteryChargeState);

#ifndef PBL_COLOR
  InverterLayer *inverter_layer;
  void update_inversion (uint8_t);
#endif