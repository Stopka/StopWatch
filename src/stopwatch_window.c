#include "stopwatch_window.h"
#include "stopwatch_model.h"
#include <string.h>

#define RUNNING_REFRESH true
	
#define FONT_TIME_DISPLAY RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_25
#define FONT_TIME_DISPLAY_LABEL RESOURCE_ID_FONT_DEJAVU_SANS_12
#define FONT_LAPS_DISPLAY_LAP RESOURCE_ID_FONT_DEJAVU_SANS_19
	
#define ICON_RESET RESOURCE_ID_ICON_RESET
#define ICON_LAP RESOURCE_ID_ICON_LAP
#define ICON_START RESOURCE_ID_ICON_STOPWATCH_START
#define ICON_STOP RESOURCE_ID_ICON_STOPWATCH_STOP
#define ICON_NEXT RESOURCE_ID_ICON_DOWN
#define IMG_MARK RESOURCE_ID_IMG_MARK
//window
static Window* stopwatch_window;
static PropertyAnimation *animation[STOPWATCH_MAX_LAPS+1+5];
//Time display
static Layer *time_display;
static TextLayer* time_display_values;
static TextLayer* time_display_labels[7];
char *measure_labels[5] = {"d","h","m","s","cs"};
//Laps
static ScrollLayer* laps_display;
static Layer* laps_display_times;
static TextLayer* laps_display_laps[STOPWATCH_MAX_LAPS+1+5];
static int laps_display_laps_count=0;
static InverterLayer* laps_display_mark;
//Action bar
static ActionBarLayer* action_bar;	
static GBitmap* bitmaps[5];
static GFont font[3];

//STATE
static int measure_offset=2;
static int measure_offset_laps=2;
static int selected_lap=0;

static void stopwatch_window_update_measure(){
	for(int i=0;i<6;i++){
		text_layer_set_text	(time_display_labels[i],measure_labels[(i<3?measure_offset:measure_offset_laps)+(i%3)]);
		layer_mark_dirty((Layer *)time_display_labels[i]);
	}
}
int stopwatch_window_lap_offset(int index){
	int result;
	Time* time=stopwatch_model_getLapTime(index);
	if((time->sec/(60*60*24))>0){
		result=0;
	}else if((time->sec/(60*60))%24>0){
		result=1;
	}else{
		result=2;
	}
	free(time);
	return result;
}

void stopwatch_window_update_lap(int index){
	char* string = (char*)text_layer_get_text(laps_display_laps[index]);
	int shift=stopwatch_model_getTotalLapsCount()-index;
	Time* time=stopwatch_model_getLapTime(stopwatch_model_getLapsCount()-index-1);
	int vals[]={time->msec/10,time->sec%60,(time->sec/60)%60,(time->sec/(60*60))%24,(time->sec/(60*60*24))%100,(time->sec/(60*60*24*100))};
	switch(measure_offset_laps){
		case 0:
			snprintf(string, 12, "%02d %02d,%02d:%02d", (shift)%100,vals[4],vals[3],vals[2]);
			break;
		case 1:
			snprintf(string, 12, "%02d %02d:%02d:%02d", (shift)%100,vals[3],vals[2],vals[1]);
			break;
		default:
			snprintf(string, 12, "%02d %02d:%02d.%02d", (shift)%100,vals[2],vals[1],stopwatch_model_isRunning()&&index==0?0:vals[0]);
			break;
	}
	layer_mark_dirty((Layer *)laps_display_laps[index]);
	free(time);
}

void checkAnimation(int i){
	if(animation[i]!=NULL){
		animation_unschedule((Animation*)animation[i]);
		property_animation_destroy(animation[i]);
		animation[i]=NULL;
	}
}

void general_animation_stopped(Animation *anim, bool finished, void *data) {
	int* i=(int*)data;
	if(animation[*i]!=NULL){
   		property_animation_destroy(animation[*i]);
		animation[*i]=NULL;
	}
	free (i);
}

void destroy_animation_stopped(Animation *anim, bool finished, void *data) {
	int* i=(int*)data;
	char* string = (char*)text_layer_get_text(laps_display_laps[*i]);
	layer_remove_from_parent((Layer *) laps_display_laps[*i]);	
	text_layer_destroy(laps_display_laps[*i]);
	laps_display_laps[*i]=NULL;
	free(string);
	general_animation_stopped(anim,finished,data);
}

void stopwatch_window_update_laps(int count,bool animate){
	if(animate&&laps_display_laps_count==count&&laps_display_laps_count==STOPWATCH_MAX_LAPS){//odeber poslední řádek
		GRect to_frame = GRect(130, 20*(STOPWATCH_MAX_LAPS-1), 121, 20);
		for(int i=STOPWATCH_MAX_LAPS+1;i<STOPWATCH_MAX_LAPS+1+5;i++){
			if(animation[i]!=NULL){
				continue;
			}
			animation[i]=property_animation_create_layer_frame((Layer*)laps_display_laps[STOPWATCH_MAX_LAPS-1],NULL,&to_frame);
			laps_display_laps[i]=laps_display_laps[STOPWATCH_MAX_LAPS-1];
			laps_display_laps[STOPWATCH_MAX_LAPS-1]=NULL;
			int* data=malloc(sizeof(int));
			*data=i;
			animation_set_handlers((Animation*) animation[i], (AnimationHandlers) {
				.stopped = (AnimationStoppedHandler) destroy_animation_stopped,
			  }, data);
			animation_schedule((Animation*) animation[i]);	
			laps_display_laps_count--;
			break;
		}
	}
	for(int i=laps_display_laps_count-1;i>=count;i--){//odstranit přebytečné
		int* data=malloc(sizeof(int));
		*data=i;
		if(!animate){
			destroy_animation_stopped(NULL, false, data);
			continue;
		}
		checkAnimation(i);
		GRect to_frame = GRect(130, 20*i, 121, 20);
		animation[i]=property_animation_create_layer_frame((Layer*)laps_display_laps[i],NULL,&to_frame);
		animation_set_handlers((Animation*) animation[i], (AnimationHandlers) {
			.stopped = (AnimationStoppedHandler) destroy_animation_stopped,
		  }, data);
		animation_schedule((Animation*) animation[i]);
	}
	for(int i=count-1;i-(count-laps_display_laps_count)>=0&&count>laps_display_laps_count;i--){//posunout existující
		laps_display_laps[i]=laps_display_laps[i-(count-laps_display_laps_count)];
		if(!animate){
			layer_set_frame((Layer*)laps_display_laps[i],GRect(0, 20*i, 121, 20));
		}else{
			checkAnimation(i);
			int* data=malloc(sizeof(int));
			*data=i;
			GRect to_frame = GRect(0, 20*i, 121, 20);
			animation[i]=property_animation_create_layer_frame((Layer*)laps_display_laps[i],NULL,&to_frame);
			animation_set_handlers((Animation*) animation[i], (AnimationHandlers) {
				.stopped = (AnimationStoppedHandler) general_animation_stopped,
			  }, data);
			animation_schedule((Animation*) animation[i]);
		}
		
	}
	for(int i=0;i<count-laps_display_laps_count;i++){//vytvořit chybějící
		char* string = (char *)malloc(12*sizeof(char));
		laps_display_laps[i]=text_layer_create(GRect(0, 20*i, 121, 20));
		text_layer_set_font(laps_display_laps[i],font[2]);
		text_layer_set_background_color	(laps_display_laps[i],GColorClear);
		text_layer_set_text_color(laps_display_laps[i],GColorWhite);
		text_layer_set_text_alignment(laps_display_laps[i], GTextAlignmentCenter);
		text_layer_set_text	(laps_display_laps[i],string);
		layer_add_child(laps_display_times, (Layer *)laps_display_laps[i]);
	}
	laps_display_laps_count=count;
	scroll_layer_set_content_size(laps_display,GSize(121,20*(laps_display_laps_count)));
	measure_offset_laps=2;
	for(int i=0;i<laps_display_laps_count;i++){//zjistit jednotky
		int off=stopwatch_window_lap_offset(i);
		if(off<measure_offset_laps){
			measure_offset_laps=off;
		}
	}
	stopwatch_window_update_measure();
	for(int i=0;i<laps_display_laps_count;i++){//nastavit text
		stopwatch_window_update_lap(i);
	}
}

void stopwatch_window_update_time(){
	char* string = (char*)text_layer_get_text(time_display_values);
	Time* time=stopwatch_model_getLapTotalTime(stopwatch_model_getLapsCount()-selected_lap-1);
	int vals[]={time->msec/10,time->sec%60,(time->sec/60)%60,(time->sec/(60*60))%24,(time->sec/(60*60*24))%100,(time->sec/(60*60*24*100))};
	if(vals[5]>0||vals[4]>0){
			snprintf(string, 9, "%02d,%02d:%02d", vals[4],vals[3],vals[2]);
			measure_offset=0;
	}else if(vals[3]>0){
			snprintf(string, 9, "%02d:%02d:%02d", vals[3],vals[2],vals[1]);
			measure_offset=1;
	}else{
			snprintf(string, 9, "%02d:%02d.%02d", vals[2],vals[1],stopwatch_model_isRunning()&&selected_lap==0?0:vals[0]);
			measure_offset=2;
	}
	int off=stopwatch_window_lap_offset(stopwatch_model_getLapsCount()-1);
	if(off<measure_offset_laps){
		measure_offset_laps=off;
		stopwatch_window_update_laps(stopwatch_model_getLapsCount(),false);
	}else{
		stopwatch_window_update_lap(0);
	}
	stopwatch_window_update_measure();
	layer_mark_dirty((Layer *)time_display_values);
	free(time);
}

static void stopwatch_window_update_running(){
	bool running=stopwatch_model_isRunning();
	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmaps[running?1:0]);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmaps[running?3:2]);
	layer_mark_dirty((Layer *)action_bar);
}

static void stopwatch_window_update_selected(){
	//layer_set_frame((Layer*)laps_display_mark,GRect(0,20*selected_lap,30,20));
	int* data=malloc(sizeof(int));
	*data=STOPWATCH_MAX_LAPS;
	GRect to_frame = GRect(0,20*selected_lap,30,20);
	animation[STOPWATCH_MAX_LAPS]=property_animation_create_layer_frame((Layer*)laps_display_mark,NULL,&to_frame);
	animation_set_handlers((Animation*) animation[STOPWATCH_MAX_LAPS], (AnimationHandlers) {
        .stopped = (AnimationStoppedHandler) general_animation_stopped,
      }, data);
	animation_schedule((Animation*) animation[STOPWATCH_MAX_LAPS]);
	stopwatch_window_update_time();
	scroll_layer_set_content_offset(laps_display,GPoint(0,-20*(selected_lap-1)),true);
}

static void click_config_provider(void *context);

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	stopwatch_window_update_time();
}

static void window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_load()");
	// fonts //////////////////ú
	font[0]=fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY));
	font[1]=fonts_load_custom_font(resource_get_handle(FONT_TIME_DISPLAY_LABEL));
	font[2]=fonts_load_custom_font(resource_get_handle(FONT_LAPS_DISPLAY_LAP));
	///////////////////////////////////////////////////////////////////
	// Time display
	///////////////////////////////////////////////////////////////////
	time_display=layer_create(GRect(0, 0, 144, 50));
	layer_add_child(window_get_root_layer(window), (Layer*)time_display);
	
	// Values ////////////////////////////////////////////////////////////
	time_display_values=text_layer_create(GRect(0, 8, 122, 40));
	text_layer_set_font(time_display_values,font[0]);
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
	
	time_display_labels[6]=text_layer_create(GRect(0, 36, 32, 14));
	time_display_labels[3]=text_layer_create(GRect(32, 36, 30, 14));
	time_display_labels[4]=text_layer_create(GRect(62, 36, 30, 14));
	time_display_labels[5]=text_layer_create(GRect(92, 36, 30, 14));
	for(int i=0;i<6;i++){
		text_layer_set_font(time_display_labels[i],font[1]);
		text_layer_set_background_color	(time_display_labels[i],(i<3?GColorClear:GColorWhite));
		text_layer_set_text_color(time_display_labels[i],(i<3?GColorWhite:GColorBlack));
		text_layer_set_text_alignment(time_display_labels[i], GTextAlignmentCenter);
		text_layer_set_overflow_mode(time_display_labels[i],GTextOverflowModeFill);
		layer_add_child((Layer *)time_display, (Layer *)time_display_labels[i]);
	}
	stopwatch_window_update_measure();
	text_layer_set_font(time_display_labels[6],font[1]);
	text_layer_set_background_color	(time_display_labels[6],GColorWhite);
	text_layer_set_text_color(time_display_labels[6],GColorBlack);
	text_layer_set_text_alignment(time_display_labels[6], GTextAlignmentCenter);
	text_layer_set_overflow_mode(time_display_labels[6],GTextOverflowModeFill);
	text_layer_set_text	(time_display_labels[6],"#");
	layer_add_child((Layer *)time_display, (Layer *)time_display_labels[6]);
	
	///////////////////////////////////////////////////////////////////
	// Laps display
	///////////////////////////////////////////////////////////////////
	laps_display=scroll_layer_create(GRect(0,50,121,100));
	layer_add_child(window_get_root_layer(window), (Layer*)laps_display);
	laps_display_times=layer_create(GRect(0,0,121,20*STOPWATCH_MAX_LAPS));
	scroll_layer_add_child(laps_display,(Layer*)laps_display_times);
	// Laps ////////////////////////////////////////////////////////////
	stopwatch_window_update_laps(stopwatch_model_getLapsCount(),false);
	laps_display_mark=inverter_layer_create(GRect(0,0,30,20));
	scroll_layer_add_child(laps_display,(Layer*)laps_display_mark);
	
	///////////////////////////////////////////////////////////////////
	// Action bar
	///////////////////////////////////////////////////////////////////
	action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_background_color(action_bar, GColorWhite);
	bitmaps[0]=gbitmap_create_with_resource(ICON_RESET);
	bitmaps[1]=gbitmap_create_with_resource(ICON_LAP);
	bitmaps[2]=gbitmap_create_with_resource(ICON_START);
	bitmaps[3]=gbitmap_create_with_resource(ICON_STOP);
	bitmaps[4]=gbitmap_create_with_resource(ICON_NEXT);
	stopwatch_window_update_running();
  	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmaps[4]);
	action_bar_layer_set_click_config_provider(action_bar,click_config_provider);
	//INIT
	/* volá se v window_appear()
	stopwatch_window_update_time();
	if(stopwatch_model_isRunning()){
		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	}*/
}

static void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_appear()");
	stopwatch_window_update_time();
	if(stopwatch_model_isRunning()){
		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	}
}

static void window_disappear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_disappear()");
	tick_timer_service_unsubscribe();
}

static void window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO,"window_unload()");
	tick_timer_service_unsubscribe();
	//Time display
	layer_destroy(time_display);
	text_layer_destroy	(time_display_values);
	for(int i=0;i<7;i++){
		text_layer_destroy	(time_display_labels[i]);		
	}
	//Laps display
	scroll_layer_destroy(laps_display);
	stopwatch_window_update_laps(0,false);
	layer_destroy(laps_display_times);
	inverter_layer_destroy(laps_display_mark);	

	//Action bar
	action_bar_layer_destroy(action_bar);
	for(int i=0;i<5;i++){
		gbitmap_destroy(bitmaps[i]);
	}
	
	for(int i=0;i<3;i++){
		fonts_unload_custom_font(font[i])	;
	}
}
////////////////////////////////////////////////////////////////////
//Commands
////////////////////////////////////////////////////////////////////
static void stopwatch_window_command_newLap(){
	stopwatch_model_newlap();
	stopwatch_window_update_laps(stopwatch_model_getLapsCount(),true);
}

static void stopwatch_window_command_reset(){
	stopwatch_model_reset();
	tick_timer_service_unsubscribe();
	selected_lap=0;
	stopwatch_window_update_selected();
	stopwatch_window_update_running();
	//stopwatch_window_update_time(); //volá se při update selected
	stopwatch_window_update_laps(stopwatch_model_getLapsCount(),true);
}

static void stopwatch_window_command_start(){
	stopwatch_model_start();
	stopwatch_window_update_running();
	//stopwatch_window_update_time();
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void stopwatch_window_command_stop(){
	stopwatch_model_stop();
	tick_timer_service_unsubscribe();
	stopwatch_window_update_running();
	stopwatch_window_update_time();
}

////////////////////////////////////////////////////////////////////
//Event handlers
////////////////////////////////////////////////////////////////////
static void handle_up_single_click(ClickRecognizerRef recognizer, void *context){
	if(stopwatch_model_isRunning()){
		stopwatch_window_command_newLap();
	}else{
		stopwatch_window_command_reset();
	}
}

static void handle_up_long_click(ClickRecognizerRef recognizer, void *context){
	stopwatch_window_command_reset();
}

static void handle_select_single_click(ClickRecognizerRef recognizer, void *context){
	if(stopwatch_model_isRunning()){
		stopwatch_window_command_stop();
	}else{
		stopwatch_window_command_start();
	}
}

static void handle_down_single_click(ClickRecognizerRef recognizer, void *context){
	selected_lap=(selected_lap+1)%stopwatch_model_getLapsCount();
	stopwatch_window_update_selected();
}

static void handle_down_long_click(ClickRecognizerRef recognizer, void *context){
	selected_lap=((selected_lap==0)?(stopwatch_model_getLapsCount()-1):(selected_lap-1));
	stopwatch_window_update_selected();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) handle_up_single_click);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) handle_select_single_click);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) handle_down_single_click);
  
  window_long_click_subscribe(BUTTON_ID_UP, 0,(ClickHandler) handle_up_long_click,NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0,(ClickHandler) handle_down_long_click,NULL);
}


////////////////////////////////////////////////////////////////////
//Public intarface
////////////////////////////////////////////////////////////////////
void stopwatch_window_init(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_init()");
	stopwatch_window = window_create();
	window_set_background_color	(stopwatch_window,GColorBlack);	
	window_set_window_handlers(stopwatch_window, (WindowHandlers) {
		.load = window_load,
		.appear = window_appear,
		.disappear = window_disappear,
		.unload = window_unload
	});
	window_stack_push(stopwatch_window, true);
}

void stopwatch_window_deinit(){
	APP_LOG(APP_LOG_LEVEL_INFO,"window_deinit()");
	window_stack_pop(true);
	window_destroy(stopwatch_window);
}