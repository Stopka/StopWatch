#include <pebble.h>
#include "stopwatch.h"

Window* window;	

void handle_init(void) {
	window = create_stopwatch_window();
	window_stack_push(window, true);
}

void handle_deinit(void) {
	  window_stack_pop(window, true);
	  window_destroy(window);
}

int main(void) {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
