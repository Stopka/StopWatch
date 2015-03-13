#pragma once
#include <pebble.h>
#include "clock.h"

void scheduler_init(AppTimerCallback,void*);//scheduler constructor, sets callback
void scheduler_update(Clock*);//schedules alarm for time
void scheduler_deinit();//destructor