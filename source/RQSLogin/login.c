#include "socket.h"
#include "debug.h"
#include "cryption.h"
#include "mem.h"
#include "login.h"
#include "zlib/zlib.h"
#include <stdio.h>
struct MultiServer mlist[16];
int mcount = 0;
unsigned char* multiserver_packet;
unsigned int multiserver_size;

char meta[0x10003];
int metalist_size=0,max_metadat,num_metadat=0;
struct s_metadat* metadat;
void login_send_website(BSocket* socket,char* website)
{
	byte packet[100];
	int len;
	packet[0] = 0xAA;
	packet[1] = 0;
	packet[3] = 0x66;
	packet[4] = 0;
	packet[5] = 3;

	packet[6] =len= strlen(website);
	memmove(packet+7,website,packet[6]);
	//packet[7+packet[6]] = 0;
	packet[2] = 4+packet[6];
	encrypt(0,packet,cryptkey);
	send(socket->handle,(char*)packet,7+len,0);
}
unsigned int agreement_crc;
unsigned long agreement_size;
byte agreement_packet[0x10000];
char agreement_msg[]= "¾È³çÇÏ¼¼¿© Äí¿ìÀÔ´Ï´Ù";
void login_send_agreementcrc(BSocket* sock)
{
	byte packet[10];
	packet[0] = 170;
	packet[1] = 0;
	packet[2] = 7;
	packet[3] = 60;
	packet[4] = 0;
	packet[5] = 0;
	*(unsigned int*)(packet+6) = agreement_crc;
	encrypt(0,packet,cryptkey);
	send(sock->handle,(char*)packet,10,0);
}
void agreement_init()
{
	char temp[65536];
	char* p;
	agreement_crc = SWAP32(crc32(0,(Bytef*)agreement_msg,sizeof(agreement_msg)));
	agreement_size = 65536;
	PANIC(!compress2((Bytef*)temp,&agreement_size,(byte*)agreement_msg,strlen(agreement_msg),Z_BEST_COMPRESSION),"Compression Error")

	p = (char*)&agreement_packet[0];
	*p++ = 170;
	*((unsigned short*)p) = SWAP16(agreement_size-3);
	p+=2;
	*p++ = 96;
	*p++ = 0;
	*p++ = 1;
	*((unsigned short*)p) = SWAP16(agreement_size-9);
	p+=2;
	memmove(p,temp,agreement_size-9);
	p+= agreement_size-9;
	*p++ = 0;
	encrypt(0,agreement_packet,cryptkey);
}
void multiserver_packet_init()
{
	char temp[65536];
	int size = 65536;
	int i;
	unsigned long msize = 0;
	unsigned char data[65536]={0};
	unsigned char* p = data+1,*packet;
	memset(temp,0,sizeof(temp));
	data[0] = mcount;
	for(i=0;i<mcount;i++)
	{
		*(p++) = i;
		*p++ = (BYTE)(mlist[i].ip&0xFF);
		*p++ = (BYTE)((mlist[i].ip>>8)&0xFF);
		*p++ = (byte)((mlist[i].ip>>16)&0xFF);
		*p++ = (byte)((mlist[i].ip>>24)&0xFF);
		*p++ = mlist[i].port%256;
		*p++ = mlist[i].port/256;
		memcpy(p,mlist[i].name,strlen(mlist[i].name));
		p+= strlen(mlist[i].name);
		*p++ = 0;
	}

	msize = 65536;
	size = p - data;
	PANIC(!compress2((Bytef*)temp,&msize,data,size,9),"Compression Error")
	packet = MALOC(unsigned char,msize+=8);
	memset(packet,0,sizeof(packet));
	packet[0]=170;
	*(unsigned short*)(packet+1) = SWAP16(msize-3);
	packet[3] = 86;
	packet[4] = 0;
	*(unsigned short*)(packet+5) = SWAP16(msize-8);
	memmove(packet+7,temp,msize-8);
	packet[msize-1] = 0;
	cpt(packet);
	multiserver_packet = packet;
	multiserver_size = msize;
}
void login_send_metalist(BSocket* sock)
{
	send(sock->handle,meta,metalist_size,0);
}
void login_send_agreement(BSocket* sock)
{
	send(sock->handle,(char*)agreement_packet,agreement_size,0);
}
void login_init()
{
	agreement_init();
	multiserver_packet_init();
	load_metadata("metadat.txt");
}
void multiserver_add(char* name ,char* ip ,unsigned short port)
{
	strcpy(mlist[mcount].name,name);
	mlist[mcount].ip = inet_addr(ip);
	mlist[mcount].port = port;
	mcount++;
}
void login_send_list(BSocket* socket)
{
	send(socket->handle,(char*)multiserver_packet,multiserver_size,0);
}
void login_send_changekey(BSocket* socket,char type,char* key)
{
	BYTE packet[0x14];
	
	packet[0] = 0xAA;
	packet[1] = 0x00;
	packet[2] = 0x11;
	packet[3] = 0;
	packet[4] = 0;
	packet[5] = 0;
	packet[6] = 0;
	packet[7] = 0;
	packet[8] = 0;
	packet[9] = type;
	packet[10] = 0x09;
	memmove(packet+11,key,9);
	send(socket->handle,(char*)packet,0x14,0);
}
int login_send_message(BSocket* sock,char type,char* message)
{
	int size = strlen(message)+7;
	char* buff = MALOC(char,size);
	buff[0] = '\xAA';
	*(unsigned short*)(buff+1) = SWAP16(size-3);
	buff[3] = 2;
	buff[4] = 0;
	buff[5] = type;
	buff[6] = size-7;
	memmove(buff+7,message,size-7);
	encrypt(0,(unsigned char*)buff,sock->cryptkey);
	return send(sock->handle,buff,size,0);
}
int login_send_servermove(BSocket* sock,unsigned int ip,unsigned short port)
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
void login_send_meta(BSocket* sd,BYTE* buf)
{
	int len = buf[6];
	int i;
	buf[7+len] = 0;
	for(i=0;i<num_metadat;i++){
		if(strcmp((char*)buf+7,(char*)metadat[i].name)==0){
			send(sd->handle,(char*)metadat[i].packet,metadat[i].packet_size,0);

			break;
		}
	}
}
void login_send_multiserver(BSocket* sock,int index)
{
	int size;
	unsigned int ip = mlist[index].ip;
	unsigned short port = mlist[index].port;
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
	*(unsigned int*)(packet+29) = SWAP32(0x381E0200);
	//crypt(packet);
	send(sock->handle,(char*)packet,33,0);
}


#define WBUFP(p,pos) (((byte*)(p)) + (pos))
#define WBUFB(p,pos) (*(byte*)WBUFP((p),(pos)))
#define WBUFW(p,pos) (*(unsigned short*)WBUFP((p),(pos)))
#define WBUFL(p,pos) (*(unsigned int*)WBUFP((p),(pos)))

void make_metadat_sub(const char* name, int entries)
{
    char line[2048], *arr[256], *search;
    unsigned char buf[0x10000];
    unsigned long buf_size;
    unsigned long size;
    unsigned long crc;
    int ret, properties, len, i, j;
    METADAT *pMetadat;
    FILE* fp;

	_snprintf((char*)buf, sizeof(buf), "metadat/%s.txt", name);
    
    if (NULL == (fp = fopen(buf, "rt"))) {
		errf("[Error] Metadat file not found");
        return;
    }

    pMetadat = &metadat[num_metadat];
    strncpy(pMetadat->name, name, sizeof(pMetadat->name));
	pMetadat->name_size = strlen(name);
    
    *((unsigned short*)(buf)) = SWAP16(entries);
    buf_size = 2;

	for (i = 0; i < entries; ++i) {
        fgets(line, sizeof(line), fp);

        search = strchr(line, '\n');
        if (search) *search = 0;

        strsplit(line, '\t', arr, 256);

        len = strlen(arr[0]);
        WBUFB(buf, buf_size) = len;
        memcpy(WBUFP(buf, buf_size + 1), arr[0], len);
        buf_size += len + 1;

        properties = atoi(arr[1]);
        WBUFW(buf, buf_size) = SWAP16(properties);
        buf_size += 2;

		for (j = 0; j < properties; ++j) {
            len = strlen(arr[j + 2]);
            WBUFW(buf, buf_size) = SWAP16(len);
            memcpy(WBUFP(buf, buf_size + 2), arr[j + 2], len);
            buf_size += len + 2;
        }
    }

    fclose(fp);

    crc = SWAP32(crc32(0, buf, buf_size));
    len = pMetadat->name_size;

    size = 0x10000;
    if (ret = compress2(&(pMetadat->packet[len + 13]), &size, buf, buf_size, Z_BEST_COMPRESSION)) {
		errf("[Error] Metadata: Data compression failure!");
        return;
    }

    WBUFB(pMetadat->packet, 0) = 0xAA;
    WBUFW(pMetadat->packet, 1) = SWAP16(len + size + 10);
    WBUFB(pMetadat->packet, 3) = 0x6F;
    WBUFB(pMetadat->packet, 4) = 0x00;
    WBUFB(pMetadat->packet, 5) = 0x00;
    WBUFB(pMetadat->packet, 6) = len;
    memcpy(WBUFP(pMetadat->packet, 7), name, len);
    WBUFL(pMetadat->packet, len + 7) = crc;
    WBUFW(pMetadat->packet, len + 11) = SWAP16(size);
    pMetadat->packet_size = len + size + 13;
	encrypt(0,pMetadat->packet,cryptkey);
    ++num_metadat;

    WBUFB(meta, metalist_size) = len;
    memcpy(WBUFP(meta, metalist_size + 1), name, len);
    WBUFL(meta, metalist_size + len + 1) = crc;
    metalist_size += len + 5;
    return;
}

int strsplit(char* s, const char ch, char** v, int max)
{
    int num = 0;

    while( s && max > num )
    {
        if( num ) *(s++) = 0;
        v[num++] = s;
        s = strchr(s, ch);
    }

    return num;
}
void load_metadata(char* filename)
{
	char line[1024],w1[1024],w2[1024];
    FILE* fp;
	char* arr[3];
	//char meta[0x10003];
    //int num_metadat;
    meta[0] = 0xAA;
    meta[1] = 0x05;
	meta[2] = 00;
	meta[3] = 0x6F;
	meta[4] = 0;
	meta[5] = 1;
	meta[6] = 0;

    metalist_size = 8;

	PANIC (NULL != (fp = fopen(filename, "rt")),"METADAT");


    //ErrorMessage("Loading metadat file\n");

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == ';')
            continue;

        if (sscanf(line, "%[^:]: %[^\r\n]", w1, w2) == 2) {
            if (strcmp(w1, "count") == 0)
                metadat = calloc((max_metadat = atoi( w2 )) * sizeof(struct s_metadat),1);
            
			else if (strcmp(w1, "load") == 0) {
                

				PANIC( max_metadat != num_metadat , "Not enough space for metadat. please increase the space!\n");


                if ( strsplit(w2,',',arr,2) == 2 )
                    make_metadat_sub(arr[0], atoi(arr[1]));
            }
        }
    }
    fclose(fp);

	WBUFW(meta, 1) = SWAP16(metalist_size - 3);
	WBUFW(meta, 6) = SWAP16(num_metadat);
	
	encrypt(0,(byte*)meta,cryptkey);
}