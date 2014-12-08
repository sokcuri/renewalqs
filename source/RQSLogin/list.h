#ifndef __LIST_HEADER
#define __LIST_HEADER

struct _list_elem
{
	struct _list_elem* prev;
	struct _list_elem* next;
};
struct _list
{
	struct _list_elem head;
	struct _list_elem tail;
};

#include <stddef.h>
#define list_empty(LIST) (LIST.head.next == NULL)
#define list_entry(ELEM,STRUCT,NAME) ((STRUCT*)((unsigned char*)(ELEM) - (unsigned char*)offsetof(STRUCT,NAME)))
void list_init(struct _list*);
#define list_head(LIST) (&LIST.head)
#define list_tail(LIST) (&LIST.tail)

struct _list_elem* list_next(struct _list_elem*);
struct _list_elem* list_prev(struct _list_elem*);
struct _list_elem* list_begin(struct _list*);
struct _list_elem* list_end(struct _list*);
void _list_add(struct _list* , struct _list_elem*);
struct _list_elem* _list_remove(struct _list_elem*);
void list_insert(struct _list_elem*,struct _list_elem*);
#endif
