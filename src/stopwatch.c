#include "stopwatch.h"
	
#define FONT_TIME_DISPLAY RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_25
#define FONT_TIME_DISPLAY_LABEL RESOURCE_ID_FONT_DEJAVU_SANS_12
//window
static Window* stopwatch_window;
//Time display
static Layer *time_display;
static TextLayer* time_display_values;
static TextLayer* time_display_labels[3];
static InverterLayer* time_display_line;
static int measure_offset=2;
char *measure_labels[5] = {"d","h","m","s","ms"};
//Laps
static ScrollLayer* laps_display;
static TextLayer* laps_display_laps[30];
//Action bar
static ActionBarLayer* action_bar;	



static void window_load(Window *window) {
	///////////////////////////////////////////////////////////////////
	// Time display
	///////////////////////////////////////////////////////////////////
	time_display=layer_create(GRect(0, 0, 144, 40));
	layer_add_child(window_get_root_layer(window), (Layer*)time_display);
	
	// Values ////////////////////////////////////////////////////////////
	time_display_values=text_layer_create(GRect(0, -3, 122, 40));
	/*time_display_values[1]=text_layer_create(GRect(50, 0, 50, 40));
	time_display_values[2]=text_layer_create(GRect(44, 0, 20, 30));*/
	GFont font = fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY));
	//for(int i=0;i<3;i++){
		text_layer_set_font(time_display_values,font);
		text_layer_set_background_color	(time_display_values,GColorClear);
		text_layer_set_text_color(time_display_values,GColorWhite);
		text_layer_set_text_alignment(time_display_values, GTextAlignmentCenter);
		text_layer_set_overflow_mode(time_display_values,GTextOverflowModeFill); 
		text_layer_set_text	(time_display_values,"00:00.00");
		layer_add_child((Layer *)time_display, (Layer *)time_display_values);
	//}
	
	// Labels ////////////////////////////////////////////////////////////
	time_display_labels[0]=text_layer_create(GRect(0, 27, 40, 20));
	time_display_labels[1]=text_layer_create(GRect(40, 27, 40, 20));
	time_display_labels[2]=text_layer_create(GRect(80, 27, 40, 20));
	font = fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY_LABEL));
	for(int i=0;i<3;i++){
		text_layer_set_font(time_display_labels[i],font);
		text_layer_set_background_color	(time_display_labels[i],GColorClear);
		text_layer_set_text_color(time_display_labels[i],GColorWhite);
		text_layer_set_text_alignment(time_display_labels[i], GTextAlignmentCenter);
		text_layer_set_overflow_mode(time_display_labels[i],GTextOverflowModeFill); 
		text_layer_set_text	(time_display_labels[i],measure_labels[measure_offset+i]);
		layer_add_child((Layer *)time_display, (Layer *)time_display_labels[i]);
	}
	
	// Line ////////////////////////////////////////////////////////////
	time_display_line=inverter_layer_create(GRect(0, 28, 121, 22));
	layer_add_child((Layer *)time_display, (Layer *)time_display_line);
	
	///////////////////////////////////////////////////////////////////
	// Laps display
	///////////////////////////////////////////////////////////////////
	laps_display=scroll_layer_create(GRect(0,50,121,146));
	layer_add_child(window_get_root_layer(window), (Layer*)time_display);
	
	// Laps ////////////////////////////////////////////////////////////
	
	
	///////////////////////////////////////////////////////////////////
	// Action bar
	///////////////////////////////////////////////////////////////////
	action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_background_color(action_bar, GColorWhite);
}

static void window_appear(Window *window) {
	
}

static void window_disappear(Window *window) {
	
}

static void window_unload(Window *window) {
	layer_destroy(time_display);
	text_layer_destroy	(time_display_values);
	for(int i=0;i<3;i++){
		text_layer_destroy	(time_display_labels[i]);		
	}
	inverter_layer_destroy(time_display_line);
	action_bar_layer_destroy(action_bar);
}

Window* create_stopwatch_window(){
	stopwatch_window = window_create();
	window_set_background_color	(stopwatch_window,GColorBlack);	
	window_set_window_handlers(stopwatch_window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
	return stopwatch_window;
}