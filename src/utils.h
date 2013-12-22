#ifndef UTILS_H
#define UTILS_H
#include <time.h>
#include <pebble.h>

typedef struct Time{
	time_t sec;
	uint16_t msec;
}Time;

Time* time_subtract(Time* a,Time* b);
Time* time_add(Time* a,Time* b);
Time* time_clone(Time* t);

Time* time_getActual();
Time* time_getNull();

#endif