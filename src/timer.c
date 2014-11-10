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

Timer* timer_create_stopwatch(){
	Timer* t=timer_create();
	t->direction=TIMER_DIRECTION_UP;
	t->laps=laps_create();
	return t;
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
	Clock* zero = clock_createNull();
	if(clock_isNull(&timer->started)){
		timer->started=*now;
		laps_start(timer->laps,now,false);
	}else{
		Clock* delay=clock_subtract(now,&timer->stopped);
		clock_add(&timer->started,delay);
		laps_start(timer->laps,delay,true);
	}
	timer->stopped=*zero;
	clock_destroy(zero);
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
	laps_reset(timer->laps);
}

void timer_lap(Timer* timer) {
	Clock* now=clock_createActual();
	laps_add(timer->laps,now);
	clock_destroy(now);
}

void timer_destroy(Timer* timer) {
	laps_destroy(timer->laps);
	free(timer);
}

Clock* timer_get_total_time(Timer* timer){
	Clock* from=&timer->started;
	Clock* to=timer_getStatus(timer)==TIMER_STATUS_RUNNING? clock_createActual(): clock_clone(&timer->stopped);
	return clock_subtract(to,from);
}

Clock* timer_get_stopwatch_total_time(Timer* timer){
	Clock* from=&timer->started;
	Clock* to=timer_getStatus(timer)==TIMER_STATUS_RUNNING? clock_createActual(): clock_clone(&timer->stopped);
	return clock_subtract(to,from);
}

uint8_t timer_setStopwatchTotalTime(Timer* timer,char* string,bool shorter){
	uint8_t measure_offset;
	Clock* time=timer_get_total_time(timer);
	int vals[]={time->ms/10,time->sec%60,(time->sec/60)%60,(time->sec/(60*60))%24,(time->sec/(60*60*24))};
	if(vals[4]%100>0){
			snprintf(string, 12, "%05d %02d:%02d", vals[4],vals[3],vals[2]);
			measure_offset=0;
	}else if(vals[4]>0){
			snprintf(string, 12, "%02d %02d:%02d:%02d", vals[4],vals[3],vals[2],vals[1]);
			measure_offset=1;
	}else{
		if(timer_getStatus(timer)==TIMER_STATUS_RUNNING){
			snprintf(string, 12, shorter?"%02d:%02d:%02d":"%02d:%02d:%02d.??", vals[3],vals[2],vals[1]);
		}else{
			snprintf(string, 12, "%02d:%02d:%02d.%02d", vals[3],vals[2],vals[1],vals[0]);
		}
		measure_offset=2;
	}
	clock_destroy(time);
	return measure_offset;
}

Clock* timer_get_stopwatch_lap_time(Timer* timer,uint8_t i){
	Clock* from=laps_get(timer->laps,i);
	Clock* prev=laps_get(timer->laps,i-1);
	Clock* to=prev!=NULL?clock_clone(prev):(timer_getStatus(timer)==TIMER_STATUS_RUNNING? clock_createActual(): clock_clone(&timer->stopped));
	return clock_subtract(to,from);
}

uint8_t timer_setLapTime(Timer* timer,char* string,uint8_t lap,bool shorter){
	uint8_t measure_offset;
	Clock* time=timer_get_stopwatch_lap_time(timer,lap);
	uint8_t lap_numer=laps_get_number(timer->laps,lap);
	int vals[]={time->ms/10,time->sec%60,(time->sec/60)%60,(time->sec/(60*60))%24,(time->sec/(60*60*24))};
	if(vals[4]%100>0){
			snprintf(string, 17, "%03d  %05d %02d:%02d", lap_numer,vals[4],vals[3],vals[2]);
			measure_offset=0;
	}else if(vals[4]>0){
			snprintf(string, 17, "%03d  %02d %02d:%02d:%02d", lap_numer,vals[4],vals[3],vals[2],vals[1]);
			measure_offset=1;
	}else{
		if(timer_getStatus(timer)==TIMER_STATUS_RUNNING&&lap==0){
			snprintf(string, 17, shorter?"%03d  %02d:%02d:%02d":"%03d  %02d:%02d:%02d.??",lap_numer, vals[3],vals[2],vals[1]);
		}else{
			snprintf(string, 17, "%03d  %02d:%02d:%02d.%02d",lap_numer, vals[3],vals[2],vals[1],vals[0]);
		}
		measure_offset=2;
	}
	clock_destroy(time);
	return measure_offset;
}