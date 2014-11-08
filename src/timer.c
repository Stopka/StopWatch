#include <pebble.h>
#include "timer.h"

Timer* timer_create() {
	Timer* timer=malloc(sizeof(Timer));
	Clock* c=clock_createNull();
	timer->started=*c;
	timer->stopped=*c;
	clock_destroy(c);
	return timer;
}

TimerStatus timer_getStatus(Timer* timer){
	if(clock_isNull(&timer->started)){
		return TIMER_STATUS_STOPPED;
	}
	if(clock_isNull(&timer->stopped)){
		return TIMER_STATUS_RUNNING;
	}
	return TIMER_STATUS_PAUSED;
}

TimerDirection timer_getDirection(Timer* timer){
	return timer->direction;
}


void timer_start(Timer* timer) {
	if (! timer) { return; }
	if(timer_getStatus(timer)==TIMER_STATUS_RUNNING){
		return; 
	}
	Clock* now = clock_createActual();
	if(clock_isNull(&timer->started)){
		timer->started=*now;
	}else{
		Clock* delay=clock_subtract(now,&timer->stopped);
		clock_add(&timer->started,delay);
	}
	clock_destroy(now);
}

void timer_stop(Timer* timer) {
	if (! timer) { return; }
	if(timer_getStatus(timer)!=TIMER_STATUS_RUNNING){
		return; 
	}
	Clock* now=clock_createActual();
	timer->stopped=*now;
	clock_destroy(now);
}

void timer_reset(Timer* timer) {
  	if (! timer) { return; }
  	Clock* n=clock_createNull();
	timer->stopped=*n;
	timer->started=*n;
	clock_destroy(n);
}

void timer_destroy(Timer* timer) {
	if (! timer) { return; }
	free(timer);
}