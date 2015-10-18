#include <pebble.h>
#pragma once

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

void update_bluetooth_indicator (bool);

void init_bt_indicator (void);
