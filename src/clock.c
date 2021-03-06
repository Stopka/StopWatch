#include "clock.h"

Clock* clock_subtract(Clock* a,Clock* b){
	a->sec-=b->sec;
	if(a->ms<b->ms){
		a->sec--;
		a->ms=1000+a->ms-b->ms;
	}else{
		a->ms-=b->ms;
	}
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
	Clock* result=clock_createNull();
	time_ms	(&result->sec,&result->ms);
	return result;
}
Clock* clock_createNull(){
	Clock* result=(Clock*) malloc(sizeof(Clock));
	result->sec=0;
	result->ms=0;
	return result;
}

Clock* clock_create(int* vals){
	Clock* result=clock_createNull();
	result->ms=vals[4];
	int units[4]={60*60*24,60*60,60,1};
	for(int8_t i=0;i<4;i++){
		result->sec+=units[i]*vals[i];
	}
	return result;
}

int* clock_getVals(Clock* c){
	int* r=(int*) malloc(5*sizeof(int));
	r[0]=(c->sec/(60*60*24));//days
	r[1]=(c->sec/(60*60))%24;//hours
	r[2]=(c->sec/60)%60;//minutes
	r[3]=c->sec%60;//secs
	r[4]=c->ms;//ms
	return r;
}

uint32_t clock_getMS(Clock* c){
	return c->sec*1000+c->ms;
}

void clock_destroy(Clock* c){
	free(c);
}

bool clock_isNull(Clock * time){
	return time->sec==0 && time->ms==0;;
}

int8_t clock_compare(Clock* a,Clock* b){
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