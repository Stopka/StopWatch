#pragma once
#include <pebble.h>

typedef struct Clock {
  time_t sec;
  uint16_t ms;
} __attribute__((__packed__)) Clock;

Clock* clock_subtract(Clock* a,Clock* b);
Clock* clock_add(Clock* a,Clock* b);
Clock* clock_clone(Clock* c);
Clock* clock_createActual();
Clock* clock_createNull();
void clock_destroy(Clock* c);
bool clock_isNull(Clock * clock);
int clock_compare(Clock* a,Clock* b);