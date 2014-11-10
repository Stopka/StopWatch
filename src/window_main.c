#include <pebble.h>
#include "window_main.h"
#include "timers.h"
#include "bitmap-loader.h"
#include "window_stopwatch.h"
	
static void window_load(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);
static void window_unload(Window* window);
	
static Window*    window_main;
static MenuLayer*    window_main_menu_layer;

void window_main_show() {
	window_stack_push(window_main, true);
}

void window_main_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	window_main = window_create();
	window_set_window_handlers(window_main, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
}

void window_main_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_destroy(window_main);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	menu_layer_reload_data(window_main_menu_layer);
	layer_mark_dirty((Layer *)window_main_menu_layer);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 2;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return timers_count();

    case 1:
      return timers_isSpace()?1:0;

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
      menu_cell_basic_header_draw(ctx, cell_layer, "Stopwatches");
      break;

    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer,"Controls");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	Timer* t=timers_get((uint8_t)cell_index->row);
	uint32_t res_id;
  switch (cell_index->section) {
    case 0:
			switch(timer_getDirection(t)){
				case TIMER_DIRECTION_UP:
					switch(timer_getStatus(t)){
						case TIMER_STATUS_STOPPED: res_id=RESOURCE_ID_STOPWATCH_STOPPED; break;
  					case TIMER_STATUS_RUNNING: res_id=RESOURCE_ID_STOPWATCH_RUNNING; break;
  					case TIMER_STATUS_PAUSED: res_id=RESOURCE_ID_STOPWATCH_PAUSED; break;
						default: return;
					}
					char* title=(char *)malloc(12*sizeof(char));//"00 00:00:00";
					char* subtitle=(char *)malloc(17*sizeof(char));//"000 00:00:00.00";
					timer_setStopwatchTotalTime(t,title,true);
					timer_setLapTime(t,subtitle,0,true);
					menu_cell_basic_draw(ctx,cell_layer,title,subtitle,bitmaps_get_bitmap(res_id));
					free(title);
					free(subtitle);
					break;
				case TIMER_DIRECTION_DOWN:
					//TODO
					break;
			}
      break;

    case 1:
      switch (cell_index->row) {
        case 0:
					menu_cell_basic_draw(ctx,cell_layer,"Add",NULL,bitmaps_get_bitmap(RESOURCE_ID_STOPWATCH_ADD));
          break;
				case 1:
					menu_cell_basic_draw(ctx,cell_layer,"Timer",NULL,bitmaps_get_bitmap(RESOURCE_ID_TIMER_ADD));
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
			timers_select(cell_index->row);
			window_stopwatch_show();
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					//Add stopwatch
						timers_add_stopwatch();
						window_stopwatch_show();
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
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
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
	layer_add_child(window_layer, menu_layer_get_layer(window_main_menu_layer));
	menu_layer_set_click_config_onto_window(window_main_menu_layer, window);
	
	window_set_status_bar_icon(window,bitmaps_get_bitmap(RESOURCE_ID_ICON16));	
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	menu_layer_reload_data(window_main_menu_layer);
	int8_t index=timers_get_selectedIndex();
	menu_layer_set_selected_index(window_main_menu_layer,(MenuIndex){
		.section=timers_count()>0?0:1,
		.row=index<0?0:index
	},MenuRowAlignCenter,false);
	layer_mark_dirty((Layer *)window_main_menu_layer);
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
	tick_timer_service_unsubscribe();
}


static void window_unload(Window* window){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	menu_layer_destroy(window_main_menu_layer);
}