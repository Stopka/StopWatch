#ifndef STOPWATCH_MODEL_H
#define STOPWATCH_MODEL_H

#include "utils.h"

#define STOPWATCH_MAX_LAPS 30

void stopwatch_model_init();
void stopwatch_model_deinit();

void stopwatch_model_reset();
void stopwatch_model_newlap();
void stopwatch_model_start();
void stopwatch_model_stop();

bool stopwatch_model_isRunning();

int stopwatch_model_getLapsCount();
int stopwatch_model_getTotalLapsCount();
Time* stopwatch_model_getLapTime(int lap);
Time* stopwatch_model_getLapTotalTime(int lap);

#endif