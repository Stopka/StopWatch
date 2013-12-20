#include "stopwatch.h"
#include <string.h>
	
#define FONT_TIME_DISPLAY RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_25
#define FONT_TIME_DISPLAY_LABEL RESOURCE_ID_FONT_DEJAVU_SANS_12
#define FONT_LAPS_DISPLAY_LAP RESOURCE_ID_FONT_DEJAVU_SANS_19
	
#define ICON_RESET RESOURCE_ID_ICON_RESET
#define ICON_LAP RESOURCE_ID_ICON_LAP
#define ICON_START RESOURCE_ID_ICON_STOPWATCH_START
#define ICON_STOP RESOURCE_ID_ICON_STOPWATCH_STOP
#define ICON_NEXT RESOURCE_ID_ICON_DOWN
//window
static Window* stopwatch_window;
//Time display
static Layer *time_display;
static TextLayer* time_display_values;
static TextLayer* time_display_labels[7];
char *measure_labels[5] = {"d","h","m","s","ms"};
//Laps
static ScrollLayer* laps_display;
static TextLayer* laps_display_laps[30];
//Action bar
static ActionBarLayer* action_bar;	

//STATE
static int measure_offset=2;
static int measure_offset_laps=2;
static bool running=false;

static void window_load(Window *window) {
	///////////////////////////////////////////////////////////////////
	// Time display
	///////////////////////////////////////////////////////////////////
	time_display=layer_create(GRect(0, 0, 144, 46));
	layer_add_child(window_get_root_layer(window), (Layer*)time_display);
	
	// Values ////////////////////////////////////////////////////////////
	time_display_values=text_layer_create(GRect(0, 6, 122, 40));
	GFont font = fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY));
	text_layer_set_font(time_display_values,font);
	text_layer_set_background_color	(time_display_values,GColorClear);
	text_layer_set_text_color(time_display_values,GColorWhite);
	text_layer_set_text_alignment(time_display_values, GTextAlignmentCenter);
	text_layer_set_overflow_mode(time_display_values,GTextOverflowModeFill); 
	text_layer_set_text	(time_display_values,"00:00.00");
	layer_add_child((Layer *)time_display, (Layer *)time_display_values);
	
	// Labels ////////////////////////////////////////////////////////////
	time_display_labels[0]=text_layer_create(GRect(0, 0, 40, 14));
	time_display_labels[1]=text_layer_create(GRect(40, 0, 41, 14));
	time_display_labels[2]=text_layer_create(GRect(81, 0, 41, 14));
	
	time_display_labels[6]=text_layer_create(GRect(0, 32, 32, 14));
	time_display_labels[3]=text_layer_create(GRect(32, 32, 30, 14));
	time_display_labels[4]=text_layer_create(GRect(62, 32, 30, 14));
	time_display_labels[5]=text_layer_create(GRect(92, 32, 30, 14));
	font = fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY_LABEL));
	for(int i=0;i<6;i++){
		text_layer_set_font(time_display_labels[i],font);
		text_layer_set_background_color	(time_display_labels[i],(i<3?GColorClear:GColorWhite));
		text_layer_set_text_color(time_display_labels[i],(i<3?GColorWhite:GColorBlack));
		text_layer_set_text_alignment(time_display_labels[i], GTextAlignmentCenter);
		text_layer_set_overflow_mode(time_display_labels[i],GTextOverflowModeFill);
		text_layer_set_text	(time_display_labels[i],measure_labels[(i<3?measure_offset:measure_offset_laps)+(i%3)]);
		layer_add_child((Layer *)time_display, (Layer *)time_display_labels[i]);
	}
	text_layer_set_font(time_display_labels[6],font);
	text_layer_set_background_color	(time_display_labels[6],GColorWhite);
	text_layer_set_text_color(time_display_labels[6],GColorBlack);
	text_layer_set_text_alignment(time_display_labels[6], GTextAlignmentCenter);
	text_layer_set_overflow_mode(time_display_labels[6],GTextOverflowModeFill);
	text_layer_set_text	(time_display_labels[6],"#");
	layer_add_child((Layer *)time_display, (Layer *)time_display_labels[6]);
	
	///////////////////////////////////////////////////////////////////
	// Laps display
	///////////////////////////////////////////////////////////////////
	laps_display=scroll_layer_create(GRect(0,46,121,105));
	layer_add_child(window_get_root_layer(window), (Layer*)laps_display);
	
	// Laps ////////////////////////////////////////////////////////////
	font=fonts_load_custom_font(resource_get_handle(FONT_LAPS_DISPLAY_LAP));
	for(int i=0;i<7;i++){
		char* string="00 00:00.00";
		//string[1]='0'+i;
		laps_display_laps[i]=text_layer_create(GRect(0, 20*i, 121, 20));
		text_layer_set_font(laps_display_laps[i],font);
		text_layer_set_background_color	(laps_display_laps[i],GColorClear);
		text_layer_set_text_color(laps_display_laps[i],GColorWhite);
		text_layer_set_text_alignment(laps_display_laps[i], GTextAlignmentCenter);
		text_layer_set_text	(laps_display_laps[i],string);
		scroll_layer_add_child(laps_display, (Layer *)laps_display_laps[i]);
	}
	scroll_layer_set_content_size(laps_display,GSize(121,20*7)); 
	
	///////////////////////////////////////////////////////////////////
	// Action bar
	///////////////////////////////////////////////////////////////////
	action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_background_color(action_bar, GColorWhite);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, gbitmap_create_with_resource(ICON_RESET));
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, gbitmap_create_with_resource(ICON_START));
  	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, gbitmap_create_with_resource(ICON_NEXT));
}

static void window_appear(Window *window) {
	
}

static void window_disappear(Window *window) {
	
}

static void window_unload(Window *window) {
	//Time display
	layer_destroy(time_display);
	text_layer_destroy	(time_display_values);
	for(int i=0;i<7;i++){
		text_layer_destroy	(time_display_labels[i]);		
	}
	//Laps display
	scroll_layer_destroy(laps_display);
	for(int i=0;i<7;i++){
		text_layer_destroy(laps_display_laps[i]);
	}
	//Action bar
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