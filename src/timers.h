#pragma once
#include "timer.h"

void timers_init();
Timer* timers_add_stopwatch(void);
Timer* timers_select(uint8_t pos);
Timer* timers_get(uint8_t pos);
Timer* timers_get_selected(void);
int8_t timers_get_selectedIndex(void);
uint8_t timers_count();
void timers_remove_selected();
void timers_deinit();