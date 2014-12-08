
#include "hashtable.h"
#include "map.h"
#include "game.h"
#include "mem.h"
#include<string.h>
#include "mob.h"
#include <stdio.h>
#include "socket.h"
#include "debug.h"
#include <stdlib.h>
#define MAP_HASH_VALUE 103
struct map* maps[MAP_MAX] = {0};
extern unsigned int game_option;
#define WARP_HASH_VALUE 13
struct mapdb* mapdbs[MAP_DB_MAX] = {0};
struct epfdb* epfdbs[EPF_DB_MAX] = {0};
#define SWAP16(n) (unsigned short)(((n)<<8)|((n)>>8))
#define SWAP32(n) (unsigned long)(((n)<<24)|(((n)<<8)&0x00FF0000)|(((n)>>8)&0x0000FF00)|((n)>>24))
#define UNSWAP16(p) ((*(unsigned char*)((p)+1))|(*(unsigned char*)(p)<<8))
#define UNSWAP32(p) ((*(unsigned char*)((p)+3))|(*(unsigned char*)((p)+2)<<8)|(*(unsigned char*)((p)+1)<<16)|(*(unsigned char*)(p)<<24))
void socket_write_map(struct map* map,char* buffer,unsigned short len)
{
	BSocket* current = socket_first(map);
	while(current != NULL)
	{
		socket_write(current,buffer,len);
		current = socket_next(current);
	}
}
void socket_write_sight(struct location* loc,int reverse,char* buffer,unsigned short len)
{
	BSocket* current = socket_first(loc->map);
	while(current != NULL)
	{
		if(reverse && check_sight(&current->obj.obj.loc,loc))
		{
			socket_write(current,buffer,len);
		}
		else if(check_sight(loc,&current->obj.obj.loc))
		{
			socket_write(current,buffer,len);
		}
		current = socket_next(current);
	}
}
void socket_write_sight2(struct mapobject* obj,int reverse,char* buffer,unsigned short len)
{
	BSocket* current = socket_first(obj->loc.map);
	while(current != NULL)
	{
		if(current->obj.obj.id == obj->id){
			current = socket_next(current);
			continue;
		}
		if(reverse && check_sight(&current->obj.obj.loc,&obj->loc))
		{
			socket_write(current,buffer,len);
		}
		else if(check_sight(&obj->loc,&current->obj.obj.loc))
		{
			socket_write(current,buffer,len);
		}
		current = socket_next(current);
	}
}

int loc_hash(struct location* loc)
{
	int hc = 0;
	hc = (loc->map->id & 0x7F) << 24;
	hc |= (loc->x & 0xFF) << 16;
	hc |= (loc->y & 0xFF) << 8;
	hc |= (loc->x + loc->y) & 0xFF;
	return hc;
}
int loc_hf(struct hashtable* ht,void* d){
	return loc_hash((struct location*)d) % WARP_HASH_VALUE;
}
int warp_hc(void* d,struct _list_elem* el)
{
	struct location* loc = (struct location*)d;
	struct warpdb* wdb = list_entry(el,struct warpdb,hash_elem);
	return !(loc->map == wdb->loc1.map && loc->x == wdb->loc1.x && loc->y == wdb->loc1.y);
}
int loc_hc(void* d,struct _list_elem* el)
{
	struct location* loc = (struct location*)d;
	struct loc_script* lc = list_entry(el,struct loc_script,hash_elem);
	return !(loc->map == lc->loc.map && loc->x == lc->loc.x && loc->y == lc->loc.y);
}
void warp_init()
{

}
void map_send_info(BSocket* sd,struct map* m)
{
	int size=14;
	sdsize(sd,m->name_len + 15);
	wb(sd,0) = 170;
	wb(sd,3) = 21;
	wb(sd,4) = 0;
	ws(sd,5) = SWAP16(m->id);
	ws(sd,7) = SWAP16(m->db->xs);
	ws(sd,9) = SWAP16(m->db->ys);
	wb(sd,11) = 4;
	wb(sd,12) = 0;

	wb(sd,13) = m->name_len;
	memmove(sd->sndBuffer+sd->sndOffset+14,m->name,wb(sd,13));
	size += wb(sd,13);
	wb(sd,size++) = 0;
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
int map_x(struct map*,int);
int map_y(struct map*,int);
void map_wakeup(struct map* m)
{
	struct _list_elem* elem = list_begin(&m->objects);
	while(elem != &m->objects.tail)
	{
		struct mobobject* obj = list_entry(elem,struct mobobject,obj.elem);
		if(obj->obj.type == MONSTER && obj->aitimer.state == t_sleep){
			timer_restart(&obj->aitimer);
		}
		elem = list_next(elem);
	}
}
void map_add(struct mapobject* obj){
	if(obj->type == USER) {
		_list_add(&obj->loc.map->users,&((struct userobject*)obj)->user_elem);
		if(obj->loc.map->user_count == 0)
		{
			map_wakeup(obj->loc.map);
		}

		obj->loc.map->user_count++;
		
	}
	if(obj->type != ITEM) BLOCK(obj->loc.map,obj->loc.x,obj->loc.y)++;
	object_show_sight(obj);
	pos_add(obj);
	_list_add(&obj->loc.map->objects,&obj->elem);
}
void map_delete(struct mapobject* obj)
{
	if(obj->type != ITEM) BLOCK(obj->loc.map,obj->loc.x,obj->loc.y)--;
	if(obj->type == USER) {
		_list_remove(&((struct userobject*)obj)->user_elem);
		obj->loc.map->user_count--;
	}
	object_hide_sight(obj);
	_list_remove(&obj->elem);
	pos_del(obj);
}
void map_msg(struct map* m,char type,char* msg)
{
	
	byte packet[300];
	unsigned short slen = strlen(msg);\
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+6);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = type;
	WB(packet,6) = SWAP16(slen);
	memmove(packet+8,msg,slen);
	WB(packet,slen+8) = 0;
	write_map(m,packet);
}
void map_msg2(struct map* m,int fc,int bc,char* msg,int timeout)
{
	
	byte packet[300];
	unsigned short slen = strlen(msg);
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+8);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = 0x12;
	WB(packet,6) = fc;
	WB(packet,7) = bc;
	WB(packet,8) = timeout;
	WB(packet,9) = (slen);
	memmove(packet+10,msg,slen);
	WB(packet,slen+10) = 0;
	write_map(m,packet);
}
void map_send(BSocket* sd,struct map* m,unsigned short x0,unsigned short y0,int x1,int y1)
{
	int i,j;
	int xs,ys,pos,indx;
	int size;
	if(x0>= m->db->xs) return;
	if(y0>=m->db->ys) return;
	sdsize(sd,y1*x1*6 + 30);
	wb(sd,0) = 170;
	wb(sd,3) = 6;
	wb(sd,4) = 0;
	wb(sd,5) = 4;
	wb(sd,6) = 0;
	ws(sd,7) = SWAP16(x0);
	ws(sd,9) = SWAP16(y0);
	wb(sd,11) = x1 & 0xFF;
	wb(sd,12) = y1 & 0xFF;
	size = 13;
	pos = (y1-y0)*(x1-x0)*6;
	for(i=0;i<y1;i++)
	{
		if(y0 + i >= m->db->ys) break;
		for(j=0;j<x1;j++)
		{
			if(x0 + j >= m->db->xs) break;
			indx = (y0+i)*m->db->xs+(j+x0);
			ws(sd,size) = SWAP16(m->db->tiles[indx].tile);
			size+=2;
			ws(sd,size) = SWAP16(m->db->block[indx]);
			size+=2;
			ws(sd,size) = SWAP16(m->db->tiles[indx].object);
			size+=2;
		}
	}
	wb(sd,size++) = 0;
	ws(sd,1) = SWAP16(size-3);
	if(size>12) cpt(sd);
}
void map_send_xy(BSocket* sd,struct location* po)
{
	int x0,y0;
	x0 = map_x(po->map,po->x);
	y0 = map_y(po->map,po->y);
	sdsize(sd,14);
	wb(sd,0) = 170;
	wb(sd,1) = 0;
	wb(sd,2) = 11;
	wb(sd,3) = 4;
	wb(sd,4) = 0;
	ws(sd,5) = SWAP16(po->x);
	ws(sd,7) = SWAP16(po->y);
	ws(sd,9) =  SWAP16(x0);
	ws(sd,11) = SWAP16(y0);
	wb(sd,13) = 0;
	cpt(sd);
}
void map_send_bgm(BSocket* sd,int bgm)
{
	sdsize(sd,19);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(16);
	wb(sd,3) = 25;
	wb(sd,4) = 0;
	wb(sd,5) = 1;
	wb(sd,6) = 5;
	ws(sd,7) = SWAP16(bgm);
	ws(sd,9) = SWAP16(bgm);
	wb(sd,11) = 100;
	ws(sd,12) = SWAP16(512);
	ws(sd,14) = SWAP16(512);
	ws(sd,16) = SWAP16(0);
	wb(sd,18) = 0;
	cpt(sd);
}
void map_del(int id)
{
	struct map* m = maps[id];
	if(m != NULL)
	{
		_free(m->block);
		_free(m);
		maps[id] = NULL;
	}
}
void map_effect_xy(struct location* loc,unsigned short effect,int time)
{
	
	byte packet[20];
	
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(16);
	WB(packet,3) = 41;
	WB(packet,4) = 0;
	WB(packet,5) = 0;
	WL(packet,6) = SWAP32(0);
	WS(packet,10) = SWAP16(effect);
	WS(packet,12) = SWAP16(time & 0xFFFF);
	WS(packet,14) = SWAP16(loc->x);
	WS(packet,16) = SWAP16(loc->y);
	write_sight(loc,packet);
}
int maxx = 0 ,maxy = 0,blockmax = 0;
void warp_add(struct map* m,int x,int y,struct map* m2,int x2,int y2,int side,int lv,int joblv)
{
	struct warpdb* wdb = MALOC(struct warpdb,1);
	struct location* loc = MALOC(struct location,1);
	warp_del(m,x,y);
	wdb->type = 0;
	wdb->loc1.map = m;
	ONLOC_SET(m,x,y,2);
	wdb->loc1.x = x & 0xFFFF; wdb->loc1.y = y & 0xFFFF ;
	loc->map = m2;
	loc->x = x2 & 0xFFFF; loc->y = y2 & 0xFFFF;
	loc->side = side & 0xff;
	wdb->data = loc;
	wdb->lv = lv & 0xff;
	wdb->joblv = joblv & 0xff;
	hash_insert(&m->warp_ht,&wdb->loc1,&wdb->hash_elem);
}
struct warpdb* getwarp(struct map* m,int x,int y)
{
	struct location loc;
	struct warpdb* wdb;
	struct _list_elem* elem;
	loc.map = m; loc.x = x;loc.y = y;
	elem = hash_get(&m->warp_ht,&loc);
	if(elem == NULL) return NULL;
	else return list_entry(elem,struct warpdb,hash_elem);
}
void warp_del(struct map* m,int x,int y)
{
	struct warpdb* wdb = getwarp(m,x,y);
	if(wdb == NULL) return ;
	_list_remove(&wdb->hash_elem);
	if(wdb->type == 0) _free(wdb->data);
	free(wdb);
}
void effect_del(struct map* m,int x,int y,unsigned short effect)
{
	struct _list_elem* elem = list_begin(&m->effects);
	while(elem != &m->effects.tail)
	{
		struct effectdb* eff = list_entry(elem,struct effectdb,elem);
		if(eff->effect == effect && eff->loc.x == x && eff->loc.y == y)
		{
			elem = _list_remove(elem);
			break;
		}
		else elem = list_next(elem);
	}
}
void effects_show(BSocket* sd)
{
	struct _list* l = &sd->obj.obj.loc.map->effects;
	struct _list_elem* elem = list_begin(l);
	while(elem != &l->tail)
	{
		struct effectdb* eff = list_entry(elem,struct effectdb,elem);
		if(check_sight(&sd->obj.obj.loc,&eff->loc))
		{
			magic_effect_xy_single(sd,eff->loc.x,eff->loc.y,eff->effect,0xFFF);
		}
		elem = list_next(elem);
	}
}

void effect_add(struct map* m,int x,int y,unsigned short effe)
{
	struct effectdb* eff = MALOC(struct effectdb,1);
	eff->loc.x = x;
	eff->loc.y = y;
	eff->loc.map = m;
	eff->effect = effe;
	
	_list_add(&m->effects,&eff->elem);
	magic_effect_loc(&eff->loc,effe,0xFFFF);
}
void map_unset(int id)
{
	struct map* m;
	if(id < 0 || id >= MAP_MAX || (m = maps[id]) == NULL) return;

	if(m->user_count > 0){
		return;
	}
	
}
struct map* map_set(int id,int code)
{
	struct map* m;
	int i;
	if(mapdbs[code] == NULL) return NULL;
	m = MALOC(struct map,1);
	m->id = id;
	m->user_count = 0;
	m->db = mapdbs[code];
	list_init(&m->users);
	strcpy(m->name,mapdbs[code]->real_name);
	m->name_len = mapdbs[code]->real_name_len;
	m->block = MALOC(unsigned short,m->db->xs * m->db->ys);
	m->pos = MALOC(struct _list,m->db->xs*m->db->ys);
	m->onloc = MALOC(unsigned char,m->db->xs  * m->db->ys);
	list_init(&m->effects);
	for(i=0;i<m->db->xs*m->db->ys;i++){
		m->block[i] = m->db->block[i];
		list_init(&m->pos[i]);
		m->onloc[i] = 0;
	}
	hash_init(&m->warp_ht,WARP_HASH_VALUE,loc_hf,warp_hc);
	hash_init(&m->loc_ht,WARP_HASH_VALUE,loc_hf,loc_hc);

	list_init(&m->objects);
	maps[m->id] = m;
	return m;
}

void map_sound(struct map* m,int x,int y,int sound)
{
	struct location loc;
	byte packet[24];
	loc.map = m;
	loc.x = x;
	loc.y = y;
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(20);
	WB(packet,3) = 25;
	WB(packet, 4) = 0;
	WB(packet, 5) = 0; //Sound Type
	WB(packet, 6) = 3;
	WS(packet, 7) = SWAP16(sound);
	WB(packet, 9) = (100); //volume
	WS(packet, 10) = SWAP16(4);
	WL(packet, 12) = SWAP32(1);
	WS(packet, 16) = SWAP16(256);
	WS(packet, 18) = SWAP16(514);
	WS(packet, 20) = SWAP16(4);
	WB(packet, 22) = 0;
	write_sight(&loc,packet);
}
int load_map(int code,char* mapfile,char* blockfile,char* name,unsigned short bgm,unsigned short effect)
{
	struct mapdb* m;
	FILE* fm;
	FILE* fb;
	char data[4];
	int i;
	fm = fopen(mapfile,"rb");
	if(fm == NULL)
	{
		warnf("%s 파일을 찾을 수 없습니다\n",mapfile);
		return 1;
	}
	if(fread(data,4,1,fm) != 1)
	{
		fclose(fm);
		warnf("%s 파일이 손상되었습니다\n",mapfile);
		return 1;
	}
	if(mapdbs[code] != NULL)
	{
		_free(mapdbs[code]->block);
		_free(mapdbs[code]->tiles);
		m = mapdbs[code];
	} else {
		m = MALOC(struct mapdb,1);
		mapdbs[code] = m;
	}
	m->xs = UNSWAP16(data);
	m->ys = UNSWAP16(data+2);
	
	if(m->xs* m->ys >= 0xFFFFFF)
	{
		fclose(fm);
		warnf("%s 파일이 손상되었거나 너무 큽니다\n",mapfile);
		return 1;
	}
	m->code = code;
	m->bgm = bgm;
	m->effect = effect;
	m->real_name_len = strlen(name);
	strncpy(m->real_name,name,MAPNAME_MAX);
	m->tiles = MALOC(struct maptile,m->xs *  m->ys);
	m->block = CALOC(unsigned char,m->xs*m->ys);
	for(i=0;i<m->xs*m->ys;i++)
	{
		if(feof(fm)) {
			_free(m->tiles);
			_free(m->block);
			_free(m);
			mapdbs[code] = NULL;
			warnf("%s 파일이 손상되었습니다.\n",mapfile);
			return 1;
		}
		else {
			fread(data,4,1,fm);
			m->tiles[i].tile = UNSWAP16(data);
			m->tiles[i].object = UNSWAP16(data+2);
		}
	}
	fclose(fm);
	fb = fopen(blockfile,"r");
	if(fb == NULL) {
		warnf("%s 파일을 찾을 수 없습니다\n",blockfile);
		return 2;
	}
	while(!feof(fb))
	{
		int j;
		char buf[1001];
		char* temp;
		fgets(buf,1001,fb);
		temp = buf;
		while( *temp=='\n' || *temp=='\r' || *temp==' ' || *temp=='\t' || *temp == ',') temp++;
		if(*temp==';' || *temp == '/' || *temp == '-' || *temp == 0) continue;
		if(sscanf(temp,"%d,%d",&i,&j) != 2)
			continue;
		if(j*m->xs+i >= m->xs*m->ys) {
			continue;
		}
		m->block[j*m->xs+i] = 1;
	}
	fclose(fb);
	return 0;
}

void lscript_set(int id,struct location* loc)
{
	struct loc_script* ls;
	ls->id = id;
	ls = MALOC(struct loc_script,1);
	ls->loc.map = loc->map;
	ls->loc.x = loc->x & 0xFFFF; ls->loc.y = loc->y & 0xFFFF ;
	ONLOC_SET(loc->map,loc->x,loc->y,1);
	hash_insert(&loc->map->loc_ht,&ls->loc,&ls->hash_elem);
}
void wmap_set(struct epfdb* epf,int id,struct location* loc,int minlv,int joblv)
{
	int i;
	struct wmapdb* wm = NULL;
	struct warpdb* wdb;
	for(i=0;i<epf->group_count;i++)
	{
		struct wmapdb* wmap = list_entry(list_begin(&epf->groups[i].wlist),struct wmapdb,elem);
		while(&wmap->elem != &epf->groups[i].wlist.tail)
		{
			if(wmap->id == id){
				wm = wmap;
				break;
			}
			wmap = list_entry(list_next(&wmap->elem),struct wmapdb,elem);
		}
	}
	if(wm == NULL) return;
	wdb = MALOC(struct warpdb,1);
	warp_del(loc->map,loc->x,loc->y);
	wdb->type = 1;
	wdb->loc1.map = loc->map;
	BLOCK(loc->map,loc->x,loc->y) = 0xD000;
	wdb->loc1.x = loc->x & 0xFFFF; wdb->loc1.y = loc->y & 0xFFFF ;
	wdb->data = wm;
	wdb->lv = minlv & 0xff;
	wdb->joblv = joblv & 0xff;
	hash_insert(&loc->map->warp_ht,&wdb->loc1,&wdb->hash_elem);
}
void register_wmapdb(struct epfdb* epf,int group,int code,int x,int y,char* name,struct location* loc)
{
	struct wmapdb* wmap = MALOC(struct wmapdb,1);
	wmap->epf = epf;
	wmap->id = code;
	wmap->loc.x = loc->x;
	wmap->loc.y = loc->y;
	wmap->loc.map = loc->map;
	wmap->loc.side = loc->side;
	wmap->wmap_x = x;
	wmap->wmap_y = y;
	strncpy(wmap->name,name,WMAP_MAX);
	wmap->name_len = strlen(wmap->name);
	epf->wmap_count++;
	_list_add(&epf->groups[group].wlist,&wmap->elem);
}
void register_epfdb(char* name,int code,int gcnt)
{
	struct epfdb* epf;
	int i;
	if(epfdbs[code] == NULL) epf = MALOC(struct epfdb,1);
	else epf = epfdbs[code];
	strncpy(epf->name,name,WMAP_MAX);
	epf->code = code;
	epf->name_len = strlen(epf->name);
	epf->group_count = gcnt;
	epf->groups = CALOC(struct wmap_groupdb,gcnt);
	for(i=0;i<gcnt;i++)
	{
		epf->groups[i].index;
		list_init(&epf->groups[i].wlist);
	}
	epf->wmap_count = 0;
	epfdbs[code] = epf;
}
void world_map(BSocket* sd,struct epfdb* epf,int code)
{
	int len,sz,i,pos,pos2,cnt=0,p,p2;
	struct wmapdb* wmap,*wmap2;
	struct wmap_groupdb* group=NULL;
	sd->epf = epf;
	if(sd->obj.obj.loc.map){
		map_delete(&sd->obj.obj);
		object_hide_sight(&sd->obj.obj);
		sd->obj.ud.m = sd->obj.obj.loc.map;
		sd->obj.ud.x = sd->obj.obj.loc.x;
		sd->obj.ud.y = sd->obj.obj.loc.y;
		sd->obj.obj.loc.map = NULL;
	}
	wb(sd,0) = 170;
	wb(sd,3) = 46;
	wb(sd,4)= 0;
	len=wb(sd,5) = epf->name_len;
	memmove(wp(sd,6),epf->name,len);
	sz=6+len;
	wb(sd,sz++) = epf->wmap_count;
	pos = sz++;
	p=0;
	for(i=0;i<epf->group_count;i++)
	{
		p2=p;
		wmap = list_entry(list_begin(&epf->groups[i].wlist),struct wmapdb,elem);
		while(&wmap->elem != &epf->groups[i].wlist.tail)
		{
			if(wmap->id == code)
			{
				wb(sd,pos) = code;
				group = &epf->groups[i];
			}
			ws(sd,sz) = SWAP16(wmap->wmap_x); sz+=2;
			ws(sd,sz) = SWAP16(wmap->wmap_y); sz+=2;
			wb(sd,sz++) = wmap->name_len;
			memmove(wp(sd,sz),wmap->name,wmap->name_len);
			sz += wmap->name_len;
			ws(sd,sz) = 0 ; sz+=2;// locked
			ws(sd,sz) = SWAP16(i);sz+=2;
			ws(sd,sz) = SWAP16(wmap->id); sz+=2;
			ws(sd,sz) = SWAP16(0);sz+=2;
			cnt=0;
			pos2=sz;
			sz+=2;
			wmap2 = list_entry(list_begin(&epf->groups[i].wlist),struct wmapdb,elem);
			while(&wmap2->elem != &epf->groups[i].wlist.tail)
			{
				ws(sd,sz) = SWAP16(p2+cnt);
				sz+=2;
				cnt++;
				wmap2 = list_entry(list_next(&wmap2->elem),struct wmapdb,elem);
			}
			ws(sd,pos2)= SWAP16(cnt);
			
		//	ws(sd,sz)= SWAP16(0); sz+=2;
			p++;
			wmap = list_entry(list_next(&wmap->elem),struct wmapdb,elem);
		}
	}
	if(group==NULL)
	{
		sd->eof=1;
		return ;
	}

	wb(sd,sz++) = 0;
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);
}