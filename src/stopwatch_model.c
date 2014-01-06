#include "stopwatch_model.h"
	
#define STOPWATCH_MAX_PERSIST 3
	
Time* started=NULL;
Time* laps[STOPWATCH_MAX_LAPS+1];
int laps_count=0;
int total_laps_count=0;
bool running=false;
        
void stopwatch_model_init(){
		APP_LOG(APP_LOG_LEVEL_INFO,"model_init()");
        if(!persist_exists(0)||!persist_exists(1)||!persist_exists(2)){
				APP_LOG(APP_LOG_LEVEL_WARNING,"missing persist data: %i %i %i %i",persist_exists(0),persist_exists(1),persist_exists(2),persist_exists(3));
                stopwatch_model_reset();
                return;
        }
        running=persist_read_bool(0);
		APP_LOG(APP_LOG_LEVEL_INFO,"running: %i",(int)running);
        laps_count=persist_read_int(1);
        APP_LOG(APP_LOG_LEVEL_INFO,"laps_count: %i",laps_count);
		total_laps_count=persist_read_int(2);
		APP_LOG(APP_LOG_LEVEL_INFO,"total_laps_count: %i",total_laps_count);
        started=(Time*) malloc(sizeof(Time));
        persist_read_data(3,started,sizeof(Time));
		APP_LOG(APP_LOG_LEVEL_INFO,"started: object");
        for(int i=0;i<=laps_count;i++){
                if(persist_exists(4+i)){
                	laps[i]=(Time*) malloc(sizeof(Time));
                    persist_read_data(4+i,laps[i],sizeof(Time));
					APP_LOG(APP_LOG_LEVEL_INFO,"laps[%i]: object",i);
				}else{
					APP_LOG(APP_LOG_LEVEL_INFO,"laps[%i]: NULL",i);
					laps[i]=NULL;
					laps_count=i==0?1:i;
					APP_LOG(APP_LOG_LEVEL_WARNING,"laps_count: %i",laps_count);
					if(i==0){
						APP_LOG(APP_LOG_LEVEL_WARNING,"reset()");
						stopwatch_model_reset();
                		return;
					}
					break;
				}
        }
}
void stopwatch_model_deinit(){
		APP_LOG(APP_LOG_LEVEL_INFO,"model_deinit()");
		int write_result;
        write_result=persist_write_bool(0,running);
		APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"write(%i) running(%i): %i",0,(int)running,write_result);
		int plaps=laps_count>STOPWATCH_MAX_PERSIST?STOPWATCH_MAX_PERSIST:laps_count;
        write_result=persist_write_int(1,plaps);
		APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"write(%i) plaps(%i): %i",1,plaps,write_result);
        write_result=persist_write_int(2,total_laps_count);
		APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"write(%i) total_laps_count(%i): %i",2,total_laps_count,write_result);
		write_result=persist_delete(3);
		APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"delete(%i) started: %i",3,write_result);
		if(started!=NULL){
                write_result=persist_write_data(3,started,sizeof(Time));
				APP_LOG(write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO,"write(%i) started(object): %i",3,write_result);
		}
        for(int i=0;i<=plaps;i++){
				write_result=persist_delete(4+i);
				APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"delete(%i) laps[%i]: %i",4+i,laps_count-plaps+i,write_result);
                if(laps[laps_count-plaps+i]!=NULL){
                        write_result=persist_write_data(4+i,laps[laps_count-plaps+i],sizeof(Time));
						APP_LOG((write_result<0?APP_LOG_LEVEL_WARNING:APP_LOG_LEVEL_INFO),"write(%i) laps[%i](object): %i",4+i,laps_count-plaps+i,write_result);
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