#pragma once
#include "clock.h"
#include "laps.h"

typedef enum {
  TIMER_STATUS_STOPPED,
  TIMER_STATUS_RUNNING,
  TIMER_STATUS_PAUSED,
  TIMER_STATUS_DONE
} TimerStatus;

typedef enum {
  TIMER_DIRECTION_UP,
  TIMER_DIRECTION_DOWN
} TimerDirection;
	
typedef struct Timer {
  TimerDirection direction;
	Clock started;
  Clock stopped;
	Laps laps;
  bool repeat;
} __attribute__((__packed__)) Timer;

Timer* timer_create();
Timer* timer_create_stopwatch();
TimerStatus timer_getStatus(Timer*);
TimerDirection timer_getDirection(Timer*);
void timer_start(Timer*);
void timer_stop(Timer*);
void timer_lap(Timer*);
void timer_reset(Timer*);
void timer_destroy(Timer*);

uint8_t timer_setStopwatchTotalTime(Timer* timer,char* string,uint8_t lap,bool shorter );
uint8_t timer_setLapTime(Timer* timer,char* string,uint8_t lap,bool shorter);
