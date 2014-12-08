#include "socket.h"
#include "debug.h"
#include "timer.h"
#include "mem.h"

#include <time.h>
#include "cryption.h"
BServer* server;
char* cryptkey = defaultCryptKey;
BSocket* sockets[0x10000];
void socket_init()
{
	WSADATA wsaData;
	PANIC(WSAStartup(MAKEWORD(2,2),&wsaData)==0,"WSA Start error");
}
void BaramOpen(BServer* serv,int (*rcv)(BSocket*),int (*close)(BSocket*),int (*acc)(SOCKET,char* addr))
{
	SOCKADDR_IN addr;
	struct timeval timeout;
	fd_set reads;
	fd_set temps;
	server = serv;
	serv->handle = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	PANIC(serv->handle != INVALID_SOCKET,"Socket Create Error");
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serv->port);
	list_init(&serv->sockets);
	PANIC(bind(serv->handle,(struct sockaddr*)&addr,sizeof(addr))!=SOCKET_ERROR,"BIND ERROR");
	PANIC(listen(serv->handle,5)!=SOCKET_ERROR,"LISTEN ERROR");
	FD_ZERO(&reads);
	FD_SET(serv->handle,&reads);
	while(TRUE)
	{
		int i;
		temps = reads;
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
		PANIC(select(0,&temps,0,0,&timeout) != SOCKET_ERROR,"SELECT ERROR");
		timer_check();
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
						FD_SET(sock,&reads);
					}
					else closesocket(sock);
				}
				else
				{
					
					BSocket* sock = sockets[reads.fd_array[i]];
					if(sock != NULL)
					{
						int size;
						if(sock->rcvOffset >= RECV_BUF_MAX) continue;
						size = recv(sock->handle,(char*)sock->rcvBuffer + sock->rcvOffset,RECV_BUF_MAX - sock->rcvOffset,0);
						if(size <= 0)
						{
							close(sock);
							FD_CLR(sock->handle,&reads);
							closesocket(sock->handle);
							continue;
						}
						else if((sock->rcvOffset+=size) >= 3){
							int psize = UNSWAP16(sock->rcvBuffer+1);
							if(sock->rcvBuffer[0] != 0xAA)
							{
								close(sock);
								FD_CLR(sock->handle,&reads);
								closesocket(sock->handle);
								continue;
							}
							if(sock->rcvOffset - 3 >= psize)
							{
								int d = rcv(sock);
								if( d < 0)
								{
									close(sock);
									FD_CLR(sock->handle,&reads);
									closesocket(sock->handle);
									continue;
								}
								memmove(sock->rcvBuffer,sock->rcvBuffer+d,sock->rcvOffset - d);
								sock->rcvOffset -= d;
							}
						}
					}
				}
			}
		}
	}
}