#include <pebble.h>
#include "timers.h"

#define STORAGE_VERSION 0
#define STORAGE_KEY_VERSION 0
#define STORAGE_KEY_COUNT 1
#define STORAGE_KEY_DATA 2
	
Timer* timers[TIMERS_MAX_COUNT];
uint8_t count=0;
int8_t selected=-1;

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
	for(uint8_t i=0;i<storage_count;i+=2){
		if(!persist_exists(STORAGE_KEY_DATA+i)){ 
			APP_LOG(APP_LOG_LEVEL_ERROR,"missing timer %d in storage %d",i/2,i); 
			continue;
		}
		timer=timer_create();
		result=persist_read_data(STORAGE_KEY_DATA+i,timer,sizeof(Timer));
		if(result<0){
			APP_LOG(APP_LOG_LEVEL_ERROR,"read error timer %d in storage %d",i/2,i);
			timer_destroy(timer);
			continue;
		}
		if(timer_getDirection(timer)==TIMER_DIRECTION_UP){
			if(!persist_exists(STORAGE_KEY_DATA+i+1)){ 
				APP_LOG(APP_LOG_LEVEL_ERROR,"missing laps %d in storage %d",i/2,i+1);
				timer_destroy(timer);
				continue;
			}
			timer->laps=laps_create();
			result=persist_read_data(STORAGE_KEY_DATA+i+1,timer->laps,sizeof(Laps));
			if(result<0){ 
				APP_LOG(APP_LOG_LEVEL_ERROR,"read error laps %d in storage %d",i/2,i+1); 
				timer_destroy(timer);
				continue;
			}
		}
		timers_add(timer);
		APP_LOG(APP_LOG_LEVEL_INFO,"loaded timer %d",i/2);
	}
		
	APP_LOG(APP_LOG_LEVEL_INFO,"loaded");
}

void timers_init() {
  APP_LOG(APP_LOG_LEVEL_INFO,"timers_init()");
	storage_load();
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
	if(count>=TIMERS_MAX_COUNT){
		return NULL;
	}
	return timers_select(timers_add(timer_create_stopwatch()));
}

Timer* timers_select(uint8_t pos) {
	selected=pos;
	return timers_get_selected();
}

bool timers_isSpace() {
	return count<TIMERS_MAX_COUNT;
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
	
void storage_store(){
	APP_LOG(APP_LOG_LEVEL_INFO,"storage_store()");
	
	int result;
	uint8_t c=timers_count();
	
	result=persist_delete(STORAGE_KEY_VERSION);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"delete error version in storage %d #%d",STORAGE_KEY_VERSION,result); 
		return;
	}
	result=persist_write_int(STORAGE_KEY_VERSION,STORAGE_VERSION);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"write error version %d in storage %d #%d",STORAGE_VERSION,STORAGE_KEY_VERSION,result); 
		return;
	}
	
	result=persist_delete(STORAGE_KEY_COUNT);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"delete error count in storage %d #%d",STORAGE_KEY_COUNT,result); 
		return;
	}
	result=persist_write_int(STORAGE_KEY_COUNT,c);
	if(result<0){
		APP_LOG(APP_LOG_LEVEL_ERROR,"write error count %d in storage %d #%d",c,STORAGE_KEY_COUNT,result); 
		return;
	}
	APP_LOG(APP_LOG_LEVEL_INFO,"timer count %d",c);
	
	Timer* timer;
	for(uint8_t i=0;i<c;i++){
		timer=timers_get(i);
		result=persist_delete(STORAGE_KEY_DATA+(2*i));
		if(result<0){
			APP_LOG(APP_LOG_LEVEL_ERROR,"delete error timer %d in storage %d #%d",i,STORAGE_KEY_DATA+(2*i),result); 
			continue;
		}
		result=persist_write_data(STORAGE_KEY_DATA+(2*i),timer,sizeof(Timer));
		if(result<0){
			APP_LOG(APP_LOG_LEVEL_ERROR,"write error timer %d in storage %d #%d",i,STORAGE_KEY_DATA+(2*i),result); 
			continue;
		}
		
		if(timer->laps!=NULL){
			result=persist_delete(STORAGE_KEY_DATA+(2*i)+1);
			if(result<0){
				APP_LOG(APP_LOG_LEVEL_ERROR,"delete error laps %d in storage %d #%d",i,STORAGE_KEY_DATA+(2*i)+1,result); 
				continue;
			}
			result=persist_write_data(STORAGE_KEY_DATA+(2*i)+1,timer->laps,sizeof(Laps));
			if(result<0){
				APP_LOG(APP_LOG_LEVEL_ERROR,"write error laps %d in storage %d #%d",i,STORAGE_KEY_DATA+(2*i)+1,result); 
				continue;
			}
		}
		APP_LOG(APP_LOG_LEVEL_INFO,"stored timer %d",i);
	}
	APP_LOG(APP_LOG_LEVEL_INFO,"stored");
}

void timers_deinit(void) {
	APP_LOG(APP_LOG_LEVEL_INFO,"timers_deinit()");
	storage_store();
	
	APP_LOG(APP_LOG_LEVEL_INFO,"cleaning..");
  for(uint8_t i=0;i<count;i++){
		timers_remove(i);
	}
	APP_LOG(APP_LOG_LEVEL_INFO,"cleaned");
}