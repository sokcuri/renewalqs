#include "object.h"
#include "hashtable.h"
#include "map.h"
#include <stdlib.h>
#include "mem.h"
#include "game.h"
#include "script.h"
#include "pc.h"
#include "npc.h"
#include "mob.h"
#include "pet.h"
#include "item.h"
#include <memory.h>
#include "userimg.h"
#define OBJECT_INIT_SIZE 0x10000
unsigned int objects_size;
unsigned int objects_last_id;
struct mapobject** objects;
struct _list obj_dump;
void object_init()
{
	list_init(&obj_dump);
	objects = CALOC(struct mapobject*,OBJECT_INIT_SIZE);
	objects_size = OBJECT_INIT_SIZE;
	objects_last_id = 1;
}
struct mapobject* object_first(struct map* md)
{
	struct _list_elem* elem = list_begin(&md->objects);
	if(elem == &md->objects.tail) return NULL;
	return list_entry(elem,struct mapobject,elem);
}
struct mapobject* object_last(struct map* md)
{
	struct _list_elem* elem = list_end(&md->objects);
	if(elem == &md->objects.head) return NULL;
	return list_entry(elem,struct mapobject,elem);
}
struct mapobject* object_next(struct mapobject* obj)
{
	struct _list_elem* elem = list_next(&obj->elem);

	if(elem == list_tail(obj->loc.map->objects)) return NULL;
	return list_entry(elem,struct mapobject,elem);
}
struct mapobject* object_prev(struct mapobject* obj)
{
	struct _list_elem* elem = list_prev(&obj->elem);
	if(elem == list_head(obj->loc.map->objects)) return NULL;
	return list_entry(elem,struct mapobject,elem);
}
struct mapobject* getobj_xy(struct map* m,int x,int y,int tp,struct mapobject* od)
{

	if(x < 0 || x >= m->db->xs || y < 0 || y >= m->db->ys) return NULL;
	if(od == NULL) od = object_first(m);
	else od = object_next(od);
	while(od != NULL)
	{
		if(od->type & tp && x == od->loc.x && od->loc.y == y)
			return od;
		od = object_next(od);
	}
	return NULL;
}
struct mapobject* getfrontobj(struct mapobject* obj,int tp,int (*comp)(struct mapobject*,struct mapobject*))
{
	struct mapobject* od;
	int sx = obj->loc.x,sy = obj->loc.y;
	if(obj->loc.map == NULL) return NULL;
	switch(obj->loc.side)
	{
	case LEFT: sx--; break;
	case RIGHT: sx++; break;
	case UP: sy--; break;
	case DOWN: sy++; break;
	default: return NULL;
	}
	od = getobj_xy(obj->loc.map,sx,sy,tp,NULL);
	while(od != NULL)
	{
		if(comp == NULL || comp(obj,od)){
			return od;
		}
		od = getobj_xy(obj->loc.map,sx,sy,tp,od);
	}
	return NULL;
}
void object_gage(struct mapobject* obj,int percent,int critical,unsigned long dam)
{
	byte packet[16];
	WB(packet, 0) = 170;
	WS(packet, 1) = SWAP16(13);
	WB(packet, 3) = 19;
	WB(packet, 4) = 0;
	WL(packet, 5) = SWAP32(obj->id);
	WB(packet, 9) = critical;
	WB(packet, 10) = percent;
	WL(packet, 11) = SWAP32(dam);
	WB(packet, 15) = 0;
	write_sight(&obj->loc,packet);
	if(obj->type == MONSTER)
	{
		((struct mobobject*)obj)->last_gage = percent & 0xFF;
	}
}
void object_effect(struct mapobject* obj,unsigned short effect, int time)
{
	byte packet[15];
	
	if(obj->type == ITEM) return ;
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(10);
	WB(packet,3) = 41;
	WB(packet,4) = 0;
	WB(packet,5) = 0;
	WL(packet,6) = SWAP32(obj->id);
	WS(packet,10) = SWAP16(effect);
	WS(packet,12) = SWAP16(0x100);
	write_sight(&obj->loc,packet);
}
int map_y(struct map* md,int y)
{
	if(md == NULL) return y;
	if(md->db->ys <= 14) // 맵이 화면보다 작으면
	{
		return (int)((15-md->db->ys)/2)+y;
	}
	else
	{
		if(y <= 7) return y;
		else if(y >= md->db->ys-7) return 15-md->db->ys+y;
		else return 7;
	}
}
int map_x(struct map* md,int x) // 화면상에서 X
{
	if(md == NULL) return x;
	if(md->db->xs <= 16) // 맵이 화면보다 작으면
	{
		return (int)((17-md->db->xs)/2)+x;
	}
	else
	{
		if(x <= 8) return x;
		else if(x >= md->db->xs-8) return 17-md->db->xs+x;
		else return 8;
	}
}
int check_sightxy(struct map* map,int x,int y,int ox,int oy)
{
	ox += map_x(map,x) - x;
	oy += map_y(map,y) - y;
	// x - mx <= ox <= x-mx+16
	// y - my <= oy <= y-my+14
	return ((ox>=0) && (ox <= 17)) && ((oy>=0)&& (oy<=15));
}
int check_sight(struct location* po1,struct location* po2)
{
	register unsigned short ox,oy;
	if(po1->map != po2->map) return 0;
	ox = po2->x + map_x(po1->map,po1->x)-po1->x;
	oy = po2->y + map_y(po1->map,po1->y)-po1->y;
	// x - mx <= ox <= x-mx+16
	// y - my <= oy <= y-my+14
	return ((ox>=0) && (ox <= 17)) && ((oy>=0)&& (oy<=15));
}
void objects_expand(unsigned int addsize)
{
	objects = (struct mapobject**)realloc(objects,sizeof(struct mapobject*)*(objects_size + addsize));
	memset(objects + objects_size,0,addsize* sizeof(struct mapobject*));
	objects_size += addsize;
}
unsigned int object_new(struct mapobject* obj,int ev)
{
	char* name;
	int name_len;
	if(objects_last_id >= objects_size) {
		int i ;
		for(i=0;i<objects_size;i++)
		{
			if(objects[i] == NULL) {
				objects_last_id = i;
				break;
			}
		}
		if(i >= objects_size) objects_expand(OBJECT_INIT_SIZE);
	}
	obj->id = objects_last_id ++ ;
	objects[obj->id] = obj;
	obj->magic = OBJECT_MAGIC_NUMBER;
	return obj->id;
}
void object_hide_sight(struct mapobject* obj)
{
	byte packet[10];
	WB(packet,0)= 170;
	WS(packet,1) = SWAP16(7);
	WB(packet,3) = 14;
	WB(packet,4) = 0;
	WL(packet,5) = SWAP32(obj->id);
	WB(packet,9) = 0;
	write_sight2(obj,packet);
}
char* object_get_name(struct mapobject* obj,int* len)
{
	char* id;
	switch(obj->type)
	{
	case USER:
		*len = obj2sd(obj)->obj.ud.id_len;
		return obj2sd(obj)->obj.ud.id;
		break;
	case NPC:
		*len = ((struct npcobject*)obj)->db->name_len;
		return ((struct npcobject*)obj)->db->name;
		break;
	case MONSTER:
		*len = ((struct mobobject*)obj)->db->name_len;
		return ((struct mobobject*)obj)->db->name;
		break;
	case ITEM:
		
		*len = ((struct itemobject*)obj)->db->name_len;
		return ((struct itemobject*)obj)->db->name;
		break;
	}
}
void object_say(struct mapobject* obj,int type,char* message)
{
	BYTE packet[300];
	unsigned char size = strlen(message);
	packet[0] = 170;
	WS(packet,1) = SWAP16(size+9);
	WB(packet,3) = 13;
	WB(packet,4) = 0;
	WB(packet,5) = type & 0xFF;
	WL(packet,6) = SWAP32(obj->id);
	WB(packet,10) = size;
	memmove(packet+11,message,size);
	WB(packet,11+size) = 0;
	if ( type == 1 ){
		write_map(obj->loc.map,packet);
	} else {
		write_sight(&obj->loc,packet);
	}
}
void object_sound(struct mapobject* obj,int sound)
{
	byte packet[24];
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(20);
	WB(packet,3) = 25;
	WB(packet, 4) = 0;
	WB(packet, 5) = 0; //Sound Type
	WB(packet, 6) = 3;
	WS(packet, 7) = SWAP16(sound);
	WB(packet, 9) = (100); //volume
	WS(packet, 10) = SWAP16(4);
	WL(packet, 12) = SWAP32(obj->id);
	WS(packet, 16) = SWAP16(256);
	WS(packet, 18) = SWAP16(514);
	WS(packet, 20) = SWAP16(4);
	WB(packet, 22) = 0;
	write_sight(&obj->loc,packet);
}
void object_show_single(BSocket* sd,struct mapobject* obj)
{
	int name_len;
	char* name;
	if(obj->type == USER){
		user_show_single(sd,obj);
		return;
	}
	if(obj->image >= 65536)
	{
		uimg_show_single(obj->image - 65536,obj,sd);
		return ;
	}
	name = object_get_name(obj,&name_len);
	sdsize(sd,23+name_len);
	wb(sd,0) = 170;
	wb(sd,3) = 7;
	wb(sd,4) = 0;
	ws(sd,5) = SWAP16(1);
	ws(sd,7) = SWAP16(obj->loc.x); //x
	ws(sd,9)=SWAP16(obj->loc.y); // y
	wb(sd,11) = obj->showtype;

	wl(sd,12) = SWAP32(obj->id); // serial
	ws(sd,16) = SWAP16(obj->image); // look
	wb(sd,18) = obj->color; // color
	wb(sd,19) = obj->loc.side; //side
	wb(sd,20) = name_len;
	memmove(wp(sd,21),name,wb(sd,20));
	wb(sd,21+name_len) = 0;
	wb(sd,22+name_len) = 0;
	ws(sd,1) = SWAP16(20+name_len);
	cpt(sd);
}
void objects_show(BSocket* sd,struct location* loc)
{
	struct mapobject* o = object_first(loc->map);
	while(o!=NULL)
	{
		if(check_sight(loc,&o->loc))
		{
			object_show_single(sd,o);
		}
		o = object_next(o);
	}
}
void object_show_sight(struct mapobject* obj)
{
	byte packet_show[255];
	char* name;
	int name_len;
	if(obj->type == USER)
	{
		user_show_sight((struct userobject*)obj);
		return;
	}
	if(obj->image >= 65536) {
		uimg_show_sight(obj->image - 65536,obj);
		return ;
	}
	name = object_get_name(obj,&name_len);
	WS(packet_show,7) = SWAP16(obj->loc.x); //x
	WS(packet_show,9) = SWAP16(obj->loc.y); // y
	WB(packet_show,11) = obj->showtype;
	WL(packet_show,12) = SWAP32(obj->id); // serial
	WS(packet_show,16) = SWAP16(obj->image); // look
	WB(packet_show,18) = obj->color; // color
	WB(packet_show,19) = obj->loc.side; //side
	WB(packet_show,20) = name_len;
	memmove(packet_show+21,name,name_len);
	packet_show[21+name_len] = 0;
	packet_show[22+name_len] = 0;

	WB(packet_show,0) = 170;
	WS(packet_show,1) = SWAP16(20+name_len);
	WB(packet_show,3) = 7;
	WB(packet_show,4) = 0;
	WS(packet_show,5) = SWAP16(1);
	write_sight(&obj->loc,packet_show);
}
void object_action(struct mapobject* obj,int type,int time,int sound)
{
	BYTE packet[14];
	WB(packet,0) = (char)170;
	WS(packet,1) = SWAP16(11);
	WB(packet,3) = 26;
	WB(packet,4) = 0;
	WL(packet,5) = SWAP32(obj->id);
	WB(packet,9) = type;
	WB(packet,10) = 0;
	WB(packet,11) = time;
	WB(packet,12) = sound;
	WB(packet,13) = 0;
	write_sight(&obj->loc,packet);
}
void object_del(struct mapobject* obj)
{
	if(obj->loc.map) {
		map_delete(obj); 
	}
	obj->magic = 0xFFFFFFFF;
	shell_event(ON_DELETE,"i",obj->id);
	objects[obj->id] = NULL;
	if(obj->events && obj->events->del)
		obj->events->del(obj);
	if(obj->type != USER) _free(obj);
}
#define is_empty_up(m,x,y) (y >0 && !m->block[(y-1)*m->db->xs + (x)])
#define is_empty_down(m,x,y) (y < m->db->ys-1 && !m->block[(y+1)*m->db->xs + (x)])
#define is_empty_left(m,x,y) (x > 0 && !m->block[(y)*m->db->xs + (x-1)])
#define is_empty_right(m,x,y) (x < m->db->xs-1 && !m->block[(y)*m->db->xs + (x+1)])

int object_canmove(struct map* m,int x,int y,int side)
{
	switch(side)
	{
	case LEFT: return is_empty_left(m,x,y);
	case RIGHT: return is_empty_right(m,x,y);
	case DOWN: return is_empty_down(m,x,y);
	case UP: return is_empty_up(m,x,y);
	}
	return 0;
}
void object_damage(struct mapobject* obj,struct mapobject* target,int dam)
{
	if(target->events != NULL && target->events->dam != NULL)
		target->events->dam(obj,target,dam);
}
int object_avoid(struct mapobject* obj,struct mapobject* target)
{
	int tmp;
	register int dx,dy;
	int side[4] = {UP,RIGHT,DOWN,LEFT};
	if(!check_sight(&obj->loc,&target->loc))
		return -1; // object missing
	dx = obj->loc.x - target->loc.x;
	dy = obj->loc.y - target->loc.y;
	if(abs(dx) + abs(dy) == 1 ) {
		int side = 0;
		if(dx > 0) side = LEFT;
		else if(dx < 0) side = RIGHT;
		else if(dy > 0) side = UP;
		else if(dy < 0) side = DOWN;
		object_side(obj,side);
		return 1;
	}
	
	if(dx > 0) // 왼쪽에 타겟이 있다
	{
		side[1] = LEFT; side[3] = RIGHT;
	}
	if(dy < 0) //밑에 타겟이 있다.
	{
		side[0] = DOWN; side[2] = UP;
	}
	if(dy == 0 || (obj->loc.side%2 == side[0]%2 && dx!=0))
	{
		SWAP(side[0],side[1],int);
		SWAP(side[2],side[3],int);
	}
	for(tmp=0;tmp<4;tmp++)
	{
		if(object_canmove(obj->loc.map,obj->loc.x,obj->loc.y,side[tmp]))
		{
			object_side(obj,(side[tmp]+2)%4);
			object_walk(obj);
			return 0;
		} 
	}
	return 0;
}
int object_follow(struct mapobject* obj,struct mapobject* target)
{
	int tmp;
	register int dx,dy;
	int side[4] = {UP,RIGHT,DOWN,LEFT};
	if(!check_sight(&obj->loc,&target->loc))
		return -1; // object missing
	dx = obj->loc.x - target->loc.x;
	dy = obj->loc.y - target->loc.y;
	if(abs(dx) + abs(dy) == 1 ) {
		int side = 0;
		if(dx > 0) side = LEFT;
		else if(dx < 0) side = RIGHT;
		else if(dy > 0) side = UP;
		else if(dy < 0) side = DOWN;
		object_side(obj,side);
		return 1;
	}
	
	if(dx > 0) // 왼쪽에 타겟이 있다
	{
		side[1] = LEFT; side[3] = RIGHT;
	}
	if(dy < 0) //밑에 타겟이 있다.
	{
		side[0] = DOWN; side[2] = UP;
	}
	if(dy == 0 || (obj->loc.side%2 == side[0]%2 && dx!=0))
	{
		SWAP(side[0],side[1],int);
		SWAP(side[2],side[3],int);
	}
	for(tmp=0;tmp<4;tmp++)
	{
		if(object_canmove(obj->loc.map,obj->loc.x,obj->loc.y,side[tmp]))
		{
			object_side(obj,side[tmp]);
			object_walk(obj);
			return 0;
		} 
	}
	return 0;
}
void object_warp(struct mapobject* obj,struct map* m,unsigned short x,unsigned short y)
{
	if(obj->loc.map != NULL)
	{
		map_delete(obj);

	}
	obj->loc.map = m;
	obj->loc.x = min(m->db->xs,x);
	obj->loc.y = min(m->db->ys,y);
	if(obj->type == USER)
	{
		BSocket* sock = obj2sd(obj);
		map_send_info(sock,m);
		map_send_xy(sock,&obj->loc);
		map_send_bgm(sock,m->db->bgm);
		objects_show(sock,&obj->loc);
		effects_show(sock);
	}
	map_add(obj);
	if(obj->type == ITEM)
	{
		struct itemobject* iobj = (struct itemobject*)obj;
		if(iobj->owner != NULL)
		{
			item_slot_hide(obj2sd(iobj->owner),iobj->slot,iobj->item_msg);
			iobj->owner->ownitems[iobj->slot] = NULL;
			iobj->owner = NULL;
			iobj->item_msg = 0;
		}
	}
	object_show_sight(obj);
}
void object_hide_single2(BSocket* sd,unsigned int objid)
{
	sdsize(sd,10);
	wb(sd,0)= 170;
	ws(sd,1) = SWAP16(7);
	wb(sd,3) = 14;
	wb(sd,4) = 0;
	wl(sd,5) = SWAP32(objid);
	wb(sd,9) = 0;
	cpt(sd);
}
void object_hide_single(BSocket* sd,struct mapobject* obj)
{
	sdsize(sd,10);
	wb(sd,0)= 170;
	ws(sd,1) = SWAP16(7);
	wb(sd,3) = 14;
	wb(sd,4) = 0;
	wl(sd,5) = SWAP32(obj->id);
	wb(sd,9) = 0;
	cpt(sd);
}
static void object_walk_single(BSocket* sd,struct mapobject* obj,int bx,int by)
{
	sdsize(sd,15);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(12);
	wb(sd,3) = 12;
	wb(sd,4) = 0;
	wl(sd,5) = SWAP32(obj->id);
	ws(sd,9) = SWAP16(bx);
	ws(sd,11) = SWAP16(by);
	wb(sd,13) = (obj->loc.side);
	wb(sd,14) = 0;
	cpt(sd);
}

struct mapobject* getnearobj(struct location* loc,int type)
{
	struct mapobject* obj;
	struct mapobject* ret = NULL;
	unsigned int ret_dist = UINT_MAX;
	if(loc->map == NULL) return NULL;
	obj = object_first(loc->map);
	while(obj != NULL)
	{
		unsigned int dist = abs(loc->x - obj->loc.x) + abs(loc->y - obj->loc.y);
		if(obj->type & type && dist < ret_dist)
		{
			ret_dist = dist;
			ret = obj;
		}
		obj = object_next(obj);
	}
	return ret;
}
int can_damage(struct mapobject* obj,struct mapobject* target)
{
	if(target->state == dead || target->type == ITEM) return 0;
	if(obj->type == USER && target->type == MONSTER) return 1;
	else if(obj->type == MONSTER && target->type == USER) return 1;
	return 0;

}
void magic_effect_xy_single(BSocket* sd,int x,int y,unsigned short eff,int time);
int object_walk(struct mapobject* obj)
{
	struct mapobject* od;
	struct _list_elem* elem;
	int bx = obj->loc.x,by = obj->loc.y;
	int sx = bx, sy = by;
	int name_len;
	char loc;
	char* name;
	if(obj->loc.map == NULL) return 0;
	switch(obj->loc.side)
	{
	case LEFT: sx--; break;
	case RIGHT : sx++; break;
	case UP : sy--; break;
	case DOWN : sy ++; break;
	}
	
	if(obj->type == USER && (loc = obj->loc.map->onloc[sy*obj->loc.map->db->xs + sx]))
	{
		if( loc & 2) {
			struct warpdb* wdb = getwarp(obj->loc.map,sx,sy);
			if(wdb != NULL)
			{
				struct userobject* uobj = (struct userobject*)obj;
				if(uobj->ud.joblv >= wdb->joblv && uobj->ud.level >= wdb->lv)
				{
					switch(wdb->type){
					case 0:
						{
							struct location* loc = (struct location*)wdb->data;
							object_warp(obj,loc->map,loc->x,loc->y);
							if(loc->side < 4) object_side(obj,loc->side );
						}
						break;
					case 1:
						{
							struct wmapdb* wmap = (struct wmapdb*)wdb->data;
							world_map(obj2sd(obj),wmap->epf,wmap->id);
						}
						break;
					case 2:
						{
							shell_event(ON_LOCATION,"i",obj->id);
						}
						break;
					}
					return 1;
				}
				else
				{
					map_send_xy(obj2sd(obj),&obj->loc);
					game_send_message(obj2sd(obj),3,"감히 접근할 수 없습니다.");
					return 1;
				}
			}
		} else if(loc & 1)
		{
			elem = hash_get(&obj->loc.map->loc_ht,&obj->loc);
			if ( elem != NULL ){
				struct loc_script *ls = list_entry(elem,struct loc_script,hash_elem);
				shell_event(ON_LOCATION,"ii",obj->id,ls->id);
				elem = hash_next(&obj->loc.map->loc_ht,&obj->loc,elem);
			}
		}
	}
	if( obj->loc.map->db->xs <= sx || obj->loc.map->db->ys <= sy || sx < 0 || sy < 0 || BLOCK(obj->loc.map,sx,sy))
	{
		if(obj->type == USER) 
		{
			map_send_xy(obj2sd(obj),&obj->loc);
		}
		return 1;
	}
	BLOCK(obj->loc.map,bx,by)--;
	BLOCK(obj->loc.map,sx,sy)++;
	pos_del(obj);
	obj->loc.x = sx;
	obj->loc.y = sy;
	pos_add(obj);
	if(obj->type == USER) {
		elem = list_begin(&obj->loc.map->effects);
		while(elem != &obj->loc.map->effects.tail)
		{
			struct effectdb* eff = list_entry(elem,struct effectdb,elem);
			byte bsight;
			byte nsight;
			BSocket* sd = obj2sd(obj);
			bsight = check_sightxy(obj->loc.map,bx,by,eff->loc.x,eff->loc.y);
			nsight = check_sightxy(obj->loc.map,sx,sy,eff->loc.x,eff->loc.y);
			if(!bsight && nsight)
			{
				magic_effect_xy_single(sd,eff->loc.x,eff->loc.y,eff->effect,0xFFF);
			}
			elem = list_next(elem);
		}
	}
	od = object_first(obj->loc.map);
	while(od != NULL)
	{
		if(od == obj) {
			od = object_next(od);
			continue;
		}
		if(obj->type == USER)
		{
			byte bsight;
			byte nsight;
			BSocket* sd = obj2sd(obj);
			bsight = check_sightxy(obj->loc.map,bx,by,od->loc.x,od->loc.y);
			nsight = check_sightxy(obj->loc.map,sx,sy,od->loc.x,od->loc.y);
			if(bsight && !nsight)
				object_hide_single(sd,od);
			else if(!bsight && nsight)
				object_show_single(sd,od);
		}
		if(od->type == USER)
		{
			byte bsight;
			byte nsight;
			BSocket* sd = obj2sd(od);
			bsight = check_sightxy(od->loc.map,od->loc.x,od->loc.y,bx,by);
			nsight = check_sightxy(od->loc.map,od->loc.x,od->loc.y,sx,sy);
			if(bsight && !nsight)
				object_hide_single(sd,obj);
			else if(!bsight && nsight)
				object_show_single(sd,obj);
			else
				object_walk_single(sd,obj,bx,by);
		}
		od = object_next(od);
	}
	return 0;
}
void object_side(struct mapobject* obj,int side)
{
	byte packet[11];
	WB(packet,0) = 170;
	WS(packet,1)= SWAP16(8);
	WB(packet,3) = 17;
	WB(packet,4) = 0;
	WL(packet,5) = SWAP32(obj->id);
	WB(packet,9) = side & 0xFF;
	WB(packet,10) = 0;
	obj->loc.side = side  & 0xFF;
	write_sight(&obj->loc,packet);
}