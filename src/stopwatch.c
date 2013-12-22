#include "stopwatch.h"
#include "stopwatch_model.h"
#include "stopwatch_window.h"
Window* window;

void stopwatch_init(){
	stopwatch_model_init(); 
	stopwatch_window_init();
}

void stopwatch_deinit(){
	stopwatch_window_deinit();
	stopwatch_model_deinit();
}