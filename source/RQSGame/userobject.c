#include "userobject.h"
#include "socket.h"
#include "userimg.h"
#include "item.h"
#include "pc.h"
#include "map.h"
void userobject_del(struct mapobject* obj)
{

}
void user_show_single(BSocket* sd,struct userobject* obj)
{
	struct itemobject* equip;
	int size;
	wb(sd,0) = 170;
	wb(sd,3) = 51;
	wb(sd,4) = 0;
	ws(sd,5) = SWAP16(obj->obj.loc.x);
	ws(sd,7) = SWAP16(obj->obj.loc.y);
	wb(sd,9) = obj->obj.loc.side; // side
	wl(sd,10) = SWAP32(obj->obj.id);
	wb(sd,14) = obj->obj.state==disguise; // look
	wb(sd,15) = obj->ud.gender; //sex
	if(obj->obj.state == 2 ){
		if((sd->obj.group != NULL && sd->obj.group == sd->obj.group ) || (sd->obj.opt & no_invisible) )
		{
			wb(sd,16) = 5;
		}
	} else {
		wb(sd,16) = obj->obj.state;
	}
	if(obj->obj.state == 4)
	{
		if(obj->obj.image < 65536){
			size = 17;
			ws(sd,size) = obj->obj.image;
			size +=2;
			wb(sd,size++) = obj->obj.color;
		}
	} else {
		if(obj->obj.image >= 65536){
			struct user_image* uimg = image_table[obj->obj.image-65536];
			ws(sd,17) = SWAP16(0); // ridign
			wb(sd,19) = 75; // walk speed
			ws(sd,20) = SWAP16(uimg->face);
			size=22;
			wb(sd,size++) = uimg->hair; // hair
			wb(sd,size++) = uimg->haircol;
			ws(sd,size) = SWAP16(uimg->img_armor);size+=2;
			wb(sd,size++) = uimg->col_armor[0];
			wb(sd,size++) = uimg->col_armor[1];
			wb(sd,size++) = uimg->col_armor[2];
	
			ws(sd,size) = SWAP16(uimg->img_weapon);size+=2;
			wb(sd,size++) = uimg->col_weapon;
			ws(sd,size) = SWAP16(uimg->img_shield);size+=2;
			wb(sd,size++) = uimg->col_shield;
			wb(sd,size++) = uimg->img_helmet!=0xffff;
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
		} else {
			ws(sd,17) = SWAP16(0); // ridign
			wb(sd,19) = 75; // walk speed
			ws(sd,20) = SWAP16(obj->ud.face);

			size=22;
			wb(sd,size++) = obj->ud.hair; // hair
			wb(sd,size++) = obj->ud.hair_color;
			if((equip = obj->equips[i_outwear]) || (equip = obj->equips[i_armor]))
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				wb(sd,size++) = 0;
				wb(sd,size++) = obj->ud.gender; // °©¿Ê sex?
				wb(sd,size++) = 0; // °©¿Ê ÄÃ¤©
				wb(sd,size++) = -1;
				wb(sd,size++) = -1;
			}
			//¹«±â 3¹ÙÀÌÆ®
			//¹æÆÐ 3¹ÙÀÌÆ®
			if(equip = obj->equips[i_weapon]){
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
			}
			else
			{
				wb(sd,size++) = -1;
				wb(sd,size++) = -1;
				wb(sd,size++) = 0;
			}
			if(equip = obj->equips[i_shield])
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
			}
			else{
				wb(sd,size++) = -1;
				wb(sd,size++) = -1;
				wb(sd,size++) = 0;
			}
			//Çï¸ä 
			if((equip = obj->equips[i_tara]) || (equip = obj->equips[i_helmet]))
			{
				wb(sd,size++) = obj->opt & helmet_visible; // helmet visible
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2
			}
			else{
				wb(sd,size++) = 0; // helmet visible
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++) = 0;
				wb(sd,size++) = -1; // helmet color 1
				wb(sd,size++) = -1; // helmet color 2
			}
			//¾ó±¼Àå½Ä
			if(equip = obj->equips[i_faceacc])
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2
			}
			else{
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++) = 0;
				wb(sd,size++) = -1; // 
				wb(sd,size++) = -1; // 
			}
			//¸ÁÅä
			if(equip = obj->equips[i_cape])
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++) = 0;
				wb(sd,size++) = -1; // 
				wb(sd,size++) = -1; // h
			}
	
			//½Å¹ß
			if(equip = obj->equips[i_shoes])
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++) = 0;
				wb(sd,size++) = -1; //
				wb(sd,size++) = -1; //
			}
			//¸ñ°ÉÀÌ 3¹ÙÀÌÆ®
			if(equip = obj->equips[i_necklace])
			{
				ws(sd,size) = SWAP16(equip->db->equip.image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];

			}
			else{
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++) = -1;
			}
		}
	}
	//»ý»êÅ¸ÀÌÆ²
	wb(sd,size++) = 1;
	wb(sd,size) = 0;
	size++;
	wb(sd,size++) = (obj->ud.id_len);
	memmove(wp(sd,size),obj->ud.id,wb(sd,size-1));
	size += wb(sd,size-1);
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
void user_show_sight(struct userobject* obj)
{
	BSocket* sd = socket_first(obj->obj.loc.map);
	while(sd != NULL)
	{
		if(check_sight(&sd->obj.obj.loc,&obj->obj.loc)) {
			user_show_single(sd,obj);
		}
		sd = socket_next(sd);
	}
}
void user_lookrefresh(struct userobject* obj)
{
	BYTE packet[256];
	struct itemobject* equip;
	register int size=12,pos,i;
	//sd¸¦ º¸¿©ÁØ´Ù. 
	WB(packet,0) = 170;
	WB(packet,3) = 29;
	WB(packet,4) = 0;
	WL(packet,5) = SWAP32(obj->obj.id);
	WB(packet,9) = obj->obj.state ==4;
	WB(packet,10) = obj->ud.gender;
	WB(packet,11) = obj->obj.state;
	if(obj->obj.state != disguise)
	{
		WS(packet,size) = SWAP16(obj->obj.state== horse ? obj->obj.image : 0 );
		size+=2;
		WB(packet,size++) = 75; // walk speed
		

		if(obj->obj.image >= 65536){
			
			struct user_image* uimg = image_table[obj->obj.image-65536];
			WS(packet,size) = SWAP16(uimg->face);
			size+=2;
			WB(packet,size++) = uimg->hair; // hair
			WB(packet,size++) = uimg->haircol;
			WS(packet,size) = SWAP16(uimg->img_armor);size+=2;
			WB(packet,size++) = uimg->col_armor[0];
			WB(packet,size++) = uimg->col_armor[1];
			WB(packet,size++) = uimg->col_armor[2];
	
			WS(packet,size) = SWAP16(uimg->img_weapon);size+=2;
			WB(packet,size++) = uimg->col_weapon;
			WS(packet,size) = SWAP16(uimg->img_shield);size+=2;
			WB(packet,size++) = uimg->col_shield;
			WB(packet,size++) = uimg->img_helmet !=0xffff;
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
		} else {
			WS(packet,size) = SWAP16(obj->ud.face);
			size +=2;

			WB(packet,size++) = (obj->ud.hair);
			WB(packet,size++) = obj->ud.hair_color; // haircolor
			if((equip = obj->equips[i_outwear]) || (equip = obj->equips[i_armor]))
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
				WB(packet,size++) = equip->db->equip.color[1]; // armcolor 1
				WB(packet,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				WB(packet,size++) = 0;
				WB(packet,size++) = obj->ud.gender; // °©¿Ê sex?
				WB(packet,size++) = 0; // °©¿Ê ÄÃ¤©
				WB(packet,size++) = -1;
				WB(packet,size++) = -1;
			}
			//¹«±â 3¹ÙÀÌÆ®
			//¹æÆÐ 3¹ÙÀÌÆ®
			if(equip = obj->equips[i_weapon]){
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
			}
			else
			{
				WB(packet,size++) = -1;
				WB(packet,size++) = -1;
				WB(packet,size++) = 0;
			}
			if(equip = obj->equips[i_shield])
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
			}
			else{
				WB(packet,size++) = -1;
				WB(packet,size++) = -1;
				WB(packet,size++) = 0;
			}
			//Çï¸ä 
			if((equip = obj->equips[i_tara]) || (equip = obj->equips[i_helmet]))
			{
				WB(packet,size++) =obj->opt & helmet_visible; // helmet visible
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
				WB(packet,size++) = equip->db->equip.color[1]; // armcolor 1
				WB(packet,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				WB(packet,size++) = 0;
				WS(packet,size) = SWAP16(-1);
				size+=2;
				WB(packet,size++) = 0;
				WB(packet,size++) = -1; // helmet color 1
				WB(packet,size++) = -1; // helmet color 2
			}
			//¾ó±¼Àå½Ä
			if(equip = obj->equips[i_faceacc])
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
				WB(packet,size++) = equip->db->equip.color[1]; // armcolor 1
				WB(packet,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				WS(packet,size) = SWAP16(-1);
				size+=2;
				WB(packet,size++) = 0;
				WB(packet,size++) = -1; // 
				WB(packet,size++) = -1; // 
			}
			//¸ÁÅä
			if(equip = obj->equips[i_cape])
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
				WB(packet,size++) = equip->db->equip.color[1]; // armcolor 1
				WB(packet,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				WS(packet,size) = SWAP16(-1);
				size+=2;
				WB(packet,size++) = 0;
				WB(packet,size++) = -1; // 
				WB(packet,size++) = -1; // h
			}
	
			//½Å¹ß
			if(equip = obj->equips[i_shoes])
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];
				WB(packet,size++) = equip->db->equip.color[1]; // armcolor 1
				WB(packet,size++) = equip->db->equip.color[2]; // armcolor 2

			}
			else{
				WS(packet,size) = SWAP16(-1);
				size+=2;
				WB(packet,size++) = 0;
				WB(packet,size++) = -1; //
				WB(packet,size++) = -1; //
			}
			//¸ñ°ÉÀÌ 3¹ÙÀÌÆ®
			if(equip = obj->equips[i_necklace])
			{
				WS(packet,size) = SWAP16(equip->db->equip.image);
				size+=2;
				WB(packet,size++) = equip->db->equip.color[0];

			}
			else{
				WS(packet,size) = SWAP16(-1);
				size+=2;
				WB(packet,size++) = -1;
			}
		}
	}
	else
	{
		WS(packet,size) = SWAP16(obj->obj.image);
		size+=2;
		WB(packet,size++) = obj->obj.color;
	}
		//»ý»êÅ¸ÀÌÆ²
	WB(packet,size++) = 1;
	WB(packet,pos=size++) = 0; // ¾ÆÀÌµð ÄÃ·¯
	WB(packet,size++) = (obj->ud.id_len);
	memmove(packet+size,obj->ud.id,packet[size-1]);
	size += packet[size-1];
	WS(packet,1) = SWAP16(size-3);
	write_sight(&obj->obj.loc,packet);
}
void user_set_state(struct userobject* obj,int state)
{
	obj->obj.state = state;
	user_lookrefresh(obj);
}

struct object_event user_event = {
	userobject_del,
//	user_damage
};
unsigned int userobject_new(struct userobject* uobj)
{
	uobj->obj.type = USER;

	uobj->ud.hp = uobj->ud.maxhp= uobj->ud.maxhp = uobj->ud.hp = 53;
	uobj->ud.mp = uobj->ud.maxmp = uobj->ud.maxmp = uobj->ud.mp = 53;
	uobj->ud.exp = 30000;
	uobj->ud.job = 1;
	uobj->ud.joblv = 1;
	uobj->ud.nation = 1;
	uobj->ud.money = 400000;
	uobj->ud.grace = uobj->ud.will = uobj->ud.might = 3;
	uobj->ud.level = 1;
	uobj->empty_count = ITEM_SLOT_MAX;
	uobj->ud.state = 0;
	uobj->obj.events = &user_event;
	uobj->last_magictick = 0;
	object_new(&uobj->obj,1);
	
	shell_event(ON_LOGIN,"is",uobj->obj.id,uobj->ud.id);
	return uobj->obj.id;
}