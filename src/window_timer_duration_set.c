#include <pebble.h>
#include "window_timer_duration_set.h"

#include <pebble.h>
#include "timers.h"
#include "bitmap-loader.h"
#include "window_timer_set.h"
#include "localize.h"
	
static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);
	
static Window*    window;
static MenuLayer*    menu_layer;
static ActionBarLayer* action_bar_layer;

uint16_t values[4];
uint16_t limits[4]={1000,24,60,60};
char* strings[4];

void window_timer_duration_set_show() {
	window_stack_push(window, true);
}

void window_timer_duration_set_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window= window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_timer_duration_set_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_destroy(window);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0://Durations
      return 4;
		
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, ("Set duration"));
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0://Durations
			switch(cell_index->row){
				case 0: 
					snprintf(strings[0], 4, "%03u", values[0]);
					menu_cell_basic_draw(ctx,cell_layer,("Days"),strings[0],NULL);
					break;
				case 1: 
					snprintf(strings[1], 3, "%02u", values[1]);
					menu_cell_basic_draw(ctx,cell_layer,("Hours"),strings[1],NULL);
					break;
				case 2: 
					snprintf(strings[2], 3, "%02u", values[2]);
					menu_cell_basic_draw(ctx,cell_layer,("Minutes"),strings[2],NULL);
					break;
				case 3: 
					snprintf(strings[3], 3, "%02u", values[3]);
					menu_cell_basic_draw(ctx,cell_layer,("Seconds"),strings[3],NULL);
					break;
			}
			break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	//never called, config provider
}

static void handle_click_up(ClickRecognizerRef recognizer, void *context){//plus
	MenuIndex index = menu_layer_get_selected_index(menu_layer);
	values[index.row]=(values[index.row]+1)%limits[index.row];
	menu_layer_reload_data(menu_layer);
}

static void handle_click_select(ClickRecognizerRef recognizer, void *context){//next
	MenuIndex index = menu_layer_get_selected_index(menu_layer);
	if(index.row!=3){
		index.row++;
	}else{
		//TODO zavřít, hotovo
	}
	menu_layer_set_selected_index(menu_layer,index,MenuRowAlignCenter,true);
}

static void handle_click_down(ClickRecognizerRef recognizer, void *context){//minus
	MenuIndex index = menu_layer_get_selected_index(menu_layer);
	values[index.row]=values[index.row]==0?(limits[index.row]-1):(values[index.row]-1);
	menu_layer_reload_data(menu_layer);
}

static void handle_click_long_select(ClickRecognizerRef recognizer, void *context){//previous
	MenuIndex index = menu_layer_get_selected_index(menu_layer);
	if(index.row!=0){
		index.row--;
	}
	menu_layer_set_selected_index(menu_layer,index,MenuRowAlignCenter,true);
}



void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, (ClickHandler) handle_click_up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, (ClickHandler) handle_click_down);
	
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) handle_click_select);
	window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) handle_click_long_select, NULL);
}

static void window_load(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	for(int i=0;i<4;i++){
		strings[i]=(char *)malloc((i==3?4:3)*sizeof(char));
	}
	menu_layer=menu_layer_create(bounds);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
	//menu_layer_set_click_config_onto_window(menu_layer, window);
	
	action_bar_layer=action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar_layer, window);
  action_bar_layer_set_click_config_provider(action_bar_layer,click_config_provider);
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, bitmaps_get_bitmap(RESOURCE_ID_ACTION_PLUS));
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ACTION_CONFIRM));
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, bitmaps_get_bitmap(RESOURCE_ID_ACTION_PLUS));
	
	window_set_status_bar_icon(window,bitmaps_get_bitmap(RESOURCE_ID_TIMER16));	
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	menu_layer_reload_data(menu_layer);
	menu_layer_set_selected_index(menu_layer,(MenuIndex){
		.section=0,
		.row=0
	},MenuRowAlignCenter,false);
	layer_mark_dirty((Layer *)menu_layer);
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
}


static void window_unload(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	menu_layer_destroy(menu_layer);
	action_bar_layer_destroy(action_bar_layer);
	for(int i=0;i<4;i++){
		free(strings[i]);
	}
}
