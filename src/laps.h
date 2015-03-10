#pragma once
#include "clock.h"
	
#define LAPS_MAX_COUNT 30
	
typedef struct Laps {
  uint16_t count;
  Clock times[LAPS_MAX_COUNT];
  bool repeat;
} __attribute__((__packed__)) Laps;

Laps* laps_create(void);//laps constructor
void laps_reset(Laps*);//Removes all laps
void laps_start(Laps*,Clock*,bool);//Called when stopwatch startes, it recounts lap times - bool param marks unpause action (for stopwatch)
void laps_add(Laps*,Clock*);//Adds new lap to the front, can oveflow counter (for stopwatch)
void laps_insert(Laps*,Clock*);//Adds new lap to the end, can't oveflow counter (for timer)
bool laps_isFull(Laps*);//true if there is no room for new lap insert
Clock* laps_get(Laps* laps,int8_t i);//return lap on position
void laps_setVals(Laps* laps,int8_t i,int* vals);//sets clock made by vals on position
uint8_t laps_count(Laps* laps);//Number of added laps
void laps_destroy(Laps*);//laps destructor

uint8_t laps_setText(Laps* laps,uint8_t i,char* string,bool shorter);
uint16_t laps_get_number(Laps* laps,uint8_t i);