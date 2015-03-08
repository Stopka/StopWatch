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

void window_timer_set_show() {
	window_stack_push(window, true);
}

void window_timer_set_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window= window_create();
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

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 3;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0://Durations
      return 1;
		
    case 1://Settings
      return 3;
		
		case 2://Confirm
      return 1;

    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return section_index==2?MENU_CELL_BASIC_HEADER_HEIGHT/3:MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, ("Duration"));
      break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer,("Settings"));
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0://Durations
			switch(cell_index->row){
				case 0: 
					menu_cell_basic_draw(ctx,cell_layer,("000 00:00:00"),NULL,bitmaps_get_bitmap(RESOURCE_ID_ACTION_EDIT));
					break;
				case 1:
					menu_cell_basic_draw(ctx,cell_layer,("Add duration"),NULL,bitmaps_get_bitmap(RESOURCE_ID_ACTION_PLUS));
					break;
			}
			break;
		
    case 1://Settings
			switch(cell_index->row){
				case 0: 
					menu_cell_basic_draw(ctx,cell_layer,("Repeat"),("Off"),bitmaps_get_bitmap(RESOURCE_ID_ACTION_LAP));
					break;
				case 1:
					menu_cell_basic_draw(ctx,cell_layer,("Vibration"),("Long"),bitmaps_get_bitmap(RESOURCE_ID_ACTION_VIBRATION));
					break;
				case 2:
					menu_cell_basic_draw(ctx,cell_layer,("Dismiss"),("Manual"),bitmaps_get_bitmap(RESOURCE_ID_ACTION_RESET));
					break;
			}
      break;
		
		case 2://Confirm
			menu_cell_basic_draw(ctx,cell_layer,("Save"),NULL,bitmaps_get_bitmap(RESOURCE_ID_ACTION_CONFIRM));
      break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	

}

static void window_load(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
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
	menu_layer_set_click_config_onto_window(menu_layer, window);
	
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
}
