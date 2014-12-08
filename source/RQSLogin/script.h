#pragma once
#include "timer.h"


#include <lua.hpp>
#include "list.h"
enum yield_message
{
	yield_sleep = 3,
	yield_chklogin = 4,
};

enum thread_state
{
	wait = 0,
	busy = 1,
};
struct lua_thread
{
	lua_State* lua;
	struct _list_elem elem;
	unsigned int ref;
	unsigned char state;
	struct timer timer;
};

int arg_check(lua_State* lua,int start,int argn,char* args,...);

int type_check(lua_State* lua,char type,int num);

void script_init(lua_State* main_lua);

int shell_script(char* func,char* arg,...);
int thread_resume(struct lua_thread* t,int args);
#define PUSHi(l,i) (lua_pushinteger(l,i));