#include "timer.h"
#include "window_main.h"
void handle_init() {
	timers_init();
	window_main_init();
	window_main_show();
}

void handle_deinit() {
	window_main_deinit();
	timers_deinit();
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
