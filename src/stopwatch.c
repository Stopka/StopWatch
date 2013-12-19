#include "stopwatch.h"

static Window* stopwatch_window;
static Layer *time_display;
static TextLayer* time_display_values[3];
static TextLayer* time_display_labels[3];

static void window_load(Window *window) {
	time_display=layer_create(GRect(0, 0, 144, 30));
	layer_add_child(window_get_root_layer(window), time_display);
	
	time_display_values[0]=text_layer_create(GRect(0, 0, 20, 30));
	time_display_values[1]=text_layer_create(GRect(22, 0, 20, 30));
	time_display_values[2]=text_layer_create(GRect(44, 0, 20, 30));
	GFont font = fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY));
	GColor b_color = GColorClear;
	GColor f_color = GColorWhite;
	for(int i=0;i<3;i++){
		text_layer_set_font	(time_display_values[i],font);
		text_layer_set_background_color	(time_display_values[i],b_color);
		text_layer_set_text_color(time_display_values[i],f_color);
		text_layer_set_text	(time_display_values[i],"00")
		layer_add_child(time_display, time_display_values[i]);
	}
}

static void window_appear(Window *window) {
	
}

static void window_disappear(Window *window) {
	
}

static void window_unload(Window *window) {
	layer_destroy(time_display);
	for(int i=0;i<3;i++){
		text_layer_destroy	(time_display_values[i]);	
	}
}

Window* create_stopwatch_window(){
	stopwatch_window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
	return stopwatch_window;
}