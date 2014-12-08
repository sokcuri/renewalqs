#include "sql.h"
#include<string.h>
#include "debug.h"
#include <stdarg.h>
#include <conio.h>
#define QUERY_MAX 0x2000
MYSQL* sqldb[SQL_MAX]= {NULL};
int queryf(int db,char* format,...)
{
	char string[QUERY_MAX];
	va_list vlist;
	if(db >= SQL_MAX || sqldb[db] == NULL) return -2;
	va_start(vlist,format);
	vsnprintf(string,QUERY_MAX,format,vlist);
	va_end(vlist);
	if(mysql_query(sqldb[db],string))
	{
		errf("[ Query Error ] %s\n",mysql_error(sqldb[db]));
		return -1;
	}
	return 0;
}
int sql_connect(int code,char* ip,char* id,char* pw,char* name,int port)
{
	MYSQL* db;
	if(code >= SQL_MAX) return -2;
	else if(sqldb[code] != NULL) {
		mysql_close(sqldb[code]);
	}
	sqldb[code] = db = mysql_init(NULL);
	if(db == NULL) return -1;
	mysql_options(db,MYSQL_SET_CHARSET_NAME,"euckr");
	db = mysql_real_connect(db,ip,id,pw,name,port,NULL,0);
	if(db == NULL) return -1;
	return 0;
}