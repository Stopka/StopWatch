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

void laps_remove_lap(Laps* laps){
	for(uint8_t i=0;i<laps->count-1;i++){
		laps->times[i]=laps->times[i+1];
	}
	laps->count--;
}

void laps_add(Laps* laps,Clock* clock){
	if(laps->count==LAPS_MAX_COUNT){
		laps_remove_lap(laps);
	}
	laps->times[laps->count]=*clock;
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