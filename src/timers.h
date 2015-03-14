#pragma once
#include "timer.h"

#define TIMERS_MAX_COUNT 10

void timers_init();
Timer* timers_add_stopwatch(void);//creates, stores, selects and returns new stopwatch
Timer* timers_add_timer(void);//creates, stores, selects and returns new timer
Timer* timers_select(uint8_t pos);//marks timer with that position as selected
Timer* timers_timer_select(uint8_t pos);//marks stopwatch with that stopwatch position as selected
Timer* timers_stopwatch_select(uint8_t pos);//marks timer with that timer position as selected
Timer* timers_get(uint8_t pos);//returns timer on position
Timer* timers_get_selected(void);//returns selected timer
bool timers_isSpace();//is there space for new timer?
int8_t timers_get_selectedIndex(void);//returns position of selected timer
uint8_t timers_count();//returns count of all timers
uint8_t timers_timer_count();//returns count of timers
uint8_t timers_stopwatch_count();//returns count of stopwatches
void timers_remove_selected();//deletes selected timer
void timers_deinit();

void timers_selected_start();//action start
void timers_selected_stop();//action start
void timers_selected_lap();//action new lap (only for stopwatch)
void timers_selected_reset();//action reset