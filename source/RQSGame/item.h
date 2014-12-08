#pragma once
#include "object.h"
#define ITEM_NAME_MAX 30
#define EQUIP_SLOT_MAX 14
struct equipdb
{
	unsigned short image;
	unsigned char option;
	unsigned char color[3];
	unsigned short sound_attack;
};
enum msg_item
{
	disappear = 0, // 없어졋습니다
	dump = 1, // 버렷습니다.
	eat = 2, // 먹었습니다.
	what=3, // 다피웠습니다.
	threw = 4, // 던졋습니다.
	shot =5, // 쐇습니다
	allused = 6, // 다썼습니다.
	attached= 7, // 붙였습니다.
	rotted = 8, // 썩어 없어졌습니다.
	give = 9,// 주었습니다.
	sell = 10, // 팔았습니다.
	nomessage = 12, // 메세지 X
	broken = 13, // 깨졌습니다.
};
enum itype
{
	i_use=15,
	i_booty=16,
	i_money=19,
	i_weapon=0,
	i_armor=1,
	i_shield=2,
	i_helmet=3,
	i_ring=4, // 
	i_sub=5,
	i_ring1=4,
	i_ring2=5,
	i_sub1=6,
	i_sub2=7,
	i_faceacc=8,
	i_tara=9,
	i_cape=10,
	i_outwear=11,
	i_shoes=12,
	i_necklace=13,
};
struct itemdb
{
	unsigned int id;
	unsigned char itype;
	//unsigned char overlap;
	char name[ITEM_NAME_MAX+1];
	unsigned char name_len;
	unsigned short image;
	char* format;
	unsigned char color;
	unsigned int max_count;
	struct equipdb equip;
	unsigned char option;
};
#define ITEM_SLOT_MAX 26
struct itemobject
{
	struct mapobject obj;
	struct itemdb* db;
	char name[ITEM_NAME_MAX];
	unsigned char name_len;
	unsigned char slot;
	unsigned char item_msg;
	unsigned int count;
	struct userobject* owner;
};
enum give_state
{
	GIVE_FAIL = -1, //
	GIVE_NEWSLOT = 0,
	GIVE_REMAIN = 1,
	GIVE_MIXXED = 2,
};
enum item_option
{
	notdrop = 1, //드랍,던지기 불가 
	overlap = 2, // 중복 가능
	notdeposit = 4,
	notrade = 8,
	ipack = 16, // 패키지 ( 동동주 류)
};
void item_slot_swap(struct userobject* uobj,unsigned char slot1,unsigned char slot2);
int item_give(struct userobject* uobj,struct itemobject* iobj,int);

int can_item_get(struct userobject* uobj,struct itemobject* iobj);
void drop_money(struct location* loc,unsigned int money);
void equip_takeon(struct userobject* obj,int slot);
int item_drop_slot(struct userobject* uobj,int slot,int count);
int item_give2(struct userobject* uobj,int id,int count,int);

int equip_takeoff(struct userobject* obj,int slot);
void item_init();
unsigned int itemobject_new(struct itemobject* obj,int id);

int item_seticount(struct itemobject* obj,int count);
#define ITEM_MAX 60000
#define MONEY_ITEM_MAX 30
extern struct itemdb* items[ITEM_MAX + MONEY_ITEM_MAX];

void register_itemdb(int code,unsigned char itype,unsigned char overlap,char* name,char* format,unsigned short image,unsigned char col,unsigned int max_count,unsigned char opt);
void register_equipdb(int code,unsigned short image,unsigned char ecol1,unsigned char ecol2,unsigned char ecol3,unsigned char opt,unsigned short atk);

struct itemobject* create_itemobj(int code);