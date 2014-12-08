#include "script.h"
#include "npc.h"
#include "debug.h"
#include "socket.h"
#include "game.h"
#include "group.h"
#include "world.h"
#include "map.h"
#include "mob.h"
#include "sql.h"
#include "pet.h"
#include "magic.h"
#include "sformat.h"
#define GETi(a,i,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isnumber(l,a)) luaL_typerror(l,a,"integer"); i = lua_tointeger(l,a); }
#define GETd(a,d,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isnumber(l,a)) luaL_typerror(l,a,"number"); d = lua_tonumber(l,a); }
#define GETs(a,s,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isstring(l,a)) luaL_typerror(l,a,"string"); s = (char*)lua_tostring(l,a); }
#define GETb(a,b,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isboolean(l,a)) luaL_typerror(l,a,"boolean"); b = lua_toboolean(l,a); }
#define BLOCK_CHECK() {if(block_lock) { luaL_error(lua,"블럭이 걸어져 있어서 사용 불가능."); return 0;}}
#define GETc(a,c,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isnumber(l,a)) luaL_typerror(l,a,"integer"); c = (char)lua_tointeger(l,a); }
#define GETw(a,w,l,n) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_isnumber(l,a)) luaL_typerror(l,a,"integer"); w = (short)lua_tointeger(l,a); }
#define GETu(a,u,l,n,t) { if(lua_gettop(lua) < a) luaL_argerror(lua,a,n); else if ( !lua_islightuserdata(l,a)) luaL_typerror(l,a,"light user data"); u = (t*)lua_topointer(l,a); }
#define _GETi(a,i) GETi(a,i,lua,"#i")
#define _GETd(a,d) GETd(a,d,lua,"#d")
#define _GETs(a,s) GETs(a,s,lua,"#s")
#define _GETb(a,b) GETb(a,b,lua,"#b")
#define _GETc(a,c) GETc(a,c,lua,"#c")
#define _GETw(a,w) GETw(a,w,lua,"#w")
#define _GETu(a,u,t) GETu(a,u,lua,"#u",t)

struct map* get_map(lua_State* lua,int mapid,int arg)
{
	struct map* m;
	if( mapid >= MAP_DB_MAX || ((m = maps[mapid]) == NULL))
	{
		errf("[script error] %d는 올바른 맵아이디가 아닙니다\n",mapid);
		luaL_argerror(lua,arg,"map id");
		return NULL;
	}
	return m;
}
struct mapobject* getod(unsigned char type,lua_State* lua,int objid,int arg,char* name)
{
	struct mapobject* obj;
	if(objid >= objects_size || (obj=objects[objid]) == NULL) {
		errf("[script error] %d는 올바른 오브젝트 id가 아닙니다\n",objid);
		luaL_argerror(lua,arg,name);
		return NULL;
	}
	if(type != NONE && !(type & obj->type))
	{
		errf("[script error] 오브젝트 타입(%d)이 올바르지 않습니다\n",objid);
		luaL_argerror(lua,arg,name);
		return NULL;
	}
	return obj;
}

struct mobdb* get_mobdb(lua_State* lua,int arg,int code)
{
	if(code >= MOB_DB_MAX || mobs[code] == NULL) {
		errf("[script error] %d는 올바른 몬스터 db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"mob db code");
	}
	return mobs[code];
}
struct npcdb* get_npcdb(lua_State* lua,int arg,int code)
{
	if(code >= NPC_DB_MAX || npcs[code] == NULL) {
		errf("[script error] %d는 올바른 엔피씨 db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"npc db code");
	}
	return npcs[code];
}
struct itemdb* get_itemdb(lua_State* lua,int arg,int code)
{
	if(code >= ITEM_MAX || items[code] == NULL) {
		errf("[script error] %d는 올바른 아이템 db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"item db code");
	}
	return items[code];
}
struct mapdb* get_mapdb(lua_State* lua,int arg,int code)
{
	if(code >= MAP_DB_MAX || mapdbs[code] == NULL)
	{
		errf("[script error] %d는 올바른 맵 db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"map db code");
	}
	return mapdbs[code];
}
MYSQL* get_sqldb(lua_State* lua,int arg,int code)
{
	if(code >= SQL_MAX || sqldb[code] == NULL)
	{
		errf("[script error] %d는 올바른 sql db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"sql db code");
	}
	return sqldb[code];
}
struct magicdb* get_magicdb(lua_State* lua,int arg,int code)
{
	if(code >= MAGIC_DB_MAX || magics[code] == NULL)
	{
		errf("[script error] %d는 올바른 magic db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"magic db code");
	}
	return magics[code];
}
struct epfdb* get_epfdb(lua_State* lua,int arg,int code)
{
	if(code >= EPF_DB_MAX || epfdbs[code] == NULL)
	{
		errf("[script error] %d는 올바른 epf db 코드가 아닙니다\n",code);
		luaL_argerror(lua,arg,"epf db code");
	}
	return epfdbs[code];

}
#define toGROUP(a,o) {struct userobject* _temp_obj;toUSER(a,_temp_obj);  if((o = _temp_obj->group) == NULL) {luaL_argerror(lua,a,"group id"); return 0;}}
#define toOBJ(tp,a,o) {int __temp_id; GETi(a,__temp_id,lua,"object id"); o = (getod(tp,lua,__temp_id,a,"user"));}
#define toUSER(a,o) {int __temp_id; GETi(a,__temp_id,lua,"user id"); o = ((struct userobject*)getod(USER,lua,__temp_id,a,"user"));}
#define toMOB(a,o) {int __temp_id; GETi(a,__temp_id,lua,"mob id"); o = ((struct mobobject*)getod(MONSTER,lua,__temp_id,a,"user"));}
#define toNPC(a,o) {int __temp_id; GETi(a,__temp_id,lua,"npc id"); o = ((struct npcobject*)getod(NPC,lua,__temp_id,a,"npc"));}
#define toITEM(a,o) {int __temp_id; GETi(a,__temp_id,lua,"item id"); o = ((struct itemobject*)getod(ITEM,lua,__temp_id,a,"item"));}
#define toMAP(a,m) {int __temp_id; GETi(a,__temp_id,lua,"map id"); m = get_map(lua,__temp_id,a);}
#define toFORMAT(a,s) {int __temp_id; GETi(a,__temp_id,lua,"format id"); if((s = getformat(__temp_id)) == NULL){ luaL_argerror(lua,a,"format id"); return 0;} }
#define toMOBDB(a,d) {int __temp_id; GETi(a,__temp_id,lua,"mob code"); d = ((struct mobdb*)get_mobdb(lua,a,__temp_id));}
#define toNPCDB(a,d) {int __temp_id; GETi(a,__temp_id,lua,"npc code"); d = ((struct npcdb*)get_npcdb(lua,a,__temp_id));}
#define toITEMDB(a,d) {int __temp_id; GETi(a,__temp_id,lua,"item code"); d = ((struct itemdb*)get_itemdb(lua,a,__temp_id));}
#define toMAPDB(a,d) {int __temp_id; GETi(a,__temp_id,lua,"map code"); d = ((struct mapdb*)get_mapdb(lua,a,__temp_id));}
#define toFUNC(f) {if( lua_gettop(lua) < 1 || !lua_isfunction(lua,-1) ) { luaL_argerror(lua,-1,"function"); return 0;} ; f = luaL_ref(lua,LUA_REGISTRYINDEX);}
#define toTABLE(t) {if( lua_gettop(lua) < 1 || !lua_istable(lua,-1) ) { luaL_argerror(lua,-1,"table"); return 0;} ; t = luaL_ref(lua,LUA_REGISTRYINDEX);}
#define toREF(r) { if(lua_gettop(lua) < 1 )  { luaL_argerror(lua,-1,"data"); return 0;}; r = luaL_ref(lua,LUA_REGISTRYINDEX);}
#define toSQL(a,s) { int __temp_id; GETi(a,__temp_id,lua,"sql id"); s = get_sqldb(lua,a,__temp_id);}
#define toMAGIC(a,m) {int __temp_id; GETi(a,__temp_id,lua,"magic id"); m = get_magicdb(lua,a,__temp_id);}
#define toEPF(a,e)  {int __temp_id; GETi(a,__temp_id,lua,"epf id"); e = get_epfdb(lua,a,__temp_id);}
#define PUSHi(i) (lua_pushinteger(lua,i))

#define PUSHb(b) (lua_pushboolean(lua,b))
#define PUSHs(s) (lua_pushstring(lua,s))
#define PUSHd(d) (lua_pushnumber(lua,d))
#define PUSHn() (lua_pushnil(lua))

BSocket* getsd(lua_State* lua,int n,char* name)
{
	struct userobject* uobj;
	toUSER(n,uobj);
	return obj2sd(uobj);
}
#define toSD(n) ((BSocket*)getsd(lua,n,"user"))


int scriptfunc_sound(lua_State* lua)
{
	struct mapobject* obj;
	int sound;
	toOBJ(MONSTER | USER | NPC | ITEM | PET,1,obj);
	GETi(2,sound,lua,"sound");
	object_sound(obj,sound);
	return 0;
}
int scriptfunc_action(lua_State* lua)
{
	struct mapobject* obj;
	int time;
	int sound;
	int type;
	toOBJ(MONSTER | USER | NPC | PET,1,obj);
	GETi(2,type,lua,"type");
	GETi(3,time,lua,"time");
	GETi(4,sound,lua,"sound");
	object_action(obj,type,time,sound);
	return 0;
}
int scriptfunc_sleep(lua_State* lua)
{
	long time;
	_GETi(1,time);
	if(time >= 0){
		BLOCK_CHECK();
		lua_pushinteger(lua,yield_sleep);
		return lua_yield(lua,2);
	}
	return 0;
}
 
int scriptfunc_msg(lua_State* lua)
{
	BSocket* sd;
	int type;
	char* msg;
	sd = toSD(1);
	GETi(2,type,lua,"message type");
	GETs(3,msg,lua,"message");
	game_send_message(sd,type,msg);
	return 0;
}
int scriptfunc_candamage(lua_State* lua)
{
	struct mapobject* obj;
	struct mapobject* target;
	toOBJ(OBJALL,1,obj);
	toOBJ(OBJALL,2,target);
	PUSHi(can_damage(obj,target));
	return 1;
}
int scriptfunc_dialog(lua_State* lua)
{
	unsigned short image;
	unsigned char color;
	int obj,wide;
	char* msg;
	int prev,next;
	BSocket* sd;
	//obj img col wide msg next prev
	BLOCK_CHECK();

	sd = toSD(1);
	GETi(2,image,lua,"image");
	GETi(3,color,lua,"color");
	GETs(4,msg,lua,"message");
	GETb(5,wide,lua,"wide");
	GETb(6,next,lua,"next");
	GETb(7,prev,lua,"prev");
	dialog(sd,image,color,wide,msg,prev,next);
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	return lua_yield(lua,2);
}
int scriptfunc_effect(lua_State* lua)
{
	struct mapobject* obj;
	int effect;
	int time;
	toOBJ(OBJLIVE,1,obj);
	GETi(2,effect,lua,"effect number");
	GETi(3,time,lua,"effect time");
	object_effect(obj,effect,time);
	return 0;
}
int scriptfunc_gage(lua_State* lua)
{
	struct mapobject* obj;
	int critical;
	int dam;
	double percent;
	toOBJ(OBJLIVE,1,obj);
	GETi(2,dam,lua,"damage");
	GETd(3,percent,lua,"percent");
	GETb(4,critical,lua,"critical");
	object_gage(obj,percent*100,critical,dam);
	return 0;
}
int scriptfunc_effect_xy(lua_State* lua)
{
	struct location loc;
	int effect;
	int time;
	toMAP(1,loc.map);
	GETi(2,loc.x,lua,"x");
	GETi(3,loc.y,lua,"y");
	GETi(4,effect,lua,"effect number");
	GETi(5,time,lua,"effect time");
	map_effect_xy(&loc,effect,time);
	return 0;
}
int scriptfunc_checkitem(lua_State* lua)
{
	struct userobject* user;
	struct itemdb* idb;
	int count,i;
	toUSER(1,user);
	toITEMDB(2,idb);
	_GETi(3,count);
	for(i=0;i<ITEM_SLOT_MAX;i++){
		if(user->ownitems[i] != NULL && user->ownitems[i]->db == idb)
		{
			if(user->ownitems[i]->count < count)
			{
				if(idb->option & overlap) {
					count -= user->ownitems[i]->count;
				} else {
					PUSHn();
					return 1;
				}
			}
			else {
				PUSHi(user->ownitems[i]->obj.id);
				return 1;
			}
		}
	}
	PUSHn();
	return 1;
}
int scriptfunc_list(lua_State* lua)
{
	unsigned short image;
	unsigned char color;
	int obj,wide;
	char* msg;
	int prev,next;
	char* menu[512];
	int i,count;
	BSocket* sd;
	//obj img col wide msg next prev
	BLOCK_CHECK();
	sd = toSD(1);
	GETi(2,image,lua,"image");
	GETi(3,color,lua,"color");
	GETs(4,msg,lua,"message");
	GETb(5,wide,lua,"wide");
	GETb(6,next,lua,"next");
	GETb(7,prev,lua,"prev");
	count = lua_gettop(lua) - 7;
	for(i=0;i<count;i++) {
		GETs(i+8,menu[i],lua,"menu");
	}
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	select_list(sd,image,color,wide,msg,count,menu,prev,next);
	return lua_yield(lua,2);
}
int scriptfunc_getobj_xy(lua_State* lua)
{
	struct location loc;
	struct mapobject* obj;
	struct _list_elem* elem;
	struct _list* p;
	int cnt = 0;
	toMAP(1,loc.map);
	GETi(2,loc.x,lua,"x");
	GETi(3,loc.y,lua,"y");
	 p = pos_list_xy(loc.map,loc.x,loc.y);
	 if(p == NULL) return 0;
	elem = list_begin(p);
	while(elem != &p->tail){
		obj = list_entry(elem,struct mapobject,pos_elem);
		PUSHi(obj->id);
		cnt++;
		elem = list_next(elem);
	}
	return cnt;
}
int scriptfunc_getside(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(OBJLIVE,1,obj);
	PUSHi(obj->loc.side);
	return 1;
}
int scriptfunc_setside(lua_State* lua)
{
	struct mapobject* obj;
	int side;
	toOBJ(OBJLIVE,1,obj);
	GETi(2,side,lua,"side");
	object_side(obj,side);
	return 0;
}
int scriptfunc_getpos(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(OBJALL,1,obj);
	if (obj->loc.map != NULL ) {
		PUSHi(obj->loc.map->id);
	}
	else {
		PUSHn();
	}
	PUSHi(obj->loc.x);
	PUSHi(obj->loc.y);
	return 3;
}
#define UOBJ(o) ((struct userobject*)(o))
#define MOBJ(o) ((struct mobobject*)(o))
#define POBJ(o) ((struct petobject*)(o))
#define IOBJ(o) ((struct itemobject*)(o))
#define NOBJ(o) ((struct npcobject*)(o))

int scriptfunc_sethp(lua_State* lua)
{
	struct userobject* obj;
	unsigned int hp;
	toUSER(1,obj);
	GETi(2,hp,lua,"hp");
	obj->ud.hp = hp;
	user_status(obj,0x20);
	return 0;
}
int scriptfunc_setmp(lua_State* lua)
{
	struct userobject* obj;
	unsigned int mp;
	toUSER(1,obj);
	GETi(2,mp,lua,"mp");
	obj->ud.mp = mp;
	user_status(obj,0x20);
	return 0;
}
int scriptfunc_setmaxhp(lua_State* lua)
{
	struct userobject* obj;
	unsigned int hp;
	toUSER(1,obj);
	GETi(2,hp,lua,"maxhp");
	obj->ud.maxhp = hp;
	user_status(obj,0x40);
	return 0;
}
int scriptfunc_teleport(lua_State* lua)
{
	BSocket* sd;
	unsigned char dist;
	sd = toSD(1);
	_GETc(2,dist);
	magic_teleport(sd,dist);
	return 0;
}
int scriptfunc_setmaxmp(lua_State* lua)
{
	struct userobject* obj;
	unsigned int mp;
	toUSER(1,obj);
	GETi(2,mp,lua,"maxmp");
	obj->ud.maxmp = mp;
	user_status(obj,0x40);
	return 0;
}
int scriptfunc_gettype(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(OBJALL,1,obj);
	PUSHi(obj->type);
	return 1;
}
int scriptfunc_setmobdb(lua_State* lua)
{
	int code;
	int aitype;
	char* name;
	int img,col,tp;
	int speed;
	GETi(1,code,lua,"mob db code");
	if(code >= MOB_DB_MAX)
	{
		errf("몹 db 코드 번호를 초과했습니다 (%d).\n",code);
		luaL_argerror(lua,1,"mob db code");
	}
	GETs(2,name,lua,"mob name");
	GETi(3,img,lua,"mob image");
	GETi(4,col,lua,"mob col");
	GETi(5,aitype,lua,"aitype");
	GETi(6,speed,lua,"speed");
	register_mobdb(code,name,img,col,aitype,speed);
	return 0;
}
int scriptfunc_posfirst(lua_State* lua)
{
	struct mapobject* obj;
	struct map* m;
	int cnt = 0;
	struct _list_elem* elem;
	int x,y , type;
	struct _list *l;
	toMAP(1,m);
	_GETi(2,x);
	_GETi(3,y);
	_GETi(4,type);
	
	l = pos_list_xy(m,x,y);
	elem = list_begin(l);
	while(elem != &l->tail){
		obj = list_entry(elem,struct mapobject,pos_elem);
		if(obj->type & type){
			PUSHi(obj->id);
			return 1;
		}
		elem = list_next(elem);
	}
	PUSHn();
	return 1;
}
int scriptfunc_setequip(lua_State* lua)
{
	struct userobject* user;
	struct itemobject* obj;
	int slot;
	toUSER(1,user);
	toITEM(2,obj);
	slot = obj->db->itype;
	if(slot < 0 || slot >= EQUIP_SLOT_MAX) return 0;
	obj->owner = user;
	obj->slot = slot;
	user->equips[slot] = obj;

	equip_show(obj2sd(user),slot);
	return 0;
}
int scriptfunc_posnext(lua_State* lua)
{
	int type;
	struct mapobject* obj;
	struct _list* l;
	struct _list_elem* elem;
	toOBJ(OBJALL,1,obj);
	_GETi(2,type);
	l = pos_list(obj);
	elem = list_next(&obj->pos_elem);
	while(elem != &l->tail){
		obj = list_entry(elem,struct mapobject,pos_elem);
		if(obj->type & type){
			PUSHi(obj->id);
			return 1;
		}
		elem = list_next(elem);
	}
	PUSHn();
	return 1;
}
int scriptfunc_posprev(lua_State* lua)
{
	int type;
	struct mapobject* obj;
	struct _list* l;
	struct _list_elem* elem;
	toOBJ(OBJALL,1,obj);
	_GETi(2,type);
	l = pos_list(obj);
	elem = list_prev(&obj->pos_elem);
	while(elem != &l->head){
		obj = list_entry(elem,struct mapobject,pos_elem);
		if(obj->type & type){
			PUSHi(obj->id);
			return 1;
		}
		elem = list_prev(elem);
	}
	PUSHn();
	return 1;
}
int scriptfunc_poslast(lua_State* lua)
{
	struct mapobject* obj;
	struct map* m;
	struct _list_elem* elem;
	int x,y , type;
	struct _list *l;
	toMAP(1,m);
	_GETi(2,x);
	_GETi(3,y);
	_GETi(4,type);
	
	l = pos_list_xy(m,x,y);
	elem = list_end(l);
	while(elem != &l->head){
		obj = list_entry(elem,struct mapobject,pos_elem);
		if(obj->type & type){
			PUSHi(obj->id);
			return 1;
		}
		elem = list_prev(elem);
	}
	PUSHn();
	return 1;
}
int scriptfunc_getrectobjects(lua_State* lua)
{
	struct mapobject* od;
	int x1,y1,x2,y2,type,cnt=0;
	struct map* m;
	toMAP(1,m);
	_GETi(2,x1);
	_GETi(3,y1);
	_GETi(4,x2);
	_GETi(5,y2);
	_GETi(6,type);
	od = object_first(m);
	while(od != NULL){
		if(od->type & type && od->loc.x >= x1 && od->loc.x <= x2 && od->loc.y >= y1 && od->loc.y <= y2)
		{
			PUSHi(od->id);
			cnt++;
		}
		od = object_next(od);
	}
	return cnt;
}
int scriptfunc_getsightobjects(lua_State* lua)
{
	struct mapobject* obj;
	struct mapobject* od;
	int type,cnt = 0;
	toOBJ(OBJALL,1,obj);
	_GETi(2,type);
	od = object_first(obj->loc.map);
	while(od != NULL)
	{
		if ( od->type & type  && check_sight(&obj->loc,&od->loc))
		{
			PUSHi(od->id);
			cnt++;
		}
		od = object_next(od);
	}
	return cnt;
}
int scriptfunc_setnpcdb(lua_State* lua)
{
	int code;
	char* name;
	int img,col;
	GETi(1,code,lua,"npc db code");
	if(code >= NPC_DB_MAX)
	{
		errf("npc db 코드 번호를 초과했습니다 (%d).\n",code);
		luaL_argerror(lua,1,"npc db code");
	}
	GETs(2,name,lua,"npc name");
	GETi(3,img,lua,"npc image");
	GETi(4,col,lua,"npc color");
	register_npcdb(code,name,img,col);
	return 0;
}
int scriptfunc_setimsg(lua_State* lua)
{
	struct itemobject* obj;
	int msg;
	toITEM(1,obj);
	_GETi(2,msg);
	obj->item_msg = msg;
	return 0;
}
int scriptfunc_setitemdb(lua_State* lua)
{
	int code;
	char* name;
	int itype;
	char* format;
	int option;
	int max_count,image,color;
	int eqp_image,eqp_color[3];
	GETi(1,code,lua,"item code");
	if(code >= ITEM_MAX + MONEY_ITEM_MAX)
	{
		errf("item db 코드 번호를 초과했습니다 (%d).\n",code);
		luaL_argerror(lua,1,"item db code");
	}
	GETi(2,itype,lua,"item type");
	GETs(3,name,lua,"item name");
	toFORMAT(4,format);
	
	GETi(5,image,lua,"item image");
	GETi(6,color,lua,"item color"); 
	GETi(7,max_count,lua,"item max count");
	GETi(8,option,lua,"item option");

	register_itemdb(code,itype,overlap,name,format,image,color,max_count,option);
	return 0;
}
int scriptfunc_setequipdb(lua_State* lua)
{
	int code;
	int opt;
	int img;
	int atksound;
	int col[3];
	GETi(1,code,lua,"item code");
	if(code >= ITEM_MAX || items[code] == NULL)
	{
		errf("item db 코드 번호를 초과했거나 등록되지 않은 아이템 코드입니다 (%d).\n",code);
		luaL_argerror(lua,1,"item db code");
	}
	GETi(2,img,lua,"equip image");
	GETi(3,col[0],lua,"equip color1");
	GETi(4,col[1],lua,"equip color2");
	GETi(5,col[2],lua,"equip color3");
	GETi(6,opt,lua,"equip option");
	GETi(7,atksound,lua,"equip sound attack");
	register_equipdb(code,img,col[0],col[1],col[2],opt,atksound);
	return 0;
}
int scriptfunc_walk(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(OBJLIVE,1,obj);
	PUSHi(object_walk(obj));
	return 0;
}
int scriptfunc_warp(lua_State* lua)
{
	struct mapobject* obj;
	struct location loc;
	toOBJ(OBJALL,1,obj);
	toMAP(2,loc.map);
	GETi(3,loc.x,lua,"x");
	GETi(4,loc.y,lua,"y");
	object_warp(obj,loc.map,loc.x,loc.y);
	return 0;
}
int scriptfunc_newitemobj(lua_State* lua)
{
	struct itemobject* obj;
	struct itemdb* idb;
	toITEMDB(1,idb);
	obj = create_itemobj(idb->id);

	PUSHi(obj->obj.id);
	return 1;
}
int scriptfunc_newmobobj(lua_State* lua)
{
	struct mobobject* obj;
	struct mobdb* mdb;
	toMOBDB(1,mdb);
	obj = create_mobobj(mdb->id);

	PUSHi(obj->obj.id);
	return 1;
}
int scriptfunc_newnpcobj(lua_State* lua)
{
	struct npcobject* obj;
	struct npcdb* ndb;
	toNPCDB(1,ndb);
	obj = create_npcobj(ndb->id);

	PUSHi(obj->obj.id);
	return 1;
}
int scriptfunc_getequip(lua_State* lua)
{
	
	struct userobject* obj;
	char slot;
	toUSER(1,obj);
	_GETc(2,slot);
	if(slot >= EQUIP_SLOT_MAX || slot < 0 || obj->equips[slot] == NULL) {
		PUSHn();
	}
	else PUSHi(obj->equips[slot]->obj.id);
	return 1;
}
int scriptfunc_getslotitem(lua_State* lua)
{
	struct userobject* obj;
	char slot;
	toUSER(1,obj);
	_GETc(2,slot);
	if(slot >= ITEM_SLOT_MAX || slot < 0 || obj->ownitems[slot] == NULL) {
		PUSHn();
	}
	else PUSHi(obj->ownitems[slot]->obj.id);
	return 1;
}
int scriptfunc_giveitem(lua_State* lua)
{
	struct userobject* uobj;
	struct itemobject* iobj;
	int remain;
	toUSER(1,uobj);
	toITEM(2,iobj);
	GETb(3,remain,lua,"remain");
	PUSHi(item_give(uobj,iobj,remain ? TRUE : FALSE));
	return 1;
}
int scriptfunc_giveitem2(lua_State* lua){
	struct userobject* uobj;
	struct itemdb* idb;
	int count,remain;
	toUSER(1,uobj);
	toITEMDB(2,idb);
	GETi(3,count,lua,"item count");
	GETb(4,remain,lua,"item remain");
	PUSHi(item_give2(uobj,idb->id,count,remain ? TRUE : FALSE));
	return 1;
}
int scriptfunc_seticount(lua_State* lua)
{
	struct itemobject* obj;
	int cnt;
	toITEM(1,obj);
	GETi(2,cnt,lua,"count");
	PUSHi(item_seticount(obj,cnt));

	return 0;
}
int scriptfunc_geticount(lua_State* lua)
{
	struct itemobject* obj;
	toITEM(1,obj);
	PUSHi(obj->count);
	return 1;
}
int scriptfunc_setmapdb(lua_State* lua)
{
	int code;
	char* mapfile;
	char* blockfile;
	int bgm;
	char* name;
	int effect;
	GETi(1,code,lua,"map code");
	if(code >= MAP_DB_MAX) {
		errf("map db 코드가 최대값을 초과했습니다 (%d)\n",code);
		luaL_argerror(lua,1,"map code");
		return 0;
	}
	GETs(2,name,lua,"map name");
	GETs(3,mapfile,lua,"map file");
	GETs(4,blockfile,lua,"block file");
	GETi(5,bgm,lua,"bgm");
	GETi(6,effect,lua,"map effect");
	PUSHb(load_map(code,mapfile,blockfile,name,bgm,effect)!=1);
	return 1;
}
int scriptfunc_setmap(lua_State* lua)
{
	int code;
	int id;
	GETi(1,id,lua,"map id");
	if ( id >= MAP_MAX) {
		errf("map id가 최대값을 초과했습니다 (%d) \n",id);
		luaL_argerror(lua,1,"map id");
		return 0;
	}
	GETi(2,code,lua,"map code");
	if(code >= MAP_DB_MAX) {
		errf("map db 코드가 최대값을 초과했습니다 (%d)\n",code);
		luaL_argerror(lua,1,"map code");
		return 0;
	}
	map_set(id,code);
	return 0;
}
int scriptfunc_setlocscript(lua_State* lua)
{
	int id;
	struct location loc;
	_GETi(1,id);
	toMAP(2,loc.map);
	_GETi(3,loc.x);
	_GETi(4,loc.y);
	lscript_set(id,&loc);
	return 0;
}
int scriptfunc_getdbid(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(MONSTER | ITEM | NPC | PET,1,obj);
	switch(obj->type)
	{
	case MONSTER:
		PUSHi(MOBJ(obj)->db->id);
		break;
	case NPC:
		PUSHi(NOBJ(obj)->db->id);
		break;
	case ITEM:
		PUSHi(IOBJ(obj)->db->id);
		break;
	case PET:
		PUSHn();
		break;
	}
	return 1;
}
int scriptfunc_getmapsize(lua_State* lua)
{
	struct map* m;
	toMAP(1,m);
	PUSHi(m->db->xs);
	PUSHi(m->db->ys);
	return 2;
}
int scriptfunc_damage(lua_State* lua)
{
	struct mapobject* obj;
	struct mapobject* target;
	int dam;
	toOBJ(PET | USER | MONSTER,1,obj);
	toOBJ(PET | USER | MONSTER,2,target);
	GETi(3,dam,lua,"damage");
	object_damage(obj,target,dam);
	return 0;
}
int scriptfunc_setwarp(lua_State* lua)
{
	struct location loc1,loc2;
	int joblv,lv;
	toMAP(1,loc1.map);
	GETw(2,loc1.x,lua,"warp location x1");
	GETw(3,loc1.y,lua,"warp location y1");
	toMAP(4,loc2.map);
	GETw(5,loc2.x,lua,"warp location x2");
	GETw(6,loc2.y,lua,"warp location y2");
	GETc(7,loc2.side,lua,"warp location side");
	GETi(8,lv,lua,"warp level");
	GETi(9,joblv,lua,"warp job level");
	warp_add(loc1.map,loc1.x,loc1.y,loc2.map,loc2.x,loc2.y,loc2.side,lv,joblv);
//	malloc(50);
	return 0;

}
int scriptfunc_setmagicdb(lua_State* lua)
{
	int code;
	char* name;
	char* msg = NULL;
	int type;
	GETi(1,code,lua,"magic code");
	if(code >= MAGIC_DB_MAX)
	{
		errf("magic code가 최대값을 초과했습니다.(%d)\n",code);
		luaL_argerror(lua,1,"magic code");
		return 0;
	}
	GETi(2,type,lua,"magic type");
	GETs(3,name,lua,"magic name");
	if(type != magic_auto) GETs(4,msg,lua,"magic message");
	register_magicdb(code,type,name,msg);
	return 0;
}
int scriptfunc_equip_takeoff(lua_State* lua)
{
	struct userobject* obj;
	int slot;
	toUSER(1,obj);
	_GETi(2,slot);
	PUSHi(equip_takeoff(obj,slot));
	return 1;
}
int scriptfunc_equip_takeon(lua_State* lua)
{
	struct userobject* obj;
	int slot;
	toUSER(1,obj);
	_GETi(2,slot);
	equip_takeon(obj,slot);
	return 0;
}
int scriptfunc_givemagic(lua_State* lua)
{
	struct userobject* obj;
	int code;
	int lv;
	toUSER(1,obj);
	GETi(2,code,lua,"magic code");
	GETi(3,lv,lua,"magic lv");
	PUSHi(give_magic(obj,code,lv));
	return 1;
}
int scriptfunc_set_ownitem(lua_State* lua)
{
	struct userobject* obj;
	int slot;
	struct itemobject* iobj;
	toUSER(1,obj);
	_GETi(2,slot);
	toITEM(3,iobj);
	if(slot < 0 || slot >= ITEM_SLOT_MAX) return 0;
	iobj->owner = obj;
	iobj->slot = slot;
	obj->ownitems[slot] = iobj;
	item_slot_show(obj2sd(obj),slot);
	return 0;
}
int scriptfunc_set_magic(lua_State* lua)
{
	struct userobject* obj;
	int slot;
	struct magicdb* mg;
	int lv;
	toUSER(1,obj);
	_GETc(2,slot);
	if (slot < 0 || slot >= MAGIC_SLOT_MAX) {
		return 0;
	}
	toMAGIC(3,mg);
	_GETi(4,lv);
	obj->ownmagics[slot].db = mg;
	obj->ownmagics[slot].lv = lv;
	magic_slot_show(obj2sd(obj),slot);
	return 0;
}
int scriptfunc_not(lua_State* lua)
{
	int n;
	GETi(1,n,lua,"integer");
	PUSHi(~n);
	return 1;
}
int scriptfunc_or(lua_State* lua)
{
	unsigned int n = 0;
	int top = lua_gettop(lua);
	while(top)
	{
		int p = 0;
		_GETi(top,p);
		n |= p;
		top--;
	}
	PUSHi(n);
	return 1;
}
int scriptfunc_and(lua_State* lua)
{
	unsigned int n = 0;
	int top = lua_gettop(lua);
	int i,p = 0;
	_GETi(1,n);
	for(i=2;i<=top;i++){
		_GETi(i,p);
		n &= p;
	}
	PUSHi(n);
	return 1;
}
int scriptfunc_sformat_set(lua_State* lua)
{
	int code;
	char* format;
	GETi(1,code,lua,"string format code");
	GETs(2,format,lua,"string format");
	
	if(code >= SFORMAT_DB_MAX || code < 0) {
		errf("문자열 포맷 db 코드가 최대값을 초과했거나 알맞지 않습니다.(%d)\n",code);
		return 0;
	}
	register_format(code,format);
	return 0;
}

int scriptfunc_setuserdata(lua_State* lua)
{
	struct userobject* uobj;
	char* id,*title;
	struct userdata* ud;
	toUSER(1,uobj);
	ud = &uobj->ud;
	GETs(2,id,lua,"user id");
	GETs(3,title,lua,"title");
	strncpy(ud->id,id,ID_MAX);
	ud->id_len = strlen(ud->id);
	strncpy(ud->title,title,TITLE_MAX);
	ud->title_len = strlen(ud->title);
	_GETw(4,ud->face);
	_GETc(5,ud->hair);
	_GETc(6,ud->hair_color);
	_GETc(7,ud->gender);
	_GETc(8,ud->nation);
	_GETc(9,ud->totem);
	_GETc(10,uobj->obj.state);
	_GETc(11,ud->job);
	_GETc(12,ud->joblv);
	_GETc(13,ud->might);
	_GETc(14,ud->will);
	_GETc(15,ud->grace);
	_GETc(16,ud->level);
	_GETc(17,ud->ac);
	_GETc(18,ud->dam);
	_GETc(19,ud->hit);
	_GETw(20,ud->personality);
	_GETw(21,ud->vote);
	_GETi(22,ud->hp);
	_GETi(23,ud->maxhp);
	_GETi(24,ud->mp);
	_GETi(25,ud->maxmp);
	_GETi(26,ud->money);
	_GETi(27,ud->exp);
	_GETi(28,ud->exp_remain);
	_GETi(29,ud->exp_percent);

	_GETc(30,uobj->opt);
	user_status(uobj,120);
	return 0;
}
int scriptfunc_sethair(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.hair);
	_GETc(3,uobj->ud.hair_color);
	return 0;
}
int scriptfunc_setstate(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(OBJLIVE,1,obj);
	_GETc(2,obj->state);
	if(obj->type == USER)
		user_lookrefresh(UOBJ(obj));
	else if(obj->state == 1 && obj->type != ITEM)
		object_side(obj,255);
	return 0;
}
int scriptfunc_setjob(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.job);
	return 0;
}
int scriptfunc_setjoblv(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.joblv);
	return 0;
}
int scriptfunc_setgender(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.gender);
	user_lookrefresh(uobj);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setnation(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.nation);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_settotem(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.totem);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setface(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETw(2,uobj->ud.face);
	user_lookrefresh(uobj);
	return 0;
}

int scriptfunc_setwill(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.will);
	user_status(uobj,0x40);
	return 0;
}

int scriptfunc_setgrace(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.grace);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setmight(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.might);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setpureac(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.ac);
	return 0;
}
int scriptfunc_setlevel(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.level);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setdam(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.dam);
	return 0;
}
int scriptfunc_sethit(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETc(2,uobj->ud.hit);
	return 0;
}
int scriptfunc_settitle(lua_State* lua)
{
	struct userobject* uobj;
	char* title;
	toUSER(1,uobj);
	_GETs(2,title);
	strncpy(uobj->ud.title,title,TITLE_MAX);
	uobj->ud.title_len = strlen(uobj->ud.title);
	return 0;
}
int scriptfunc_setpersonality(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETw(2,uobj->ud.personality);
	user_status(uobj,0x40);
	return 0;
}
int scriptfunc_setvote(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETw(2,uobj->ud.vote);
	user_status(uobj,0x40);
	return 0;
}

int scriptfunc_setexp(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETi(2,uobj->ud.exp);
	_GETi(3,uobj->ud.exp_remain);
	_GETd(4,uobj->ud.exp_percent);

	user_status(uobj,0x10);
	return 0;
}
int scriptfunc_setmoney(lua_State* lua)
{
	struct userobject* uobj;
	toUSER(1,uobj);
	_GETi(2,uobj->ud.money);
	user_status(uobj,0x10);
	return 0;
}
int scriptfunc_blocklock(lua_State* lua)
{
	int d;
	_GETb(1,d);
	block_lock = d;
	return 0;
}
int scriptfunc_timeradd(lua_State* lua)
{
	unsigned int dref,fref;
	double time;
	_GETd(1,time);
	toREF(dref);
	toFUNC(fref);
	PUSHi(luatimer_add(time*1000,fref,dref));
	return 1;
}
int scriptfunc_timerdel(lua_State* lua)
{
	int id;
	_GETi(1,id);
	luatimer_del(id);
	return 0;
}
int scriptfunc_sql_conn(lua_State* lua)
{
	int code;
	char* ip;
	unsigned short port;
	char* id,*pw;
	char* name;
	_GETi(1,code);
	if(code >= SQL_MAX)
	{
		errf("sql code가 최대치를 초과했습니다\n");
		luaL_argerror(lua,1,"sql code");
	}
	_GETs(2,ip);
	_GETw(3,port);
	_GETs(4,id);
	_GETs(5,pw);
	_GETs(6,name);
	PUSHi(sql_connect(code,ip,id,pw,name,port));
	return 1;
}
int scriptfunc_sql_query(lua_State* lua)
{
	char* query;
	MYSQL* db;
	toSQL(1,db);
	_GETs(2,query);
	if(mysql_query(db,query))
	{
		errf("[Query Error] - %s\n",mysql_error(db));
		PUSHb(0);
	}
	PUSHb(1);
	return 0;
}
int scriptfunc_sql_store_result(lua_State* lua)
{
	MYSQL* db;
	toSQL(1,db);
	lua_pushlightuserdata(lua,mysql_store_result(db));
	return 1;
}
int scriptfunc_sql_free_result(lua_State* lua)
{
	MYSQL_RES* res;
	_GETu(1,res,MYSQL_RES);
	mysql_free_result(res);
	return 0;
}
int scriptfunc_sql_fetch_row(lua_State* lua)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	_GETu(1,res,MYSQL_RES);
	row = mysql_fetch_row(res);
	if(row) lua_pushlightuserdata(lua,row);
	else lua_pushnil(lua);
	return 1;
}
int scriptfunc_row_geti(lua_State* lua)
{
	MYSQL_ROW row;
	int d;
	_GETu(1,row,char*);
	_GETi(2,d);
	PUSHi(atoi(row[d]));
	return 1;
}
int scriptfunc_row_gets(lua_State* lua)
{
	MYSQL_ROW row;
	int d;
	_GETu(1,row,char*);
	_GETi(2,d);
	PUSHs((row[d]));
	return 1;
}
int scriptfunc_row_getb(lua_State* lua)
{
	MYSQL_ROW row;
	int d;
	_GETu(1,row,char*);
	_GETi(2,d);
	PUSHb(atoi(row[d]));
	return 1;
}

int scriptfunc_logout(lua_State* lua)
{
	BSocket* sd;
	sd = toSD(1);
	sd->eof = 1;
	return 0;
}
int scriptfunc_obj_die(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(MONSTER | PET | USER,1,obj);
	switch(obj->type)
	{
	case MONSTER:
		mob_die(MOBJ(obj));
		break;
	case PET:
		break;
	case USER:
		user_set_state(UOBJ(obj),dead);
		break;
	}
	return 0;
}
int scriptfunc_check_sight(lua_State* lua)
{
	struct mapobject* obj,*target;
	toOBJ(OBJALL,1,obj);
	toOBJ(OBJALL,2,target);
	PUSHb(check_sight(&obj->loc,&target->loc));
	return 1;
}
int scriptfunc_drop_money(lua_State* lua)
{
	struct location loc;
	unsigned int money;
	
	toMAP(1,loc.map);
	_GETi(2,loc.x);
	_GETi(3,loc.y);
	_GETi(4,money);
	drop_money(&loc,money);
	return 0;
}
int scriptfunc_job_max_set(lua_State* lua)
{
	int job,joblv;
	_GETi(1,job);
	_GETi(2,joblv);
	job_set(job,joblv);
	return 0;
}
extern struct job** jobs;
int scriptfunc_job_set(lua_State* lua)
{
	int top = lua_gettop(lua);
	char* jobname;
	int job;
	int joblv;
	_GETi(1,job);
	_GETi(2,joblv);
	_GETs(3,jobname);
	strncpy(jobs[job][joblv].name,jobname,JOB_NAME_MAX);
	jobs[job][joblv].name_len = strlen(jobs[job][joblv].name);
	return 0;
}
int scriptfunc_input(lua_State* lua)
{
	BSocket* sd;
	char* message;
	unsigned short image;
	unsigned char color;
	BLOCK_CHECK();

	sd = toSD(1);
	_GETw(2,image);
	_GETc(3,color);
	_GETs(4,message);
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	dialog_input(sd,image,color,message);
	
	return lua_yield(lua,2);
}
int scriptfunc_islotlist(lua_State* lua)
{
	char slot[ITEM_SLOT_MAX] = {0};
	BSocket* sd;
	char* message;
	int count = lua_gettop(lua)-4,i;
	unsigned short image;
	unsigned char color;
	BLOCK_CHECK();

	count = min(count,ITEM_SLOT_MAX);
	sd = toSD(1);
	_GETw(2,image);
	_GETc(3,color);
	_GETs(4,message);
	for(i=0;i<count;i++){
		char s;
		_GETc(5+i,s);
		if(s < ITEM_SLOT_MAX){
			slot[s] = 1;
		}
	}
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	select_islotlist(sd,image,color,message,slot);
	return lua_yield(lua,2);
}
int scriptfunc_say(lua_State* lua)
{
	struct mapobject* obj;
	int loud;
	char* message;
	toOBJ(OBJLIVE,1,obj);
	_GETi(2,loud);
	_GETs(3,message);
	object_say(obj,loud,message);
	return 0;
}
int scriptfunc_menu(lua_State* lua)
{
	char* menu[256];
	BSocket* sd;
	int count = (lua_gettop(lua)-4),i;
	char* message;
	unsigned short image;
	unsigned char color;
	BLOCK_CHECK();

	count = min(count,256);
	sd = toSD(1);
	_GETw(2,image);
	_GETc(3,color);
	_GETs(4,message);
	for(i=0;i<count;i++) {
		_GETs(i+5,menu[i]);
	}
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	select_menu(sd,image,color,message,count,menu);
	return lua_yield(lua,2);
}
int scriptfunc_can_move(lua_State* lua)
{
	struct map* m;
	int x,y;
	toMAP(1,m);
	_GETi(2,x);
	_GETi(3,y);
	if(x < 0 || y < 0 || x>= m->db->xs || y  >= m->db->ys) {
		PUSHb(0); return 1;
	}
	PUSHb(!BLOCK(m,x,y));
	return 1;
}
int scriptfunc_ilist(lua_State* lua)
{
	struct ilist_element elem[256];
	BSocket* sd;
	int count = (lua_gettop(lua)-4)/5, i,p;
	char* message;
	unsigned short image;
	unsigned char color;
	BLOCK_CHECK();

	count = min(count,256);
	sd = toSD(1);
	_GETw(2,image);
	_GETc(3,color);
	_GETs(4,message);   p = 5;
	for(i=0;i<count;i++){
		_GETs(p,elem[i].name); p++;
		_GETw(p,elem[i].img); p++;
		_GETc(p,elem[i].color); p++;
		_GETi(p,elem[i].price); p++;
		_GETs(p,elem[i].note); p++;
	}
	lua_pushinteger(lua,sd->handle);
	lua_pushinteger(lua,yield_dialog);
	select_ilist(sd,image,color,message,count,elem);
	
	return lua_yield(lua,2);
}
int scriptfunc_groupmake(lua_State* lua)
{
	struct userobject* master;
	struct userobject* user;
	toUSER(1,master);
	toUSER(2,user);
	group_make(master,user);
	return 0;
}
int scriptfunc_groupjoin(lua_State* lua)
{
	struct group* g;
	struct userobject* user;
	toGROUP(1,g);
	toUSER(2,user);
	group_join(g,user);
	return 0;
}
int scriptfunc_groupleave(lua_State* lua)
{
	struct userobject* obj;
	toUSER(1,obj);
	PUSHi(group_leave(obj));
	return 1;
}
int scriptfunc_groupmaster(lua_State* lua)
{
	struct group* g;
	toGROUP(1,g);
	if(g != NULL) PUSHi(group_master(g)->obj.id);
	else PUSHn();
	return 1;
}
int scriptfunc_set_uoption(lua_State* lua)
{
	struct userobject* user;
	toUSER(1,user);
	_GETi(2,user->opt);
	user_status(user,0x08);
	return 0;
}
int scriptfunc_trade(lua_State* lua)
{
	BSocket* sd;
	struct userobject* obj;
	sd = toSD(1);
	toUSER(2,obj);
	PUSHi(request_trade(sd,obj));
	return 1;
}
int scriptfunc_groupdelete(lua_State* lua)
{
	struct group* g;
	struct _list_elem* elem;
	toGROUP(1,g);
	elem = list_begin(&g->members);
	while(elem != &g->members.tail)
	{
		struct userobject* user = list_entry(user,struct userobject,group_elem);
		user->group = NULL;
		_list_remove(&user->group_elem);
		elem = list_next(elem);
	}
	 free(g);
	return 0;
}
int scriptfunc_groupmessage(lua_State* lua)
{
	struct group* g;
	struct userobject* user;
	int type;
	char* message;
	struct _list_elem* elem;
	toGROUP(1,g);
	_GETi(2,type);
	_GETs(3,message);
	elem = list_begin(&g->members);
	while(elem != &g->members.tail)
	{
		user = list_entry(elem,struct userobject,group_elem);
		game_send_message(obj2sd(user),type,message);
		elem = list_next(elem);
	}
	return 0;
}
int scriptfunc_magic_delay(lua_State* lua)
{
	BSocket* sd;
	int slot,time;
	sd = toSD(1);
	_GETi(2,slot);
	_GETi(3,time);
	if(slot >=0 && slot < MAGIC_SLOT_MAX){
		magic_delay2(sd,slot,time);
	}
	return 0;
}
int scriptfunc_magic_fall(lua_State* lua)
{
	BSocket* sd;
	int time;
	char* message;
	sd = toSD(1);
	_GETi(2,time);
	_GETs(3,message);
	magic_delay(sd,time,message);
	return 0;
}
int scriptfunc_consolecol(lua_State* lua)
{
	int bc,fc;
	_GETi(1,bc);
	_GETi(2,fc)
	SetColor(bc,fc);
	return 0;
}
int scriptfunc_eventinit(lua_State* lua)
{
	event_init();
	return 0;
}
int scriptfunc_settarget(lua_State* lua)
{
	struct mapobject* obj;
	struct mapobject* target;
	toOBJ(MONSTER | PET,1,obj);
	toOBJ(OBJLIVE,2,target);
	switch(obj->type){
	case MONSTER:
		MOBJ(obj)->target = target;
		break;
	case PET:
		break;

	}
	return 0;
}
int scriptfunc_setautoeffect(lua_State* lua)
{
	struct map* m;
	int x,y;
	unsigned short eff;
	toMAP(1,m);
	_GETw(2,x);
	_GETw(3,y);
	_GETw(4,eff);
	effect_add(m,x,y,eff);
	return 0;
}
static int global_i(lua_State* lua,char* name,int def){
	lua_getglobal(lua,name);
	if(lua_isnumber(lua,-1)){
		def = lua_tointeger(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
static int global_b(lua_State* lua,char* name,int def){
	lua_getglobal(lua,name);
	if(lua_isboolean(lua,-1)){
		def = lua_toboolean(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
static char* global_s(lua_State* lua,char* name,char* def){
	lua_getglobal(lua,name);
	if(lua_isstring(lua,-1)){
		def = (char*)lua_tostring(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
int scriptfunc_setlastobject(lua_State* lua)
{
	BSocket* sd;
	struct mapobject* obj;
	sd = toSD(1);
	toOBJ(OBJALL,2,obj);
	sd->last_object = obj;
	return 0;
}
int scriptfunc_message2(lua_State* lua)
{
	BSocket* sd;
	int timeout,fc,bc;
	char* message;
	sd = toSD(1);
	_GETi(2,bc);
	_GETi(3,fc);
	_GETs(4,message);
	_GETi(5,timeout);
	game_send_message2(sd,fc,bc,message,timeout);
	return 0;
}
int scriptfunc_map_message(lua_State* lua)
{
	struct map* m;
	char* message;
	char type;
	toMAP(1,m);
	_GETc(2,type);
	_GETs(3,message);
	map_msg(m,type,message);
	return 0;
}
int scriptfunc_map_message2(lua_State* lua)
{
	struct map* m;
	char* message;
	int fc,bc,timeout;
	toMAP(1,m);
	_GETi(2,bc);
	_GETi(3,fc);
	_GETs(4,message);
	_GETi(5,timeout);
	map_msg2(m,fc,bc,message,timeout);
	return 0;
}
int scriptfunc_world_message(lua_State* lua)
{
	char* message;
	char type;
	_GETc(1,type);
	_GETs(2,message);
	world_msg(type,message);
	return 0;
}
int scriptfunc_world_message2(lua_State* lua)
{
	char* message;
	int fc,bc,timeout;
	_GETi(1,bc);
	_GETi(2,fc);
	_GETs(3,message);
	_GETi(4,timeout);
	world_msg2(fc,bc,message,timeout);
	return 0;
}
int scriptfunc_gettarget(lua_State* lua)
{
	struct mapobject* obj;
	toOBJ(MONSTER | PET,1,obj);
	switch(obj->type)
	{
	case MONSTER:
		if(MOBJ(obj)->target)
			PUSHi(MOBJ(obj)->target->id);
		else
			PUSHn();
		break;
	default:
		PUSHn();
		break;
	}
	return 1;
}
int scriptfunc_setepf(lua_State* lua)
{
	int id;
	char* name;
	int gcnt;
	_GETi(1,id);
	if(id >= EPF_DB_MAX || id < 0){
		luaL_error(lua,"EPF 아이디를 초과했습니다. ( %d ) ",id);
		return 0;
	}
	_GETs(2,name);
	_GETi(3,gcnt);
	register_epfdb(name,id,gcnt);
	return 0;
}
int scriptfunc_setwmapdb(lua_State* lua)
{
	struct epfdb* epf;
	int group;
	int x,y,id;
	char* name;
	struct location loc;
	toEPF(1,epf);
	_GETi(2,group);
	_GETi(3,id);
	_GETi(4,x);
	_GETi(5,y);
	_GETs(6,name);
	toMAP(7,loc.map);
	_GETi(8,loc.x);
	_GETi(9,loc.y);
	register_wmapdb(epf,group,id,x,y,name,&loc);
	return 0;
}
int scriptfunc_wmapset(lua_State* lua)
{
	struct location loc;
	struct epfdb* epf;
	int id;
	int minlv,maxlv,joblv;
	toMAP(1,loc.map);
	_GETi(2,loc.x);
	_GETi(3,loc.y);
	toEPF(4,epf);
	_GETi(5,id);
	_GETi(6,minlv);
	_GETi(7,joblv);
	wmap_set(epf,id,&loc,minlv,joblv);
	return 0;
}
int scriptfunc_clannew(lua_State* lua)
{
	char* name,*title;
	struct userobject* obj;
	_GETs(1,name);
	_GETs(2,title);
	toUSER(3,obj);
	obj->clan = clan_new(name,title);
	_list_add(&obj->clan->members,&obj->clan_elem);
	obj->clan->member_count++;
	return 0;
}
int scriptfunc_clanadd(lua_State* lua)
{
	struct userobject* obj;
	struct userobject* od;
	toUSER(1,obj);
	toUSER(2,od);
	if(obj->clan == NULL){
		PUSHi(1);
		return 1;
	} else if(od->clan != NULL){
		PUSHi(-1);
		return 1;
	}
	else {
		od->clan = obj->clan;
		_list_add(&obj->clan->members,&od->clan_elem);
		obj->clan->member_count++;
		return 0;
	}
}
int scriptfunc_clandel(lua_State* lua)
{
	struct userobject* obj;
	toUSER(1,obj);
	if(obj->clan != NULL)
	{
		struct clan* cl = obj->clan;
		_list_remove(&obj->clan_elem);
		obj->clan = NULL;
		if(--cl->member_count == 0) {
			free(cl);
		}
	}
	return 0;
}
int scriptfunc_clanmaster(lua_State* lua)
{
	struct userobject* obj;
	toUSER(1,obj);
	if(obj->clan == NULL) {
		PUSHn();
		return 1;
	} else {
		obj = list_entry(list_begin(&obj->clan->members),struct userobject,clan_elem);
		PUSHi(obj->obj.id);
		return 1;
	}
}
int scriptfunc_clanmsg(lua_State* lua)
{
	struct userobject* obj;
	char type;
	char* msg;
	toUSER(1,obj);
	if(obj->clan == NULL) return 0;
	_GETc(2,type);
	_GETs(3,msg);
	clan_msg(obj->clan,type,msg);
	return 0;
}
int scriptfunc_clanmsg2(lua_State* lua)
{
	struct userobject* obj;
	int fc,bc,tmout;
	char* msg;
	toUSER(1,obj);
	if(obj->clan == NULL) return 0;
	_GETi(2,bc);
	_GETi(3,fc);
	_GETs(4,msg);
	_GETi(5,tmout);
	clan_msg2(obj->clan,fc,bc,msg,tmout);
	return 0;
}
int scriptfunc_setnotice(lua_State* lua)
{
	struct userobject* obj;
	char* notice;
	toUSER(1,obj);
	_GETs(2,notice);
	game_notice(obj2sd(obj),notice);
	return 0;
}
int scriptfunc_getslot(lua_State* lua)
{
	struct itemobject* iobj;
	toITEM(1,iobj);
	if(iobj->owner)
		PUSHi(iobj->slot);
	else
		PUSHn();
	return 1;
}
int scriptfunc_soundxy(lua_State* lua)
{
	struct map* m;
	int x,y;
	int sound;
	toMAP(1,m);
	_GETi(2,x);
	_GETi(3,y);
	_GETi(4,sound);
	map_sound(m,x,y,sound);
	return 0;
}
extern BServer gserver;
extern unsigned int login_ip;
extern unsigned short login_port;
extern char login_pw[32];
extern unsigned int game_option;
void conf_init(lua_State* lua){
	char* ip = "127.0.0.1";
	char* pw;
	char* name;
	gserver.port = global_i(lua,"_GAME_SERVER_PORT_",3356);
	ip = global_s(lua,"_LOGIN_SERVER_IP_",ip);
	login_ip = inet_addr(ip);
	login_port = global_i(lua,"_LOGIN_SERVER_PORT_",8998);
	pw = global_s(lua,"_LOGIN_SERVER_PW_","897mkd");
	strncpy(login_pw,pw,32);
	game_option = global_i(lua,"_GAME_OPTION_",0);
	name = global_s(lua,"_SERVER_NAME_","쿠우");
	strncpy(gserver.name,name,256);
}

void lua_init(lua_State* lua)
{
	luaL_openlibs(lua);
	script_init(lua);
	lua_register(lua,"_dialog",scriptfunc_dialog);
	lua_register(lua,"sleep",scriptfunc_sleep);
	lua_register(lua,"_msg",scriptfunc_msg);
	lua_register(lua,"_action",scriptfunc_action);
	lua_register(lua,"_sound",scriptfunc_sound);
	lua_register(lua,"_effect",scriptfunc_effect);
	lua_register(lua,"_effect_xy",scriptfunc_effect_xy);
	lua_register(lua,"_list",scriptfunc_list);
	lua_register(lua,"_gage",scriptfunc_gage);
	lua_register(lua,"_getobj_xy",scriptfunc_getobj_xy);
	lua_register(lua,"_getside",scriptfunc_getside);
	lua_register(lua,"_setside",scriptfunc_setside);
	lua_register(lua,"_getpos",scriptfunc_getpos);
	lua_register(lua,"_sethp",scriptfunc_sethp);
	lua_register(lua,"_setmp",scriptfunc_setmp);
	lua_register(lua,"_setmaxhp",scriptfunc_setmaxhp);
	lua_register(lua,"_setmaxmp",scriptfunc_setmaxmp);
	lua_register(lua,"_gettype",scriptfunc_gettype);
	lua_register(lua,"_mobdb_set",scriptfunc_setmobdb);
	lua_register(lua,"_npcdb_set",scriptfunc_setnpcdb);
	lua_register(lua,"_itemdb_set",scriptfunc_setitemdb);
	lua_register(lua,"_itemdb_set_equip",scriptfunc_setequipdb);
	lua_register(lua,"_walk",scriptfunc_walk);
	lua_register(lua,"_consolecol",scriptfunc_consolecol);
	lua_register(lua,"_warp",scriptfunc_warp);
	lua_register(lua,"_new_itemobj",scriptfunc_newitemobj);
	lua_register(lua,"_new_npcobj",scriptfunc_newnpcobj);
	lua_register(lua,"_new_mobobj",scriptfunc_newmobobj);
	lua_register(lua,"_give_item",scriptfunc_giveitem);
	lua_register(lua,"_give_item2",scriptfunc_giveitem2);
	lua_register(lua,"_set_icount",scriptfunc_seticount);
	lua_register(lua,"_get_icount",scriptfunc_geticount);
	lua_register(lua,"_mapdb_set",scriptfunc_setmapdb);
	lua_register(lua,"_map_set",scriptfunc_setmap);
	lua_register(lua,"_check_sight",scriptfunc_check_sight);
	lua_register(lua,"_get_mapsize",scriptfunc_getmapsize);
	lua_register(lua,"_warp_set",scriptfunc_setwarp);
	lua_register(lua,"_getdbid",scriptfunc_getdbid);
	lua_register(lua,"_magicdb_set",scriptfunc_setmagicdb);
	lua_register(lua,"_give_magic",scriptfunc_givemagic);
	lua_register(lua,"NOT",scriptfunc_not);
	lua_register(lua,"OR",scriptfunc_or);
	lua_register(lua,"AND",scriptfunc_and);
	lua_register(lua,"_sformat_set",scriptfunc_sformat_set);
	lua_register(lua,"_set_userdata",scriptfunc_setuserdata);
	lua_register(lua,"_sethair",scriptfunc_sethair);
	lua_register(lua,"_setface",scriptfunc_setface);
	lua_register(lua,"_setgender",scriptfunc_setgender);
	lua_register(lua,"_setnation",scriptfunc_setnation);
	lua_register(lua,"_settotem",scriptfunc_settotem);
	lua_register(lua,"_setstate",scriptfunc_setstate);
	lua_register(lua,"_setjob",scriptfunc_setjob);
	lua_register(lua,"_setjoblv",scriptfunc_setjoblv);
	lua_register(lua,"_setmight",scriptfunc_setmight);
	lua_register(lua,"_setac",scriptfunc_setpureac);
	lua_register(lua,"_setimsg",scriptfunc_setimsg);
	lua_register(lua,"_setgrace",scriptfunc_setgrace);
	lua_register(lua,"_setwill",scriptfunc_setwill);
	lua_register(lua,"_setlevel",scriptfunc_setlevel);
	lua_register(lua,"_setdam",scriptfunc_setdam);
	lua_register(lua,"_sethit",scriptfunc_sethit);
	lua_register(lua,"_settitle",scriptfunc_settitle);
	lua_register(lua,"_setexp",scriptfunc_setexp);
	lua_register(lua,"_setpersonality",scriptfunc_setpersonality);
	lua_register(lua,"_setvote",scriptfunc_setvote);
	lua_register(lua,"_setmoney",scriptfunc_setmoney);
	lua_register(lua,"_blocklock",scriptfunc_blocklock);
	lua_register(lua,"_timer_add",scriptfunc_timeradd);
	lua_register(lua,"_timer_del",scriptfunc_timerdel);
	lua_register(lua,"_logout",scriptfunc_logout);
	lua_register(lua,"_sql_conn",scriptfunc_sql_conn);
	lua_register(lua,"_sql_query",scriptfunc_sql_query);
	lua_register(lua,"_sql_store_result",scriptfunc_sql_store_result);
	lua_register(lua,"_sql_fetch_row",scriptfunc_sql_fetch_row);
	lua_register(lua,"_sql_free_result",scriptfunc_sql_free_result);
	lua_register(lua,"_row_geti",scriptfunc_row_geti);
	lua_register(lua,"_row_getb",scriptfunc_row_getb);
	lua_register(lua,"_row_gets",scriptfunc_row_gets);
	lua_register(lua,"_getequip",scriptfunc_getequip);
	lua_register(lua,"_getslotitem",scriptfunc_getslotitem);
	lua_register(lua,"_teleport",scriptfunc_teleport);
	lua_register(lua,"_equip_takeon",scriptfunc_equip_takeon);
	lua_register(lua,"_dropmoney",scriptfunc_drop_money);
	lua_register(lua,"_jobmax_set",scriptfunc_job_max_set);
	lua_register(lua,"_job_set",scriptfunc_job_set);
	lua_register(lua,"_pos_first",scriptfunc_posfirst);
	lua_register(lua,"_pos_next",scriptfunc_posnext);
	lua_register(lua,"_pos_last",scriptfunc_poslast);
	lua_register(lua,"_pos_prev",scriptfunc_posprev);
	lua_register(lua,"_input",scriptfunc_input);
	lua_register(lua,"_ilist",scriptfunc_ilist); 
	lua_register(lua,"_islotlist",scriptfunc_islotlist);
	lua_register(lua,"_menu",scriptfunc_menu);
	lua_register(lua,"_say",scriptfunc_say);
	lua_register(lua,"_equip_takeoff",scriptfunc_equip_takeoff);
	lua_register(lua,"_can_move",scriptfunc_can_move);
	lua_register(lua,"_group_master",scriptfunc_groupmaster);
	lua_register(lua,"_group_make",scriptfunc_groupmake);
	lua_register(lua,"_group_join",scriptfunc_groupjoin);
	lua_register(lua,"_group_leave",scriptfunc_groupleave);
	lua_register(lua,"_group_message",scriptfunc_groupmessage);
	lua_register(lua,"_group_delete",scriptfunc_groupdelete);
	lua_register(lua,"_trade",scriptfunc_trade);
	lua_register(lua,"_set_uoption",scriptfunc_set_uoption);
	lua_register(lua,"_magic_fall",scriptfunc_magic_fall);
	lua_register(lua,"_magic_delay",scriptfunc_magic_delay);
	lua_register(lua,"_eventinit",scriptfunc_eventinit);
	lua_register(lua,"_set_target",scriptfunc_settarget);
	lua_register(lua,"_get_target",scriptfunc_gettarget);
	lua_register(lua,"_set_autoeffect",scriptfunc_setautoeffect);
	lua_register(lua,"_set_magic",scriptfunc_set_magic);
	lua_register(lua,"_set_ownitem",scriptfunc_set_ownitem);
	lua_register(lua,"_set_equip",scriptfunc_setequip);
	lua_register(lua,"_set_imsg",scriptfunc_setimsg);
	lua_register(lua,"_notice",scriptfunc_setnotice);
	lua_register(lua,"_check_item",scriptfunc_checkitem);
	lua_register(lua,"_set_lastobject",scriptfunc_setlastobject);
	lua_register(lua,"_message2",scriptfunc_message2);
	lua_register(lua,"_map_message",scriptfunc_map_message);
	lua_register(lua,"_map_message2",scriptfunc_map_message2);
	lua_register(lua,"world_message",scriptfunc_world_message);
	lua_register(lua,"world_message2",scriptfunc_world_message2);
	lua_register(lua,"_set_epfdb",scriptfunc_setepf);
	lua_register(lua,"_set_wmapdb",scriptfunc_setwmapdb);
	lua_register(lua,"_wmap_set",scriptfunc_wmapset);
	lua_register(lua,"_locscript_set",scriptfunc_setlocscript);
	lua_register(lua,"_clan_new",scriptfunc_clannew);
	lua_register(lua,"_clan_addmember",scriptfunc_clanadd);
	lua_register(lua,"_clan_delmember",scriptfunc_clandel);
	lua_register(lua,"_clan_message",scriptfunc_clanmsg);
	lua_register(lua,"_clan_message2",scriptfunc_clanmsg2);
	lua_register(lua,"_getslot",scriptfunc_getslot);
	lua_register(lua,"_getsightobjects",scriptfunc_getsightobjects);
	lua_register(lua,"_sound_xy",scriptfunc_soundxy);
	if(luaL_dofile(lua,"game.lua")!= 0)
	{
		warnf("[script error]%s\n",lua_tostring(lua,-1));
	}
	conf_init(lua);
}