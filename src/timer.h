#pragma once
#include "clock.h"

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
  Clock times[10];
  Clock started;
  Clock stopped;
  bool repeat;
} __attribute__((__packed__)) Timer;


Timer* timer_create();
TimerStatus timer_getStatus(Timer*);
TimerDirection timer_getDirection(Timer*);
void timer_start(Timer*);
void timer_stop(Timer*);
void timer_reset(Timer*);
void timer_destroy(Timer*);
