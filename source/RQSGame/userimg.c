
#include <stdlib.h>
#include "socket.h"
#include "userimg.h"
#include "map.h"
#include "object.h"
#include "hashtable.h"
#define UIMG_MAX 4000
struct user_image* image_table[UIMG_MAX] = {0};

struct user_image standard_img = {
	0,-1,0,0,{0,-1,-1},-1,0,1,-1,{0,-1,-1},-1,{0,-1,-1},-1,{0,-1,-1},-1,{0,-1,-1},-1,-1,60,0,0,201
};
void uimg_show_single(int id,struct mapobject* obj,BSocket* sd)
{
	char* name;
	int size;
	int namelen;
	struct user_image* uimg = image_table[id];
	name = object_get_name(obj,&namelen);
	
	sdsize(sd,100);
	wb(sd,0) = 170;
	wb(sd,3) = 51;
	wb(sd,4) = 0;
	ws(sd,5) = SWAP16(obj->loc.x);
	ws(sd,7) = SWAP16(obj->loc.y);
	wb(sd,9) = obj->loc.side; // side
	wl(sd,10) = SWAP32(obj->id);
	wb(sd,14) = 0; // look
	wb(sd,15) = uimg->gender; //sex
	wb(sd,16) = 0; // state
	ws(sd,17) = SWAP16(0); // ridign
	wb(sd,19) = 75; // walk speed
	ws(sd,20) = SWAP16(uimg->face);
	size=22;
	wb(sd,size++) = uimg->hair; // hair
	wb(sd,size++) = 0;
	ws(sd,size) = SWAP16(uimg->img_armor);size+=2;
	wb(sd,size++) = uimg->col_armor[0];
	wb(sd,size++) = uimg->col_armor[1];
	wb(sd,size++) = uimg->col_armor[2];
	
	ws(sd,size) = SWAP16(uimg->img_weapon);size+=2;
	wb(sd,size++) = uimg->col_weapon;
	ws(sd,size) = SWAP16(uimg->img_shield);size+=2;
	wb(sd,size++) = uimg->col_shield;
	wb(sd,size++) = uimg->img_helmet!=0xFFFF;
	ws(sd,size) = SWAP16(uimg->img_helmet);size+=2;
	wb(sd,size++) = uimg->col_helmet[0];
	wb(sd,size++) = uimg->col_helmet[1];
	wb(sd,size++) = uimg->col_helmet[2];
	ws(sd,size) = SWAP16(uimg->img_faceacc);size+=2;
	wb(sd,size++) = uimg->col_faceacc[0];
	wb(sd,size++) = uimg->col_faceacc[1];
	wb(sd,size++) = uimg->col_faceacc[2];
	
	ws(sd,size) = SWAP16(uimg->img_cape);size+=2;
	wb(sd,size++) = uimg->col_cape[0];
	wb(sd,size++) = uimg->col_cape[1];
	wb(sd,size++) = uimg->col_cape[2];
	ws(sd,size) = SWAP16(uimg->img_shoes);size+=2;
	wb(sd,size++) = uimg->col_shoes[0];
	wb(sd,size++) = uimg->col_shoes[1];
	wb(sd,size++) = uimg->col_shoes[2];
	ws(sd,size) = SWAP16(uimg->img_necklace);size+=2;
	wb(sd,size++) = uimg->col_neck;
	
	//생산타이틀
	wb(sd,size++) = 0;
	wb(sd,size++) = 0; // 아이디 컬러
	
	wb(sd,size++) = (namelen);
	memmove(wp(sd,size),name,wb(sd,size-1));
	size += wb(sd,size-1);
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
void uimg_show_sight(int id,struct mapobject* obj)
{
	BYTE packet[100];
	struct user_image* uimg = image_table[id];
	char* name;
	int size,pos;
	int namelen;
	if(uimg == NULL) return ;
	name = object_get_name(obj,&namelen);
	WB(packet,0) = 170;
	WB(packet,3) = 51;
	WB(packet,4) = 0;
	WS(packet,5) = SWAP16(obj->loc.x);
	WS(packet,7) = SWAP16(obj->loc.y);
	WB(packet,9) = obj->loc.side; // side
	WL(packet,10) = SWAP32(obj->id);
	WB(packet,14) = 0; // look
	WB(packet,15) = uimg->gender; //sex
	WB(packet,16) = 0; // state
	WS(packet,17) = SWAP16(0); // ridign
	WB(packet,19) = 75; // walk speed
	WS(packet,20) = SWAP16(uimg->face);
	size=22;
	WB(packet,size++) = uimg->hair; // hair
	WB(packet,size++) = 0;
	WS(packet,size) = SWAP16(uimg->img_armor);size+=2;
	WB(packet,size++) = uimg->col_armor[0];
	WB(packet,size++) = uimg->col_armor[1];
	WB(packet,size++) = uimg->col_armor[2];
	
	WS(packet,size) = SWAP16(uimg->img_weapon);size+=2;
	WB(packet,size++) = uimg->col_weapon;
	WS(packet,size) = SWAP16(uimg->img_shield);size+=2;
	WB(packet,size++) = uimg->col_shield;
	WB(packet,size++) = uimg->img_helmet != 0xFFFF;
	WS(packet,size) = SWAP16(uimg->img_helmet);size+=2;
	WB(packet,size++) = uimg->col_helmet[0];
	WB(packet,size++) = uimg->col_helmet[1];
	WB(packet,size++) = uimg->col_helmet[2];
	WS(packet,size) = SWAP16(uimg->img_faceacc);size+=2;
	WB(packet,size++) = uimg->col_faceacc[0];
	WB(packet,size++) = uimg->col_faceacc[1];
	WB(packet,size++) = uimg->col_faceacc[2];
	
	WS(packet,size) = SWAP16(uimg->img_cape);size+=2;
	WB(packet,size++) = uimg->col_cape[0];
	WB(packet,size++) = uimg->col_cape[1];
	WB(packet,size++) = uimg->col_cape[2];
	WS(packet,size) = SWAP16(uimg->img_shoes);size+=2;
	WB(packet,size++) = uimg->col_shoes[0];
	WB(packet,size++) = uimg->col_shoes[1];
	WB(packet,size++) = uimg->col_shoes[2];
	WS(packet,size) = SWAP16(uimg->img_necklace);size+=2;
	WB(packet,size++) = uimg->col_neck;
	
	//생산타이틀
	WB(packet,size++) = 0;
	WB(packet,pos=size++) = 0; // 아이디 컬러
	
	WB(packet,size++) = (namelen);
	memmove(packet+size,name,WB(packet,size-1));
	size += WB(packet,size-1);
	WS(packet,1) = SWAP16(size-3);
	write_sight(&obj->loc,packet);
}
void uimg_init()
{
	image_table[0] = &standard_img;
}