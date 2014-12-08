#pragma once
#include "object.h"
#include "socket.h"
#define NPC_DB_MAX 10000
#define NPC_NAME_MAX 30
struct npcdb
{
	unsigned int id;
	unsigned int image;
	unsigned short color;
	unsigned char name_len;
	char name[NPC_NAME_MAX+1];
};
struct npcobject
{
	struct mapobject obj;
	struct npcdb* db;
};
struct ilist_element
{
	unsigned short img;
	unsigned char color;
	unsigned int price;
	char* note;
	char* name;
};
enum talk_type
{
	none=0,
	list_type=1,
	msg_type=2,
	input_type=4,
	shop_buy_type=8,
	shop_sell_type=16,
	shop_input_type = shop_buy_type | shop_sell_type | input_type,
	shop_msg_type = msg_type | shop_buy_type | shop_sell_type,
	menu_type = 32,
	shop_menu_type = shop_buy_type | shop_sell_type | menu_type,
	deposit_type = menu_type | shop_sell_type,
	withdraw_type = menu_type | shop_buy_type,
	pursuit_type = 64,
	pursuit_menu_type = pursuit_type | menu_type,
	ilist_type = 128,
	delmagic_type=list_type | pursuit_type,
	ilist_selltype = ilist_type | shop_sell_type,
	spell_list_type = 5,
};
void select_list(BSocket* sd,unsigned short img,unsigned char color,int type,char* message,int count,char** menu,int prev,int next);

void select_menu(BSocket* sd,unsigned short img,unsigned char color,char* message,int count,char** menu);
void select_islotlist(BSocket* sd,unsigned short img,unsigned char color,char* message,char* slot);
void dialog_input(BSocket* sd,unsigned short img,unsigned char color,char* message);
void select_ilist(BSocket* sd,unsigned short img,unsigned char color,char* message,int count,struct ilist_element* elem);
void npc_click(struct npcobject* obj);
void npcobject_new(struct npcobject* obj,int code);
void dialog(BSocket* sd,unsigned short image,unsigned char color,int type,char* msg,int prev,int next);
extern struct npcdb* npcs[NPC_DB_MAX];
void register_npcdb(int code,char* name,unsigned int img,unsigned char col);
struct npcobject* create_npcobj(int code);