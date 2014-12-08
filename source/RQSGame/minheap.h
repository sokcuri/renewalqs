#pragma once



struct minheap
{
	void** heaptree;
	unsigned int size;
	unsigned int count;
	unsigned int init_size;
	int (*comp)(void*,void*);
};
void minheap_init(struct minheap*,unsigned int,int (*comp)(void*,void*));

void minheap_push(struct minheap* hp,void* data);
void* minheap_pop(struct minheap* hp);
