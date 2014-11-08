#pragma once
#include "timer.h"

void timers_init();
uint8_t timers_add(Timer* timer);
Timer* timers_get(int pos);
uint8_t timers_count();
Timer* timers_remove(int pos);
void timers_deinit();