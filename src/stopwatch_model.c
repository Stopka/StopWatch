#include "stopwatch_model.h"
	
#define STOPWATCH_MAX_PERSIST 3
	
Time* started=NULL;
Time* laps[STOPWATCH_MAX_LAPS+1];
int laps_count=0;
int total_laps_count=0;
bool running=false;
        
void stopwatch_model_init(){
        if(!persist_exists(0)||!persist_exists(1)||!persist_exists(2)||!persist_exists(3)){
				app_log(APP_LOG_LEVEL_WARNING,"model init",13,"missing persist data: %i %i %i %i",persist_exists(0),persist_exists(1),persist_exists(2),persist_exists(3));
                stopwatch_model_reset();
                return;
        }
        running=persist_read_bool(0);
		app_log(APP_LOG_LEVEL_INFO,"model init",18,"running: %i",(int)running);
        laps_count=persist_read_int(1);
        app_log(APP_LOG_LEVEL_INFO,"model init",20,"laps_c: %i",laps_count);
		total_laps_count=persist_read_int(2);
		app_log(APP_LOG_LEVEL_INFO,"model init",22,"t_laps_c: %i",total_laps_count);
        started=(Time*) malloc(sizeof(Time));
        persist_read_data(3,started,sizeof(Time));
		app_log(APP_LOG_LEVEL_INFO,"model init",28,"started: object");
        for(int i=0;i<=laps_count;i++){
                if(persist_exists(4+i)){
                        laps[i]=(Time*) malloc(sizeof(Time));
                        persist_read_data(4+i,laps[i],sizeof(Time));
						app_log(APP_LOG_LEVEL_INFO,"model init",35,"laps[%i]: object",i);
				}else{
					app_log(APP_LOG_LEVEL_INFO,"model init",38,"laps[%i]: NULL",i);
					laps[i]=NULL;
					if(i!=0&&laps[i-1]==NULL){
						laps_count=i;
						app_log(APP_LOG_LEVEL_WARNING,"model init",39,"laps_c: %i",laps_count);
						break;
					}
				}
        }
}
void stopwatch_model_deinit(){
		int write_result;
        write_result=persist_write_bool(0,running);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",36,"write: %i",write_result);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",37,"running: %i",(int)persist_exists(0));
		int plaps=laps_count>STOPWATCH_MAX_PERSIST?STOPWATCH_MAX_PERSIST:laps_count;
        write_result=persist_write_int(1,plaps);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",41,"write: %i",write_result);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",42,"laps: %i",(int)persist_exists(1));
        write_result=persist_write_int(2,total_laps_count);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",44,"write: %i",write_result);
		app_log(APP_LOG_LEVEL_INFO,"model deinit",45,"total laps: %i",(int)persist_exists(2));
		write_result=persist_delete(3);
		if(started!=NULL){
                write_result=persist_write_data(3,started,sizeof(Time));
				app_log(APP_LOG_LEVEL_INFO,"model deinit",49,"write: %i",write_result);
				app_log(APP_LOG_LEVEL_INFO,"model deinit",50,"started: %i",(int)persist_exists(3));
		}
        for(int i=0;i<=plaps;i++){
				write_result=persist_delete(4+i);
				app_log(APP_LOG_LEVEL_INFO,"model deinit",55,"delete: %i",write_result);
                if(laps[laps_count-plaps+i]!=NULL){
                        write_result=persist_write_data(4+i,laps[laps_count-plaps+i],sizeof(Time));
						app_log(APP_LOG_LEVEL_INFO,"model deinit",56,"write: %i",write_result);
						app_log(APP_LOG_LEVEL_INFO,"model deinit",56,"lap[%i]: %i",i,(int)persist_exists(4+i));
				}
        }
        stopwatch_model_reset();
}



void stopwatch_model_reset(){
        running=false;
        free(started);
        started=NULL;
        for(int i=0;i<=laps_count;i++){
                if(laps[i]!=NULL){
                        free(laps[i]);
                        laps[i]=NULL;
                }
        }
        laps_count=1;
        total_laps_count=1;
}

void stopwatch_model_newlap(){
        Time* now=time_getActual();
        if(laps_count==STOPWATCH_MAX_LAPS){
                free(laps[0]);
                for(int i=0;i<laps_count-1;i++){
                        laps[i]=laps[i+1];
                }
                laps[laps_count-1]=now;
        }else{
                laps[laps_count]=now;
                laps_count++;
        }
        total_laps_count++;
}
void stopwatch_model_start(){
        if(running){
                return;
        }
        Time* now=time_getActual();
        if(started==NULL){
                started=now;
                laps[0]=time_clone(now);
        }else{
                Time* delay=time_subtract(now,laps[laps_count]);
                free(laps[laps_count]);
                laps[laps_count]=NULL;
                time_add(started,delay);
                for(int i=0;i<laps_count;i++){
                        time_add(laps[i],delay);
                }
                free(delay);
        }
        running=true;
}
void stopwatch_model_stop(){
        if(!running){
                return;
        }
        laps[laps_count]=time_getActual();
        running=false;
}

bool stopwatch_model_isRunning(){
        return running;
}

int stopwatch_model_getLapsCount(){
        return laps_count;
}

int stopwatch_model_getTotalLapsCount(){
        return total_laps_count;
}

Time* stopwatch_model_getLapTime(int lap){
        if(running&&lap==laps_count-1){
                return time_subtract(time_getActual(),laps[lap]);
        }
        if(laps[lap]==NULL){
                return time_getNull();
        }
        return time_subtract(time_clone(laps[lap+1]),laps[lap]);
}
Time* stopwatch_model_getLapTotalTime(int lap){
        if(started==NULL){
                return time_getNull();
        }
        if(running&&lap==laps_count-1){
                return time_subtract(time_getActual(),started);
        }
        return time_subtract(time_clone(laps[lap+1]),started);
}