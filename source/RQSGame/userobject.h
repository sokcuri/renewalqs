#pragma once
#include "object.h"
#include "trade.h"
#include "item.h"
#include "userdata.h"
#include "magic.h"
#include "clan.h"
struct userobject
{
	struct mapobject obj;
	struct userdata ud;
	struct mobobject* riding;
	struct itemobject* equips[EQUIP_SLOT_MAX];
	struct itemobject* ownitems[ITEM_SLOT_MAX];
	struct magicslot ownmagics[MAGIC_SLOT_MAX];
	unsigned char empty_count;
	long last_magictick;
	struct _list_elem user_elem;
	struct _list_elem group_elem;
	struct _list_elem clan_elem;
	struct clan* clan;
	struct group* group;
	unsigned int opt;
	struct trade_info trade;
};
enum useropt
{
	trade_ok = 1,
	group_ok = 2,
	pkprotect_ok = 4,
	keyreverse = 8,
	despair = 16,
	hidechat = 32,
	nochat = 64,
	helmet_visible = 128,
	no_invisible = 256,
};
unsigned int userobject_new(struct userobject* uobj);
void user_lookrefresh(struct userobject* obj);
void user_show_sight(struct userobject* obj);
void user_set_state(struct userobject* obj,int state);