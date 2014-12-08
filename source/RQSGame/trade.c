
#include "trade.h"
#include "userobject.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
#include "mem.h"
#include "game.h"
int request_trade(BSocket* sd,struct userobject* obj)
{
	BSocket* sd2 = obj2sd(obj);
	int i;
	if(obj == NULL) return -1;
	if(obj->trade.user){
		//game_send_message(sd,3,"상대방이 이미 교환중입니다.");
		return 1;
	} else if(!(obj->opt & trade_ok)){
		//game_send_message(sd,3,"상대방이 교환 거부 중입니다.");
		return 2;
	} else if(!(sd->obj.opt & trade_ok)){
		//game_send_message(sd,3,"교환 거부 중입니다.");
		return 3;
	}
	
	sd->obj.trade.user = obj;
	obj->trade.user = &sd->obj;
	sdsize(sd,100);
	wb(sd,0) = 170;
	wb(sd,3) = 66;
	wb(sd,4) = 0;
	wb(sd,5) = 0;
	wl(sd,6) = SWAP32(obj->obj.id);
	wb(sd,10) = obj->ud.id_len;
	memmove(wp(sd,11),obj->ud.id,wb(sd,10));
	ws(sd,11+wb(sd,10)) = SWAP16(11);
	wb(sd,13+wb(sd,10)) = 0;
	ws(sd,1) = SWAP16(wb(sd,10)+11);
	cpt(sd);
	
	sdsize(sd2,100);
	wb(sd2,0) = 170;
	wb(sd2,3) = 66;
	wb(sd2,4) = 0;
	wb(sd2,5) = 0;
	wl(sd2,6) = SWAP32(sd->obj.obj.id);
	wb(sd2,10) = sd->obj.ud.id_len;
	memmove(wp(sd2,11),sd->obj.ud.id,wb(sd2,10));
	ws(sd2,11+wb(sd2,10)) = SWAP16(10);
	wb(sd2,13+wb(sd2,10)) = 0;
	ws(sd2,1) = SWAP16(wb(sd2,10)+11);
	cpt(sd2);
	sd->obj.trade.trade_count  = obj->trade.trade_count = 0;
	sd->obj.trade.trade_money = obj->trade.trade_money = 0;
	
	sd->obj.trade.ok = obj->trade.ok = 0;
	return 0;
}

int trade_money(BSocket* sd,struct userobject* obj,unsigned int money)
{
	
	byte packet[12];
	BSocket* td = obj2sd(obj);
	if(sd->obj.trade.ok || obj->trade.ok) return -1;
	if(money >= sd->obj.ud.money)
	{
		money = sd->obj.ud.money;
	}
	else if(0xFFFFFFFF-sd->obj.ud.money < money)
	{
		game_send_message(sd,3,"더 이상 줄 수 없습니다.");
		money = 0xFFFFFFFF-obj->ud.money;
	}
	sd->obj.trade.trade_money= money;
	wb(td,0) = wb(sd,0) = 170;
	ws(td,1) = ws(sd,1) = SWAP16(9);
	wb(td,3) = wb(sd,3) = 66;
	wb(td,4) = wb(sd,4) = 0;
	wb(td,5) = wb(sd,5) = 3;
	wb(td,6) = 1;
	wb(sd,6) = 0;
	wl(td,7) = wl(sd,7) = SWAP32(money);
	wb(td,11) = wb(sd,11) = 0;
	cpt(sd);
	cpt(td);
	return 0;
}
void trade_msg(BSocket* sd,int type,char* message)
{
	register unsigned int size = strlen(message) + 9;
	sdsize(sd,size);
	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(size-3);
	wb(sd, 3) = 66;
	wb(sd, 4) = 0;
	wb(sd, 5) = type;
	ws(sd, 6) = SWAP16(size-9);
	memmove(wp(sd, 8), message, size-9);
	wb(sd, size-1) = 0;
	cpt(sd);
}
int trade_item(BSocket* sd,struct userobject* obj,int slot,int count)
{
	struct itemobject* item;
	int sz = 13;
	BSocket* td = obj2sd(obj);
	byte packet[300];
	int tcnt = sd->obj.trade.trade_count;
	if(slot >= ITEM_SLOT_MAX || (item = sd->obj.ownitems[slot])==NULL) return -1;
	if(sd->obj.trade.ok || obj->trade.ok) return -1;
	if(count == 0 && item->db->max_count > 1 && item->count > 1) {
		sd->obj.trade.selectslot = slot;
		trade_msg(sd,1,"");
		return 0;
	}
	else if(count == 0) count = 1;
	if(tcnt >= TRADE_ITEM_MAX) {
		game_send_message(sd,3,"더 이상 아이템을 줄 수 없습니다.");
		return 0;
	}
	if(item->db->option & notrade){
		game_send_message(sd,3,"교환할 수 없는 아이템입니다.");
		return 0;
	}
	if(!can_item_get(obj,item))
	{
		game_send_message(sd,3,"상대방 소지품이 꽉 찼거나 더 이상 가질 수 없습니다.");
		return 0;
	}
	count = min(count,item->count);
	sd->obj.trade.trade_items[tcnt].count = count;
	sd->obj.trade.trade_items[tcnt].slot = slot;
	wb(td,0) = wb(sd,0) = 170;
	wb(td,3) = wb(sd,3) = 66;
	wb(td,4) = wb(sd,4) = 0;
	wb(td,5) = wb(sd,5) = 2;
	wb(sd,6) = 0;
	wb(td,6) = 1;
	wb(td,7) = wb(sd,7) = sd->obj.trade.trade_count++;
	ws(td,8) = ws(sd,8) = SWAP16(item->obj.image);
	wb(td,10) = wb(sd,10) = item->obj.color;
	wb(td,11) = wb(sd,11) = 0xFF;
	wb(td,12) = wb(sd,12) = 0xFF;

	wb(td,sz) = wb(sd,sz) = sprintf((char*)wp(sd,sz+1),item->db->format,item->db->name,count);
	memmove(wp(td,sz+1),wp(sd,sz+1),wb(sd,sz));
	sz+= wb(sd,sz) + 1;
	wb(td,sz) = wb(sd,sz++) = item->name_len;
	
	memmove(wp(sd,sz),item->name,wb(sd,sz-1));
	memmove(wp(td,sz),item->name,wb(td,sz-1));
	sz+= wb(sd,sz-1);
	wl(td,sz) = wl(sd,sz) = SWAP32(item->obj.id),sz+=4;
	wl(td,sz) = wl(sd,sz) = SWAP32(1000), sz+=4;
	wb(td,sz) = wb(sd,sz++) = 0;
	wb(td,sz) = wb(sd,sz++) = 0;

	ws(sd,1) = ws(td,1) = SWAP16(sz-3);
	cpt(sd);
	cpt(td);
	return 0;
}

void trade_cancel(BSocket* sd,struct userobject* obj)
{
	int i;
	BSocket* td = obj2sd(obj);
	trade_msg(sd,4,"교환을 취소 했습니다.");
	trade_msg(td,4,"상대방이 교환을 취소 했습니다.");
	
	sd->obj.trade.trade_count = obj->trade.trade_count = sd->obj.trade.trade_money = obj->trade.trade_money = 0;
	sd->obj.trade.user = obj->trade.user = NULL;
}
int trade_success(BSocket* sd,struct userobject* obj)
{
	int i;
	BSocket* td = obj2sd(obj);
	if(sd->obj.trade.ok)
		return -1;
	sd->obj.trade.ok = 1;
	if(!obj->trade.ok)
	{
		sdsize(td,7);
		wb(td,0) = 170;
		wb(td,1) = 0;
		wb(td,2) = 4;
		wb(td,3) = 66;
		wb(td,4) = 0;
		wb(td,5) = 5;
		wb(td,6) =1 ;
		cpt(td);
		game_send_message(td,3,"상대방이 확인을 눌렀습니다.");
		return 0;
	}
	for(i=0;i<sd->obj.trade.trade_count;i++)
	{
		char slot = sd->obj.trade.trade_items[i].slot;
		struct itemobject* item = sd->obj.ownitems[slot];
		if(sd->obj.trade.trade_items[i].count != item->count)
		{
			item->count -= sd->obj.trade.trade_items[i].count;
			item_slot_show(sd,slot);
			item = create_itemobj(item->db->id);
			item->count = sd->obj.trade.trade_items[i].count;
		}
		else{
			sd->obj.ownitems[slot] = NULL;
			item_slot_hide(sd,slot,12);
		}
		item_give(obj,item,0);
		money_give(obj,sd->obj.trade.trade_money,0);
	
	}
	for(i=0;i<obj->trade.trade_count;i++)
	{
		char slot = obj->trade.trade_items[i].slot;
		struct itemobject* item = obj->ownitems[slot];
		if(obj->trade.trade_items[i].count != item->count)
		{
			item->count -= obj->trade.trade_items[i].count;
			item_slot_show(td, slot);
			item = create_itemobj(item->db->id);
			item->count = sd->obj.trade.trade_items[i].count;
		}
		else{
			obj->ownitems[slot] = NULL;
			item_slot_hide(td,slot,12);
		}
		item_give(&sd->obj,item,0);
		money_give(&sd->obj,obj->trade.trade_money,0);
	}
	
	sd->obj.trade.trade_count = obj->trade.trade_count = sd->obj.trade.trade_money = obj->trade.trade_money = 0;
	sd->obj.trade.user = obj->trade.user = NULL;
	trade_msg(sd,4,"교환에 성공했습니다.");
	trade_msg(td,4,"교환에 성공했습니다.");
	user_status(&sd->obj,0x10);
	user_status(obj,0x10);
}