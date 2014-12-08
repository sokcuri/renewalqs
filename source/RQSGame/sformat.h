#pragma once

#define SFORMAT_SIZE_MAX 128
struct sformat
{
	int code;
	char format[SFORMAT_SIZE_MAX+1];
	unsigned int size;
};
#define SFORMAT_DB_MAX 300
extern struct sformat* formats[SFORMAT_DB_MAX];

void register_format(int code,char* format);

char* getformat(int code);