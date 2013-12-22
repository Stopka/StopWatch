#include "stopwatch_model.h"
Time* started=NULL;
Time* laps[STOPWATCH_MAX_LAPS+1];
int laps_count=0;
int total_laps_count=0;
bool running=false;
	
void stopwatch_model_init(){
	stopwatch_model_reset();
}
void stopwatch_model_deinit(){
	stopwatch_model_reset();
}



void stopwatch_model_reset(){
	running=false;
	free(started);
	started=NULL;
	for(int i=0;i<laps_count+1;i++){
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
		Time* delay=time_subtract(laps[laps_count],now);
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
	return laps_count;
}

Time* stopwatch_model_getLapTime(int lap){
	if(running&&lap==laps_count-1){
		return time_subtract(time_getActual(),laps[lap]);
	}
	return time_subtract(time_clone(laps[lap+1]),laps[lap]);
}
Time* stopwatch_model_getLapTotalTime(int lap){
	if(running&&lap==laps_count-1){
		return time_subtract(time_getActual(),started);
	}
	return time_subtract(laps[lap+1],started);
}