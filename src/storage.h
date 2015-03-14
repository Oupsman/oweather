#pragma once

#define KEY_CONDITIONS 0
#define KEY_TEMPERATURE 1
#define KEY_WINDCHILL 2
#define KEY_TOWN 3
#define KEY_UPDATETIME 4
#define KEY_UNIT 5
  
char temperature[15];
char windchill[15];
uint8_t conditions;

char time_update[6];
char town[129];

void load_datas (void);
void save_datas (void);
