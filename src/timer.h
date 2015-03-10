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

Timer* timer_create();//General constructor (for persitent load)
Timer* timer_create_stopwatch();//Stopwatch constructor
Timer* timer_create_timer();//Timer constructor
TimerStatus timer_getStatus(Timer*);//returns status (running...)
TimerDirection timer_getDirection(Timer*);//returns if it is stopwatch/timer
void timer_start(Timer*);//action start
void timer_stop(Timer*);//action start
void timer_lap(Timer*);//action new lap (only for stopwatch)
void timer_reset(Timer*);//action reset
void timer_destroy(Timer*);//destructor
uint8_t timer_getActualLap(Timer*);//gets number of actual lap running(only for timer)

//formats total time text to string buffer (rest for timer = ignores lap, lap total time for stopwatch), shorter means for menu view
uint8_t timer_setStopwatchTotalTime(Timer* timer,char* string,uint8_t lap,bool shorter );
//formats lap time text to string buffer
uint8_t timer_setLapTime(Timer* timer,char* string,uint8_t lap,bool shorter);
