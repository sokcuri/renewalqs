#include "sformat.h"
#include "debug.h"
#include<stdio.h>
#include "mem.h"
#include <string.h>
struct sformat* formats[SFORMAT_DB_MAX]={NULL};

void register_format(int code,char* format)
{
	struct sformat* sfm;
	if(code >= SFORMAT_DB_MAX || code < 0) {
		errf("문자열 포맷 db 코드가 최대값을 초과했거나 알맞지 않습니다.(%d)\n",code);
		return;
	}
	if(formats[code] != NULL)
		sfm = formats[code];
	else
		formats[code] = sfm = MALOC(struct sformat,1);
	sfm->code = code;
	strncpy(sfm->format,format,SFORMAT_SIZE_MAX);
	sfm->size = strlen(sfm->format);
}
char* getformat(int code)
{
	struct sformat* sfm;
	if(code >= SFORMAT_DB_MAX || code < 0 || (sfm = formats[code]) == NULL) {
		errf("문자열 포맷 db 코드가 최대값을 초과했거나 알맞지 않습니다.(%d)\n",code);
		return;
	}
	return sfm->format;
}