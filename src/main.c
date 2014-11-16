#include "timer.h"
#include "bitmap-loader.h"
#include "timers.h"
#include "window_main.h"
#include "window_stopwatch.h"
#include "window_timer_set.h"

void handle_init() {
	timers_init();
	
	bitmaps_init();
	
	window_main_init();
	window_stopwatch_init();
	window_timer_set_init();
	
	window_main_show();
}

void handle_deinit() {
	window_main_deinit();
	window_stopwatch_deinit();
	window_timer_set_deinit();
	
	bitmaps_cleanup();
	
	timers_deinit();
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
