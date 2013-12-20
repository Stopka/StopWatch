#include <pebble.h>
#include "stopwatch_window.h"

Window* window;	

void handle_init() {
	window = create_stopwatch_window();
	window_stack_push(window, true);
}

void handle_deinit() {
	  window_stack_pop(true);
	  window_destroy(window);
}

int main() {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
