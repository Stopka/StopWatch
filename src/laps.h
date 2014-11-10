#pragma once
#include "clock.h"
	
#define LAPS_MAX_COUNT 30
	
typedef struct Laps {
  uint16_t count;
  Clock times[LAPS_MAX_COUNT];
  bool repeat;
} __attribute__((__packed__)) Laps;

Laps* laps_create(void);
void laps_reset(Laps*);
void laps_start(Laps*,Clock*,bool);
void laps_add(Laps*,Clock*);
Clock* laps_get(Laps* laps,int8_t i);
uint8_t laps_count(Laps* laps);
void laps_destroy(Laps*);

uint8_t laps_setText(Laps* laps,uint8_t i,char* string,bool shorter);
uint16_t laps_get_number(Laps* laps,uint8_t i);