#pragma once
#include "login.h"

#define TITLE_MAX 32
#define JOB_NAME_MAX 32
struct job
{
	char name[JOB_NAME_MAX];
	unsigned char name_len;
};
struct map;
struct userdata
{
	char id[ID_MAX+1];
	unsigned char id_len;
	char title[TITLE_MAX + 1];
	unsigned char title_len;
	unsigned char state;
	char ac;
	unsigned char job,joblv;
	unsigned char hair,hair_color;
	unsigned short face;
	unsigned char gender;
	unsigned char hit,dam, nation,mailflag,totem;
	unsigned short personality,vote;
	unsigned int level;
	unsigned int money,exp,hp,mp,maxhp,maxmp;
	unsigned char might,grace,will;
	unsigned int exp_remain;
	double exp_percent;
	struct map* m;
	int x,y;
};
void user_status(struct userobject* obj,int type);

void job_set(int job,int joblv);