
#include "socket.h"
#include "magic.h"

#include "userobject.h"
#include "debug.h"
#include "mem.h"
#include "map.h"
struct magicdb* magics[MAGIC_DB_MAX];
void magic_init()
{
}
void magic_delay(BSocket* sd,int time,char* message){
	register unsigned int  size;
	if(sd==NULL) return;
	wb(sd, 0) = 170;
	wb(sd, 3) = 58;
	wb(sd, 4) = 0;
	wb(sd,5)= strlen(message);
	memmove(wp(sd, 6), message, wb(sd, 5));
	size = wb(sd, 5) + 6;
	wl(sd, size) = SWAP32(time);
	size += 4;
	wb(sd, size++) = 0;
	ws(sd, 1) = SWAP16(size-3);
	cpt(sd);
}

void magic_teleport(BSocket* sd,int dist){ // 축지령서
	
	wb(sd,0) =170;
	wb(sd,3) = 125;
	wb(sd,4) = 0;
	wb(sd,5) = dist & 0xFF;
	wb(sd,6) = 0;
	ws(sd,1) = SWAP16(3);
	cpt(sd);
}
void magic_delay2(BSocket* sd,int slot,int time)
{
	
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(8);
	wb(sd,3) = 63;
	wb(sd,4) = 0;
	wb(sd,5) = 0;
	wb(sd,6) = (slot+1) & 0xFF;
	wl(sd,7) = SWAP32(time);
	cpt(sd);
}
void magic_slot_show(BSocket* sd,int slot)
{
	struct magicslot* mg = &sd->obj.ownmagics[slot];
	struct magicdb* mdb;
	int size = 8;
	if(mg->db == NULL) return ;
	mdb = mg->db;
	sdsize(sd,MAGIC_NAME_MAX + MAGIC_MESSAGE_MAX + 30);
	wb(sd,0) = 170;
	wb(sd,3) = 23;
	wb(sd,4) = 0;
	wb(sd,5) = slot+1;
	wb(sd,6) = mdb->type;
	wb(sd,7) = mdb->name_len;
	memmove(wp(sd,8),mdb->name,wb(sd,7));
	size += wb(sd,7);
	wb(sd,size++) = mdb->message_len;
	memmove(wp(sd,size),mdb->message,wb(sd,size-1));
	size += wb(sd,size-1);
	wb(sd,size++) = 32;
	wb(sd,size++) = 0;
	wb(sd,size++) = 0;
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
void magic_effect3(struct mapobject* obj,unsigned short effect,unsigned char len,BYTE* data)
{
	
	byte packet[1024];
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(len*2+13);
	WB(packet,3) = 0x29;
	WB(packet,4) = 0;
	WB(packet,5) = 0x01;
	WS(packet,6) = SWAP16(effect);
	WB(packet,8) = obj->loc.x;
	WB(packet,9) = obj->loc.y;
	WL(packet,10) = SWAP32(0x32000010);
	WB(packet,14) = len;
	memmove(packet+15,data,len*2);
	write_sight(&obj->loc,packet);
}
void magic_slot_hide(BSocket* sd,int slot)
{
	sdsize(sd,9);
	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(6);
	wb(sd, 3) = 24;
	wb(sd, 4) = 0;
	wb(sd, 5) = (slot + 1);
	ws(sd, 6) = SWAP16(0);
	wb(sd, 8) = 0;
	cpt(sd);
}
void magic_slot_show_all(BSocket* sd)
{
	int i;
	for(i=0;i<MAGIC_SLOT_MAX;i++)
		magic_slot_show(sd,i);
}
void magic_effect_loc(struct location* loc,unsigned short effect,int time)
{
	byte p[19];
	p[0] = 170;
	p[1] = 0;
	p[2] = 16;
	p[3] = 0x29;
	p[4] = 0;
	p[5]= 0;
	WL(p,6) = 0;
	WS(p,10) = SWAP16(effect);
	WS(p,12) = SWAP16(time);
	WS(p,14) = SWAP16(loc->x);
	WS(p,16) = SWAP16(loc->y);
	p[18] = 0;
	write_sight(loc,p);
}
void register_magicdb(int code,unsigned char type,char* name,char* msg)
{
	struct magicdb* m;
	if(code >= MAGIC_DB_MAX) {
		errf("MAGIC 코드 범위를 초과했습니다(%d) \n",code);
		return;
	}
	if(magics[code] != NULL) {
		m = magics[code];
	}
	else magics[code] = m = CALOC(struct magicdb,1);
	m->id = code;
	m->type = type;
	strncpy(m->name,name,MAGIC_NAME_MAX);
	m->name_len = strlen(m->name);
	if(msg != NULL) {
		strncpy(m->message,msg,MAGIC_MESSAGE_MAX);
		m->message_len = strlen(m->message);
	}
}
void magic_effect_xy_single(BSocket* sd,int x,int y,unsigned short eff,int time)
{
	struct location loc;
	sdsize(sd,19);
	wb(sd,0) = 170;
	wb(sd,1) = 0;
	wb(sd,2) = 16;
	wb(sd,3) = 0x29;
	wb(sd,4) = 0;
	wb(sd,5)= 0;
	wl(sd,6) = 0;
	ws(sd,10) = SWAP16(eff);
	ws(sd,12) = SWAP16(time);
	ws(sd,14) = SWAP16(x);
	ws(sd,16) = SWAP16(y);
	wb(sd,18) = 0;
	cpt(sd);
}
void magic_effect_xy(struct map* m,int x,int y,unsigned short effect,int time)
{
	struct location loc;
	byte p[19];
	p[0] = 170;
	p[1] = 0;
	p[2] = 16;
	p[3] = 0x29;
	p[4] = 0;
	p[5]= 0;
	WL(p,6) = 0;
	WS(p,10) = SWAP16(effect);
	WS(p,12) = SWAP16(time);
	WS(p,14) = SWAP16(x);
	WS(p,16) = SWAP16(y);
	p[18] = 0;
	loc.map = m;
	loc.x = x;
	loc.y = y;
	write_sight(&loc,p);
}
void magic_slot_swap(struct userobject* obj,unsigned char slot1,unsigned char slot2)
{
	struct magicslot magic;
	if(slot1 >= MAGIC_SLOT_MAX || slot2 >= MAGIC_SLOT_MAX
		|| ( obj->ownmagics[slot1].db == NULL && obj->ownmagics[slot2].db == NULL ))
		return ;
	magic.db = obj->ownmagics[slot1].db;
	magic.lv = obj->ownmagics[slot1].lv;
	obj->ownmagics[slot1].db = obj->ownmagics[slot2].db;
	obj->ownmagics[slot1].lv = obj->ownmagics[slot2].lv;
	obj->ownmagics[slot2].db = magic.db;
	obj->ownmagics[slot2].lv = magic.lv;
	if(obj->ownmagics[slot2].db == NULL) magic_slot_hide(obj2sd(obj),slot2);
	else magic_slot_show(obj2sd(obj),slot2);
	if(obj->ownmagics[slot1].db == NULL) magic_slot_hide(obj2sd(obj),slot1);
	else magic_slot_show(obj2sd(obj),slot1);

}
int give_magic(struct userobject* obj,int id,int lv)
{
	int i,empty = -1;
	if(magics[id] == NULL) return -1;
	for(i=0;i<MAGIC_SLOT_MAX;i++){
		if(obj->ownmagics[i].db == NULL && empty < 0) {
			empty = i;
		}else if(obj->ownmagics[i].db !=NULL && obj->ownmagics[i].db->id == id)
		{
			if(obj->ownmagics[i].lv >= lv) {
				return -2;
			}
			else {
				obj->ownmagics[i].lv = lv;
				magic_slot_show(obj2sd(obj),empty);
				return i;
			}
		}
	}
	if(empty < 0) return -1;
	obj->ownmagics[empty].db = magics[id];
	obj->ownmagics[empty].lv = lv;
	magic_slot_show(obj2sd(obj),empty);
	return empty;
}