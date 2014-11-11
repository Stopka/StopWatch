#include "clock.h"

Clock* clock_subtract(Clock* a,Clock* b){
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"clock_subtract a(%d,%d)-b(%d,%d)",(int)a->sec,(int)a->ms,(int)b->sec,(int)b->ms);
	a->sec-=b->sec;
	if(a->ms<b->ms){
		a->sec--;
		a->ms=1000+a->ms-b->ms;
	}else{
		a->ms-=b->ms;
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"result(%d,%d)",(int)a->sec,(int)a->ms);
	return a;
}

Clock* clock_add(Clock* a,Clock* b){
	a->sec+=b->sec;
	a->ms=(a->ms+b->ms)%1000;
	while(a->ms>=1000){
		a->sec++;
		a->ms-=100;	
	}
	return a;
}

Clock* clock_clone(Clock* t){
	Clock* result=(Clock*) malloc(sizeof(Clock));
	result->sec=t->sec;
	result->ms=t->ms;
	return result;
}

Clock* clock_createActual(){
	Clock* result=(Clock*) malloc(sizeof(Clock));
	time_ms	(&result->sec,&result->ms);
	return result;
}
Clock* clock_createNull(){
	Clock* result=(Clock*) malloc(sizeof(Clock));
	result->sec=0;
	result->ms=0;
	return result;
}

void clock_destroy(Clock* c){
	free(c);
}

bool clock_isNull(Clock * time){
	return time->sec==0 && time->ms==0;;
}

int clock_compare(Clock* a,Clock* b){
	if(a->sec>b->sec){
		return 1;
	}
	if(a->sec<b->sec){
		return -1;
	}
	if(a->ms>b->ms){
		return 1;
	}
	if(a->ms<b->ms){
		return -1;
	}
	return 0;
}