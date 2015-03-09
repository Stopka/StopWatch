#pragma once
#include <pebble.h>

typedef struct Clock {
  time_t sec;
  uint16_t ms;
} __attribute__((__packed__)) Clock;

Clock* clock_subtract(Clock* a,Clock* b);//a-b (edits a)
Clock* clock_add(Clock* a,Clock* b);//a+b (edits a)
Clock* clock_clone(Clock* c);//duplicates
Clock* clock_create(int*);//constructor from int array (days,hours,min,sec,ms)
Clock* clock_createActual();//contructor actual time
Clock* clock_createNull();//constructor 0
int* clock_getVals(Clock* c);//returns array of values (days,hours,min,sec,ms)
void clock_destroy(Clock* c);//destructor
bool clock_isNull(Clock * clock);//is 0?
int8_t clock_compare(Clock* a,Clock* b);//0:a=b 1:a>b -1:a<b