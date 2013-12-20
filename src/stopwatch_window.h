#ifndef STOPWATCH_WINDOW_H
#define STOPWATCH_WINDOW_H

#include <pebble.h>

Window* create_stopwatch_window();
static void stopwatch_window_setRunning(bool running);
static void stopwatch_window_setMeasure(int offset);
static void stopwatch_window_setLapMeasure(int offset);
#endif