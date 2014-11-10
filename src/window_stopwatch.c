#include <pebble.h>
#include "window_stopwatch.h"
#include "bitmap-loader.h"
#include "timers.h"
	
#define BITMAP_GROUP_BUTTON_UP 1

#define TEXT_LAYERS_COUNT 2+LAPS_MAX_COUNT
#define TEXT_LAYER_TIME 0 //1 item
#define TEXT_LAYER_LAP 1 //laps_count+1 items
	
static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);
	
static Window*    window_stopwatch;
static TextLayer* text_layers[TEXT_LAYERS_COUNT];
ActionBarLayer* action_bar;
InverterLayer* line;
InverterLayer* laps_mark;
BitmapLayer* state;
ScrollLayer* scroll_layer;
Layer* laps_layer;
uint8_t lap_count=0;
uint8_t selected_lap=0;

void window_stopwatch_show() {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_show()");
	window_stack_push(window_stopwatch, true);
}

void window_stopwatch_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window_stopwatch = window_create();
	window_set_window_handlers(window_stopwatch, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_stopwatch_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_destroy(window_stopwatch);
}

void update_lap(Timer* timer,uint8_t i){
	timer_setLapTime(timer,(char*)text_layer_get_text(text_layers[TEXT_LAYER_LAP+i]),i,false);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_LAP+i]);
}

void update_time(){
	Timer* timer=timers_get_selected();
	timer_setStopwatchTotalTime(timer,(char*)text_layer_get_text(text_layers[TEXT_LAYER_TIME]),false);
	layer_mark_dirty((Layer *)text_layers[TEXT_LAYER_TIME]);
	update_lap(timer,0);
}

void update_selected(){
	update_time();
	GRect to_frame = GRect(0,21*selected_lap,33,22);
	layer_set_frame(inverter_layer_get_layer(laps_mark),to_frame);
	scroll_layer_set_content_offset(scroll_layer,GPoint(0,-21*(selected_lap-1)),true);
}

void update_laps(){
	Layer *window_layer = window_get_root_layer(window_stopwatch);
	GRect bounds = layer_get_frame(window_layer);	
	const int16_t width = bounds.size.w - ACTION_BAR_WIDTH - 3;
	Timer* timer=timers_get_selected();
	int8_t actual_count=laps_count(timer->laps);
	for(int8_t i=lap_count-1;i>=actual_count;i--){//remove
		layer_remove_from_parent((Layer *) text_layers[TEXT_LAYER_LAP+i]);	
		text_layer_destroy(text_layers[TEXT_LAYER_LAP+i]);
		text_layers[TEXT_LAYER_LAP+i]=NULL;
		lap_count--;
	}
	for(uint8_t i=lap_count;i<actual_count;i++){//add
		char* string = (char *)malloc(17*sizeof(char));
		text_layers[TEXT_LAYER_LAP+i] = text_layer_create(GRect(3,-5+(i*21), width, 24));
		text_layer_set_text(text_layers[TEXT_LAYER_LAP+i], string);
		text_layer_set_font(text_layers[TEXT_LAYER_LAP+i],fonts_get_system_font(FONT_KEY_GOTHIC_24));
		text_layer_set_text_alignment(text_layers[TEXT_LAYER_LAP+i],GTextAlignmentLeft);
		text_layer_set_text_color(text_layers[TEXT_LAYER_LAP+i],GColorWhite);
		text_layer_set_background_color(text_layers[TEXT_LAYER_LAP+i],GColorClear);
		layer_add_child(laps_layer, text_layer_get_layer(text_layers[TEXT_LAYER_LAP+i]));
		lap_count++;
	}
	scroll_layer_set_content_size(scroll_layer,GSize(width,21*(lap_count)));
	for(uint8_t i=0;i<lap_count;i++){//update text
		update_lap(timer,i);
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
		case TIMER_STATUS_RUNNING://new lap
			timer_lap(timer);
			update_laps();
			break;
		case TIMER_STATUS_PAUSED://reset
			timer_reset(timer);
			update_state();
			update_laps();
			selected_lap=0;
			update_selected();
			break;
		default://delete
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
	selected_lap=(selected_lap+1)%lap_count;
	update_selected();
}

static void handle_down_long_click(ClickRecognizerRef recognizer, void *context){
	selected_lap=((selected_lap==0)?(lap_count-1):(selected_lap-1));
	update_selected();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) handle_click_up);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) handle_click_select);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) handle_click_down);
	
  window_long_click_subscribe(BUTTON_ID_DOWN, 0,(ClickHandler) handle_down_long_click,NULL);
}

static void window_load(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	window_set_status_bar_icon(window,bitmaps_get_bitmap(RESOURCE_ID_STOPWATCH16));
	window_set_background_color	(window,GColorBlack);
	
	const int16_t width = bounds.size.w - ACTION_BAR_WIDTH - 3;
	const int16_t time_width = width-14;
	
	state=bitmap_layer_create(GRect(0, 4, 14, 28));
	bitmap_layer_set_alignment(state,GAlignBottom);
	bitmap_layer_set_compositing_mode(state,GCompOpSet);
	layer_add_child(window_layer,bitmap_layer_get_layer(state));
	
	text_layers[TEXT_LAYER_TIME] = text_layer_create(GRect(14, 5, time_width, 28));
  text_layer_set_text(text_layers[TEXT_LAYER_TIME], "00:00:00.00");
	text_layer_set_font(text_layers[TEXT_LAYER_TIME],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layers[TEXT_LAYER_TIME],GTextAlignmentCenter);
	text_layer_set_text_color(text_layers[TEXT_LAYER_TIME],GColorWhite);
	text_layer_set_background_color(text_layers[TEXT_LAYER_TIME],GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layers[TEXT_LAYER_TIME]));
	
	scroll_layer=scroll_layer_create(GRect(0, 47, width, bounds.size.h-47));
	layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
	
	laps_layer=layer_create(GRect(0,0,width,44*LAPS_MAX_COUNT));
	scroll_layer_add_child(scroll_layer, laps_layer);
	
	laps_mark=inverter_layer_create(GRect(0,0,33,22));
	scroll_layer_add_child(scroll_layer,(Layer*)laps_mark);
	
	line=inverter_layer_create(GRect(0, 45, width, 2));
	layer_add_child(window_layer,inverter_layer_get_layer(line));
	
	action_bar=action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_background_color(action_bar,GColorWhite);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmaps_get_bitmap(RESOURCE_ID_ACTION_DOWN));
	action_bar_layer_set_click_config_provider(action_bar,click_config_provider);
	
	update_laps();
	update_state();
	update_selected();
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	Timer* timer=timers_get_selected();
	if(timer_getStatus(timer)==TIMER_STATUS_RUNNING){
		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	}
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
	tick_timer_service_unsubscribe();
}

static void window_unload(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	for(uint8_t i=0;i<TEXT_LAYERS_COUNT;i++){
		if(text_layers[i]!=NULL){
			text_layer_destroy(text_layers[i]);
		}
	}
	lap_count=0;
	inverter_layer_destroy(line);
	inverter_layer_destroy(laps_mark);
	bitmap_layer_destroy(state);
	action_bar_layer_destroy(action_bar);
	scroll_layer_destroy(scroll_layer);
	layer_destroy(laps_layer);
}