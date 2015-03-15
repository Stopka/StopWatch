#include <pebble.h>
#include "window_main.h"
#include "timers.h"
#include "bitmap-loader.h"
#include "window_stopwatch.h"
#include "window_timer_set.h"
#include "window_timer_duration_set.h"
#include "localize.h"
	
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
  return 3;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0://Stopwatches
      return timers_stopwatch_count();
		
    case 1://Timers
      return timers_timer_count();

    case 2://Add new menu
      return timers_isSpace()?2:0;

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
      menu_cell_basic_header_draw(ctx, cell_layer, _("Stopwatches"));
      break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer,_("Timers"));
      break;
		
    case 2:
      menu_cell_basic_header_draw(ctx, cell_layer,_("Add new..."));
      break;
  }
}

void drawTimer(GContext* ctx, const Layer *cell_layer, uint8_t id, bool stopwatch){
	Timer* t=timers_get(id);
	uint32_t res_id;
	switch(timer_getStatus(t)){
		case TIMER_STATUS_STOPPED: res_id=RESOURCE_ID_ACTION_STOP; break;
		case TIMER_STATUS_RUNNING: res_id=RESOURCE_ID_ACTION_START; break;
		case TIMER_STATUS_PAUSED: res_id=RESOURCE_ID_ACTION_PAUSE; break;
		default: return;
	}
	char* title=(char *)malloc(12*sizeof(char));//"00 00:00:00";
	char* subtitle=(char *)malloc(17*sizeof(char));//"000 00:00:00.00";
	uint8_t lap=0;//stopwatch?0:timer_getActualLap(t);
	APP_LOG(APP_LOG_LEVEL_DEBUG,"lap= %d",timer_getActualLap(t));
	timer_setTotalTime(t,title,lap,true);
	timer_setLapTime(t,subtitle,lap,true);
	menu_cell_basic_draw(ctx,cell_layer,title,subtitle,bitmaps_get_bitmap(stopwatch?RESOURCE_ID_STOPWATCH:RESOURCE_ID_TIMER));
	graphics_draw_bitmap_in_rect(ctx,bitmaps_get_bitmap(res_id),GRect(18, 25, 14, 14));
	free(title);
	free(subtitle);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
    case 0:
			drawTimer(ctx,cell_layer,(uint8_t)cell_index->row, true);
			break;
		
		case 1:
			drawTimer(ctx,cell_layer,timers_stopwatch_count()+(uint8_t)cell_index->row, false);
			break;

    case 2:
      switch (cell_index->row) {
	    	case 0:
	  			menu_cell_basic_draw(ctx,cell_layer,_("Stopwatch"),NULL,bitmaps_get_bitmap(RESOURCE_ID_STOPWATCH));
      		break;
				case 1:
	  			menu_cell_basic_draw(ctx,cell_layer,_("Timer"),NULL,bitmaps_get_bitmap(RESOURCE_ID_TIMER));
          break;
      }
      graphics_draw_bitmap_in_rect(ctx,bitmaps_get_bitmap(RESOURCE_ID_ACTION_PLUS),GRect(18, 25, 14, 14));
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			timers_stopwatch_select(cell_index->row);
			window_stopwatch_show();
			break;
		case 1:
			timers_timer_select(cell_index->row);
			window_stopwatch_show();
			break;
		case 2:
			switch (cell_index->row) {
				case 0:
					//Add stopwatch
					timers_add_stopwatch();
					window_stopwatch_show();
			  	break;
				case 1:
					//Add timer
					window_timer_duration_set_show();
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

void updateSelection(){
	int8_t index=timers_get_selectedIndex();
	if(index>=0&&index<timers_stopwatch_count()){
		menu_layer_set_selected_index(window_main_menu_layer,(MenuIndex){
			.section=0,
			.row=index
		},MenuRowAlignCenter,false);
		return;
	}
	if(index>=timers_stopwatch_count()&&index<timers_count()){
		menu_layer_set_selected_index(window_main_menu_layer,(MenuIndex){
			.section=1,
			.row=index-timers_stopwatch_count()
		},MenuRowAlignCenter,false);
		return;
	}
	menu_layer_set_selected_index(window_main_menu_layer,(MenuIndex){
			.section=0,
			.row=0
		},MenuRowAlignCenter,false);
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	menu_layer_reload_data(window_main_menu_layer);
	
	updateSelection();
	
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
