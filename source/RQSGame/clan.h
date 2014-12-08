#pragma once
#include "list.h"
#define CLAN_NAME_MAX 32
#define CLAN_TITLE_MAX 32
struct clan
{
	struct _list members;
	int member_count;
	char name[CLAN_NAME_MAX+1];
	unsigned char name_len;
	char title[CLAN_TITLE_MAX+1];
	unsigned char title_len;
};
struct clan* clan_new(char*,char*);
int clan_write(struct clan*,unsigned char* packet);
void clan_msg(struct clan* cl,char type,char* msg);
void clan_msg2(struct clan* c,int fc,int bc,char* msg,int timeout);