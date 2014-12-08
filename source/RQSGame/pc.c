#include "pc.h"
#include "game.h"
#include "npc.h"
#include <time.h>
#include "timer.h"
#include "debug.h"
#include "magic.h"
#include <string.h>
#include "map.h"
#include "trade.h"
#include <stdio.h>
#include "script.h"
#include "userobject.h"
#include "userdata.h"
struct pc_event pc_events[256]={NULL};
unsigned int ref_events[100] = {0};
#define SIZE(sz) {if(UNSWAP16(buf+1) < (sz)-3) return -1;}
#define EVENT_SET(cd,crypt,func) (pc_events[cd].cryption = crypt, pc_events[cd].event_func = func)
#define LUA_EVENT_SET(cd,fc) {if(ref_events[cd] != 0) { unregisterfunc(ref_events[cd]); } ref_events[cd] = registerfunc(fc);}
void pc_sound(BSocket* sd,unsigned short sound)
{
	sdsize(sd,23);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(20);
	wb(sd,3) = 25;
	wb(sd, 4) = 0;
	wb(sd, 5) = 0; //Sound Type
	wb(sd, 6) = 3;
	ws(sd, 7) = SWAP16(sound);
	wb(sd, 9) = (100); //volume
	ws(sd, 10) = SWAP16(4);
	wl(sd, 12) = SWAP32(sd->obj.obj.id);
	ws(sd, 16) = SWAP16(256);
	ws(sd, 18) = SWAP16(514);
	ws(sd, 20) = SWAP16(4);
	wb(sd, 22) = 0;
	cpt(sd);
}
void pc_option_sub(BSocket* sd)
{
	//user_refresh_status3(sd);
	sdsize(sd,12);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(9);
	wb(sd,3) = 35;
	wb(sd,4) = 0;
	wb(sd,5) = 1;
	wb(sd,6) = 1;
	wb(sd,7) = 1;
	wb(sd,8) = 1;
	wb(sd,9) = 1;
	wb(sd,10) = 1;
	wb(sd,11) = 1;
	cpt(sd);
}
int pc_call(BYTE type,BSocket* sd,BYTE* buf)
{
	if(pc_events[type].event_func == NULL)
	{
		deff("[Unknown packet] %d\n",type);
		return 0;
	}
	if(pc_events[type].cryption)
		decrypt(0,buf,cryptkey);
	return pc_events[type].event_func(sd,buf);
}
int pc_dialog(BSocket* sd,BYTE* buf)
{
	if(sd->dlg_type == input_type && buf[5] != input_type)
	{
		if(sd->dlg_thread == NULL) return -1;
		else{
			lua_pushnil(sd->dlg_thread->lua);
			if(thread_resume(sd->dlg_thread,1) == 0) {
				sd->dlg_thread = NULL;
			}
			return 0;
		}
	}
	if(buf[5] != 0 && sd->dlg_type != buf[5]) return -1;
	SIZE(6);
	
	switch(buf[5])
	{
	default: return -1;
	case menu_type:
		if(sd->dlg_thread == NULL) return -1;
		else{
			SIZE(12);
			lua_pushinteger(sd->dlg_thread->lua,buf[11] + 1);
			if(thread_resume(sd->dlg_thread,1) == 0) {
				sd->dlg_thread = NULL;
			}
		}
		break;
	case ilist_type:
		if(sd->dlg_thread == NULL) return -1;
		else{
		
			int len;
			SIZE(13);
			len = buf[12];
			if(len == 0){
				lua_pushnil(sd->dlg_thread->lua);
			} else {
				SIZE(14+len);
				buf[13+len] = 0;
				lua_pushstring(sd->dlg_thread->lua,(char*)buf+13);
			}
			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	case ilist_selltype:
		if(sd->dlg_thread == NULL) return -1;
		else{
			SIZE(13);
			lua_pushinteger(sd->dlg_thread->lua,buf[12]-1);
			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	case 0:
		if(sd->dlg_type == input_type){
			if(sd->dlg_thread == NULL) return -1;
			lua_pushnil(sd->dlg_thread->lua);
			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	case input_type:
		
		if(sd->dlg_thread == NULL) return -1;
		else
		{
			int ilen;
			SIZE(14);
			ilen = UNSWAP16(buf+12);
			SIZE(15+ilen);
			buf[14+ilen] = 0;
			lua_pushstring(sd->dlg_thread->lua,(char*)buf+14);
			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	case msg_type:
		if(sd->dlg_thread == NULL) return -1;
		else
		{
			SIZE(14);
			lua_pushinteger(sd->dlg_thread->lua,buf[13]);
			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	case list_type:
		if(sd->dlg_thread == NULL) return -1;
		else
		{
			SIZE(14);
			if(buf[13] == 2){
				SIZE(16);
				lua_pushinteger(sd->dlg_thread->lua,buf[15]);
			} else if(buf[13] == 1){
				lua_pushinteger(sd->dlg_thread->lua,0);
			}
			else if(buf[13] == 0) {
				lua_pushinteger(sd->dlg_thread->lua,-1);
			}
			else return -1;

			if(thread_resume(sd->dlg_thread,1) == 0)
			{
				sd->dlg_thread = NULL;
			}
		}
		break;
	}
	return 0;
}
int pc_takeoff(BSocket* sd,BYTE* buf)
{
	unsigned char slot;
	SIZE(6);
	slot = buf[5];
	
	switch(slot)
	{
	case 1: slot = 0; break;
	case 2: slot = 1; break;
	case 3: slot = 2; break;
	case 4: slot = 3; break;
	case 7: slot = 4; break;
	case 8: slot = 5; break;
	case 20: slot = 6; break;
	case 21: slot = 7; break;
	case 22: slot = 8; break;
	case 23: slot = 9; break;
	case 24: slot = 10; break;
	case 25: slot = 11; break;
	case 26: slot = 12; break;
	case 27: slot = 13; break;
	default: return -1;
	}
	if(sd->obj.equips[slot]){
		shell_event(ON_TAKEOFF,"iii",sd->obj.obj.id,sd->obj.equips[slot]->obj.id,slot);
	}
	return 0;
}
int pc_useitem(BSocket* sd,BYTE* buf)
{
	struct itemobject* obj;
	SIZE(6);
	obj = sd->obj.ownitems[buf[5]-1];
	if(obj == NULL) return 0;
	if(obj->db->itype < 14)
	{
		shell_event(ON_TAKEON,"iii",sd->obj.obj.id,obj->obj.id,buf[5]-1);
	}
	else if(obj) {
		
		shell_event(ON_USEITEM,"iii",sd->obj.obj.id,buf[5]-1,obj->obj.id);
	}
	return 0;
}
int pc_userlist(BSocket* sd,BYTE* buf)
{
	game_user_list(sd);
	return 0;
}
int pc_takeon(BSocket* sd,BYTE* buf)
{
	SIZE(6);
	shell_event(ON_TAKEON,"ii",sd->obj.obj.id,buf[5]-1);
	return 0;
}

int request_trade(BSocket* sd,struct userobject* obj);
int trade_money(BSocket* sd,struct userobject* obj,unsigned int money);
int trade_item(BSocket* sd,struct userobject* obj,int slot,int count);
void trade_cancel(BSocket* sd,struct userobject* obj);
int trade_success(BSocket* sd,struct userobject* obj);
int pc_trade(BSocket* sd,BYTE* buf)
{
	unsigned int id;
	struct userobject* obj;
	SIZE(10);
	id = UNSWAP32(buf+6);
	if(id >= objects_size || objects[id] == NULL || objects[id]->type != USER) return -1;
	obj = (struct userobject*)objects[id];
	if(obj != sd->obj.trade.user) {
		return -1;
	}
	switch(buf[5])
	{
	case 1:
		SIZE(11);
		return trade_item(sd,obj,buf[10]-1,0);
	case 2:
		SIZE(12);
		return trade_item(sd,obj,sd->obj.trade.selectslot,UNSWAP16(buf+10));
	case 3:
		SIZE(14);
		return trade_money(sd,obj,UNSWAP32(buf+10));
	case 4:
		trade_cancel(sd,obj);
		break;
	case 5:
		return trade_success(sd,obj);

	}
}
int pc_attack(BSocket* sd,BYTE* buf)
{
	struct mapobject* obj;
	struct itemobject* item = sd->obj.equips[0];
	if ( item == NULL || item->db->equip.option == 0 || item->db->equip.option == 1 ){
		object_action(&sd->obj.obj,1,20,0);
		if(item)
			object_sound(&sd->obj.obj,item->db->equip.sound_attack);
		obj = getfrontobj(&sd->obj.obj,OBJLIVE,NULL);
		if(obj != NULL) shell_event(ON_ATTACK,"ini",sd->obj.obj.id,obj->id);
		else shell_event(ON_ATTACK,"i",sd->obj.obj.id);
	} else if(item->db->equip.option == 2 || item->db->equip.option == 3) {
		SIZE (7);
		if ( buf[5] == 0){
			object_action(&sd->obj.obj,27,5,0);
		} else if(buf[5] == 4){
			object_action(&sd->obj.obj,28,5,0);
			object_sound(&sd->obj.obj,item->db->equip.sound_attack);
			shell_event(ON_ATTACK,"i",sd->obj.obj.id);
		} else if(buf[5] == 3){
			struct lua_thread* th;
			int mcnt,i;
			//노
			object_action(&sd->obj.obj,28,5,0);
			object_sound(&sd->obj.obj,item->db->equip.sound_attack);
			SIZE(8);
			mcnt = buf[7];
			SIZE(mcnt*4 + 8);
			if(ref_events[ON_ATTACK]){
				th = thread_get();
				lua_rawgeti(th->lua,LUA_REGISTRYINDEX,ref_events[ON_ATTACK]);
				lua_pushinteger(th->lua,sd->obj.obj.id);
				lua_pushinteger(th->lua,buf[6]);
				for(i=0;i<mcnt;i++){
					unsigned int id = UNSWAP32(buf+8+i*4);
					if(objects[id] == NULL) {
						lua_pop(th->lua,i + 3);
						thread_back(th);
						return -1;
					}
					lua_pushinteger(th->lua,id);
				}
				thread_resume(th,mcnt+2);
			}
		} else if(buf[5] == 2 ) {
			unsigned int id;
			object_action(&sd->obj.obj,28,5,0);
			object_sound(&sd->obj.obj,item->db->equip.sound_attack);
			SIZE(13);
			id = UNSWAP32(buf+8);
			if(objects[id] == NULL) return -1;
			shell_event(ON_ATTACK,"iii",sd->obj.obj.id,buf[6],id);
		}
	}
	return 0;
}
int pc_refresh(BSocket* sd,BYTE* buf)
{
	user_status(&sd->obj,120);
	pc_option_sub(sd);
	map_send_xy(sd,&sd->obj.obj.loc);
	return 0;
}
int pc_lookself(BSocket* sd,BYTE* buf)
{
	SIZE(6);
	if(buf[5] == 0)
	{
		user_lookself(sd);
	}
	else if(buf[5] == 1)
	{
		group_list(sd);
	}
	return 0;
}
int pc_emotion(BSocket* sd,BYTE* buf)
{
	SIZE(6);
	object_action(&sd->obj.obj,buf[5]+11,78,0);
	shell_event(ON_EMOTION,"ii",sd->obj.obj.id,buf[5]);
	return 0;
}
int pc_pickup(BSocket* sd,BYTE* buf)
{
	struct _list_elem* elem;
	struct mapobject* obj;
	struct _list * p = pos_list_xy(sd->obj.obj.loc.map,sd->obj.obj.loc.x,sd->obj.obj.loc.y);
	SIZE(6);
	object_action(&sd->obj.obj,4,30,1);
	shell_event(ON_PICKUP,"i",sd->obj.obj.id);
	elem = list_end(p);
	while(elem != &p->head)
	{
		obj = list_entry(elem,struct mapobject,pos_elem);
		if(obj->magic == OBJECT_MAGIC_NUMBER && obj->type == ITEM)
		{
			int res;
			struct itemobject* iobj = (struct itemobject*)obj;
			elem = list_prev(elem);
			shell_event(ON_PICKUP,"ii",sd->obj.obj.id,iobj->obj.id);
			if(buf[5] == 0) break;
		}
		else elem = list_prev(elem);
	}
	return 0;
}
int pc_throw(BSocket* sd,BYTE* buf)
{
	return 0;
}
int pc_send_map(BSocket* sd,BYTE* buf)
{
	SIZE(11);
	if(sd->obj.obj.loc.map != NULL) {
		map_send(sd,sd->obj.obj.loc.map,UNSWAP16(buf+5),UNSWAP16(buf+7),buf[9],buf[10]);
	}
	return 0;
}
int pc_walk(BSocket* sd,BYTE* buf)
{
	unsigned short nsize = UNSWAP16(buf+1);
	if(!object_walk(&sd->obj.obj))
	{
		
		if(buf[3] == 6 && nsize >= 14) map_send(sd,sd->obj.obj.loc.map,UNSWAP16(buf+11),UNSWAP16(buf+13),buf[15],buf[16]);
		
		shell_event(ON_WALK,"iiii",sd->obj.obj.id,sd->obj.obj.loc.map->id,sd->obj.obj.loc.x,sd->obj.obj.loc.y);

	}
	return 0;
}
int pc_say(BSocket* sd,BYTE* buf)
{
	int size;
	char message [260 + ID_MAX];
	SIZE(7);
	size = buf[6];
	SIZE(8 + size);
	sprintf(message,"%s : ",sd->obj.ud.id);
	strncat(message,(char*)buf+7,size);
	object_say(&sd->obj.obj,buf[5],message);
	shell_event(ON_SAY,"iis",sd->obj.obj.id,buf[5],buf + 7);
	return 0;
}
int pc_side(BSocket* sd,BYTE* buf)
{
	SIZE(6);
	object_side(&sd->obj.obj,buf[5]);
	shell_event(ON_SIDE,"ii",sd->obj.obj.id,buf[5]);
	return 0;
}
void object_show_single(BSocket* sd,struct mapobject* obj);
void pc_object_resend(BSocket* sd,int id)
{
	if(objects[id] == NULL)
	{
		wb(sd,0)= 170;
		ws(sd,1) = SWAP16(7);
		wb(sd,3) = 14;
		wb(sd,4) = 0;
		wl(sd,5) = SWAP32(id);
		wb(sd,9) = 0;
		cpt(sd);
	}else
	{
		object_show_single(sd,objects[id]);
	}
}
int pc_request(BSocket* sd,BYTE* buf)
{
	int idlen;
	SIZE(7);
	idlen= buf[6];
	SIZE(7+idlen);
	buf[7+idlen] = 0;

	if(buf[5] == 5) {
		shell_event(ON_REQUEST_TRADE,"is",sd->obj.obj.id,buf+7);
	}
	else if(buf[5] == 7)
	{
		shell_event(ON_REQUEST_GROUP,"is",sd->obj.obj.id,buf+7);
	}
	else return -1;
	return 0;
}
int pc_resend(BSocket* sd,BYTE* buf)
{
	SIZE(10);
	pc_object_resend(sd,UNSWAP32(buf+5));
	return 0;
}
int pc_dropitem(BSocket* sd,BYTE* buf)
{

	return 0;
}
int pc_click(BSocket* sd,BYTE* buf)
{
	struct mapobject* obj;
	int id;
	id = UNSWAP32(buf+6);
	if(sd->dlg_thread != NULL){
		sd->dlg_type = 0;
		thread_delete(sd->dlg_thread);
		sd->dlg_thread = NULL;
	}
	if(id < 0)
	{
		if(id == -1) {
			shell_event(15,"i",sd->obj.obj.id);
		}
		else if(id == -2)
		{
			shell_event(16,"i",sd->obj.obj.id);
		}
		else return -1;
		return 0;
	}
	if(id >= objects_size || (obj=objects[id]) == NULL) return -1;
	if(obj->type == USER)
	{
		user_showinfo(sd,(struct userobject*)obj);
	} 
	else if(obj->type == NPC)
	{
		npc_click((struct npcobject*)obj);
	}
	shell_event(ON_CLICK,"ii",sd->obj.obj.id,obj->id);
	return 0;
}
int pc_getxy(BSocket* sd,BYTE* buf)
{
	map_send_xy(sd,&sd->obj.obj.loc);
	return 0;
}
extern unsigned int login_ip;
extern unsigned short login_port;
int pc_logout(BSocket* sd,BYTE* buf)
{
	game_send_servermove(sd,login_ip,login_port);
	return 0;
}
int pc_lookfront(BSocket* sd,BYTE* buf)
{
	int x = sd->obj.obj.loc.x,y = sd->obj.obj.loc.y;
	struct mapobject* obj ;
	if(sd->obj.obj.loc.map == NULL) return 0;
	obj = object_first(sd->obj.obj.loc.map);
	switch(sd->obj.obj.loc.side)
	{
	case LEFT: x--; break;
	case RIGHT: x++; break;
	case UP: y--; break;
	case DOWN: y++; break;
	}
	while(obj)
	{
		if(obj->loc.x == x && obj->loc.y == y)
		{
			int namelen;
			char* name;
			name = object_get_name(obj,&namelen);
			if(obj->type == ITEM && ((struct itemobject*)obj)->db->itype < 14){
				char mes[256];
				struct itemobject* i = (struct itemobject*)obj;
				sprintf(mes,i->db->format,name,i->count);
				game_send_message(sd,3,mes);
			} else {	
				game_send_message(sd,3,name);
			}
		}
		obj = object_next(obj);
	}
	return 0;
}
int pc_slotswap(BSocket* sd,BYTE* buf)
{
	SIZE(8);
	if(buf[5] == 0) item_slot_swap(&sd->obj,buf[6]-1,buf[7]-1);
	else if(buf[5] == 1) magic_slot_swap(&sd->obj,buf[6]-1,buf[7]-1);
	else return -1;
	return 0;
}
int pc_teleport(BSocket* sd,BYTE* buf)
{
	int i;
	int len;
	int sx = sd->obj.obj.loc.x,sy = sd->obj.obj.loc.y;
	SIZE(8);
	len = buf[7];
	SIZE(len*2+8);
	for(i=0;i<len;i++)
	{
		switch(buf[8+i*2])
		{
		case LEFT:
			sx -= buf[9+i*2];
			break;
		case RIGHT:
			sx += buf[9+i*2];
			break;
		case DOWN:
			sy += buf[9+i*2];
			break;
		case UP:
			sy -= buf[9+i*2];
			break;
		}
	}
	magic_effect3(&sd->obj.obj,279,len,buf+8);
	object_sound(&sd->obj.obj,370);
	object_warp(&sd->obj.obj,sd->obj.obj.loc.map,sx,sy);
	return 0;
}

int pc_magic(BSocket* sd,BYTE* buf)
{
	int slot;
	struct magicslot* magic;
	struct mapobject* obj;
	SIZE(6);
	if(sd->obj.last_magictick + 300 >= clock()) return 0;
	
	slot = buf[5]-1;
	if(slot < 0 || slot >= MAGIC_SLOT_MAX || (magic = &sd->obj.ownmagics[slot]) == NULL) return -1;
	if(magic->next_time > clock())
	{
		int remain = (magic->next_time - clock())/1000;
		if(remain > 0)
		{
			char message[256];
			sprintf(message,"%d 초 남았습니다.",message);
			game_send_message(sd,3,message);
		}
		return 0;
	}
	switch(magic->db->type)
	{
	case magic_auto:
		shell_event(ON_USEMAGIC,"iiii",sd->obj.obj.id,magic->db->id,magic->lv,slot);
		break;
	case magic_target:
		SIZE(11);
		obj = getobj(UNSWAP32(buf+6));
		if(obj == NULL)
			object_hide_single2(sd,UNSWAP32(buf+6));
		else shell_event(ON_USEMAGIC,"iiiii",sd->obj.obj.id,magic->db->id,magic->lv,slot,obj->id);
		break;
	case magic_input:
		SIZE(7);
		shell_event(ON_USEMAGIC,"iiiis",sd->obj.obj.id,magic->db->id,magic->lv,slot,buf+6);
		break;
	}
	sd->obj.last_magictick = clock();
	return 0; 
}
int pc_dropmoney(BSocket* sd,BYTE* buf)
{
	unsigned int money;
	SIZE(10);
	money = UNSWAP32(buf+5);
	object_action(&sd->obj.obj,4,30,1);
	if(sd->obj.ud.money < money){
		money = sd->obj.ud.money;
	}
	
	drop_money(&sd->obj.obj.loc,money);
	sd->obj.ud.money -= money;
	user_status(&sd->obj,0x10);
	return 0;
}
int pc_whisper(BSocket* sd,BYTE* buf)
{
	int idlen = buf[5];
	int msglen;
	SIZE(idlen+7);
	msglen = buf[idlen+6];
	buf[idlen+6] = 0;
	SIZE(idlen+msglen+8);
	buf[idlen+msglen+7] = 0;
	return shell_event(ON_WHISPER,"ss",buf+6,buf+7+idlen);
}
int pc_itemdrop(BSocket* sd,BYTE* buf)
{
	SIZE(7);
	object_action(&sd->obj.obj,4,30,1);
	return item_drop_slot(&sd->obj,buf[5]-1,buf[6]);
}
int pc_worldmap(BSocket* sd,BYTE* buf)
{
	int i;
	int j;
	struct wmap_groupdb* g;
	struct wmapdb* wmap;
	if(sd->epf == NULL) return -1;
	else
	{
		i = UNSWAP16(buf+5);
		if(i >= sd->epf->group_count)
		{
			sd->eof=1;
			return ;
		}
		g = &sd->epf->groups[i];
		if(g == NULL)
		{
			sd->eof =1 ;
			return ;
		}
		wmap = list_entry(list_begin(&g->wlist),struct wmapdb,elem);
		i=0; j = UNSWAP16(buf+7);
		while(&wmap->elem != &g->wlist.tail){
			if(wmap->id==j) break;
			wmap = list_entry(list_next(&wmap->elem),struct wmapdb,elem);
			i++;
		}
		if(wmap==NULL)
		{
			return -1;
		}
		sd->epf = NULL;
		object_warp(&sd->obj.obj,wmap->loc.map,wmap->loc.x,wmap->loc.y);
	}
}
int pc_option(BSocket* sd,BYTE* buf)
{
	SIZE(7);
	switch(buf[6])
	{
	case 0:
		shell_event(ON_RIDING,"i",sd->obj.obj.id);
		break;
	case 11:
		shell_event(ON_PKPROTECT,"i",sd->obj.obj.id);
		break;
	case 2:
		shell_event(ON_GROUPON,"i",sd->obj.obj.id);
		break;
	case 8:
		shell_event(ON_TRADEON,"i",sd->obj.obj.id);
		break;
	case 14:
		shell_event(ON_HELMET,"i",sd->obj.obj.id);
		break;
	}
	return 0;
}
void event_init()
{
	LUA_EVENT_SET(ON_LOAD,"onOpen");
	LUA_EVENT_SET(ON_CLICK,"onClick");
	LUA_EVENT_SET(ON_SAY,"onSay");
	LUA_EVENT_SET(ON_SIDE,"onSide");
	LUA_EVENT_SET(ON_WALK,"onWalk");
	LUA_EVENT_SET(ON_PICKUP,"onPickup");
	LUA_EVENT_SET(ON_EMOTION,"onEmotion");
	LUA_EVENT_SET(ON_ATTACK,"onAttack");
	LUA_EVENT_SET(ON_TAKEON,"onTakeon");
	LUA_EVENT_SET(ON_TAKEOFF,"onTakeoff");
	LUA_EVENT_SET(ON_USEITEM,"onUseItem");
	LUA_EVENT_SET(ON_USEMAGIC,"onUseMagic");
	LUA_EVENT_SET(ON_DELETE,"onDelete");
	LUA_EVENT_SET(ON_LOGIN,"onLogin");
	LUA_EVENT_SET(ON_SECOND,"onSecond");
	LUA_EVENT_SET(ON_F1,"onF1");
	LUA_EVENT_SET(ON_F2,"onF2");
	LUA_EVENT_SET(ON_WHISPER,"onWhisper");
	LUA_EVENT_SET(ON_PICKUPMONEY,"onPickupMoney");
	LUA_EVENT_SET(ON_REQUEST_GROUP,"onRequestGroup");
	LUA_EVENT_SET(ON_GROUPON,"onGroupOnOff");
	LUA_EVENT_SET(ON_TRADEON,"onTradeOnOff");
	LUA_EVENT_SET(ON_PKPROTECT,"onPKProtect");
	LUA_EVENT_SET(ON_HELMET,"onHelmetVisibleOnOff");
	LUA_EVENT_SET(ON_REQUEST_TRADE,"onRequestTrade");
	LUA_EVENT_SET(ON_AI,"onAI");
	LUA_EVENT_SET(ON_ERROR,"onError");
	LUA_EVENT_SET(ON_LOCATION,"onLocation");

}
struct timer second_tm;
clock_t next_time;
long pc_second(struct timer* tm)
{
	//clock_t t = clock();
	shell_event(ON_SECOND,"");
	return 1000;
}
void pc_init()
{
	EVENT_SET(19,1,pc_attack);
	EVENT_SET(5,1,pc_send_map);
	EVENT_SET(6,1,pc_walk);
	EVENT_SET(50,1,pc_walk);
	EVENT_SET(14,1,pc_say);
	EVENT_SET(17,1,pc_side);
	EVENT_SET(12,1,pc_resend);
	EVENT_SET(29,1,pc_emotion);
	EVENT_SET(36,1,pc_dropmoney);
	EVENT_SET(28,1,pc_useitem);
	EVENT_SET(18,1,pc_takeon);
	EVENT_SET(45,1,pc_lookself);
	EVENT_SET(56,1,pc_refresh);
	EVENT_SET(31,1,pc_takeoff);
	EVENT_SET(7,1,pc_pickup);
	EVENT_SET(23,1,pc_throw);
	EVENT_SET(27,1,pc_option);
	EVENT_SET(67,1,pc_click);
	EVENT_SET(105,1,pc_getxy);
	EVENT_SET(11,1,pc_logout);
	EVENT_SET(57,1,pc_dialog);
	EVENT_SET(58,1,pc_dialog);
	EVENT_SET(24,1,pc_userlist);
	EVENT_SET(15,1,pc_magic);
	EVENT_SET(9,0,pc_lookfront);
	EVENT_SET(8,1,pc_itemdrop);
	EVENT_SET(36,1,pc_dropmoney);
	EVENT_SET(11,1,pc_logout);
	EVENT_SET(25,1,pc_whisper);
	EVENT_SET(48,1,pc_slotswap);
	EVENT_SET(131,1,pc_teleport);
	EVENT_SET(74,1,pc_trade);
	EVENT_SET(49,1,pc_request);
	EVENT_SET(27,1,pc_option);
	EVENT_SET(63,1,pc_worldmap);
	event_init();
	second_tm.data = NULL;
	second_tm.wakeup = pc_second;
	next_time = second_tm.time = clock();
	timer_add(&second_tm);
}