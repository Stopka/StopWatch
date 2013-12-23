#include "stopwatch_model.h"
Time* started=NULL;
Time* laps[STOPWATCH_MAX_LAPS+1];
int laps_count=0;
int total_laps_count=0;
bool running=false;
	
void stopwatch_model_init(){
	if(!persist_exists(0)){
		stopwatch_model_reset();
		return;
	}
	running=persist_read_bool(0);
	persist_delete(0);
	laps_count=persist_read_int(1);
	persist_delete(1);
	total_laps_count=persist_read_int(2);
	persist_delete(2);
	if(persist_exists(3)){
		started=(Time*) malloc(sizeof(Time));
		persist_read_data(3,started,sizeof(Time));
		persist_delete(3);
	}
	for(int i=0;i<=laps_count;i++){
		if(persist_exists(4+i)){
			laps[i]=(Time*) malloc(sizeof(Time));
			persist_read_data(4+i,laps[i],sizeof(Time));
			persist_delete(4+i);
		}
	}
}
void stopwatch_model_deinit(){
	persist_write_bool(0,running);
	persist_write_int(1,laps_count);
	persist_write_int(2,total_laps_count);
	if(started!=NULL){
		persist_write_data(3,started,sizeof(Time));
	}
	for(int i=0;i<=laps_count;i++){
		if(laps[i]!=NULL){
			persist_write_data(4+i,laps[i],sizeof(Time));
		}
	}
	stopwatch_model_reset();
}



void stopwatch_model_reset(){
	running=false;
	free(started);
	started=NULL;
	for(int i=0;i<=laps_count;i++){
		if(laps[i]!=NULL){
			free(laps[i]);
			laps[i]=NULL;
		}
	}
	laps_count=1;
	total_laps_count=1;
}

void stopwatch_model_newlap(){
	Time* now=time_getActual();
	if(laps_count==STOPWATCH_MAX_LAPS){
		free(laps[0]);
		for(int i=0;i<laps_count-1;i++){
			laps[i]=laps[i+1];
		}
		laps[laps_count-1]=now;
	}else{
		laps[laps_count]=now;
		laps_count++;
	}
	total_laps_count++;
}
void stopwatch_model_start(){
	if(running){
		return;
	}
	Time* now=time_getActual();
	if(started==NULL){
		started=now;
		laps[0]=time_clone(now);
	}else{
		Time* delay=time_subtract(now,laps[laps_count]);
		free(laps[laps_count]);
		laps[laps_count]=NULL;
		time_add(started,delay);
		for(int i=0;i<laps_count;i++){
			time_add(laps[i],delay);
		}
		free(delay);
	}
	running=true;
}
void stopwatch_model_stop(){
	if(!running){
		return;
	}
	laps[laps_count]=time_getActual();
	running=false;
}

bool stopwatch_model_isRunning(){
	return running;
}

int stopwatch_model_getLapsCount(){
	return laps_count;
}

int stopwatch_model_getTotalLapsCount(){
	return total_laps_count;
}

Time* stopwatch_model_getLapTime(int lap){
	if(running&&lap==laps_count-1){
		return time_subtract(time_getActual(),laps[lap]);
	}
	if(laps[lap]==NULL){
		return time_getNull();
	}
	return time_subtract(time_clone(laps[lap+1]),laps[lap]);
}
Time* stopwatch_model_getLapTotalTime(int lap){
	if(started==NULL){
		return time_getNull();
	}
	if(running&&lap==laps_count-1){
		return time_subtract(time_getActual(),started);
	}
	return time_subtract(time_clone(laps[lap+1]),started);
}