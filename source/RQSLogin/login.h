#pragma once
#include "list.h"
struct MultiServer
{
	char name[256];
	unsigned int ip;
	unsigned short port;
};

typedef struct s_metadat {
    unsigned char packet[0x10003]; // 0x6F
    int packet_size;
    unsigned char name[48];
    int name_size;
} METADAT;
void load_metadata(char* filename);
void multiserver_add(char*,char*,unsigned short);

int login_send_servermove(BSocket* sock,unsigned int ip,unsigned short port);
void login_send_meta(BSocket* sd,BYTE* buf);
void login_send_metalist(BSocket* sock);
void login_send_agreementcrc(BSocket* sock);
int login_send_message(BSocket* sock,char type,char* message);
void login_send_agreement(BSocket* sock);
void login_send_multiserver(BSocket* sock,int index);

void login_send_list(BSocket* socket);
void login_init();
void login_send_changekey(BSocket* socket,char type,char* key);
void login_send_website(BSocket* socket,char* website);