#include "utils.h"

Time* time_subtract(Time* a,Time* b){
	a->sec-=b->sec;
	if(a->msec<b->msec){
		a->sec--;
		a->msec=1000-b->msec;
	}else{
		a->msec-=b->msec;
	}
	return a;
}

Time* time_add(Time* a,Time* b){
	a->sec+=b->sec;
	a->msec=(a->msec+b->msec)%1000;
	while(a->msec>=1000){
		a->sec++;
		a->msec-=100;	
	}
	return a;
}

Time* time_clone(Time* t){
	Time* result=(Time*) malloc(sizeof(Time));
	result->sec=t->sec;
	result->msec=t->msec;
	return result;
}

Time* time_getActual(){
	Time* result=(Time*) malloc(sizeof(Time));
	time_ms	(&result->sec,&result->msec);
	return result;
}
Time* time_getNull(){
	Time* result=(Time*) malloc(sizeof(Time));
	result->sec=0;
	result->msec=0;
	return result;
}
