#pragma once
#include <pebble.h>

Window *s_main_window;

TextLayer *s_time_layer;
TextLayer *s_weekday_layer;
TextLayer *s_temperature_layer;
TextLayer *s_windforce_layer;
TextLayer *s_date_layer;

void update_conditions (uint8_t);
void update_time(void);
void main_window_unload(Window *);
void main_window_load(Window *);
void main_window_init (void);
