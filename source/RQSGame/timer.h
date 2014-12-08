#pragma once
#include "list.h"
struct timer
{
	long time;
	void* data;
	int state;
	struct _list_elem elem;
	long (*wakeup)(struct timer*);
};

enum timer_state
{
	t_sleep,
	t_wakeup,
	t_running,
	t_dead,
};
void timer_init();
void timer_add(struct timer*);
void timer_restart(struct timer* t);
void timer_set(struct timer*,long,void*, long (*)(struct timer*));

//void timer_check2();
void timer_check();