#include "list.h"
#include "debug.h"
#include <stdlib.h>
#define NULL 0

void list_init(struct _list* l)
{
	l->head.next = &l->tail;
	l->head.prev = NULL;
	l->tail.prev = &l->head;
	l->tail.next = NULL;
}
struct _list_elem* list_next(struct _list_elem* el)
{
	PANIC(el != NULL,"list elem == NULL");
	return el->next;
}
struct _list_elem* list_prev(struct _list_elem* el)
{
	PANIC(el != NULL,"list elem == NULL");
	return el->prev;
}
struct _list_elem* list_begin(struct _list* l)
{
	return l->head.next;
}
struct _list_elem* list_end(struct _list* l)
{
	return l->tail.prev;
}
void _list_add(struct _list* l,struct _list_elem* el)
{
	if(el == NULL){
		printf("here!\n");
	}
	_list_insert(list_end(l),el);
}
struct _list_elem* _list_remove(struct _list_elem* el)
{
	struct _list_elem* ret;
	PANIC(el != NULL,"list elem == NULL");
	el->prev->next = ret = el->next;
	el->next->prev = el->prev;
	return ret;
}
void _list_insert(struct _list_elem* target,struct _list_elem* el)
{
	el->next = target->next;
	el->next->prev = el;
	el->prev = target;
	target->next = el;
}