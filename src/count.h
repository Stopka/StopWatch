#pragma once
#include <pebble.h>

typedef struct Count {
  uint8_t timer;
  uint8_t stopwatch;
} __attribute__((__packed__)) Count;

Count* count_create(){
	Count* c=malloc(sizeof(Count));
	c->timer=0;
	c->stopwatch=0;
	return c;
}

void count_destroy(Count* c){
	free(c);
}

uint8_t count_total(Count * c){
	return c->timer+c->stopwatch;
}