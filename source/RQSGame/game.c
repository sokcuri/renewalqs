#include "game.h"
#include <stdio.h>
#include "debug.h"
#include "world.h"
#include "userimg.h"
#include "userobject.h"
#include "pc.h"
#include "npc.h"
#include "map.h"
void game_init()
{
	object_init();
	uimg_init();
}

void game_notice(BSocket* sd,char* message)
{
	register int len;
	wb(sd,0) = 170;
	wb(sd,3) = 88;
	wb(sd,5) = 2;
	ws(sd,6) = SWAP16(len=strlen(message));
	memmove(wp(sd,8),message,len);
	wb(sd,8+len) = 0;
	ws(sd,1) = SWAP16(len+6);
	cpt(sd);
}
void game_send_message2(BSocket* sd,int fc,int bc,char* message,int timeout)
{
	
	unsigned short slen = strlen(message);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(slen+8);
	wb(sd,3) = 10;
	wb(sd,4) = 0;
	wb(sd,5) = 0x12;
	wb(sd,6) = fc;
	wb(sd,7) = bc;
	wb(sd,8) = timeout;
	wb(sd,9) = (slen);
	memmove(wp(sd,10),message,slen);
	wb(sd,slen+10) = 0;
	cpt(sd);
}
void game_send_message(BSocket* sd,char type,char* message)
{
	unsigned short slen = strlen(message);
	sdsize(sd,slen+9);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(slen+6);
	wb(sd,3) = 10;
	wb(sd,4) = 0;
	wb(sd,5) = type;
	ws(sd,6) = SWAP16(slen);
	memmove(sd->sndBuffer+sd->sndOffset+8,message,slen);
	wb(sd,slen+8) = 0;
	cpt(sd);
}
void game_id(BSocket* sd)
{
	sdsize(sd,17);
	wb(sd,0) = 170;
	ws(sd,1) = SWAP16(14);
	wb(sd,3) = 5;
	wb(sd,4) = 0;
	wl(sd,5) = SWAP32(sd->obj.obj.id);
	wl(sd,9) = SWAP32(2);
	wb(sd,13) = 0;
	ws(sd,14) = SWAP16(0);
	wb(sd,16) = 0;
	cpt(sd);
}
void item_slot_show_all(BSocket* sd);
void magic_slot_show_all(BSocket* sd);
struct npcobject npca;
void game_login(BSocket* sock)
{
	int i;
	char temp[256];
	sock->login = TRUE;
	sprintf(temp,"%s님 바람의나라에 오신것을 환영합니다!",sock->obj.ud.id);
	game_send_message(sock,3,temp);
	//user_status(&sock->obj,120);
	userobject_new(&sock->obj);
	game_id(sock);
	pc_option_sub(sock);
	
	world_time_single(sock);
	//object_warp(&sock->obj.obj,maps[0],40,40);
	//user_status(&sock->obj,120);
	item_slot_show_all(sock);
	magic_slot_show_all(sock);
}

int game_receive(BSocket* sock)
{
	int i;
	int off = 0;
	while(off < sock->rcvOffset)
	{
		int nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		BYTE* buf = sock->rcvBuffer + off;
		if(nsize < 2) return -1;
		if(!sock->login)
		{
			if(buf[3] == 0x10 && !sock->serialon)
			{
				serial_on(UNSWAP32(buf+22),(void*)sock);
				sock->serialon = TRUE;
			}
			continue;
		}
		if(pc_call(buf[3],sock,buf) < 0)
		{
			return -1;
		}
		off += nsize + 3;
		if(off+3 > sock->rcvOffset) break;
		nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		if(off + 3 + nsize > sock->rcvOffset) break;
	}
	return off;
}

int game_send_servermove(BSocket* sock,unsigned int ip,unsigned short port)
{
	int serial;
	int size;
	BYTE packet[34]="\xAA\x00\x13\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	*(unsigned short*)(packet+1) = SWAP16(30);
	*(unsigned int*)(packet+4) = SWAP32(ip);
	
	*(unsigned short*)(packet+8) = SWAP16(port);
	packet[10] = 22;
	packet[11] = 0;
	packet[12] = 9;
	memmove(packet+13,sock->cryptkey,9);
	packet[22] = 6;
	memmove(packet+23,"socket",6);
	
	serial = (rand()+1)*(rand()+1);
	*(unsigned int*)(packet+29) = SWAP32(serial);
	//crypt(packet);
	send(sock->handle,(char*)packet,33,0);
	return serial;
}
void game_user_list(BSocket* sd)
{
	register int i,cur=0,size=10;
	BSocket* sock;
	sdsize(sd,get_socket_count()*(ID_MAX + 6) + 15);
	wb(sd,0) =170;
	wb(sd,3) = 54;
	wb(sd,4) = 0;
	wb(sd,9) = 0;
	sock = socket_first(NULL);
	while(sock!=NULL)
	{
		if(sock->login)
		{
			wb(sd,size++) = 16* sock->obj.ud.nation + sock->obj.ud.job;
			wb(sd,size++) = 16* sock->obj.ud.joblv;
			wb(sd,size++) = 143;
			wb(sd,size++) = sock->obj.ud.id_len;
			memmove(wp(sd,size),sock->obj.ud.id,wb(sd,size-1));
			size += wb(sd,size-1);
			cur++;
		}
		sock = socket_next(sock);
	}
	ws(sd,5) = SWAP16(cur);
	ws(sd,7) = SWAP16(cur);
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
int game_accept(SOCKET handle,char* address)
{
	deff("%s 접속\n",address);
	return 0;
}
int game_close(BSocket* sock)
{
	deff("%s 접속해제\n",sock->address);
	if(sock->login)
	{
		if(sock->obj.trade.user != NULL)
		{
			trade_cancel(sock,sock->obj.trade.user);
		}
		object_del(&sock->obj.obj);
	}
	return 0;
}