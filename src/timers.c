#include <pebble.h>
#include "timers.h"
#define TIMER_MAX_COUNT 15

Timer* timers[TIMER_MAX_COUNT];
uint8_t count=0;

void timers_init() {
  
}

uint8_t timers_count() {
	return count;
}

uint8_t timers_add(Timer* timer) {
	uint8_t pos=count;
	count++;
	timers[pos]=timer;
	return pos;
}

Timer* timers_get(int pos) {
	return timers[pos];
}

Timer* timers_remove(int pos) {
	Timer* res=timers[pos];
	count--;
	for(uint8_t i=pos;i<count;i++){
		timers[i]=timers[i+1];
	}
	return res;
}

void timers_deinit(void) {
  
}