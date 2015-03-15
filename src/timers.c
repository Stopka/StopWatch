#include <pebble.h>
#include "timers.h"
#include "count.h"
#include "scheduler.h"
#include "window_alarm.h"

#define STORAGE_VERSION 1
#define STORAGE_KEY_VERSION 0
#define STORAGE_KEY_COUNT 1
#define STORAGE_KEY_DATA 2
	
Timer* timers[TIMERS_MAX_COUNT];

Count* count;
int8_t selected=-1;
int8_t nearest=-1;

uint8_t timers_add(Timer*);

void storage_load(){
	APP_LOG(APP_LOG_LEVEL_INFO,"storage_load()");
	
	if(!persist_exists(STORAGE_KEY_VERSION)){ 
		APP_LOG(APP_LOG_LEVEL_WARNING,"no version"); 
		return;
	}
	if(persist_read_int(STORAGE_KEY_VERSION)!=STORAGE_VERSION){
		APP_LOG(APP_LOG_LEVEL_WARNING,"wrong version");
		return;
	}
	if(!persist_exists(STORAGE_KEY_COUNT)){ 
		APP_LOG(APP_LOG_LEVEL_ERROR,"no count"); 
		return;
	}
	uint8_t storage_count=(uint8_t)persist_read_int(STORAGE_KEY_COUNT);
 	APP_LOG(APP_LOG_LEVEL_INFO,"storage count: %d",storage_count);
	Timer* timer;
	int result;
	for(uint8_t i=0;i<storage_count;i++){
		if(!persist_exists(STORAGE_KEY_DATA+i)){ 
			APP_LOG(APP_LOG_LEVEL_ERROR,"missing timer %d in storage %d",i,STORAGE_KEY_DATA+i); 
			continue;
		}
		timer=timer_create();
		result=persist_read_data(STORAGE_KEY_DATA+i,timer,sizeof(Timer));
		if(result<0){
			APP_LOG(APP_LOG_LEVEL_ERROR,"read error timer %d in storage %d",i,STORAGE_KEY_DATA+i);
			timer_destroy(timer);
			continue;
		}
		timers_add(timer);
		APP_LOG(APP_LOG_LEVEL_INFO,"loaded timer %d",i);
	}
		
	APP_LOG(APP_LOG_LEVEL_INFO,"loaded");
}

void timers_updateNearest();

void handleAlarm(void* data){
	APP_LOG(APP_LOG_LEVEL_INFO,"timer finished");
	for(uint8_t i=timers_stopwatch_count();i<timers_count();i++){
		timer_checkEnd(timers_get(i));
	}
	window_alarm_show();
	timers_updateNearest();
}

void timers_init() {
	APP_LOG(APP_LOG_LEVEL_INFO,"timers_init()");
	count=count_create();
	storage_load();
	scheduler_init(handleAlarm,&nearest);
	timers_updateNearest();
}

uint8_t timers_count() {
	return timers_timer_count()+timers_stopwatch_count();
}

uint8_t timers_timer_count(){
	return count->timer;
}
uint8_t timers_stopwatch_count(){
	return count->stopwatch;
}

uint8_t timers_add(Timer* timer) {
	uint8_t pos;
	if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
		pos=count->stopwatch;
		for(uint8_t i=count_total(count);i>pos;i--){
			timers[i]=timers[i-1];
		}
		count->stopwatch++;
	}else{
		pos=count_total(count);
		count->timer++;
	}
	timers[pos]=timer;
	return pos;
}

Timer* timers_add_stopwatch() {
	if(timers_count()>=TIMERS_MAX_COUNT){
		return NULL;
	}
	return timers_select(timers_add(timer_create_stopwatch()));
}

Timer* timers_add_timer() {
	if(timers_count()>=TIMERS_MAX_COUNT){
		return NULL;
	}
	return timers_select(timers_add(timer_create_timer()));
}

Timer* timers_select(uint8_t pos) {
	selected=pos;
	return timers_get_selected();
}

Timer* timers_timer_select(uint8_t pos) {
	return timers_select(pos+timers_stopwatch_count());
}

Timer* timers_stopwatch_select(uint8_t pos) {
	return timers_select(pos);
}

bool timers_isSpace() {
	return timers_count()<TIMERS_MAX_COUNT;
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
	if(timer_getDirection(timers[pos])==TIMER_DIRECTION_UP){
		count->stopwatch--;
	}else{
		count->timer--;
	}
	timer_destroy(timers[pos]);
	for(uint8_t i=pos;i<timers_count();i++){
		timers[i]=timers[i+1];
	}
	timers_updateNearest();
}

void timers_remove_selected() {
	if(selected<0){
		return;
	}
	timers_remove(selected);
}
	
void storage_store(){
	APP_LOG(APP_LOG_LEVEL_INFO,"storage_store()");
	APP_LOG(APP_LOG_LEVEL_INFO,"max_cell_size: %d timer_size: %d laps_size: %d clock_size: %d",PERSIST_DATA_MAX_LENGTH,sizeof(Timer),sizeof(Laps),sizeof(Clock));
	
	int result;
	uint8_t c=timers_count();
	
	result=persist_write_int(STORAGE_KEY_VERSION,STORAGE_VERSION);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"write error count %d in storage %d #%d",c,STORAGE_KEY_COUNT,result);  
		return;
	}
	
	result=persist_write_int(STORAGE_KEY_COUNT,c);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"write error count %d in storage %d #%d",count_total(count),STORAGE_KEY_COUNT,result); 
		return;
	}
	APP_LOG(APP_LOG_LEVEL_INFO,"timer count %d",count_total(count));
	
	Timer* timer;
	for(uint8_t i=0;i<c;i++){
		timer=timers_get(i);
		result=persist_write_data(STORAGE_KEY_DATA+i,timer,sizeof(Timer));
		if(result<0){
			APP_LOG(APP_LOG_LEVEL_ERROR,"write error timer %d in storage %d #%d",i,STORAGE_KEY_DATA+i,result); 
			continue;
		}
		APP_LOG(APP_LOG_LEVEL_INFO,"stored timer %d",i);
	}
	APP_LOG(APP_LOG_LEVEL_INFO,"stored");
}

void timers_deinit(void) {
	APP_LOG(APP_LOG_LEVEL_INFO,"timers_deinit()");
	storage_store();
	
	APP_LOG(APP_LOG_LEVEL_INFO,"cleaning..");
  for(uint8_t i=0;i<count_total(count);i++){
		free(timers[i]);
	}
	count_destroy(count);
	scheduler_deinit();
	APP_LOG(APP_LOG_LEVEL_INFO,"cleaned");
}

void timers_selected_start(){
	timer_start(timers_get_selected());
	timers_updateNearest();
}
void timers_selected_stop(){
	timer_stop(timers_get_selected());
	timers_updateNearest();
}
void timers_selected_lap(){
	timer_lap(timers_get_selected());
	timers_updateNearest();
}
void timers_selected_reset(){
	timer_reset(timers_get_selected());
	timers_updateNearest();
}

void timers_updateNearest(){
	uint8_t min=0;
	Clock* minc=NULL;
	for(uint8_t i=timers_stopwatch_count();i<timers_count();i++){
		Timer* t=timers_get(i);
		Clock* c=timer_getNextFinish(t);
		if(c!=NULL&&(minc==NULL||clock_compare(minc,c)>0)){
			clock_destroy(minc);
			minc=c;
			min=i;
		}else{
			clock_destroy(minc);
		}
	}
	nearest=min;
	scheduler_update(minc);
}