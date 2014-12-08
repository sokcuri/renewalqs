
#pragma once
#include "socket.h"

int game_receive(BSocket*);
int game_close(BSocket*);
int game_accept(SOCKET,char*);
void game_login(BSocket*);
void game_init();
void game_send_message(BSocket* sd,char type,char* message);
void game_notice(BSocket* sd,char* message);
void game_user_list(BSocket* sd);

int game_send_servermove(BSocket* sock,unsigned int ip,unsigned short port);
void game_send_message2(BSocket* sd,int fc,int bc,char* message,int timeout);
enum GAME_OPTION {
	use_autoeffect = 1,
	debugging = 2,
};