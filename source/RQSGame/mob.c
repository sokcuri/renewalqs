#include "mob.h"
#include "map.h"
#include "mem.h"
#include "timer.h"
#include <stdlib.h>
#include <time.h>
#include "debug.h"
#include "script.h"
#include "pc.h"
#include <string.h>
struct mobdb* mobs[MOB_DB_MAX];

struct mapobject* findtarget(struct mapobject* od)
{
	struct mapobject* obj;
	struct mapobject* ret = NULL;
	unsigned int ret_dist = UINT_MAX;
	if(od->loc.map == NULL) return NULL;
	obj = object_first(od->loc.map);
	while(obj != NULL)
	{
		unsigned int dist = abs(od->loc.x - obj->loc.x) + abs(od->loc.y - obj->loc.y);
		if(check_sight(&obj->loc,&od->loc) &&  dist < ret_dist && can_damage(od,obj))
		{
			ret_dist = dist;
			ret = obj;
		}
		obj = object_next(obj);
	}
	return ret;
}
long mob_ai(struct timer* tm)
{
	struct mobobject* obj = (struct mobobject*)tm->data;
	int type = obj->db->aitype;
	if(obj->obj.loc.map->user_count == 0) return 0;
	if(obj->obj.state == dead) {
		object_del(&obj->obj);
		return -1;
	}
	if(type & ai_script) {
		shell_event(ON_AI,"i",obj->obj.id);
	}
	else
	{
		if(type & ai_avoid && obj->last_gage < 10 && obj->target)
		{
			object_avoid(&obj->obj,obj->target);
		}
		else if(!(type & ai_naive) && obj->target)
		{
			int fol;
			if(!OBJ_CHECK(obj->target) || !can_damage((struct mapobject*)obj,obj->target) || (fol=object_follow(&obj->obj,obj->target)) == -1)
			{
				obj->target = NULL;
			}
			else if(fol == 1)
			{
				shell_event(ON_ATTACK,"ini",obj->obj.id,obj->target->id);
			}
		}
		else if(type & ai_aggressive)
		{
			int fol;
			obj->target = findtarget(&obj->obj);
			if ( obj->target != NULL )
			{
				fol = object_follow(&obj->obj,obj->target);
				if(fol == 1)
					shell_event(ON_ATTACK,"ini",obj->obj.id,obj->target->id);
			}
		}
		else
		{
			int rnd = rand()%2;
			if(rnd == 0)
			{
				//side
				object_side(&obj->obj,rand()%4);
				object_walk(&obj->obj);
			}else if(rnd == 1)
			{
				object_walk(&obj->obj);
			}
		}
	}
	return obj->db->speed;
}
void mob_die(struct mobobject* obj)
{
	obj->obj.state = dead;
	object_side(&obj->obj,255);
}
void mob_del(struct mapobject* obj)
{
	
}
struct object_event mob_event = {
	mob_del,
};
void mobobject_new(struct mobobject* obj,int code)
{
	obj->db = mobs[code];
	obj->obj.image = obj->db->image;
	obj->obj.color = obj->db->color;
	obj->obj.type = MONSTER;
	obj->obj.showtype = 5;
	obj->aitimer.data = obj;
	obj->target = NULL;
	obj->obj.events = &mob_event;
	obj->aitimer.time = clock();
	obj->aitimer.wakeup = mob_ai;
	timer_add(&obj->aitimer);
	object_new(&obj->obj,1);
}
void register_mobdb(int code,char* name,unsigned int img,unsigned char col,unsigned char aitype,long speed)
{
	struct mobdb* m;
	if(code >= MOB_DB_MAX) {
		errf("몹 코드 범위를 초과했습니다(%d) \n",code);
		return;
	}
	if(mobs[code] != NULL) {
		m = mobs[code];
	}
	else mobs[code] = m = MALOC(struct mobdb,1);
	
	strncpy(m->name,name,MOB_NAME_MAX);
	m->id = code;
	m->speed = speed < 100 ? 100 : speed;
	m->name_len = strlen(m->name);
	m->image = img;
	m->aitype = aitype;
	m->color = col;
}
struct mobobject* create_mobobj(int code)
{
	struct mobobject* obj = CALOC(struct mobobject,1);
	mobobject_new(obj,code);
	return obj;
}