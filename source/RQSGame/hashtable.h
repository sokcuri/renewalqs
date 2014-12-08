#pragma once
struct _list;
struct _list_elem;
struct hashtable
{
	int list_size;
	struct _list* hash_lists;
	int (*hashfunc)(struct hashtable*,void*);
	int (*hashcompare)(void*,struct _list_elem*);
};
struct _list_elem* hash_next(struct hashtable* ht,void* d,struct _list_elem* elem);
void hash_init(struct hashtable*,int,int (*hashfunc)(struct hashtable*,void*),int (*hashcomp)(void*,struct _list_elem*));
#define hash_free(ht) (_free((ht)->hash_lists))
struct _list_elem* hash_get(struct hashtable*,void*);
void hash_insert(struct hashtable* ht,void* d,struct _list_elem* elem);
