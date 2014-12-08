#pragma once
#define MAGIC_NAME_MAX 30
#define MAGIC_MESSAGE_MAX 50
#define MAGIC_SLOT_MAX 52
#define MAGIC_DB_MAX 4000
struct magicdb
{
	int id;
	unsigned char type;
	char name[MAGIC_NAME_MAX+1];
	unsigned char name_len;
	char message[MAGIC_MESSAGE_MAX + 1];
	unsigned char message_len;
};
enum magic_type
{
	magic_auto = 5,
	magic_target = 2,
	magic_input = 1,

};
/*
	magic type

*/

void magic_slot_swap(struct userobject* obj,unsigned char slot1,unsigned char slot2);
void magic_init();
int give_magic(struct userobject* obj,int id,int lv);
struct magicslot
{
	struct magicdb* db;
	unsigned char lv;
	long next_time;
};
void register_magicdb(int code,unsigned char type,char* name,char* msg);
void magic_effect_loc(struct location* loc,unsigned short effect,int time);
void magic_effect_xy(struct map* m,int x,int y,unsigned short effect,int time);
extern struct magicdb* magics[MAGIC_DB_MAX];