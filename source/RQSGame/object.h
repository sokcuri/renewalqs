#pragma once
#include "list.h"
#define OBJECT_MAGIC_NUMBER 0x3815EF88
struct map;
enum SIDE_TYPE
{
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};
struct location
{
	struct map* map;
	unsigned short x,y;
	unsigned char side;
};
enum OBJECT_STATUS
{
	live = 0,
	dead = 1,
	halftransparent=2,
	horse=3,
	disguise=4,
	hide=5,
	transparent=6,
	nontarget=255,
};
typedef enum OBJ_TYPE
{
	NONE = 0,
	USER = 1,
	MONSTER = 2,
	ITEM = 4,
	NPC = 8,
	PET = 16,
	OBJLIVE = USER | MONSTER | NPC | PET,
	OBJALL = USER | MONSTER | NPC | PET | ITEM,
}objType;
struct object_event;
struct mapobject
{
	unsigned int magic;
	objType type;
	char showtype;
	unsigned char state;
	unsigned int id;
	unsigned int image;
	unsigned char color;
	struct location loc;
	struct _list_elem elem;
	struct _list_elem pos_elem;
	struct object_event* events;
};
int object_avoid(struct mapobject* obj,struct mapobject* target);
void object_damage(struct mapobject* obj,struct mapobject* target,int dam);
struct object_event
{
	void (*del)(struct mapobject*);
	void (*dam)(struct mapobject*,struct mapobject*,int);
};
void object_action(struct mapobject* obj,int type,int time,int sound);
int object_follow(struct mapobject* obj,struct mapobject* target);
void object_side(struct mapobject* obj,int side);
void object_gage(struct mapobject* obj,int percent,int critical,unsigned long dam);
void object_del(struct mapobject* obj);
void object_hide_sight(struct mapobject* obj);
unsigned int object_new(struct mapobject* obj,int ev);
void object_init();
struct mapobject* getfrontobj(struct mapobject* obj,int tp,int (*comp)(struct mapobject*,struct mapobject*));
int check_sight(struct location* po1,struct location* po2);
int can_damage(struct mapobject* obj,struct mapobject* target);
char* object_get_name(struct mapobject* obj,int* len);
struct mapobject* object_first(struct map* md);
struct mapobject* object_last(struct map* md);
void object_healhp(struct mapobject* obj,unsigned int hp);
#define getobj(id) (id >= objects_size ? NULL : objects[id])
struct mapobject* getnearobj(struct location* loc,int type);
void object_effect(struct mapobject* obj,unsigned short effect, int time);
int object_walk(struct mapobject* obj);
struct mapobject* object_next(struct mapobject* obj);
void object_say(struct mapobject* obj,int type,char* message);
struct mapobject* object_prev(struct mapobject* obj);
void object_sound(struct mapobject* obj,int sound);
#define OBJ_CHECK(obj) (obj->magic == OBJECT_MAGIC_NUMBER)
void object_warp(struct mapobject* obj,struct map* m,int x,int y);
int check_sightxy(struct map* map,int x,int y,int ox,int oy);
extern unsigned int objects_size;
extern unsigned int objects_last_id;
extern struct mapobject** objects;
