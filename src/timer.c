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
	laps_reset(&t->laps);
	return t;
}

Timer* timer_create_timer(){
	Timer* t=timer_create();
	t->direction=TIMER_DIRECTION_DOWN;
	laps_reset(&t->laps);
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
		if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
			laps_start(&timer->laps,now,false);
		}
	}else{
		Clock* delay=clock_subtract(now,&timer->stopped);
		clock_add(&timer->started,delay);
		if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
			laps_start(&timer->laps,delay,true);
		}
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
	if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
		laps_reset(&timer->laps);
	}
}

void timer_lap(Timer* timer) {
	if(!timer || timer_getDirection(timer)!=TIMER_DIRECTION_UP || timer_getStatus(timer)!=TIMER_STATUS_RUNNING){
		return;
	}
	Clock* now=clock_createActual();
	laps_add(&timer->laps,now);
	clock_destroy(now);
}

void timer_destroy(Timer* timer) {
	free(timer);
}

Clock* timer_get_end(Timer* timer){
	return timer_getStatus(timer)==TIMER_STATUS_RUNNING?clock_createActual():clock_clone(&timer->stopped);
}

Clock* timer_get_stopwatch_lap_time(Timer* timer,uint8_t i,bool total){
	Clock* from=total?&timer->started:laps_get(&timer->laps,i);
	Clock* prev=laps_get(&timer->laps,i-1);
	Clock* to=prev!=NULL?clock_clone(prev):timer_get_end(timer);
	return clock_subtract(to,from);
}

uint8_t timer_getActualLap(Timer* timer){//neozkoušeno
	if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
		return 0;
	}
	Clock* from=clock_clone(&timer->started);
	Clock* to=timer_get_end(timer);
	uint8_t lap;
	for(lap=0;clock_compare(to,from)>=0&&lap<laps_count(&timer->laps);lap++){
		clock_add(from,laps_get(&timer->laps,lap));
	}
	clock_destroy(to);
	clock_destroy(from);
	return lap-1;
}

Clock* timer_getNextFinish(Timer* timer){
	if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
		return NULL;
	}
	if(timer_getStatus(timer)!=TIMER_STATUS_RUNNING){
		return NULL;
	}
	Clock* from=clock_clone(&timer->started);
	Clock* to=timer_get_end(timer);
	for(uint8_t lap=0;clock_compare(to,from)>=0&&lap<laps_count(&timer->laps);lap++){
		clock_add(from,laps_get(&timer->laps,lap));
	}
	clock_destroy(to);
	return from;
}

Clock* timer_get_timer_lap_time(Timer* timer,uint8_t lap,bool total){
	if(!total){
		return clock_clone(laps_get(&timer->laps,lap));
	}
	Clock* finish=clock_clone(&timer->started);
	for(uint8_t i=0;i<=lap;i++){
		clock_add(finish,laps_get(&timer->laps,i));
	}
	Clock* to=timer_get_end(timer);
	if(clock_compare(finish,to)>0){
		clock_subtract(finish,to);	
	}else{
		clock_destroy(finish);
		finish=clock_createNull();
	}
	clock_destroy(to);
	return finish;
}

uint8_t timer_setTotalTime(Timer* timer,char* string,uint8_t lap,bool shorter){
	uint8_t measure_offset;
	bool stopwatch=timer_getDirection(timer)==TIMER_DIRECTION_UP;
	shorter=!stopwatch||shorter;
	Clock* time=stopwatch?timer_get_stopwatch_lap_time(timer,lap,true):timer_get_timer_lap_time(timer,lap,true);
	int* vals=clock_getVals(time);
	if(vals[0]/100>0){
			snprintf(string, 12, "%04d %02d:%02d", vals[0],vals[1],vals[2]);
			measure_offset=0;
	}else if(vals[0]>0||shorter){
			snprintf(string, 12, "%02d %02d:%02d:%02d", vals[0],vals[1],vals[2],vals[3]);
			measure_offset=1;
	}else{
		if(timer_getStatus(timer)==TIMER_STATUS_RUNNING&&lap==0){
			snprintf(string, 12, "%02d:%02d:%02d.??", vals[1],vals[2],vals[3]);
		}else{
			snprintf(string, 12, "%02d:%02d:%02d.%02d", vals[1],vals[2],vals[3],vals[4]/10);
		}
		measure_offset=2;
	}
	free(vals);
	clock_destroy(time);
	return measure_offset;
}

uint8_t timer_setLapTime(Timer* timer,char* string,uint8_t lap,bool shorter){
	uint8_t measure_offset;
	bool stopwatch=timer_getDirection(timer)==TIMER_DIRECTION_UP;
	shorter=!stopwatch||shorter;
	Clock* time=stopwatch?timer_get_stopwatch_lap_time(timer,lap,false):timer_get_timer_lap_time(timer,lap,false);
	uint8_t lap_numer=laps_get_number(&timer->laps,lap);
	int* vals=clock_getVals(time);
	if(vals[0]/100>0){
			snprintf(string, 17, "%03d  %04d %02d:%02d", lap_numer,vals[0],vals[1],vals[2]);
			measure_offset=0;
	}else if(vals[0]>0||shorter){
			snprintf(string, 17, "%03d  %02d %02d:%02d:%02d", lap_numer,vals[0],vals[1],vals[2],vals[3]);
			measure_offset=1;
	}else{
		if(timer_getStatus(timer)==TIMER_STATUS_RUNNING&&lap==0){
			snprintf(string, 17, "%03d  %02d:%02d:%02d.??",lap_numer, vals[1],vals[2],vals[3]);
		}else{
			snprintf(string, 17, "%03d  %02d:%02d:%02d.%02d",lap_numer, vals[1],vals[2],vals[3],vals[4]);
		}
		measure_offset=2;
	}
	free(vals);
	clock_destroy(time);
	return measure_offset;
}

void timer_checkEnd(Timer* timer){
	if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
		return;
	}
	Clock* from=clock_clone(&timer->started);
	Clock* to=timer_get_end(timer);
	for(uint8_t lap=0;lap<laps_count(&timer->laps);lap++){
		clock_add(from,laps_get(&timer->laps,lap));
	}
	if(clock_compare(to,from)>=0){
		APP_LOG(APP_LOG_LEVEL_INFO,"finalizing timer");
		timer_reset(timer);
	}
	clock_destroy(to);
	clock_destroy(from);
}