#include "world.h"
void world_write(byte* packet)
{
	BSocket* sd;
	int size = UNSWAP16(packet+1) + 3;
	CPT(packet);
	sd = socket_first(NULL);
	while(sd != NULL)
	{
		socket_write(sd,(char*)packet,size);
		sd = socket_next(sd);
	}
}
void world_time_single(BSocket* sd)
{
	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(5);
	wb(sd, 3) = 32;
	wb(sd, 4) = 0;
	wb(sd, 5) = 5;
	wb(sd, 6) = 19;
	wb(sd, 7) = 0;
	cpt(sd);
}
void world_msg(int type,char* msg)
{

	byte packet[300];
	unsigned short slen = strlen(msg);
	
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+6);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = type;
	WB(packet,6) = SWAP16(slen);
	memmove(packet+8,msg,slen);
	WB(packet,slen+8) = 0;
	world_write(packet);
}
void world_msg2(int fc,int bc,char* text,int timeout)
{
	byte packet[300];
	unsigned short slen = strlen(text);
	WB(packet,0) = 170;
	WS(packet,1) = SWAP16(slen+8);
	WB(packet,3) = 10;
	WB(packet,4) = 0;
	WB(packet,5) = 0x12;
	WB(packet,6) = fc;
	WB(packet,7) = bc;
	WB(packet,8) = timeout;
	WB(packet,9) = (slen);
	memmove(packet+10,text,slen);
	WB(packet,slen+10) = 0;
	world_write(packet);
}