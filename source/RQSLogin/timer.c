#include "timer.h"
#include "list.h"
#include <time.h>
#include <stdio.h>
struct _list timerlist;
void timer_init()
{
	list_init(&timerlist);
}
void timer_set(struct timer* t,long tm,void* d,long (*wakeup)(void*))
{
	t->time = tm;
	t->data = d;
	t->wakeup = wakeup;
}
void timer_add(struct timer* t) {
	_list_add(&timerlist,&t->elem);
}
#define TIMER_MIN_TICK 100

void timer_check()
{
	struct _list_elem* elem = list_begin(&timerlist);
	while(elem != &timerlist.tail)
	{
		struct timer* tm = list_entry(elem,struct timer,elem);
		if(tm->time <= clock())
		{
			long next_time;
			elem = _list_remove(elem);
			next_time = tm->wakeup(tm);
			if(next_time >= 0)
			{
				next_time = next_time > TIMER_MIN_TICK ? next_time : TIMER_MIN_TICK;
				tm->time = clock() + next_time;
				_list_add(&timerlist,&tm->elem);
			}
		} //else break;
		else elem = list_next(elem);
	}
}