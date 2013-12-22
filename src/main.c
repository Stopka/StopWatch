#include <pebble.h>
#include "stopwatch.h"

void init() {
	stopwatch_init();
}

void deinit() {
	stopwatch_deinit();  
}

int main() {
	  init();
	  app_event_loop();
	  deinit();
}
