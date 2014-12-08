#include <stdlib.h>
#include "hashtable.h"
#include "list.h"
#include "debug.h"
#include "mem.h"
void hash_init(struct hashtable* ht,int size,int (*hashfunc)(struct hashtable*,void*),int (*hashcomp)(void*,struct _list_elem*))
{
	int i;
	ht->hashcompare = hashcomp;
	ht->hashfunc = hashfunc;
	ht->list_size = size;
	ht->hash_lists = MALOC(struct _list,size);
	for(i=0;i<size;i++) list_init(&ht->hash_lists[i]);
}

struct _list_elem* hash_get(struct hashtable* ht,void* d)
{
	struct _list_elem * elem;
	int indx = ht->hashfunc(ht,d);
	PANIC(indx < ht->list_size,"Hash value is larger than size");
	elem = list_begin(&ht->hash_lists[indx]);
	while(elem != list_tail(ht->hash_lists[indx]))
	{
		if(ht->hashcompare(d,elem) == 0) return elem;
		elem = list_next(elem);
	}
	return NULL;
}

void hash_insert(struct hashtable* ht,void* d,struct _list_elem* elem)
{
	int indx = ht->hashfunc(ht,d);
	PANIC(indx < ht->list_size,"Hash value is larger than size");
	_list_add(&ht->hash_lists[indx],elem);
}