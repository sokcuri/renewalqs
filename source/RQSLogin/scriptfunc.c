#include "script.h"
#include "socket.h"
#include "login.h"
#include "debug.h"
#include "scriptfunc.h"
int scriptfunc_sleep(lua_State* lua)
{
	if(arg_check(lua,1,1,"i","time")) return 0;
	PUSHi(lua,yield_sleep);
	return lua_yield(lua,2);
}
int scriptfunc_login_check(lua_State* lua)
{
	char* id;
	if(arg_check(lua,1,1,"is","sd","id")) return 0;
	PUSHi(lua,yield_chklogin);
	return lua_yield(lua,3);
}
BSocket* get_socket(lua_State* lua,int sd)
{
	BSocket* sock = sockets[sd];
	if(sock == NULL ) {
		luaL_error(lua,"소켓(%d)이 존재하지 않습니다\n",sd);
		return NULL;
	}
	return sock;
}
int scriptfunc_send_message(lua_State* lua)
{
	int id;
	int type;
	BSocket* sd;
	
	char* msg;
	if(arg_check(lua,1,3,"iis")) return 0;
	id = lua_tointeger(lua,1);
	sd = get_socket(lua,id);
	if(sd == NULL) return 0;
	type = lua_tointeger(lua,2);
	msg = (char*)lua_tostring(lua,3);
	login_send_message(sd,type,msg);
	return 0;
}

void game_login(int serial,char* id);
extern unsigned int game_ip;
extern unsigned short game_port;
int scriptfunc_game_login(lua_State* lua)
{
	int sdid;
	BSocket* sd;
	char* id;
	if(arg_check(lua,1,2,"is")) return 0;
	sdid = lua_tointeger(lua,1);
	sd = get_socket(lua,sdid);
	if(sd == NULL) return 0;
	id = (char*)lua_tostring(lua,2);
	login_send_message(sd,0,"");
	game_login(login_send_servermove(sd,game_ip,game_port),id);
	return 0;
}
extern unsigned int game_ip;
extern unsigned short game_port;
extern char game_pw[32];
static int global_i(lua_State* lua,char* name,int def){
	lua_getglobal(lua,name);
	if(lua_isnumber(lua,-1)){
		def = lua_tointeger(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
static int global_b(lua_State* lua,char* name,int def){
	lua_getglobal(lua,name);
	if(lua_isboolean(lua,-1)){
		def = lua_toboolean(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
static char* global_s(lua_State* lua,char* name,char* def){
	lua_getglobal(lua,name);
	if(lua_isstring(lua,-1)){
		def = (char*)lua_tostring(lua,-1);
	}
	lua_pop(lua,1);
	return def;
}
extern BServer lserver;
void conf_init(lua_State* lua){
	char* ip = "127.0.0.1";
	char* pw;
	char* name;
	ip = global_s(lua,"_GAME_SERVER_IP_",ip);
	game_ip = inet_addr(ip);
	game_port = global_i(lua,"_GAME_SERVER_PORT_",3356);
	pw = global_s(lua,"_LOGIN_SERVER_PW_","897mkd");
	strncpy(game_pw,pw,32);
	name = global_s(lua,"_SERVER_NAME_","쿠우");
	strncpy(lserver.name,name,256);
	lserver.port = global_i(lua,"_LOGIN_SERVER_PORT_",8998);
}
int scriptfunc_multiserveradd(lua_State* lua)
{
	char* name;
	char* ip;
	unsigned short port;
	if(arg_check(lua,1,3,"ssi","name","ip","port")) return 0;
	name = (char*)lua_tostring(lua,1);
	ip = (char*)lua_tostring(lua,2);
	port = lua_tointeger(lua,3);
	multiserver_add(name,ip,port);
	return 0;
}
void lua_init(lua_State* lua)
{
	luaL_openlibs(lua);
	script_init(lua);
	lua_register(lua,"sleep",scriptfunc_sleep);
	lua_register(lua,"send_message",scriptfunc_send_message);
	lua_register(lua,"login",scriptfunc_game_login);
	lua_register(lua,"chklogin",scriptfunc_login_check);
	lua_register(lua,"multiserver_add",scriptfunc_multiserveradd);
	if(luaL_dofile(lua,"login.lua") != 0)
	{
		errf("[script error] %s\n",lua_tostring(lua,-1));
		return;
	}
	conf_init(lua);
}