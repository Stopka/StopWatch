#include "scheduler.h"

AppTimer* app_timer=NULL;
AppTimerCallback callback;
Clock* timer_clock;
void* data;	

void cancelAppTimer(){
	if(app_timer!=NULL){
		app_timer_cancel(app_timer);
		app_timer=NULL;
	}
}

void replaceClock(Clock* c){
	if(timer_clock!=NULL){
		clock_destroy(timer_clock);
	}
	timer_clock=c;
}

static void handlerAppTimer(void* d){
	replaceClock(NULL);
	app_timer=NULL;
	cancelAppTimer();
	callback(data);
}

void scheduleAppTimer(){
	cancelAppTimer();
	APP_LOG(APP_LOG_LEVEL_INFO,"Scheduling time.");
	if(timer_clock==NULL){
		APP_LOG(APP_LOG_LEVEL_WARNING,"Nothing to schedule.");
		return;
	}
	Clock* now=clock_createActual();
	if(clock_compare(timer_clock,now)<=0){
		clock_destroy(now);
		APP_LOG(APP_LOG_LEVEL_WARNING,"Schedule time already passed, firing now.");
		callback(data);
		return;
	}
	Clock* rest=clock_subtract(clock_clone(timer_clock),now);
	app_timer = app_timer_register(clock_getMS(rest), handlerAppTimer, NULL);
	clock_destroy(now);
	clock_destroy(rest);
}

void cancelWakeUp(){
	wakeup_cancel_all();
}

void scheduleWakeUp(){
	cancelWakeUp();
	APP_LOG(APP_LOG_LEVEL_INFO,"Scheduling wakeup.");
	if(timer_clock==NULL){
		APP_LOG(APP_LOG_LEVEL_WARNING,"Nothing to schedule.");
		return;
	}
	WakeupId result;
	uint16_t correct=0;
	do{
		result = wakeup_schedule(timer_clock->sec+correct, 0, true);
		if(result==E_INVALID_ARGUMENT){
			APP_LOG(APP_LOG_LEVEL_ERROR,"Shedule error time already passed.");
			return;
		}
		if(result==E_OUT_OF_RESOURCES){
			APP_LOG(APP_LOG_LEVEL_ERROR,"Schedule error out of resources.");
			return;
		}
		if(result==E_INTERNAL){
			APP_LOG(APP_LOG_LEVEL_ERROR,"Schedule internal error.");
			return;
		}
		if(result==E_RANGE){
			correct+=5;
			APP_LOG(APP_LOG_LEVEL_WARNING,"Schedule time range already used. Trying schedule time + %d sec",correct);
		}
	}while(result==E_RANGE&&correct<=120);
	if(result==E_RANGE){
		APP_LOG(APP_LOG_LEVEL_WARNING,"Schedule time range already used. Giving up.");
	}
	if(result>=0){
		APP_LOG(APP_LOG_LEVEL_INFO,"Wake up scheduled.");
	}
}

void scheduler_update(Clock* c){
	replaceClock(c);
	scheduleAppTimer();
}

void scheduler_init(AppTimerCallback cb,void* d){
	callback=cb;
	data=d;
	cancelWakeUp();
}

void scheduler_deinit(){
	scheduleWakeUp();
	cancelAppTimer();
	replaceClock(NULL);
}