#pragma once
#include <pebble.h>
  
#define KEY_CONDITIONS 0
#define KEY_TEMPERATURE 1
#define KEY_WINDCHILL 2
#define KEY_TOWN 3
#define KEY_UPDATETIME 4
#define KEY_UNIT 5
#define KEY_WINDDIR 6
#define KEY_WINDFORCE 7  
#define KEY_HOURLYVIBE 8
#define KEY_DND 9
#define KEY_DNDPERIODSTART 10
#define KEY_DNDPERIODEND 11
#define KEY_INTERVAL 12
#define KEY_OWNER 13
#define KEY_SHIFTTIME 14
#define KEY_INVERT 15
#define KEY_SECONDS 16
  
char temperature[15];
uint8_t conditions;
uint8_t hourlyvibe;
uint8_t interval;
uint8_t invert;
char windforce[15];

uint16_t shift_time;

char time_update[6];
char town[129];
char owner[129];
uint8_t dnd;
uint8_t dndperiodstart;
uint8_t dndperiodend;


void load_datas (void);
void save_datas (void);
