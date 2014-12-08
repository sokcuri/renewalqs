#include "debug.h"
#include <stdio.h>
#include <Windows.h>
#include <stdarg.h>
void SetColor(char bc,char fc)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(bc<<8)|fc);
}
void panicf(char* format,...)
{
	va_list vlist;
	va_start(vlist,format);
	SetColor(0,4);
	vfprintf(stderr,format,vlist);
	va_end(vlist);
	system("PAUSE");
	exit(-1);
}
void warnf(char* format,...)
{
	va_list vlist;
	va_start(vlist,format);
	SetColor(0,14);
	vfprintf(stderr,format,vlist);
	va_end(vlist);
}
void deff(char* format,...)
{
	va_list vlist;
	va_start(vlist,format);
	SetColor(0,15);
	vprintf(format,vlist);
	va_end(vlist);
}

void debugf(char* format,...)
{
	va_list vlist;
	va_start(vlist,format);
	SetColor(0,10);
	vprintf(format,vlist);
	va_end(vlist);
}
void errf(char* format,...)
{
	va_list vlist;
	va_start(vlist,format);
	SetColor(0,12);
	vfprintf(stderr,format,vlist);
	va_end(vlist);
}