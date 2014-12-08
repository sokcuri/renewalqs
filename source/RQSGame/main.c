#include "socket.h"
#include "map.h"
#include "game.h"
#include "login.h"
#include "pc.h"
#include "item.h"
#include "magic.h"
#include "npc.h"

#include "scriptfunc.h"
#include "timer.h"
#include <time.h>
//#include "scriptfunc.h"
BServer gserver;
SOCKET hLoginSocket;
unsigned int game_option;
int main()
{
	int i,ref;
	clock_t t;
	lua_State* lua = lua_open();
	socket_init();
	game_init();
	warp_init();
	timer_init();
	npc_init();
	magic_init();
	item_init();
	lua_init(lua);
	pc_init();
	login_init();
	BaramOpen(&gserver,game_receive,game_close,game_accept);

}