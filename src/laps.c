#include <pebble.h>
#include "laps.h"

void laps_reset(Laps* laps){
	if(!laps){return;}
	laps->count=1;
	Clock* n=clock_createNull();
	laps->times[0]=*n;
	clock_destroy(n);
}
	
Laps* laps_create(void){
	Laps* laps=malloc(sizeof(Laps));
	laps_reset(laps);
	return laps;
}

void laps_add(Laps* laps,Clock* clock){
	for(uint8_t i=LAPS_MAX_COUNT-1;i>0;i--){
			laps->times[i]=laps->times[i-1];
	}
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

void laps_destroy(Laps* laps){
	if(!laps){return;}
	free(laps);
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