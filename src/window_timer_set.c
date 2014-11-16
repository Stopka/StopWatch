#include <pebble.h>
#include "window_timer_set.h"

Window* window;

static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);

void window_timer_set_show() {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_show()");
	window_stack_push(window, true);
}

void window_timer_set_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_timer_set_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_destroy(window);
}

static void window_load(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	
	
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	
	
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
	
	
}


static void window_unload(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	
	
}