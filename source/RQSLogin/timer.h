#pragma once
#include "list.h"
struct timer
{
	long time;
	void* data;
	long (*wakeup)(void*);
	struct _list_elem elem;
};


void timer_init();
void timer_add(struct timer*);
void timer_set(struct timer*,long,void*, long (*)(void*));

void timer_check();