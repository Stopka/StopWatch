#pragma once
#include <pebble.h>
#include "clock.h"

void scheduler_init(AppTimerCallback,void*);
void scheduler_update(Clock*);
void scheduler_deinit();