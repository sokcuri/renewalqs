#include "userdata.h"
#include "userimg.h"
#include "mem.h"
#include "socket.h"
struct job** jobs = NULL;
int job_max = 5;
int joblv_max = 5;
void job_set(int job,int joblv)
{
	int i;
	if(jobs){
		for(i=0;i<job_max;i++) {
			_free(jobs[i]);
		}
		_free(jobs);
	}
	jobs = CALOC(struct job*,job);
	for(i=0;i<job;i++)
	{
		jobs[i] = CALOC(struct job,joblv);
	}
	job_max = job; joblv_max = joblv;
}
void user_lookself(BSocket* sd)
{
	int i;
	struct itemobject* equip;
	int size = 9;
	sdsize(sd,400);
	wb(sd, 0) = 170;
	wb(sd, 3) = 57;
	wb(sd, 4) = 0;
	wb(sd, 5) = sd->obj.ud.ac; // ac
	wb(sd, 6) = sd->obj.ud.dam; // dam?
	wb(sd, 7) = sd->obj.ud.hit; // hit?
	if(sd->obj.clan != NULL){
		wb(sd,8) = sd->obj.clan->name_len;
		memmove(wp(sd,size),sd->obj.clan->name,wb(sd,8)); size += wb(sd,8);
		wb(sd,size++) = sd->obj.clan->title_len;
		memmove(wp(sd,size),sd->obj.clan->title,sd->obj.clan->title_len) ; size += sd->obj.clan->title_len;
	}
	else {
		wb(sd,8) = 0;
		wb(sd,size++) = 0;
	}
	wb(sd,size++) = sd->obj.ud.title_len;
	memmove(wp(sd,size),sd->obj.ud.title,wb(sd,size-1));
	size += wb(sd,size-1);
	wb(sd,size++) = 12; // 배우자
	memmove(wp(sd,size),"배우자: 없음",12); size += 12;
	wb(sd,size++) = sd->obj.opt & group_ok ? 1 : 0;//그룹여부
	wl(sd,size) = SWAP32(sd->obj.ud.exp_remain); //남은 경험치
	size+=4;
	wb(sd,size++) = jobs[sd->obj.ud.job][sd->obj.ud.joblv].name_len;
	memmove(wp(sd,size),jobs[sd->obj.ud.job][sd->obj.ud.joblv].name,wb(sd,size-1));
	size+=wb(sd,size-1);

	for(i=0;i<14;i++)
	{
		if( (equip=sd->obj.equips[i]) != NULL)
		{
			ws(sd,size) = SWAP16(equip->db->image);
			size+=2;
			wb(sd,size++) = equip->db->equip.color[0];
			wb(sd,size++) = equip->db->equip.color[1]; // color 1
			if(equip->db->equip.color[0] != 0xFF) {
				wl(sd,size) = SWAP32(0); size+=4;
			}
			wb(sd,size++) = equip->db->equip.color[2];
			if(equip->db->equip.color[1] != 0xFF) {
				wl(sd,size) = SWAP32(0); size+=4;
			}
			wb(sd,size++) = equip->db->name_len; // 아이템 이름(명명)
			memmove(wp(sd,size),equip->db->name,wb(sd,size-1));
			size+= wb(sd,size-1);
			wb(sd,size++) = equip->db->name_len; // 아이템 이름
			memmove(wp(sd,size),equip->db->name,wb(sd,size-1));
			size+= wb(sd,size-1);
			wb(sd,size++) = 0;
			wl(sd,size) = SWAP32(0);
			size+=4;
			wb(sd,size++) = 0;
			wb(sd,size++) = 0;
		}
		else
		{
			ws(sd,size) = SWAP16(-1);
			size+=2;
			wb(sd,size++)=0;
			wb(sd,size++)=0xFf;
			wb(sd,size++)=0xff;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
			wb(sd,size++) = 0;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
			wb(sd,size++)=0;
		}
	}
	wb(sd,size++) = 6;
	memmove(wp(sd,size),"테스트",6); size += 6;
	wb(sd,size++) = (sd->obj.opt & trade_ok) ? 1 : 0 ;// trade
	wb(sd,size++) = sd->obj.opt & pkprotect_ok ? 1 : 0;  // pk
	wb(sd,size++) = 0;//레전드 갯수
	wb(sd,size++) = 0;
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
void user_showinfo(BSocket* sd,struct userobject* obj)
{
	int size = 6;
	int i;
	struct userimage* img;
	struct itemobject* equip;
	struct job* j = &jobs[obj->ud.job][obj->ud.joblv];
	sdsize(sd,400);
	wb(sd,0) = 170;
	wb(sd,3) = 52;
	wb(sd,4) = 0;
	wb(sd,5) = obj->ud.title_len;
	memmove(wp(sd,size),obj->ud.title,wb(sd,5));
	size += wb(sd,5);
	if(obj->clan != NULL){	
		wb(sd,size++) = obj->clan->name_len;
		memmove(wp(sd,size),obj->clan->name,obj->clan->name_len);
		size += obj->clan->name_len;
		wb(sd,size++) = obj->clan->title_len;
		memmove(wp(sd,size),obj->clan->title,obj->clan->title_len);
		size += obj->clan->title_len;
	} else {
		wb(sd,size++) = 0;
		wb(sd,size++) = 0;
	}
	wb(sd,size++) = j->name_len;
	memmove(wp(sd,size), j->name,wb(sd,size-1));
	size+=wb(sd,size-1);
	wb(sd,size++) = obj->ud.id_len;
	memmove(wp(sd,size),obj->ud.id,wb(sd,size-1));
	size+=wb(sd,size-1);
	wb(sd,size++) = obj->obj.state==4;
	wb(sd,size++) = obj->ud.gender;
	wb(sd,size++) = obj->obj.state;
	if(obj->ud.state == 4)
	{
		ws(sd,size) = SWAP16( 0); // ridign
		size+=2;
		wb(sd,size++) = 75; // walk speed
		ws(sd,size) = SWAP16(obj->ud.face);
		size += 2;
		wb(sd,size++) = (obj->obj.image);
		wb(sd,size++) = obj->obj.color;
	}
	else
	{
		if(obj->obj.image >= 65536){
			struct user_image* uimg = image_table[obj->obj.image-65536];
			ws(sd,size) = SWAP16(0); // ridign
			size+=2;
			wb(sd,size++) = 75; // walk speed
			ws(sd,size) = SWAP16(uimg->face);
			size+=2;
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
			ws(sd,size) = SWAP16(0); // ridign
			size+=2;
			wb(sd,size++) = 75; // walk speed
			ws(sd,size) = SWAP16(obj->ud.face);
			size += 2;
			wb(sd,size++) = (obj->ud.hair);
			wb(sd,size++) = obj->ud.hair_color; // haircolor
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
				wb(sd,size++) = obj->ud.gender; // 갑옷 sex?
				wb(sd,size++) = 0; // 갑옷 컬ㄹ
				wb(sd,size++) = -1;
				wb(sd,size++) = -1;
			}
			//무기 3바이트
			//방패 3바이트
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
			//헬멧 
			wb(sd,size++) = 0; // helmet visible
			if((equip = obj->equips[i_tara]) || (equip = obj->equips[i_helmet]))
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
				wb(sd,size++) = -1; // helmet color 1
				wb(sd,size++) = -1; // helmet color 2
			}
			//얼굴장식
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
			//망토
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
	
			//신발
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
			//목걸이 3바이트
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
	
		for(i=0;i<14;i++)
		{
			if((equip=obj->equips[i]) != NULL)
			{
				ws(sd,size) = SWAP16(equip->db->image);
				size+=2;
				wb(sd,size++) = equip->db->equip.color[0];
				wb(sd,size++) = equip->db->equip.color[1]; // armcolor 1
				wb(sd,size++) = equip->db->equip.color[2]; // armcolor 2
				wb(sd,size++) = equip->name_len;
				memmove(wp(sd,size),equip->name,wb(sd,size-1));
				size+= wb(sd,size-1);
				wb(sd,size++) = equip->db->name_len;
				memmove(wp(sd,size),equip->db->name,wb(sd,size-1));
				size+= wb(sd,size-1);
				wb(sd,size++) = 0;
				wl(sd,size) = SWAP32(0);
				size+=4;
				wb(sd,size++) = 0;
				wb(sd,size++) = 0;
			}
			else
			{
				ws(sd,size) = SWAP16(-1);
				size+=2;
				wb(sd,size++)=0;
				wb(sd,size++)=0xFf;
				wb(sd,size++)=0xff;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
				wb(sd,size++) = 0;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
				wb(sd,size++)=0;
			}
		}
		wb(sd,size++) = 0;
		wl(sd,size) = SWAP32(obj->obj.id);
		size+=4;
		wb(sd,size++) = (obj->opt & group_ok) ? 1 : 0;
		wb(sd,size++) = (obj->opt & trade_ok) ? 1 : 0;
		wb(sd,size++) = 2;
		ws(sd,size) = SWAP16(obj->ud.personality); // personality?
		size+=2;
		ws(sd,size) = SWAP16(obj->ud.vote); // personality
		size+=2;
		wb(sd,size++) = 0;
		wb(sd,size++) = 0;
	ws(sd,1) = SWAP16(size-3);
	cpt(sd);
}
void user_status(struct userobject* obj,int type)
{
	int sz=6,i;
	unsigned int mhp,mmp;
	struct userdata* ud = &obj->ud;
	BSocket* sd = obj2sd(obj);
	sdsize(sd,70);
	wb(sd, 0) = 170;
	wb(sd, 3) = 8;
	wb(sd, 4) = 1;
	wb(sd, 5) = type;
	if(type & 0x40)
	{
		wb(sd, sz++) = 0;
		wb(sd, sz++) = ud->nation;
		wb(sd, sz++) = ud->totem;// totem
		wb(sd, sz++) = ud->gender;
		wb(sd, sz++) = ud->level;
		wb(sd,sz++) = 0;
		wl(sd, sz) = SWAP32(ud->maxhp);sz+=4;
		wl(sd, sz) = SWAP32(ud->maxmp);sz+=4;
		wb(sd, sz++) = ud->might;
		wb(sd, sz++) = ud->grace;
		wb(sd, sz++) = 1;
		wb(sd,sz++) = 1;
		wb(sd, sz++) = ud->will;
		ws(sd, sz) = SWAP16(0x0000);sz+=2;
		wl(sd, sz) = SWAP32(0x00000000); sz+=4;
		ws(sd, sz) = SWAP16(ud->personality);sz+=2;
		ws(sd, sz) = SWAP16(ud->vote); sz+=2;
		wl(sd, sz) = SWAP32(0x00010000);sz+=4;
	}
	if(type & 0x20){
		wl(sd, sz) = SWAP32(ud->hp);sz+=4;
		wl(sd, sz) = SWAP32(ud->mp);sz+=4;
	}
	if(type & 0x10){
		wl(sd, sz) = SWAP32(ud->exp); sz+=4;
		wl(sd, sz) = SWAP32(ud->money); sz+=4;
		wb(sd, sz++) = (int)(ud->exp_percent*100)&0xFF; //경험치 퍼센트
	}
	if(type & 0x08){ // condition data
		wb(sd,sz++) = obj->opt & keyreverse; // 키보드반대
		wb(sd,sz++) = obj->opt & despair;//절망
		wb(sd,sz++) = obj->opt & hidechat;//채팅안보임
		wb(sd,sz++) = obj->opt & nochat;//채팅 금지
		wb(sd,sz++) = 0;//맵?
	}
	wb(sd,sz++) = 0xFF;
	wb(sd,sz++) = 1;// fastmove
	wb(sd,sz++) = 7; //생산종류갯수
	for(i=0;i<7;i++)
		wb(sd,sz++) = 1;
	wb(sd,sz++) = 1; // 현재칭호
	wb(sd,sz++) = 0;
	wb(sd,sz++) = 0;
	wb(sd,sz) = 1 | 16 ; // 1: 빠른이동 16: 효과음 32: 투구보이기
	wb(sd,sz++) |= (obj->opt & helmet_visible) ? 32 : 0;
	wb(sd,sz++) = 1 | 2 | 4 | 8| 16 | 32 | 64;
	ws(sd,1) = SWAP16(sz-3);
	cpt(sd);
}