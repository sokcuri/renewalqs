
#include "timer.h"
#include "socket.h"
#include "mem.h"

#include "debug.h"
#include "map.h"
#include <time.h>
#include "cryption.h"
#include "login.h"
#include "script.h"
extern SOCKET hLogin;
static BServer* server;
char* cryptkey = defaultCryptKey;
extern SOCKET hLogin;
BSocket* sockets[0x10000];
fd_set reads;
void socket_write(BSocket* sock,char* buffer,unsigned short len)
{
	if(sock->sndOffset + len > SEND_BUF_MAX) socket_flush(sock);
	memcpy(sock->sndBuffer + sock->sndOffset,buffer,len);
	sock->sndOffset += len;
}

int get_socket_count()
{
	return server->socket_count;
}
BSocket* socket_first(struct map* m)
{
	struct _list * l = m == NULL ? &server->sockets : &m->users;
	struct _list_elem* elem = list_begin(l);
	if(elem == &l->tail) return NULL;
	if(m == NULL) return list_entry(elem,BSocket,elem);
	else return list_entry(elem,BSocket,obj.user_elem);
}
BSocket* socket_next(BSocket* sock)
{
	BSocket* next = list_entry(list_next(&sock->elem),BSocket,elem);
	if(&next->elem == list_tail(server->sockets)) return NULL;
	else return next;
}
BSocket* socket_prev(BSocket* sock)
{
	BSocket* prev = list_entry(list_prev(&sock->elem),BSocket,elem);
	if(&prev->elem == list_head(server->sockets)) return NULL;
	else return prev;
}
void socket_init()
{
	WSADATA wsaData;
	PANIC(WSAStartup(MAKEWORD(2,2),&wsaData)==0,"WSA Start error");
}
static int (*closefunc)(BSocket*);
void socket_close(BSocket* sock)
{
	_list_remove(&sock->elem);
	closefunc(sock);
	FD_CLR(sock->handle,&reads);
	closesocket(sock->handle);
	server->socket_count--;
}
long timer_test(struct timer* a)
{
	return rand();
}

extern int maxx ,maxy,blockmax;
void BaramOpen(BServer* serv,int (*rcv)(BSocket*),int (*close)(BSocket*),int (*acc)(SOCKET,char* addr))
{
	SOCKADDR_IN addr;
	struct timeval timeout;
	int i;
	clock_t last_timecheck_tick = 0;
	SOCKET fd_max= 0;
	fd_set temps;
	server = serv;
	closefunc = close;
	serv->handle = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	PANIC(serv->handle != INVALID_SOCKET,"Socket Create Error");
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serv->port);
	list_init(&serv->sockets);
	PANIC(bind(serv->handle,(struct sockaddr*)&addr,sizeof(addr))!=SOCKET_ERROR,"BIND ERROR");
	PANIC(listen(serv->handle,5)!=SOCKET_ERROR,"LISTEN ERROR");
	FD_ZERO(&reads);
	FD_SET(hLogin,&reads);
	FD_SET(serv->handle,&reads);
	fd_max = serv->handle + 1;
	shell_script("onOpen","");
	while(TRUE)
	{
		unsigned int i;
		temps = reads;
		timeout.tv_sec = 0;
		timeout.tv_usec = 50000;
		PANIC(select(fd_max,&temps,0,0,&timeout) != SOCKET_ERROR,"SELECT ERROR");
		if(last_timecheck_tick + 100 < clock()){
			timer_check();
			last_timecheck_tick = clock();
		}
		//deff("timer check %d\n",clock()-t);
		for( i = 0;i<reads.fd_count;i++)
		{
			if(FD_ISSET(reads.fd_array[i],&temps))
			{
				if(reads.fd_array[i] == serv->handle)
				{
					SOCKADDR_IN clntAddr;
					int clntLen = sizeof(clntAddr);
					SOCKET sock = accept(serv->handle,(struct sockaddr*)&clntAddr,&clntLen);
					if(!acc(sock,inet_ntoa(clntAddr.sin_addr)))
					{
						BSocket* socket = CALOC(BSocket,1);
						socket->handle = sock;
						socket->cryptkey = cryptkey;
						strcpy(socket->address,inet_ntoa(clntAddr.sin_addr));
						sockets[sock] = socket;
						_list_add(&server->sockets,&socket->elem);
						FD_SET(sock,&reads);
						fd_max = max(sock+1,fd_max);
						server->socket_count++;
					}
					else closesocket(sock);
				}
				else if(reads.fd_array[i] == hLogin)
				{
					login_receive();
				}
				else
				{
					
					BSocket* sock = sockets[reads.fd_array[i]];
					int size;
					if(sock->last_time > clock()) {
						if(sock->sndOffset) socket_flush(sock);
						continue;
					}
					if(sock->rcvOffset >= RECV_BUF_MAX) {
						
						int psize = UNSWAP16(sock->rcvBuffer+1);
						if(sock->rcvOffset - 3 >= psize)
						{
							int d = rcv(sock);
							if( d < 0 || sock->eof )
							{
								socket_close(sock); i--;
								continue;
							}
							memmove(sock->rcvBuffer,sock->rcvBuffer+d,sock->rcvOffset - d);
							sock->rcvOffset -= d;
							sock->last_time = clock() + CLOCK_RECEIVE;
						}
						else {
							socket_close(sock);
							continue;
						}
					}
					size = recv(sock->handle,(char*)sock->rcvBuffer + sock->rcvOffset,RECV_BUF_MAX - sock->rcvOffset,0);
					if(size <= 0)
					{
						socket_close(sock); i--;
						continue;
					}
					else if((sock->rcvOffset+=size) >= 3){
						int psize = UNSWAP16(sock->rcvBuffer+1);
						if(sock->rcvBuffer[0] != 0xAA)
						{
							socket_close(sock); i--;
							continue;
						}
						if(sock->rcvOffset - 3 >= psize)
						{
							int d = rcv(sock);
							if( d < 0 || sock->eof )
							{
								socket_close(sock); i--;
								continue;
							}
							memmove(sock->rcvBuffer,sock->rcvBuffer+d,sock->rcvOffset - d);
							sock->rcvOffset -= d;
							sock->last_time = clock() + CLOCK_RECEIVE;
							if(sock->sndOffset) socket_flush(sock);
						}
					}
				}
			}
			else
			{
				BSocket* sd = sockets[reads.fd_array[i]];
				if(sd != NULL && sd->sndOffset) socket_flush(sd);
			}
		}
	}
}