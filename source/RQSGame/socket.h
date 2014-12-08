#pragma once
#define FD_SETSIZE 1024
#include <WinSock2.h>
#include "list.h"
#include "cryption.h"
#include "userobject.h"
#include "login.h"

typedef struct BARAM_SOCKET_CLIENT BSocket;

typedef struct BARAM_SOCKET_SERVER BServer;
typedef struct BARAM_SOCKET_EVENT BEvent;

#define RECV_BUF_MAX 4096
#define CLOCK_RECEIVE 40
#define SEND_BUF_MAX 0x8000
#define obj2sd(o) (((BSocket*)((BYTE*)(o) - (BYTE*)offsetof(BSocket,obj.obj))))
struct lua_thread;
struct map;
struct epfdb;

struct BARAM_SOCKET_CLIENT
{
	SOCKET handle;
	char address[16];
	char* cryptkey;
	BYTE rcvBuffer[RECV_BUF_MAX+100];
	BYTE sndBuffer[SEND_BUF_MAX+100];
	unsigned short rcvOffset;
	unsigned short sndOffset;
	struct _list_elem elem;
	BOOL login;
	int eof;
	BOOL serialon;
	struct lua_thread* dlg_thread;
	struct mapobject* last_object;
	unsigned char dlg_type;
	struct userobject obj;
	struct epfdb* epf;
	long last_time;
};
void socket_close(BSocket* sock);
extern char* cryptkey;
struct BARAM_SOCKET_SERVER
{
	SOCKET handle;
	char name[256];
	unsigned short port;
	struct _list sockets;
	int socket_count;
};
extern BSocket* sockets[0x10000];

int get_socket_count();
#define SWAP16(n) (unsigned short)(((n)<<8)|((n)>>8))
#define SWAP32(n) (unsigned long)(((n)<<24)|(((n)<<8)&0x00FF0000)|(((n)>>8)&0x0000FF00)|((n)>>24))
#define UNSWAP16(p) ((*(unsigned char*)((p)+1))|(*(unsigned char*)(p)<<8))
#define UNSWAP32(p) ((*(unsigned char*)((p)+3))|(*(unsigned char*)((p)+2)<<8)|(*(unsigned char*)((p)+1)<<16)|(*(unsigned char*)(p)<<24))

#define cpt(sd) (encrypt(0,sd->sndBuffer+sd->sndOffset,sd->cryptkey),sd->sndOffset+=(UNSWAP16(sd->sndBuffer+sd->sndOffset+1)+3))
#define cpt2(sd) (encrypt(0,sd->sndBuffer+sd->sndOffset,sd->cryptkey))
#define wb(sd,i) (*(byte*)((sd->sndBuffer+(i)+sd->sndOffset)))
#define sdsize(sd,sz) {if(SEND_BUF_MAX - (sd)->sndOffset < (sz)) socket_flush(sd);}
#define ws(sd,i) (*(unsigned short*)(sd->sndBuffer+(i)+sd->sndOffset))
#define wi(sd,i) (*(int*)(sd->sndBuffer+(i)+sd->sndOffset))
#define wl(sd,i) (*(long*)(sd->sndBuffer+(i)+sd->sndOffset))
#define wp(sd,i) ((sd->sndBuffer+sd->sndOffset+(i)))
#define WB(sd,i) (*(byte*)(((sd)+(i))))
#define WS(sd,i) (*(unsigned short*)((sd)+(i)))
#define WI(sd,i) (*(int*)((sd)+(i)))
#define WL(sd,i) (*(long*)((sd)+(i)))
void socket_init();

BSocket* socket_first(struct map* m);
BSocket* socket_next(BSocket*);
BSocket* socket_prev(BSocket*);

#define socket_flush(s) (send(s->handle,(char*)s->sndBuffer,s->sndOffset,0), s->sndOffset = 0)
void socket_write(BSocket*,char*,unsigned short);
void BaramOpen(BServer* server,int (*rcv)(BSocket*),int (*close)(BSocket*),int (*acc)(SOCKET,char* addr));
