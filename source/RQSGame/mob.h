#pragma once
#include "object.h"
#include "timer.h"
#define MOB_DB_MAX 10000
#define MOB_NAME_MAX 30
enum mobai
{
	ai_naive = 2,
	ai_aggressive = 4,
	ai_stop = 1,
	ai_avoid = 8, 
	ai_script = 16,
};
struct mobdb
{
	unsigned int id;
	unsigned int image;
	unsigned char color;
	long speed;
	unsigned char aitype;
	unsigned char name_len;
	char name[MOB_NAME_MAX+1];
};
struct mobobject
{
	struct mapobject obj;
	struct mobdb* db;
	struct timer aitimer;
	struct mapobject* target;
	unsigned char last_gage;
};
extern struct mobdb* mobs[MOB_DB_MAX];
struct mobobject* create_mobobj(int code);
void mob_die(struct mobobject* obj);
void mobobject_new(struct mobobject*,int code);

void register_mobdb(int code,char* name,unsigned int img,unsigned char col,unsigned char aitype,long speed);