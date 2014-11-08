#pragma once
#include "clock.h"
	
#define LAPS_MAX_COUNT 30
	
typedef struct Laps {
  uint16_t number;
	uint8_t selected;
  Clock times[LAPS_MAX_COUNT];
  bool repeat;
} __attribute__((__packed__)) Laps;