#include <pebble.h>
#include "window_stopwatch.h"
#include "bitmap-loader.h"
#include "timers.h"
	
#define BITMAP_GROUP_BUTTON_UP 1

#define TEXT_LAYERS_COUNT 5
#define TEXT_LAYER_TIME 0
#define TEXT_LAYER_DESC_TIME 1 //4 items
//#define TEXT_LAYER_DESC_LAP 2 //4 items
	
static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);
	
static Window*    window_stopwatch;
static TextLayer* text_layers[TEXT_LAYERS_COUNT];
ActionBarLayer* action_bar;
InverterLayer* line;
BitmapLayer* state;
uint8_t measure_offsets[2]={5,5};
char* measure_labels[6] = {" ","d","h","m","s","cs"};

void window_stopwatch_show() {
	window_stack_push(window_stopwatch, true);
}

void window_stopwatch_init(){
	window_stopwatch = window_create();
	window_set_window_handlers(window_stopwatch, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_stopwatch_deinit(){
	window_destroy(window_stopwatch);
}

void update_time(){
	Timer* timer=timers_get_selected();
	uint8_t offset=timer_setStopwatchTotalTime(timer,(char*)text_layer_get_text(text_layers[TEXT_LAYER_TIME]),false);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_TIME]);
	if(offset!=measure_offsets[0]){
		for(uint8_t i=0;i<4;i++){
			text_layer_set_text(text_layers[TEXT_LAYER_DESC_TIME+i], measure_labels[offset+i]);
			layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_DESC_TIME+i]);
		}
		measure_offsets[0]=offset;
	}
}

void update_state(){
	Timer* timer=timers_get_selected();
	switch(timer_getStatus(timer)){
		case TIMER_STATUS_RUNNING:
			action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmaps_get_bitmap_in_group(RESOURCE_ID_ACTION_LAP,BITMAP_GROUP_BUTTON_UP));
			action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ACTION_PAUSE));
			bitmap_layer_set_bitmap(state,bitmaps_get_bitmap(RESOURCE_ID_ACTION_START));
			break;
		case TIMER_STATUS_PAUSED:
			action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmaps_get_bitmap_in_group(RESOURCE_ID_ACTION_RESET,BITMAP_GROUP_BUTTON_UP));
			action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ACTION_START));
			bitmap_layer_set_bitmap(state,bitmaps_get_bitmap(RESOURCE_ID_ACTION_PAUSE));
			break;
		default:
			action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmaps_get_bitmap_in_group(RESOURCE_ID_ACTION_DELETE,BITMAP_GROUP_BUTTON_UP));
			action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ACTION_START));
			bitmap_layer_set_bitmap(state,bitmaps_get_bitmap(RESOURCE_ID_ACTION_STOP));
	}
	layer_mark_dirty((Layer *)action_bar);
	layer_mark_dirty((Layer *)state);
	update_time();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

void handle_click_up(ClickRecognizerRef recognizer, void *context){
	Timer* timer=timers_get_selected();
	switch(timer_getStatus(timer)){
		case TIMER_STATUS_RUNNING:
			//timer_lap(timer);
			break;
		case TIMER_STATUS_PAUSED:
			timer_reset(timer);
			update_state();
			break;
		default:
			tick_timer_service_unsubscribe();
			timers_remove_selected();
			window_stack_pop(true);
	}
}

void handle_click_select(ClickRecognizerRef recognizer, void *context){
	Timer* timer=timers_get_selected();
	switch(timer_getStatus(timer)){
		case TIMER_STATUS_RUNNING:
			timer_stop(timer);
			tick_timer_service_unsubscribe();
			break;
		default:
			timer_start(timer);
			tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	}
	update_state();
}

void handle_click_down(ClickRecognizerRef recognizer, void *context){
	
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) handle_click_up);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) handle_click_select);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) handle_click_down);
}

static void window_load(Window* window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	window_set_status_bar_icon(window,bitmaps_get_bitmap(RESOURCE_ID_STOPWATCH16));
	window_set_background_color	(window,GColorBlack);
	
	const int16_t width = bounds.size.w - ACTION_BAR_WIDTH - 3;
	const int16_t time_width = width-14;
	
	for(uint8_t i=0;i<4;i++){
		text_layers[TEXT_LAYER_DESC_TIME+i] = text_layer_create(GRect(14+(i*time_width/4), 3, time_width/4, 14));
		text_layer_set_font(text_layers[TEXT_LAYER_DESC_TIME+i],fonts_get_system_font(FONT_KEY_GOTHIC_14));
		text_layer_set_text_alignment(text_layers[TEXT_LAYER_DESC_TIME+i],GTextAlignmentCenter);
		text_layer_set_text_color(text_layers[TEXT_LAYER_DESC_TIME+i],GColorWhite);
		text_layer_set_background_color(text_layers[TEXT_LAYER_DESC_TIME+i],GColorClear);
		layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DESC_TIME+i]));
	}
	
	state=bitmap_layer_create(GRect(0, 9, 14, 28));
	bitmap_layer_set_alignment(state,GAlignBottom);
	bitmap_layer_set_compositing_mode(state,GCompOpSet);
	layer_add_child(window_layer,bitmap_layer_get_layer(state));
	
	text_layers[TEXT_LAYER_TIME] = text_layer_create(GRect(14, 10, time_width, 28));
  text_layer_set_text(text_layers[TEXT_LAYER_TIME], "00:00:00.00");
	text_layer_set_font(text_layers[TEXT_LAYER_TIME],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_TIME],GTextAlignmentCenter);
	text_layer_set_text_color(text_layers[TEXT_LAYER_TIME],GColorWhite);
	text_layer_set_background_color(text_layers[TEXT_LAYER_TIME],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_TIME]));
	
	line=inverter_layer_create(GRect(0, 44, width, 2));
	layer_add_child(window_layer,inverter_layer_get_layer(line));
	/*
	text_layers[TEXT_LAYER_DESC_LAP] = text_layer_create(bounds);
  text_layer_set_text(text_layers[TEXT_LAYER_DESC_LAP], "h  m  s  ds");
	text_layer_set_font(text_layers[TEXT_LAYER_DESC_LAP],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_color(text_layers[TEXT_LAYER_DESC_LAP],GColorWhite);
	text_layer_set_background_color(text_layers[TEXT_LAYER_DESC_LAP],GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_DESC_LAP]));
	*/
	action_bar=action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_background_color(action_bar,GColorWhite);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmaps_get_bitmap(RESOURCE_ID_ACTION_DOWN));
	action_bar_layer_set_click_config_provider(action_bar,click_config_provider);
	
	update_state();
}

static void window_appear(Window *window) {
	Timer* timer=timers_get_selected();
	if(timer_getStatus(timer)==TIMER_STATUS_RUNNING){
		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	}
}

static void window_disappear(Window *window) {
	tick_timer_service_unsubscribe();
}

static void window_unload(Window* window){
	for(uint8_t i=0;i<TEXT_LAYERS_COUNT;i++){
		text_layer_destroy(text_layers[i]);
	}
	inverter_layer_destroy(line);
	bitmap_layer_destroy(state);
	action_bar_layer_destroy(action_bar);
}