#pragma once
#include "object.h"

struct petdb
{
	unsigned int image;
	unsigned char color;
};
struct petobject
{
	struct petdb* db;
	unsigned int hp,mp,mhp,mmp;
};