#include "item.h"
#include "mem.h"
#include "debug.h"
#include "game.h"
#include "pc.h"
#include "script.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
struct itemdb* items[ITEM_MAX + MONEY_ITEM_MAX];

void item_init()
{
	
}
void item_slot_hide(BSocket* sd,int slot,int msg)
{
	register unsigned int count=0,i;
	if(slot>=ITEM_SLOT_MAX) return;
	for(i=0;i<ITEM_SLOT_MAX;i++)
		if(sd->obj.ownitems[i]) count++;
	sdsize(sd,10);
	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(7);
	wb(sd, 3) = 16;
	wb(sd, 4) = 0;
	wb(sd, 5) = (slot+1) & 0xFF;
	wb(sd, 6) = msg & 0xFF;
	ws(sd, 7) = SWAP16(count);
	wb(sd, 9) = 0;
	cpt(sd);
}
void item_slot_show(BSocket* sd,int slot)
{
	char showname[256];
	int sz = 11;
	struct itemobject* item = sd->obj.ownitems[slot];
	if(item == NULL) return ;
	sdsize(sd,ITEM_NAME_MAX + 286);
	wb(sd,0) = 170;
	wb(sd,3) = 15;
	wb(sd,4) = 0;
	wb(sd,5) = (slot+1) & 0xFF;
	ws(sd,6) = SWAP16(item->obj.image);
	wb(sd,8) = item->obj.color;
	wb(sd,9) = 0xFF; // 밑 4바이트 
	wb(sd,10) = 0xFF; // 밑 4바이트

	if(item->count > 1 ){
		wb(sd,sz++) = sprintf(showname,item->db->format,item->name,item->count) & 0xFF;
		memmove(wp(sd,sz),showname,wb(sd,sz-1));
		sz += wb(sd,sz-1);
	} else {
		wb(sd,sz++) = item->name_len;
		memmove(wp(sd,sz),item->name,wb(sd,sz-1));
		sz += wb(sd,sz-1);
	}
	wb(sd,sz++) = item->name_len;
	memmove(wp(sd,sz),item->name,wb(sd,sz-1));
	sz += item->name_len;
	wl(sd,sz) = SWAP32(item->db->itype == i_use ? item->count : 1); sz += 4;
	wb(sd,sz++) = 0;
	wb(sd,sz++) = 0;
	wl(sd,sz) = SWAP32(0);
	sz += 4;
	wb(sd,sz++) = 0;
	wb(sd,sz++) = 0;
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);
}
void item_slot_show_all(BSocket* sd)
{
	int i;
	for(i=0;i<ITEM_SLOT_MAX;i++)
		item_slot_show(sd,i);
}
int money_give(struct userobject* uobj,int money,int remain)
{
	if( UINT_MAX - uobj->ud.money < money)
	{
		if(remain)
		{
			money -= UINT_MAX - uobj->ud.money;
			uobj->ud.money = UINT_MAX;
			user_status(uobj,0x10);
			return money;
		}
		else return money;
	}
	uobj->ud.money += money;
	shell_event(ON_PICKUPMONEY,"ii",uobj->obj.id,money);
	user_status(uobj,0x10);
	return 0;
}
int item_drop(struct location* loc,struct itemobject* obj,int all)
{
	if(obj->db->itype != i_use || obj->count == 1 || all){
		obj->item_msg = 1;
		object_warp(&obj->obj,loc->map,loc->x,loc->y);
		return 0;
	} else {
		struct itemobject* obj_new = create_itemobj(obj->db->id);
		obj->item_msg = 1;
		item_seticount(obj,obj->count-1);
		obj_new->count = 1;
		object_warp(&obj_new->obj,loc->map,loc->x,loc->y);
		return 1;
	}
}
int item_drop_slot(struct userobject* uobj,int slot,int all)
{
	struct itemobject* obj;
	if(slot >= ITEM_SLOT_MAX || (obj = uobj->ownitems[slot]) == NULL) return 0;
	return item_drop(&uobj->obj.loc,obj,all);
}
void item_slot_swap(struct userobject* uobj,unsigned char slot1,unsigned char slot2)
{
	struct itemobject* item;
	if( slot1 >= ITEM_SLOT_MAX || slot2 >= ITEM_SLOT_MAX || (uobj->ownitems[slot1] == NULL && uobj->ownitems[slot2])) return ;
	item = uobj->ownitems[slot1];
	uobj->ownitems[slot1] = uobj->ownitems[slot2];
	uobj->ownitems[slot2] = item;
	if(item == NULL) item_slot_hide(obj2sd(uobj),slot2,12);
	else item_slot_show(obj2sd(uobj),slot2);
	
	if(uobj->ownitems[slot1] == NULL) item_slot_hide(obj2sd(uobj),slot1,12);
	else item_slot_show(obj2sd(uobj),slot1);
}
int can_item_get(struct userobject* uobj,struct itemobject* iobj)
{
	BSocket* sd = obj2sd(uobj);
	int i,count;
	int emptyslot = -1;
	if(uobj->empty_count == 0 && iobj->db->max_count == 1)
		return 0;
	if(iobj->db->itype == i_money)
	{
		if(UINT_MAX - uobj->ud.money < iobj->count)
			return 0;
		else
			return 1;
	}
	count = iobj->count;
	for(i=0;i<ITEM_SLOT_MAX;i++)	{
		if(!uobj->ownitems[i] && emptyslot < 0 ) {
			return 1;
		}
		else if(uobj->ownitems[i] && uobj->ownitems[i]->db->id == iobj->db->id)
		{
			if(uobj->ownitems[i]->count + count > iobj->db->max_count)
			{
				if(iobj->db->option & overlap){
					count -= iobj->db->max_count - uobj->ownitems[i]->count;
					continue;
				}
				else return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	return 0;
}
int item_give(struct userobject* uobj,struct itemobject* iobj,int remain)
{
	BSocket* sd = obj2sd(uobj);
	int i;
	int emptyslot = -1;
	if(uobj->empty_count == 0 && iobj->db->max_count == 1)
		return GIVE_FAIL;
	if(iobj->db->itype == i_money)
	{
		i=money_give(uobj,iobj->count,remain);
		if(i == 0) {
			object_del(&iobj->obj);
			return 0;
		} else {
			iobj->count = i;
			
			return GIVE_REMAIN;
		}
	}
	for(i=0;i<ITEM_SLOT_MAX;i++)	{
		if(!uobj->ownitems[i] && emptyslot < 0 ) {
			emptyslot = i;
			if(iobj->db->option & ipack) break;
		}
		else if(uobj->ownitems[i] && !(iobj->db->option & ipack) && uobj->ownitems[i]->db->id == iobj->db->id)
		{
			if(uobj->ownitems[i]->count + iobj->count > iobj->db->max_count)
			{
				if(remain || iobj->db->option & overlap){
					iobj->count -= iobj->db->max_count - uobj->ownitems[i]->count;
					uobj->ownitems[i]->count = iobj->db->max_count;
					item_slot_show(obj2sd(uobj),i);
					if(iobj->db->option & overlap) continue; 
				}
				return GIVE_REMAIN;
			}
			else
			{
				uobj->ownitems[i]->count += iobj->count;
				item_slot_show(obj2sd(uobj),i);
				object_del((struct mapobject*)iobj);
				return GIVE_MIXXED;
			}
		}
	}
	if(emptyslot < 0)
	{
		return GIVE_FAIL;
	}
	uobj->ownitems[emptyslot] = iobj;
	iobj->owner = uobj;
	iobj->slot = emptyslot;
	item_slot_show(obj2sd(uobj),emptyslot);
	uobj->empty_count--; 
	if(iobj->obj.loc.map)
		map_delete(&iobj->obj);
	iobj->obj.loc.map = NULL;
	return GIVE_NEWSLOT;
}
int item_give2(struct userobject* uobj,int id,int count,int remain)
{
	int st;
	struct itemobject* obj;
	obj = MALOC(struct itemobject,1);
	itemobject_new(obj,id);
	obj->count = count;
	st=item_give(uobj,obj,remain);
	if(st < 0) {
		object_del((struct mapobject*)obj);
	}
	return st;
}
void itemobject_del(struct mapobject* obj)
{
	struct itemobject* iobj = (struct itemobject*)obj;
	if(iobj->owner != NULL)
	{
		/*
			주인이 있는 경우 ( 슬롯 아이템의 경우 )
		*/
		iobj->owner->ownitems[iobj->slot] = NULL;
		item_slot_hide(obj2sd(iobj->owner),iobj->slot,iobj->item_msg);
	}
}
struct object_event item_event = {
	itemobject_del
};

void equip_hide(BSocket* sd,int i)
{
	sdsize(sd,7);
	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(4);
	wb(sd, 3) = 56;
	wb(sd, 4) = 0;
	switch(i)
	{
	case 0:
		wb(sd,5) = 1;
		break;
	case 1:
		wb(sd,5) = 2;
		break;
	case 2:
		wb(sd,5) = 3;
		break;
	case 3:
		wb(sd,5) = 4;
		break;
	case 4:
		wb(sd,5) = 7;
		break;
	case 5:
		wb(sd,5) = 8;
		break;
	case 6:
		wb(sd,5) = 20;
		break;
	case 7:
		wb(sd,5) = 21;
		break;
	case i_faceacc:
		wb(sd,5) = 22;
		break;
	case i_tara:
		wb(sd,5) = 23;
		break;
	case i_cape:
		wb(sd,5) = 24;
		break;
	case i_outwear:
		wb(sd,5) = 25;
		break;
	case i_shoes:
		wb(sd,5) = 26;
		break;
	case i_necklace:
		wb(sd,5) = 27;
		break;
	}
	wb(sd, 6) = 0;
	cpt(sd);
}
void equip_show(BSocket* sd,int i)
{
	/*
	0 : 무기
	1: 갑옷
	2: 방패
	3: 투구
	7:왼손
	8:오른손
	20: 보조1
	21 : 보조2
	*/
	char msg[256];
	int size=6;
	struct itemobject* obj = sd->obj.equips[i];
	if(obj==NULL) return;
	sdsize(sd,ITEM_NAME_MAX*2 + 15);
	wb(sd,0) = 170;
	wb(sd,3) = 55;
	wb(sd,4) = 0;
	ws(sd,size) = SWAP16(obj->obj.image);
	size+=2;
	wb(sd,size++) = obj->obj.color;
	wb(sd,size++) = obj->db->equip.color[1];
	if(obj->db->equip.color[1] != 0xFF)
	{
		wl(sd,size) = SWAP32(0);
		size+=4;
	}
	wb(sd,size++) = obj->db->equip.color[2];
	if(obj->db->equip.color[2] != 0xFF)
	{
		wl(sd,size) = SWAP32(0);
		size+=4;
	}
	wb(sd,size++) = obj->name_len;
	memmove(wp(sd,size),obj->name,wb(sd,size-1));
	size+=wb(sd,size-1);
	wb(sd,size++) = obj->db->name_len;
	memmove(wp(sd,size),obj->db->name,wb(sd,size-1));
	size += wb(sd,size-1);
	wb(sd,size++) = 0;
	wl(sd,size) = SWAP32(1000);
	size+=4;
	wb(sd,size++) = (i==0 && obj->db->equip.option>0) ? obj->db->equip.option-1 : 0; // 활,노인가
	wb(sd,size++) = 0;
//	wl(sd,size) = SWAP32(obj->endurance);
//	size+=2;
	ws(sd,1) = SWAP16(size-3);
	switch(i)
	{
	case 0:
		wb(sd,5) = 1;
		break;
	case 1:
		wb(sd,5) = 2;
		break;
	case 2:
		wb(sd,5) = 3;
		break;
	case 3:
		wb(sd,5) = 4;
		break;
	case 4:
		wb(sd,5) = 7;
		break;
	case 5:
		wb(sd,5) = 8;
		break;
	case 6:
		wb(sd,5) = 20;
		break;
	case 7:
		wb(sd,5) = 21;
		break;
	case i_faceacc:
		wb(sd,5) = 22;
		break;
	case i_tara:
		wb(sd,5) = 23;
		break;
	case i_cape:
		wb(sd,5) = 24;
		break;
	case i_outwear:
		wb(sd,5) = 25;
		break;
	case i_shoes:
		wb(sd,5) = 26;
		break;
	case i_necklace:
		wb(sd,5) = 27;
		break;
	}
	cpt(sd);
}
int item_seticount(struct itemobject* obj,int count)
{
	if(obj->db->max_count< count) return -1;
	obj->count = count;
	if(count == 0) {
		object_del(&obj->obj);
		return 1;
	}
	else if(obj->owner)
	{
		item_slot_show(obj2sd(obj->owner),obj->slot);
	}
	return 0;
}
int equip_takeoff(struct userobject* obj,int slot)
{
	struct itemobject* eq;
	eq = obj->equips[slot];
	if(item_give(obj,eq,0) == GIVE_FAIL)
	{
		game_send_message(obj2sd(obj),3,"소지품이 꽉 찼습니다\n");
		return -1;
	}
	obj->equips[slot] = NULL;
	equip_hide(obj2sd(obj),slot);
	user_lookrefresh(obj);

	return 0;
}
void drop_money(struct location* loc,unsigned int money)
{
	int i;
	struct itemobject* obj;
	for(i=ITEM_MAX;i<ITEM_MAX + MONEY_ITEM_MAX;i++)
	{
		if(items[i] == NULL || money < items[i]->max_count) {
			if(i==0) return ;
			obj = create_itemobj(i-1);
			obj->count = money;
			object_warp(&obj->obj,loc->map,loc->x,loc->y);
			break;
		}
	}
}
void equip_takeon(struct userobject* obj,int slot)
{
	struct itemobject* item;
	int eslot ;
	if(slot < 0 || slot >= ITEM_SLOT_MAX || ((item = obj->ownitems[slot]) == NULL) || item->db->itype >= 14)
		return;
	eslot = item->db->itype;
	switch(eslot)
	{
	case 4:
		if(obj->equips[4] == NULL)
			eslot = 4;
		else
			eslot = 5;
		break;
	case 5:
		if(obj->equips[6] == NULL)
			eslot = 6;
		else
			eslot = 7;
	}
	obj->ownitems[slot] = NULL;
	obj->empty_count++;
	if(obj->equips[item->db->itype] != NULL){
		block_lock = 1;
		shell_event(ON_TAKEOFF,"iii",obj->obj.id,obj->equips[item->db->itype]->obj.id,item->db->itype);
		block_lock = 0;
		if(obj->equips[item->db->itype]) return ;
	}
	if(obj->ownitems[slot] == NULL) item_slot_hide(obj2sd(obj),slot,12);
	
	obj->equips[item->db->itype] = item;
	equip_show(obj2sd(obj),item->db->itype);
	user_lookrefresh(obj);
}
void register_equipdb(int code,unsigned short image,unsigned char ecol1,unsigned char ecol2,unsigned char ecol3,unsigned char opt,unsigned short atk)
{
	struct itemdb* m;
	if(code >= ITEM_MAX || items[code] == NULL)
	{
		errf("ITEM 코드 범위를 초과하거나 존재하지 않는 아이템입니다 (%d)\n",code);
		return;
	}
	m = items[code];
	m->equip.option = opt;
	m->equip.image = image;
	m->equip.color[0] = ecol1;
	m->equip.color[1] = ecol2;
	m->equip.color[2] = ecol3;
	m->equip.sound_attack = atk;
}
void register_itemdb(int code,unsigned char type,unsigned char overlap,char* name,char* format,unsigned short image,unsigned char col,unsigned int max_count,unsigned char opt)
{
	
	struct itemdb* m;
	if(code >= ITEM_MAX + MONEY_ITEM_MAX) {
		errf("ITEM 코드 범위를 초과했습니다(%d) \n",code);
		return;
	}
	if(items[code] != NULL) {
		m = items[code];
	}
	else items[code] = m = MALOC(struct itemdb,1);
	strncpy(m->name,name,ITEM_NAME_MAX);
	m->itype = type;
	m->format = format;
	m->name_len = strlen(m->name);
	m->id = code;
	m->image = image;
	m->color = col;
	if(m->itype < 14) m->max_count = 1;
	else m->max_count = max_count;
	m->option = opt;
}
struct itemobject* create_itemobj(int code)
{
	struct itemobject* obj = CALOC(struct itemobject,1);
	itemobject_new(obj,code);
	return obj;
}
unsigned int itemobject_new(struct itemobject* obj,int code)
{
	if(items[code] == NULL) {
		warnf("itemdb(%d)가 존재하지 않습니다\n",code);
		return ;
	}
	obj->obj.showtype = 2;
	obj->obj.type = ITEM;
	obj->obj.loc.map = NULL;
	obj->item_msg = 0;
	obj->db = items[code];
	strcpy(obj->name,obj->db->name);
	obj->name_len = obj->db->name_len;
	obj->obj.image = obj->db->image;
	obj->obj.color = obj->db->color;
	obj->owner = NULL;
	obj->slot = 0;
	obj->count = 1;
	obj->obj.events = &item_event;
	object_new(&obj->obj,obj->db->itype < 14 ? 1 : 0);
	return obj->obj.id;
}