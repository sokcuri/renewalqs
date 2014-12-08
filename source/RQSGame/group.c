#include "group.h"
#include "mem.h"
#include "pc.h"
#include "script.h"
#include "userobject.h"
void group_list(BSocket* sd)
{
	register int sz=5,i=0;
	struct group* g = sd->obj.group;
	wb(sd,0) = 170;
	wb(sd,3) = 99;
	wb(sd,4) = 0;
	if(g == NULL) 
	{
		ws(sd,sz) = SWAP16(0);
		sz+=2;
	}
	else
	{
		struct _list_elem* elem;
		struct userobject* us;
		wb(sd,sz++) = (g->count);
		wb(sd,sz++) = g->count;

		wb(sd,sz++) = sd->obj.ud.id_len;
		memmove(wp(sd,sz),sd->obj.ud.id,wb(sd,sz-1));
		sz += wb(sd,sz-1);
		wb(sd,sz++) = 1;
		ws(sd,sz) = SWAP16(sd->obj.ud.face);
		sz+=2;
		wb(sd,sz++) = (sd->obj.ud.hair);
		wb(sd,sz++) = sd->obj.ud.hair_color;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wb(sd,sz++) = 0;
		wl(sd,sz) = SWAP32(sd->obj.ud.maxhp);
		sz+=4;
		wl(sd,sz) = SWAP32(sd->obj.ud.hp);
		sz+=4;
		elem = list_begin(&g->members);
		for(i=0;i < g->count;i++)
		{
			us = list_entry(elem,struct userobject,group_elem);
			if(us == &sd->obj)
			{
				i++;
				elem = list_next(elem);
				continue;
			}
			wb(sd,sz++) = us->ud.id_len;
			memmove(wp(sd,sz),us->ud.id,wb(sd,sz-1));
			sz += wb(sd,sz-1);
			wb(sd,sz++) = 0;
			ws(sd,sz) = SWAP16(us->ud.face);
			sz+=2;
			wb(sd,sz++) = (us->ud.hair);
			wb(sd,sz++) = us->ud.hair_color;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wb(sd,sz++) = -1;
			wl(sd,sz) = SWAP32(us->ud.maxhp);
			sz+=4;
			wl(sd,sz) = SWAP32(us->ud.hp);
			sz+=4;
			elem= list_next(elem);
			i++;
		}
	}
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);

}
void group_make(struct userobject* user1,struct userobject* user2)
{
	struct group* g = MALOC(struct group,1);
	g->count = 2;
	list_init(&g->members);
	_list_add(&g->members,&user1->group_elem);
	_list_add(&g->members,&user2->group_elem);
	user1->group = user2->group = g;
}
void group_join(struct group* g,struct userobject* user)
{
	g->count++;
	_list_add(&g->members,&user->group_elem);
}
struct userobject* group_master(struct group* g)
{
	if(g->count == 0) return NULL;
	return list_entry(list_begin(&g->members),struct userobject,group_elem);
}
int group_leave(struct userobject* user)
{
	struct group* g = user->group;
	if(g != NULL){
		_list_remove(&user->group_elem);
		user->group = NULL;
		g->count--;
		return g->count;
	}
	return 0;
}