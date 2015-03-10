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
	for(uint8_t i=LAPS_MAX_COUNT-1;i>0;i--){
			laps->times[i]=laps->times[i-1];
	}
	laps->times[0]=*clock;
	laps->count++;
}

void laps_insert(Laps* laps,Clock* clock){
	if(!laps||laps_isFull(laps)){ return; }
	laps->times[laps->count++]=*clock;
}

bool laps_isFull(Laps* laps){
	return laps_count(laps)>=LAPS_MAX_COUNT;
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

void laps_setVals(Laps* laps,int8_t i,int* vals){
	Clock* c=clock_create(vals);
	laps->times[i]=*c;
	clock_destroy(c);
}

uint16_t laps_get_number(Laps* laps,uint8_t i){
	return (laps->count-i)%1000;
}