#pragma once
#include "list.h"
#include "socket.h"
#include "timer.h"
struct maptile
{
	unsigned short tile;
	unsigned short object;
};
#define MAPNAME_MAX 30
struct mapdb
{
	struct maptile* tiles;
	unsigned char* block;
	unsigned short xs,ys;
	unsigned short bgm;
	unsigned short effect;
	int type;
	int code;
	char real_name[MAPNAME_MAX+1];
	unsigned char real_name_len;
};
struct effectdb
{
	unsigned short effect;
	struct location loc;
	struct _list_elem elem;
};
struct map
{
	int id;
	struct mapdb* db;
	unsigned short* block;
	unsigned char* onloc;
	char name[MAPNAME_MAX+1];
	unsigned char name_len;
	struct _list objects;
	struct _list users;
	int user_count;
	struct _list * pos;
	struct _list effects;
	struct hashtable warp_ht;
	struct hashtable loc_ht;
};
struct warpdb
{
	int type;
	struct location loc1;
	void* data;
	unsigned char lv;
	unsigned char joblv;
	struct _list_elem hash_elem;
};
#define WMAP_MAX 30
struct loc_script
{
	int id;
	struct location loc;
	struct _list_elem hash_elem;
};
struct wmap_groupdb
{
	int index;
	struct _list wlist;
};
struct epfdb
{
	int code;
	struct wmap_groupdb* groups;
	int group_count;
	int wmap_count;
	char name[WMAP_MAX+1];
	unsigned char name_len;
};
struct wmapdb
{
	struct location loc;
	unsigned short id;
	char name[WMAP_MAX + 1];
	unsigned char name_len;
	int wmap_x,wmap_y;
	struct epfdb* epf;
	struct _list_elem elem;
};
#define BLOCK(m,i,j) (m->block[(j)*m->db->xs + i])
#define ONLOC_SET(m,i,j,s) (m->onloc[(j)*m->db->xs + i] |= (s))
#define ONLOC_UNSET(m,i,j,s) (m->onloc[(j)*m->db->xs + i] &= ~(s))
void socket_write_sight(struct location* loc,int reverse,char* buffer,unsigned short len);
void warp_add(struct map* m,int x,int y,struct map* m2,int x2,int y2,int side,int lv,int joblv);
void warp_del(struct map* m,int x,int y);
void effects_show(BSocket* sd);
void effect_add(struct map* m,int x,int y,unsigned short effect);
void effect_del(struct map* m,int x,int y,unsigned short eff);
struct warpdb* getwarp(struct map* m,int x,int y);
void warp_init();

void world_map(BSocket* sd,struct epfdb* epf,int code);
void map_effect_xy(struct location* loc,unsigned short effect,int time);
void map_send_info(BSocket*,struct map*);
void map_send_bgm(BSocket*,int);
void map_send_xy(BSocket* sd,struct location* po);
void map_del(int id);

void map_sound(struct map* m,int x,int y,int sound);
void register_epfdb(char* name,int code,int gcnt);

void lscript_set(int id,struct location* loc);
void wmap_set(struct epfdb* epf,int id,struct location* loc,int minlv,int joblv);
void register_wmapdb(struct epfdb* epf,int group,int code,int x,int y,char* name,struct location* loc);
void world_map(BSocket* sd,struct epfdb* epf,int code);
void map_delete(struct mapobject* obj);
void map_send(BSocket* sd,struct map* m,unsigned short x0,unsigned short y0,int x1,int y1);
#define pos_list(obj) (pos_list_xy(obj->loc.map,obj->loc.x,obj->loc.y))
#define pos_list_xy(m,x,y) (&m->pos[m->db->xs*(y) + (x)])
#define MAP_MAX 4000
extern struct map* maps[MAP_MAX];
#define MAP_DB_MAX 2000
extern struct mapdb* mapdbs[MAP_DB_MAX];
#define EPF_DB_MAX 256
extern struct epfdb* epfdbs[EPF_DB_MAX];
struct map* map_set(int id,int code);
int load_map(int code,char* mapfile,char* blockfile,char* name,unsigned short bgm,unsigned short effect);
#define write_sight(loc,p) (CPT(p),socket_write_sight(loc,1,(char*)p,(p[1]<<16 | p[2])+3))
#define write_sight2(obj,p) (CPT(p),socket_write_sight2(obj,1,(char*)p,(p[1]<<16 | p[2])+3))
#define write_map(m,p) ((CPT(p),socket_write_map(m,(char*)p,(p[1]<<16 | p[2])+3)))
#define pos_add(obj) _list_add(&obj->loc.map->pos[obj->loc.y * obj->loc.map->db->xs + obj->loc.x],&obj->pos_elem)
#define pos_del(obj) _list_remove(&obj->pos_elem)
void socket_write_map(struct map* map,char* buffer,unsigned short len);
void map_add(struct mapobject* obj);
void map_msg(struct map* m,char type,char* msg);
void map_msg2(struct map* m,int fc,int bc,char* msg,int timeout);