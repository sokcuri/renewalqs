#pragma once
#include "socket.h"

void world_time_single(BSocket*);
void world_time();
void world_msg(int type,char* msg);
void world_msg2(int fc,int bc,char* text,int timeout);