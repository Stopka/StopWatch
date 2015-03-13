#include "scheduler.h"

AppTimer* app_timer=NULL;
AppTimerCallback callback;
Clock* timer_clock;
void* data;	


void cancelAppTimer(){
	if(app_timer==NULL){
		return;
	}
	app_timer_cancel(app_timer);
	app_timer=NULL;
}

void scheduleAppTimer(){
	cancelAppTimer();
	Clock* now=clock_createActual();
	if(clock_compare(timer_clock,now)<=0){
		clock_destroy(now);
		return;
	}
	Clock* rest=clock_subtract(clock_clone(timer_clock),now);
	app_timer = app_timer_register(clock_getMS(rest), callback, data);
	clock_destroy(now);
	clock_destroy(rest);
}

void cancelWakeUp(){
	//TODO
}

void scheduleWakeUp(){
	//TODO
}

void scheduler_update(Clock* c){
	clock_destroy(timer_clock);
	timer_clock=c;
	scheduleAppTimer();
}

void scheduler_init(AppTimerCallback cb,void* d){
	callback=cb;
	data=d;
	cancelWakeUp();
}

void scheduler_deinit(){
	cancelAppTimer();
	scheduleWakeUp();
}