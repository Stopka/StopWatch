#include <pebble.h>
#include "window_alarm.h"
#include "bitmap-loader.h"
#include "localize.h"

static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);
	
static Window* window;
TextLayer *text_layer;
BitmapLayer *bitmap_layer;
ActionBarLayer* action_bar_layer;

void window_alarm_show() {
	window_stack_push(window, false);
}

void window_alarm_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window= window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_alarm_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_destroy(window);
}

static void window_alarm_handle_click_up(ClickRecognizerRef recognizer, void *context){
		window_stack_remove(window, false);
}

static void window_alarm_handle_click_back(ClickRecognizerRef recognizer, void *context){
		return;
}

static void window_alarm_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) window_alarm_handle_click_up);
	window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) window_alarm_handle_click_back);
}

static void window_load(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	window_set_background_color(window, GColorBlack);
	window_set_status_bar_icon(window,bitmaps_get_bitmap(RESOURCE_ID_ICON16));
	
	bitmap_layer=bitmap_layer_create(GRect(0, 0, bounds.size.w-ACTION_BAR_WIDTH, bounds.size.h));
	bitmap_layer_set_alignment(bitmap_layer,GAlignCenter);
	bitmap_layer_set_background_color(bitmap_layer, GColorClear);
	bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpSet);
	bitmap_layer_set_bitmap(bitmap_layer,bitmaps_get_bitmap(RESOURCE_ID_TIMER));
	layer_add_child(window_layer,bitmap_layer_get_layer(bitmap_layer));
	
	text_layer=text_layer_create(GRect(0, (bounds.size.h-18)/4, bounds.size.w-ACTION_BAR_WIDTH, 18));
	text_layer_set_text(text_layer,("Timer finished"));
	text_layer_set_text_color(text_layer, GColorWhite);
	text_layer_set_background_color(text_layer,GColorClear);
	text_layer_set_text_alignment(text_layer,GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer,text_layer_get_layer(text_layer));
	
	action_bar_layer=action_bar_layer_create();
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, bitmaps_get_bitmap(RESOURCE_ID_ACTION_RESET));
	action_bar_layer_set_background_color(action_bar_layer,GColorWhite);
	action_bar_layer_set_click_config_provider(action_bar_layer,window_alarm_click_config_provider);
	action_bar_layer_add_to_window(action_bar_layer, window);
}

static void window_alarm_vibe(struct tm *tick_time, TimeUnits units_changed) {
	vibes_long_pulse();
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	light_enable_interaction();
	tick_timer_service_subscribe(SECOND_UNIT, window_alarm_vibe);
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
	tick_timer_service_unsubscribe();
}


static void window_unload(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	text_layer_destroy(text_layer);
	bitmap_layer_destroy(bitmap_layer);
	action_bar_layer_destroy(action_bar_layer);
}
