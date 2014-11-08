#include <pebble.h>
#include "window_main.h"
#define BITMAPS_COUNT 3
#define BITMAP_ICON 0
#define BITMAP_STOPWATCH_ADD 1
#define BITMAP_TIMER_ADD 2
	
static void window_load(Window* window);
static void window_unload(Window* window);
	
static Window*    window_main;
static MenuLayer*    window_main_menu_layer;
static GBitmap*    window_main_bitmaps[BITMAPS_COUNT];

void window_main_show() {
	window_stack_push(window_main, true);
}

void window_main_init(){
	window_main = window_create();
	window_set_window_handlers(window_main, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
}

void window_main_deinit(){
	window_destroy(window_main);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 2;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return timers_count();

    case 1:
      return 2;

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
      menu_cell_basic_header_draw(ctx, cell_layer, "Timers & Stopwatches");
      break;

    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer,"Controls");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0:
      break;

    case 1:
      switch (cell_index->row) {
        case 0:
					menu_cell_basic_draw(ctx,cell_layer,"Add stopwatch",NULL,window_main_bitmaps[BITMAP_STOPWATCH_ADD]);
          break;
				case 1:
					menu_cell_basic_draw(ctx,cell_layer,"Add timer",NULL,window_main_bitmaps[BITMAP_TIMER_ADD]);
          break;
				case 2:
          menu_cell_title_draw(ctx, cell_layer, "All...");
          break;
      }
  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					//Add stopwatch
			  		break;
				case 1:
					//Add timer
			  		break;
				case 2:
					//All...
			  		break;
			break;
			}
	}

}

static void window_load(Window* window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	window_main_menu_layer=menu_layer_create(bounds);
	menu_layer_set_callbacks(window_main_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	
	menu_layer_set_click_config_onto_window(window_main_menu_layer, window);
	
	window_main_bitmaps[BITMAP_ICON]=gbitmap_create_with_resource(RESOURCE_ID_ICON16);
	window_main_bitmaps[BITMAP_STOPWATCH_ADD]=gbitmap_create_with_resource(RESOURCE_ID_STOPWATCH_ADD);
	window_main_bitmaps[BITMAP_TIMER_ADD]=gbitmap_create_with_resource(RESOURCE_ID_TIMER_ADD);
	
	window_set_status_bar_icon(window,window_main_bitmaps[BITMAP_ICON]);	
	
	layer_add_child(window_layer, menu_layer_get_layer(window_main_menu_layer));
}

static void window_unload(Window* window){
	menu_layer_destroy(window_main_menu_layer);
	for(uint8_t i=0;i<BITMAPS_COUNT;i++){
		gbitmap_destroy(window_main_bitmaps[i]);	
	}
}