#include "clan.h"
#include "mem.h"
#include "socket.h"
#include <string.h>
int clan_write(struct clan* cl,unsigned char* packet)
{
	struct _list_elem* elem;
	BSocket* sd;
	int size = UNSWAP16(packet+1);
	CPT(packet);
	sd = list_entry(elem=list_begin(&cl->members),BSocket,obj.clan_elem);
	while(elem != &cl->members.tail)
	{
		socket_write(sd,(char*)packet,size+3);
		sd = list_entry(elem = list_next(elem),BSocket,obj.clan_elem);
	}
	return 0;
}
struct clan* clan_new(char* name,char* title)
{
	struct clan* cl = MALOC(struct clan,1);
	list_init(&cl->members);
	cl->member_count = 0;
	strncpy(cl->name,name,CLAN_NAME_MAX);
	strncpy(cl->title,title,CLAN_TITLE_MAX);
	cl->name_len = strlen(cl->name);
	cl->title_len = strlen(cl->title);
	return cl;
}
void clan_msg(struct clan* cl,char type,char* msg)
{
	
	byte packet[300];
	unsigned short slen = strlen(msg);\
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+6);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = type;
	WB(packet,6) = SWAP16(slen);
	memmove(packet+8,msg,slen);
	WB(packet,slen+8) = 0;
	clan_write(cl,packet);
}

void clan_msg2(struct clan* c,int fc,int bc,char* msg,int timeout)
{
	
	byte packet[300];
	unsigned short slen = strlen(msg);
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+8);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = 0x12;
	WB(packet,6) = fc;
	WB(packet,7) = bc;
	WB(packet,8) = timeout;
	WB(packet,9) = (slen);
	memmove(packet+10,msg,slen);
	WB(packet,slen+10) = 0;
	clan_write(c,packet);
}