#pragma once
#include "socket.h"
struct pc_event
{
	BYTE cryption;
	int (*event_func)(BSocket*,BYTE*);
};
void pc_init();
enum script_event
{
	ON_LOAD = 0,
	ON_CLICK = 1,
	ON_SAY = 2,
	ON_SIDE = 3,
	ON_WALK = 4,
	ON_PICKUP = 5,
	ON_EMOTION = 6,
	ON_ATTACK = 7,
	ON_TAKEON = 8,
	ON_TAKEOFF = 9,
	ON_USEITEM = 10,
	ON_USEMAGIC = 11,
	ON_DELETE = 12,
	ON_LOGIN = 13,
	ON_SECOND = 14,
	ON_F1 = 15,
	ON_F2 = 16,
	ON_DROPMONEY = 17,
	ON_WHISPER = 18,
	ON_PICKUPMONEY = 19,
	ON_REQUEST_GROUP = 20,
	ON_RIDING = 21,
	ON_PKPROTECT = 22,
	ON_GROUPON = 23,
	ON_TRADEON = 24,
	ON_HELMET = 25,
	ON_REQUEST_TRADE = 26,
	ON_AI = 27,
	ON_ERROR = 28,
	ON_LOCATION = 29,
};
extern unsigned int ref_events[100];
int pc_call(BYTE type,BSocket* sd,BYTE* buf);


void pc_sound(BSocket* sd,unsigned short sound);

void event_init();
void pc_option_sub(BSocket* sd);