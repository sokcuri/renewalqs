#include "socket.h"
#include<time.h>
#include "script.h"

#include "debug.h"
#include "mem.h"
#include "list.h"
#include <string.h>

int block_lock;
#define LUA_TIMER_MAX 256
unsigned int threads_count = 0;

void game_chk_login(BSocket* sd,char* id);
int type_check(lua_State* lua,char type,int num)
{
	switch(type)
	{
	case 'i':
		if(!lua_isnumber(lua,num))
		{
			luaL_typerror(lua,num,"number");
			return 1;
		}
		break;
	case 's':
		if(!lua_isstring(lua,num))
		{
			luaL_typerror(lua,num,"string");
			return 1;
		}
		break;
	case 'b':
		if(!lua_isboolean(lua,num))
		{
			luaL_typerror(lua,num,"boolean");
			return 1;
		}
		break;
	case 't':
		if(!lua_istable(lua,num))
		{
			luaL_typerror(lua,num,"table");
			return 1;
		}
		break;
	case '-':
		break;
	}
	return 0;
}
int arg_check(lua_State* lua,int start,int argn,char* args,...)
{
	char mes[256],*a;
	int i=0,k=lua_gettop(lua);
	va_list list;
	va_start(list,args);
	for(i=0;i<argn;i++)
	{
		a = va_arg(list,char*);
		if(k<start+i)
		{
			sprintf(mes,"%s excepted.",a);
			luaL_argerror(lua,i+start,mes);
			va_end(list);
			return 1;
		}
		if(type_check(lua,args[i],i+start))
		{
			va_end(list);
			return 1;
		}
	}
	va_end(list);
	return 0;
}
static struct _list busy_threads;
static struct _list wait_threads;
unsigned int wait_count;
struct lua_thread* waiting_thread = NULL;
static struct lua_thread main_thread;
void thread_delete(struct lua_thread* t)
{
	if(t->lua == main_thread.lua) return;
	if(t == waiting_thread){
		waiting_thread = NULL;
	}
	luaL_unref(t->lua,LUA_REGISTRYINDEX,t->ref);
	_list_remove(&t->elem);
	threads_count--;
	debugf("del thread %x\n",t);
	_free(t);
	
}
void thread_back(struct lua_thread* t)
{
	if(t->state != busy) return;
	_list_remove(&t->elem);
	t->state = wait;
	if(waiting_thread == NULL){
		waiting_thread = t;
		return ;
	}
	wait_count++;
	_list_add(&wait_threads,&t->elem);
}
long thread_wakeup(struct timer* tm)
{
	struct lua_thread* t = (struct lua_thread*) tm->data;
	int result;
	clock_t _t = clock();
	result = lua_resume(t->lua,0);
	debugf("[time debug] %d miliseconds\n",clock() - _t);
	if(result == 2)
	{
		// Error
		warnf("[script error] %s\n",lua_tostring(t->lua,-1));
		thread_delete(t);
		return -1;
	}
	else if(result == 1)
	{
		int msg = lua_tointeger(t->lua,-1);
		if(msg == yield_chklogin){
			int sdid = lua_tointeger(t->lua,-3);
			BSocket* sd;
			sd = sockets[sdid];
			if(sd != NULL) {
				sd->thread = t;
				game_chk_login(sd,lua_tostring(t->lua,-2));
			}
		}
		else if(msg == yield_sleep)
		{
			int time = lua_tointeger(t->lua,-2);
			return time;
		}
		return -1;
	}
	else if(result == 0)
	{
		thread_back(t);
		return -1;
	}
	return -1;
}
struct lua_thread* thread_get()
{
	struct lua_thread* t;
	if(waiting_thread){
		t = waiting_thread;
		
		waiting_thread = NULL;
	}
	else if(wait_count)
	{
		struct _list_elem* elem;
		t = list_entry(elem=list_begin(&wait_threads),struct lua_thread,elem);
		_list_remove(elem);
		wait_count--;
	}
	else {
		t = MALOC(struct lua_thread,1);
		debugf("new thread %d:%x\n",threads_count,t);
		t->lua = lua_newthread(main_thread.lua);
		t->timer.data = t;
		t->timer.wakeup = thread_wakeup;
		t->ref = luaL_ref(main_thread.lua,LUA_REGISTRYINDEX);
		threads_count++;
	}
	t->state = busy;
	_list_add(&busy_threads,&t->elem);
	return t;
}
void script_init(lua_State* main_lua)
{
	main_thread.lua = main_lua;
	main_thread.state = wait;
	list_init(&wait_threads);
	list_init(&busy_threads);
	block_lock = 0;
}
int thread_resume(struct lua_thread* t,int args)
{
	int result;
	clock_t _t = clock();
	result = lua_resume(t->lua,args);
	debugf("[time debug] %d miliseconds\n",clock() - _t);

	if(result == 2)
	{
		// Error
		warnf("[script error] %s\n",lua_tostring(t->lua,-1));
		thread_delete(t);
		return -1;
	}
	else if(result == 1)
	{
		int msg = lua_tointeger(t->lua,-1);
		if(msg == yield_chklogin){
			int sdid = lua_tointeger(t->lua,-3);
			BSocket* sd;
			sd = sockets[sdid];
			if(sd != NULL) {
				sd->thread = t;
				game_chk_login(sd,lua_tostring(t->lua,-2));
			}
		}
		else if(msg == yield_sleep)
		{
			int time = lua_tointeger(t->lua,-2);
			t->timer.time = clock() + time;
			timer_add(&t->timer);
			return 0;
		}
		return 1;
	}
	else if(result == 0)
	{
		thread_back(t);
		return 0;
	}
	return 0;
}
unsigned int registerfunc(char* func)
{
	unsigned int ref;
	lua_getglobal(main_thread.lua,func);
	if(lua_isnil(main_thread.lua,-1))
	{
		warnf("[script warning] %s 함수를 찾을 수 없습니다\n",func);
		return 0;
	}
	ref = luaL_ref(main_thread.lua,LUA_REGISTRYINDEX);
	return ref;
}
int shell_script2(unsigned int ref,char* arg,...)
{
	int i,result;
	va_list arglist;
	int len = strlen(arg);
	int args = len;
	struct lua_thread* t = thread_get();
	if(t == NULL) return -1;
	va_start(arglist,arg);
	lua_rawgeti(t->lua,LUA_REGISTRYINDEX,ref);
	
	for(i=0;i<len;i++)
	{
		switch(arg[i]){
		case 'i':
			lua_pushinteger(t->lua,va_arg(arglist,int));
			break;
		case 'd':
			lua_pushnumber(t->lua,va_arg(arglist,double));
			break;
		case 's':
			lua_pushstring(t->lua,va_arg(arglist,char*));
			break;
		case 'b':
			lua_pushboolean(t->lua,va_arg(arglist,int));
			break;
		case 'n':
			arg--;
			lua_pushnil(t->lua);
			break;
		}
	}
	va_end(arglist,arg);
	return thread_resume(t,args);
}
int shell_script(char* func,char* arg,...)
{
	int i,result;
	va_list arglist;
	int len = strlen(arg);
	int args = len;
	struct lua_thread* t = thread_get();
	if(t == NULL) return -1;
	va_start(arglist,arg);
	lua_getglobal(t->lua,func);
	if(lua_isnil(t->lua,-1))
	{
		errf("[script error]%s 함수를 찾을 수 없습니다",func);
		lua_pop(t->lua,1);
		return -1;
	}
	for(i=0;i<len;i++)
	{
		switch(arg[i]){
		case 'i':
			lua_pushinteger(t->lua,va_arg(arglist,int));
			break;
		case 'd':
			lua_pushnumber(t->lua,va_arg(arglist,double));
			break;
		case 's':
			lua_pushstring(t->lua,va_arg(arglist,char*));
			break;
		case 'b':
			lua_pushboolean(t->lua,va_arg(arglist,int));
			break;
		case 'n':
			arg--;
			lua_pushnil(t->lua);
			break;
		}
	}
	va_end(arglist,arg);
	return thread_resume(t,args);
}