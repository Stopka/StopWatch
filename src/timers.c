#include <pebble.h>
#include "timers.h"
#define TIMER_MAX_COUNT 15

Timer* timers[TIMER_MAX_COUNT];
uint8_t count=0;
int8_t selected=-1;

void timers_init() {
  APP_LOG(APP_LOG_LEVEL_INFO,"timers_init()");
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

Timer* timers_add_stopwatch() {
	return timers_select(timers_add(timer_create_stopwatch()));
}

Timer* timers_select(uint8_t pos) {
	selected=pos;
	return timers_get_selected();
}

Timer* timers_get_selected() {
	return timers_get(selected);
}

int8_t timers_get_selectedIndex() {
	return selected;
}

Timer* timers_get(uint8_t pos) {
	return timers[pos];
}

void timers_remove(uint8_t pos) {
	if(selected==pos){
		selected=-1;
	}
	if(selected>pos){
		selected--;
	}
	timer_destroy(timers[pos]);
	count--;
	for(uint8_t i=pos;i<count;i++){
		timers[i]=timers[i+1];
	}
}

void timers_remove_selected() {
	if(selected<0){
		return;
	}
	timers_remove(selected);
}

void timers_deinit(void) {
	APP_LOG(APP_LOG_LEVEL_INFO,"timers_deinit()");
  for(uint8_t i=0;i<count;i++){
		timers_remove(i);
	}
}