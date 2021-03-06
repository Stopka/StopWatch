#include "bitmap-loader.h"
#include "timers.h"
#include "window_main.h"
#include "window_stopwatch.h"
//#include "window_timer_set.h"
#include "window_timer_duration_set.h"
#include "window_alarm.h"
#include "localize.h"

void handle_init() {
	
	bitmaps_init();
	locale_init();
	
	window_alarm_init();
	window_main_init();
	window_stopwatch_init();
	//window_timer_set_init();
	window_timer_duration_set_init();
	
	timers_init();
	
	window_main_show();
}

void handle_deinit() {
	window_alarm_deinit();
	window_main_deinit();
	window_stopwatch_deinit();
//	window_timer_set_deinit();
	window_timer_duration_set_deinit();
	
	timers_deinit();
	
	bitmaps_cleanup();
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
