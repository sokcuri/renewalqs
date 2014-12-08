#ifndef _DEBUG_HEADER
#define _DEBUG_HEADER
void SetColor(char bc,char fc);
void warnf(char* format,...);
void deff(char* format,...);
void errf(char* format,...);
void debugf(char* format,...);
void panicf(char* format,...);
#define PANIC(COND,MSG) if(!(COND)) {	errf("CORE PANIC %s (%d) : %s\n",__FILE__,__LINE__,MSG); system("PAUSE"); exit(-1); }

#endif