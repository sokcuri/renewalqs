#ifndef _H_CRYPTION
#define _H_CRYPTION


int encrypt2(int type, unsigned char *packet,unsigned char* out,char* cryptkey);
int encrypt(int type, unsigned char* packet,char* cryptkey);
void decrypt(int type,unsigned char *packet,char* cryptkey);
extern char defaultCryptKey[10];

#define cpt(ch) (encrypt(0,(unsigned char*)ch,defaultCryptKey))
#endif