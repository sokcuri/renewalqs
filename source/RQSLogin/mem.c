#include "mem.h"
#include "debug.h"
#include <stdlib.h>
void _free(void* d)
{
	free(d);
}
void* _realoc(void* d,unsigned int sz)
{
	void* d2 = realloc(d,sz);
	PANIC(d2 != NULL,"memory overflow");
	return d2;
}
void* _maloc(unsigned int sz)
{
	void* d = malloc(sz);
	PANIC(d != NULL,"memory overflow");
	return d;
}
void* _caloc(unsigned int cnt,unsigned int sz)
{
	void* d = calloc(cnt,sz);
	
	PANIC(d != NULL,"memory overflow");
	return d;
}