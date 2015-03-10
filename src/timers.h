#pragma once
#include "timer.h"

#define TIMERS_MAX_COUNT 10

void timers_init();
Timer* timers_add_stopwatch(void);
Timer* timers_add_timer(void);
Timer* timers_select(uint8_t pos);
Timer* timers_timer_select(uint8_t pos);
Timer* timers_stopwatch_select(uint8_t pos);
Timer* timers_get(uint8_t pos);
Timer* timers_get_selected(void);
bool timers_isSpace();
int8_t timers_get_selectedIndex(void);
uint8_t timers_count();
uint8_t timers_timer_count();
uint8_t timers_stopwatch_count();
void timers_remove_selected();
void timers_deinit();