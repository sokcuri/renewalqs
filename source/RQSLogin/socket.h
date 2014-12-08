#pragma once
#define FD_SETSIZE 1024
#include <WinSock2.h>
#include "list.h"
#include "hashtable.h"

typedef struct BARAM_SOCKET_CLIENT BSocket;

typedef struct BARAM_SOCKET_SERVER BServer;
typedef struct BARAM_SOCKET_EVENT BEvent;
struct lua_thread;
#define RECV_BUF_MAX 4096
struct BARAM_SOCKET_CLIENT
{
	SOCKET handle;
	char address[16];
	char* cryptkey;
	BYTE rcvBuffer[RECV_BUF_MAX];
	unsigned short rcvOffset;
	struct _list_elem hash_elem;
	struct _list_elem elem;
	struct lua_thread* thread;
};
extern BSocket* sockets[0x10000];
extern char* cryptkey;
struct BARAM_SOCKET_SERVER
{
	SOCKET handle;
	unsigned short port;
	struct _list sockets;
	char name[256];
};
#define SWAP16(n) (unsigned short)(((n)<<8)|((n)>>8))
#define SWAP32(n) (unsigned long)(((n)<<24)|(((n)<<8)&0x00FF0000)|(((n)>>8)&0x0000FF00)|((n)>>24))
#define UNSWAP16(p) ((*(unsigned char*)((p)+1))|(*(unsigned char*)(p)<<8))
#define UNSWAP32(p) ((*(unsigned char*)((p)+3))|(*(unsigned char*)((p)+2)<<8)|(*(unsigned char*)((p)+1)<<16)|(*(unsigned char*)(p)<<24))

void socket_init();
void BaramOpen(BServer* server,int (*rcv)(BSocket*),int (*close)(BSocket*),int (*acc)(SOCKET,char* addr));
