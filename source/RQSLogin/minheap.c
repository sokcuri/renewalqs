#include "minheap.h"
#include <stdio.h>
#include "mem.h"
#include <memory.h>
void minheap_init(struct minheap* hp,unsigned int initsize,int (*comp)(void*,void*))
{
	hp->comp = comp;
	hp ->init_size = hp->size = initsize;
	hp->size += 1;
	hp->count = 1;
	hp->heaptree = CALOC(void*,initsize+1);
}
void minheap_expand(struct minheap* hp,int sz)
{
	hp->heaptree = REALOC(void*,hp->heaptree,sz+hp->size);
	memset(hp->heaptree + hp->size,0,sz * sizeof( void*));
	hp->size += sz;
}
void minheap_pop_sub(struct minheap* hp,int parent)
{
	int child = parent*2;
	if(parent >= hp->count || child >= hp->count) return ;
	if(hp->comp(hp->heaptree[parent],hp->heaptree[child]) > 0)
	{
		void* temp = hp->heaptree[parent];
		hp->heaptree[parent] = hp->heaptree[child];
		hp->heaptree[child] = temp;
		minheap_pop_sub(hp,child);
	}
	if(child+1 < hp->count && hp->comp(hp->heaptree[parent],hp->heaptree[child+1])>0)
	{
		void* temp = hp->heaptree[parent];
		hp->heaptree[parent] = hp->heaptree[child+1];
		hp->heaptree[child+1] = temp;
		minheap_pop_sub(hp,child+1);
	}
}
void* minheap_pop(struct minheap* hp)
{
	int i = hp->count - 1;
	void* ret;
	if(i == 0) return NULL;
	hp->count--;
	ret = hp->heaptree[1];
	hp->heaptree[1] = hp->heaptree[i];
	hp->heaptree[i] = NULL;
	minheap_pop_sub(hp,1);
	return ret;
}
void minheap_push(struct minheap* hp,void* data)
{
	int i;
	if(hp->count >= hp->size)
	{
		minheap_expand(hp,hp->init_size);
	}
	hp->heaptree[(i = hp->count)] = data;
	while(i != 1 && hp->comp(hp->heaptree[i],hp->heaptree[i/2])<0)
	{
		void* temp = hp->heaptree[i];
		hp->heaptree[i] = hp->heaptree[i/2];
		hp->heaptree[i/2] = temp;
		i = i / 2;
	}
	if(i == hp->count) hp->count++;
}