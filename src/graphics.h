#include <pebble.h>
#pragma once

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

GBitmap *s_battery_level_100;
GBitmap *s_battery_level_80;
GBitmap *s_battery_level_60;
GBitmap *s_battery_level_40;
GBitmap *s_battery_level_20;
GBitmap *s_battery_level_0;

void load_graphics (void);

void destroy_graphics (void);
