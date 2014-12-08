#pragma once
#include "hashtable.h"
#include "list.h"
#define ID_MAX 30
typedef struct LOGIN_USER_DATA
{
	int serial;
	char id[ID_MAX+1];
	struct _list_elem hash_elem;
	void* data;
}login_ud;
void login_receive();
void login(login_ud*);
void login_init();
int serial_on(int serial,void* data);