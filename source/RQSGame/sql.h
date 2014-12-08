#pragma once
#include <mysql.h>

int queryf(MYSQL* db,char* format,...);
int sql_connect(int code,char* ip,char* id,char* pw,char* name,int port);

#define SQL_MAX 256
extern MYSQL* sqldb[SQL_MAX];