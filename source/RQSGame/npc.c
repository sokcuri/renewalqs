#include "npc.h"
#include "socket.h"
#include "debug.h"
#include "mem.h"
struct npcdb* npcs[NPC_DB_MAX];
struct npcdb standard_npc = {1,32768,0,6,"테스트"};
void npc_init()
{
}
void register_npcdb(int code,char* name,unsigned int img,unsigned char col)
{
	struct npcdb* m;
	if(code >= NPC_DB_MAX) {
		errf("NPC 코드 범위를 초과했습니다(%d) \n",code);
		return;
	}
	if(npcs[code] != NULL) {
		m = npcs[code];
	}
	else npcs[code] = m = MALOC(struct npcdb,1);
	m->id = code;
	strncpy(m->name,name,NPC_NAME_MAX);
	m->name_len = strlen(m->name);
	m->image = img ;
	m->color = col;
}
void select_menu(BSocket* sd,unsigned short img,unsigned char color,char* message,int count,char** menu)
{
	
	register unsigned int size = strlen(message) + 22, i;
	wb(sd, 0) = 170;
	wb(sd, 3) = 47;
	wb(sd, 4) = 0;
	wb(sd, 5) = 0;
	sd->dlg_type = wb(sd, 6) = menu_type;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (img > 49151)?2:((img)?1:0);
	wb(sd, 12) = 1;
	wi(sd, 13) = SWAP16(img);
	wb(sd, 15) = color;
	wb(sd, 16) = (img > 49151)?2:((img)?1:0);
	wi(sd, 17) = SWAP16(img);
	wb(sd, 19) = color;
	wi(sd, 20) = SWAP16(size-22);
	memmove(wp(sd, 22), message, size-22);
	wb(sd, size++) = count;
	for(i=0; i<count; i++) {
		wb(sd, size++) = strlen(menu[i]);
		memmove(wp(sd, size), menu[i], wb(sd, size-1));
		size += wb(sd, size-1);
		ws(sd, size) = SWAP16(i);
		size += 2;
	}
	wb(sd, size++) = 0;
	ws(sd, 1) = SWAP16(size-3);
	cpt(sd);
}
void select_islotlist(BSocket* sd,unsigned short img,unsigned char color,char* message,char* slot)
{
	register int i;
	register unsigned int pos,sz = 22+strlen(message);
	wb(sd, 0) = 170;
	wb(sd, 3) = 47;
	wb(sd, 4) = 0;
	wb(sd, 5) = 5; // NPCTYPE
	sd->dlg_type = wb(sd, 6) = ilist_selltype;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (img > 49151)?2:((img)?1:0); // npc look 0,1,2
	wb(sd, 12) = 1;
	ws(sd, 13) = SWAP16(img); // npc look
	wb(sd, 15) = color; // color
	wb(sd, 16) = wb(sd,11); // npc look 0 1 2?
	ws(sd, 17) = SWAP16(img);
	wb(sd, 19) = 0;
	ws(sd, 20) = SWAP16(sz-22);
	memmove(wp(sd, 22), message, sz-22);
	ws(sd,sz) = SWAP16(0);
	sz+=2;
	wb(sd,pos=sz++) = 0;
	for(i=0;i<ITEM_SLOT_MAX;i++)
	{
		if(sd->obj.ownitems[i] == NULL) continue;
		if(slot[i]){
			wb(sd,pos) ++;
			wb(sd,sz++) = i+1;
		}
	}
	wb(sd,sz++) = 0;
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);
}
void dialog_input(BSocket* sd,unsigned short img,unsigned char color,char* message)
{
	int i=0;
	register unsigned int sz = 23+strlen(message);
	wb(sd, 0) = 170;
	wb(sd, 3) = 47;
	wb(sd, 4) = 0;
	wb(sd, 5) = 3; // NPCTYPE
	wb(sd, 6) = input_type;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (img > 49151)?2:((img)?1:0); // npc look 0,1,2
	wb(sd, 12) = 1;
	ws(sd, 13) = SWAP16(img); // npc look
	wb(sd, 15) = color; // color
	wb(sd, 16) = wb(sd,11); // npc look 0 1 2?
	ws(sd, 17) = SWAP16(img);
	wb(sd, 19) = 0;
	ws(sd, 20) = SWAP16(sz-23);
	memmove(wp(sd, 22), message, sz-23);
	wb(sd,sz-1) = 0;
	ws(sd,1) = SWAP16(sz-3);
	sd->dlg_type = input_type;
	cpt(sd);
}

void select_ilist(BSocket* sd,unsigned short img,unsigned char color,char* message,int count,struct ilist_element* elem)
{
	int i=0;
	register unsigned int sz = 22+strlen(message);
	wb(sd, 0) = 170;
	wb(sd, 3) = 47;
	wb(sd, 4) = 0;
	wb(sd, 5) = 4; // NPCTYPE
	sd->dlg_type = wb(sd, 6) = ilist_type;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (img > 49151)?2:((img)?1:0); // npc look 0,1,2
	wb(sd, 12) = 1;
	ws(sd, 13) = SWAP16(img); // npc look
	wb(sd, 15) = color;; // color
	wb(sd, 16) = wb(sd,11); // npc look 0 1 2?
	ws(sd, 17) = SWAP16(img);
	wb(sd, 19) = 0;
	ws(sd, 20) = SWAP16(sz-22);
	memmove(wp(sd, 22), message, sz-22);
	ws(sd,sz) = SWAP16(1);
	sz+=2;
	ws(sd,sz) = SWAP16(count);
	sz+=2;
	for(i=0;i<count;i++)
	{
		ws(sd,sz) = SWAP16(elem[i].img);
		sz+=2;
		wb(sd,sz++) = elem[i].color;
		wl(sd,sz) = SWAP32(elem[i].price);
		sz+=4;
		wb(sd,sz++) = strlen(elem[i].name);
		memmove(wp(sd,sz),elem[i].name,wb(sd,sz-1));
		sz+= wb(sd,sz-1);
		wb(sd,sz++) = strlen(elem[i].note);
		memmove(wp(sd,sz),elem[i].note,wb(sd,sz-1));
		sz+= wb(sd,sz-1);
	}
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);
}
void select_list(BSocket* sd,unsigned short img,unsigned char color,int type,char* message,int count,char** menu,int prev,int next)
{
	int i;
	int size = strlen(message) + 29;
	wb(sd, 0) = 170;
	wb(sd, 3) = 48;
	wb(sd, 4) = 0;
	wb(sd, 5) = type ? 18 : 2; // NPCTYPE
	wb(sd, 6) = list_type;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (img > 49151)?2:((img)?1:0); // npc look 0,1,2
	wb(sd, 12) = 1;
	ws(sd, 13) = SWAP16(img); // npc look
	wb(sd, 15) = color; // color
	wb(sd, 16) = wb(sd,11); // npc look 0 1 2?
	ws(sd, 17) = SWAP16(img);
	wb(sd, 19) = 0;
	wl(sd, 20) = SWAP32(1);
	wb(sd, 24) = prev & 0xFF;
	wb(sd, 25) = next & 0xFF;
	ws(sd, 26) = SWAP16(size-29);
	memmove(wp(sd, 28), message, size-29);
	wb(sd, size-1) = count;
	for(i=0;i<count;i++)
	{
		wb(sd,size++) = strlen(menu[i]);
		memmove(wp(sd,size),menu[i],wb(sd,size-1));
		size += wb(sd,size-1);
	}
	ws(sd,1) = SWAP16(size-3);
	sd->dlg_type = list_type;
	cpt(sd);
}
void dialog(BSocket* sd,unsigned short image,unsigned char color,int type,char* msg,int prev,int next)
{
	register unsigned int size = strlen(msg) + 29;

	wb(sd, 0) = 170;
	ws(sd, 1) = SWAP16(size-3);
	wb(sd, 3) = 48;
	wb(sd, 4) = 0;
	wb(sd, 5) = type ? 16 : 0; // NPCTYPE
	wb(sd, 6) = 2;
	wl(sd, 7) = SWAP32(sd->last_object->id);
	wb(sd, 11) = (image > 49151)?2:((image)?1:0); // npc look 0,1,2
	wb(sd, 12) = 1;
	ws(sd, 13) = SWAP16(image); // npc look
	wb(sd, 15) = color; // color
	wb(sd, 16) = wb(sd,11); // npc look 0 1 2?
	ws(sd, 17) = SWAP16(image);
	wb(sd, 19) = 0;
	wl(sd, 20) = SWAP32(1);
	wb(sd, 24) = prev&0xFF;
	wb(sd, 25) = next&0xFF;
	ws(sd, 26) = SWAP16(size-29);
	memmove(wp(sd, 28), msg, size-29);
	wb(sd, size-1) = 0;
	sd->dlg_type = msg_type;
	cpt(sd);
}
void npc_click(struct npcobject* obj)
{

}
void npcobject_new(struct npcobject* obj,int code)
{
	obj->obj.showtype = 12;
	obj->db = npcs[code];
	obj->obj.type = NPC;
	obj->obj.image = obj->db->image;
	obj->obj.color = obj->db->color;
	object_new(&obj->obj,1);
}
struct npcobject* create_npcobj(int code)
{
	struct npcobject* obj = CALOC(struct npcobject,1);
	npcobject_new(obj,code);
	return obj;
}