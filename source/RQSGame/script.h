#pragma once


#include <lua.hpp>
#include "list.h"
#include "timer.h"
enum yield_message
{
	yield_sleep = 3,
	yield_dialog = 4,
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
struct luatimer
{
	unsigned int id;
	struct timer timer;
	unsigned int func_ref;
	unsigned int data_ref;
	struct _list_elem hash_elem;
	struct lua_thread* running;
};
extern int block_lock;
int arg_check(lua_State* lua,int start,int argn,char* args,...);

int type_check(lua_State* lua,char type,int num);
int thread_resume(struct lua_thread* t,int args);
void script_init(lua_State* main_lua);

void thread_delete(struct lua_thread* t);
int luatimer_add(unsigned int time,unsigned int fref,unsigned int dref);

void unregisterfunc(int fc);
int thread_resume(struct lua_thread* t,int args);
void thread_back(struct lua_thread* t);
struct lua_thread* thread_get();
void luatimer_del(int id);
unsigned int registerfunc(char* func);
int shell_script(char* func,char* arg,...);

int shell_script2(unsigned int ref,char* arg,...);



int shell_event(int ev,char* arg,...);

#define PUSHi(l,i) (lua_pushinteger(l,i));