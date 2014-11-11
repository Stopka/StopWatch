#include <pebble.h>
#include "laps.h"

void laps_reset(Laps* laps){
	if(!laps){return;}
	laps->count=1;
	Clock* n=clock_createNull();
	laps->times[0]=*n;
	clock_destroy(n);
}

void laps_add(Laps* laps,Clock* clock){
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"laps_add()");
	for(uint8_t i=LAPS_MAX_COUNT-1;i>0;i--){
			//APP_LOG(APP_LOG_LEVEL_DEBUG,"move clock(%d,%d) %d>%d",(int)laps->times[i].sec,laps->times[i].ms,i-1,i);
			laps->times[i]=laps->times[i-1];
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"save clock(%d,%d) >0",(int)clock->sec,clock->ms);
	laps->times[0]=*clock;
	laps->count++;
}

void laps_start(Laps* laps,Clock* clock,bool restart){
	if(!laps){return;}
	if(restart){
		for(uint8_t i=0;i<laps->count;i++){
			clock_add(&laps->times[i],clock);
		}
		return;
	}
	laps->times[0]=*clock;
	laps->count=1;
}

uint8_t laps_count(Laps* laps){
	return laps->count>LAPS_MAX_COUNT?LAPS_MAX_COUNT:laps->count;
}

Clock* laps_get(Laps* laps,int8_t i){
	if(i>=laps_count(laps)||i<0){
		return NULL;
	}
	return &laps->times[i];
}

uint16_t laps_get_number(Laps* laps,uint8_t i){
	return (laps->count-i)%1000;
}