#pragma once
#include "socket.h"
struct user_image
{
	int id; 
	unsigned short img_weapon;
	unsigned char col_weapon;
	unsigned short img_armor;
	unsigned char col_armor[3];
	unsigned short img_shield;
	unsigned char col_shield;
	unsigned char helmet_vis;
	unsigned short img_helmet;
	unsigned char col_helmet[3];
	unsigned short img_faceacc;
	unsigned char col_faceacc[3];
	unsigned short img_cape;
	unsigned char col_cape[3];
	unsigned short img_shoes;
	unsigned char col_shoes[3];
	unsigned short img_necklace;
	unsigned char col_neck;
	unsigned char hair;
	unsigned char haircol;
	unsigned char gender;
	unsigned short face;
};
void user_image_del(int id);
#define UIMG_MAX 4000
extern struct user_image* image_table[UIMG_MAX];
void uimg_show_single(int id,struct mapobject* obj,BSocket* sd);

void uimg_show_sight(int id,struct mapobject* obj);

void uimg_init();
#define user_img_del(id) (_free(image_table[id]),image_table[id] = NULL)