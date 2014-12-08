#include "domain.h"
#include "userobject.h"
#include "map.h"
#include "world.h"
#include "clan.h"
#include "group.h"
typedef int (*WRITEFUNC)(struct domain*,char*);

WRITEFUNC wfunc[] = {
	socket_write,
	socket_write_sight,
	socket_write_map,
	socket_write_group,
	socket_write_clan,
	socket_write_world,
};

int domain_write(struct domain* dm,char* packet)
{
	WRITEFUNC wf = wfunc[dm->type];
	return wf(dm,packet);
}