#include "login.h"
#include "socket.h"
#include "mem.h"
#include "debug.h"
#include "userdata.h"
#include "game.h"
struct hashtable h_login;
SOCKET hLogin;
unsigned int login_ip;
unsigned short login_port;
char login_pw[32];
#define LOGIN_BUF_MAX 0x10000
byte lg_buffer[LOGIN_BUF_MAX];
unsigned int lg_offset;
int login_hfunc(struct hashtable* ht,void* data)
{
	return ((int)data)% ht->list_size;
}
int login_hcomp(void* d,struct _list_elem* elem)
{
	login_ud* ud = list_entry(elem,login_ud,hash_elem);
	if(ud->serial == (int)d) return 0;
	return -1;
}
void login_init()
{
	int len = strlen(login_pw);
	char buffer[0x1000] = "\xAA\x00\x00\xFF";
	SOCKADDR_IN addr;
	hash_init(&h_login,7,login_hfunc,login_hcomp);
	addr.sin_addr.S_un.S_addr = login_ip;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(login_port);
	hLogin = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	PANIC(connect(hLogin,(struct sockaddr*)&addr,sizeof(addr)) != SOCKET_ERROR,"Login Server Connection failed");
	memmove(buffer+4,login_pw,len);
	*(unsigned short*)(buffer+1) = SWAP16(len+1);
	send(hLogin,buffer,len+4,0);
}
int serial_on(unsigned int serial,void* data)
{
	struct _list_elem* elem = hash_get(&h_login,(void*)serial);
	login_ud* ud;
	if(elem != NULL) {
		BSocket* sd = (BSocket*)(data);
		ud = list_entry(elem,login_ud,hash_elem);
		_list_remove(&ud->hash_elem);
		memcpy(sd->obj.ud.id,ud->id,sizeof(sd->obj.ud.id));
		sd->obj.ud.id_len = strlen(ud->id);
		_free(ud);
		game_login(sd);
		return 1;
	}
	 ud = MALOC(login_ud,1);
	ud->serial = serial;
	ud->data = data;
	hash_insert(&h_login,(void*)serial,&ud->hash_elem);
	return 0;
}
int serial_set(int serial,char* id)
{
	//hash_get(&h_login,(void*)serial)
	 struct _list_elem* elem = NULL;
	 login_ud* ud;
	 if(elem != NULL)
	 {
		 BSocket* sock;
		 ud = list_entry(elem,login_ud,hash_elem);
		 sock = (BSocket*)ud->data;
		 strncpy(sock->obj.ud.id,id,ID_MAX);
		 sock->obj.ud.id_len = strlen(id);
		 _list_remove(elem);
		 _free(ud);
		 game_login(sock);
		 return 0;
	 }
	 ud = MALOC(login_ud,1);
	 ud->serial = serial;
	 strncpy(ud->id,id,30);
	 hash_insert(&h_login,(void*)serial,&ud->hash_elem);
	 return 0;
}
void login_receive()
{	
	int size = recv(hLogin,(char*)lg_buffer + lg_offset,LOGIN_BUF_MAX - lg_offset,0);
	int off = 0;
	PANIC(size > 0,"Login Server Connection Broken");

	lg_offset += size;
	if(lg_offset < 3) return ;
	while(off < lg_offset)
	{
		int psize;
		byte* buff = lg_buffer + off;
		if(off + 3 >= lg_offset) break;
		psize = UNSWAP16(buff+1);
		if(off + psize + 3 > lg_offset) break;
		switch(buff[3])
		{
		case 0x05:
			{
				char packet[100] = "\xAA\x00\x06\x05";
				BSocket* sd;
				int serial = UNSWAP32(buff+4);
				int len = buff[8];
				buff[9+len] = 0;
				sd = socket_first(NULL);
				*(unsigned int*)(packet+4) = SWAP32(serial);
				packet[8] = 0;
				while(sd != NULL)
				{
					if(sd->login && strcmp((char*)buff+9,sd->obj.ud.id) == 0)
					{
						packet[8] = 1;
						break;
					}
					sd = socket_next(sd);
				}
				send(hLogin,packet,9,0);
			}
			break;
		case 0x10: 
			{
				int serial = UNSWAP32(buff+4);
				serial_set(serial,(char*)buff+8);
			}
			break;
		}
		off += psize + 3;
	}
	memmove(lg_buffer,lg_buffer+off,lg_offset - off);
	lg_offset -= off;
}