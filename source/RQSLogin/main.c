#include <WinSock2.h>
#include "socket.h"
#include "debug.h"
#include "login.h"
#include "script.h"
#include <time.h>
#include "cryption.h"
#include "scriptfunc.h"
#include <stdio.h>
BSocket* GameSocket = NULL;
char game_pw[32];
unsigned int game_ip;
unsigned short game_port = 3356;
void game_login(int serial,char* id)
{
	char buffer[0x1000] = "\xAA\x00\x00\x10";
	int len;
	*(unsigned int*)(buffer+4) = SWAP32(serial);
	strcpy(buffer+8,id);
	len = strlen(id);
	*(unsigned short*)(buffer+1) = SWAP16(len+6);
	send(GameSocket->handle,buffer,len+9,0);
}
void game_chk_login(BSocket* sd,char* id)
{
	char buffer[100]="\xAA\x00\x00\x05";
	*(unsigned int*)(buffer+4) = SWAP32(sd->handle);
	buffer[8] = strlen(id);
	memmove(buffer+9,id,buffer[8]);
	*(unsigned short*)(buffer+1) =SWAP16(buffer[8]+6);
	send(GameSocket->handle,buffer,buffer[8]+9,0);
}
void game_send_message(char type,char* msg)
{
	int len = strlen(msg);
	char buffer[0x1000]="\xAA\x00\x00";
	buffer[3] = type;
	memmove(buffer+4,msg,len);
	*(unsigned short*)(buffer+1) = SWAP16(len+1);
	send(GameSocket->handle,buffer,len+4,0);
}
int game_receive()
{
	BSocket* sock = GameSocket;
	int off = 0;
	if(sock == NULL) return -1;
	while(off < sock->rcvOffset)
	{
		int nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		BYTE* buf = sock->rcvBuffer + off;
		if(nsize < 2) return -1;
		switch(buf[3])
		{
		case 0x05:
			// check login
			{
				int id = UNSWAP32(buf+4);
				BSocket* sd = sockets[id];
				if(sd->thread)
				{
					lua_pushboolean(sd->thread->lua,buf[8]);
					thread_resume(sd->thread,1);
				}
			}
			break;
		}
		off += nsize + 3;
		if(off+3 > sock->rcvOffset) break;
		nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		if(off + 3 + nsize > sock->rcvOffset) break;
	}
	return off;
}
int receive(BSocket* sock)
{
	int off = 0;
	if(sock == GameSocket) {
		return game_receive();
	}
	while(off < sock->rcvOffset)
	{
		int nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		BYTE* buf = sock->rcvBuffer + off;
		if(nsize < 2) return -1;
		switch(buf[3])
		{
		case 0xFF:
			if ( GameSocket != NULL ) return -1;
			if(strcmp(game_pw,(char*)buf+4) == 0)
			{
				deff("%s 가 게임서버로 지정되었습니다.\n",sock->address);
				GameSocket = sock;
			}
			break;
		case 0:
			login_send_changekey(sock,0,cryptkey);
			break;
		case 4:
			decrypt(0,buf,cryptkey);
			shell_script("onUserCreate","iiiiii",sock->handle,buf[6],buf[7],buf[8],buf[9],buf[10]);

			break;
		case 2:
			decrypt(0,buf,cryptkey);
			{
				int idsize = buf[5];
				int pwsize;
				pwsize = buf[6+idsize];
				buf[6+idsize] = 0;
				buf[7 + idsize + pwsize] = 0;
				shell_script("onUserCheck","iss",sock->handle,buf+6,buf+7+idsize);

			}
			break;
		case 3:
			decrypt(0,buf,cryptkey);
			{
				int idlen = buf[5];
				int pwlen;
				pwlen = buf[6 + idlen];
				buf[6 + idlen] = 0;
				buf[7 + idlen + pwlen] = 0;
				shell_script("onUserLogin","iss",sock->handle,buf+6,buf+7+idlen);
			}
			break;

		case 87:
			decrypt(0,buf,cryptkey);
			if(buf[5] == 1)
			{
				login_send_list(sock);
			}
			else if(buf[5]== 0)
			{
				if(buf[6] >= 16) return -1;
				login_send_multiserver(sock,buf[6]);
			}
			else return -1;
			break;
		case 0x10:
			if(buf[5] != 9) return -1;
			login_send_agreement(sock);
			break;
		case 0x4B:
			break;
		case 123:
			decrypt(0,buf,cryptkey);
			if(buf[5] == 1)
				login_send_metalist(sock);
			else if(buf[5] == 0)
				login_send_meta(sock,buf);
			break;
		case 104:
			login_send_website(sock,"http://127.0.0.1:80/nx_auth.php?user_id=%s&password=%s");
			break;
		case 113:
			break;
		default:
			deff("[UnknownPacket] %d\n",buf[3]);
			break;
		}
		off += nsize + 3;
		if(off+3 > sock->rcvOffset) break;
		nsize = UNSWAP16(sock->rcvBuffer + off + 1);
		if(off + 3 + nsize > sock->rcvOffset) break;
	}
	return off;
}
int acc(SOCKET s,char* ip)
{
	deff("[Connected] %s\n",ip);
	send(s,"\xAA\x00\x13\x7E\x1B\x43\x4F\x4E\x4E\x45\x43\x54\x45\x44\x20\x53\x45\x52\x56\x45\x52\x0A",22,0);
	return 0;
}
int close(BSocket* sock)
{
	if(GameSocket == sock)
	{
		GameSocket = NULL;
	}
	deff("%s Close \n",sock->address);
	return 0;
}
BServer lserver;
int main()
{
	lua_State* lua = lua_open();
	srand(time(NULL));
	lua_init(lua);
	login_init();
	socket_init();
	timer_init();
	
	//printf("%d\n",offsetof(BSocket,hash_elem));
	BaramOpen(&lserver,receive,close,acc);

}