#pragma once
#include "clock.h"
	
#define LAPS_MAX_COUNT 30
	
typedef struct Laps {
  uint16_t count;
	uint8_t selected;
  Clock times[LAPS_MAX_COUNT];
  bool repeat;
} __attribute__((__packed__)) Laps;

Laps* laps_create(void);
void laps_reset(Laps*);
void laps_start(Laps*,Clock*,bool);
void laps_add(Laps*,Clock*);
void laps_destroy(Laps*);