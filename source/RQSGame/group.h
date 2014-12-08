#pragma once
#include "list.h"
struct userobject;
struct group
{
	struct _list members;
	unsigned int count;
};

void group_make(struct userobject*,struct userobject*);
void group_join(struct group*,struct userobject*);
int group_leave(struct userobject*);

struct userobject* group_master(struct group* g);