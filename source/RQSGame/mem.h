#pragma once
void _free(void*);
void* _maloc(unsigned int size);
void* _caloc(unsigned int count,unsigned int size);
void* _realoc(void*,unsigned int size);

#define MALOC(tp,cnt) ((tp*)_maloc(sizeof(tp)*(cnt)))
#define CALOC(tp,cnt) ((tp*)_caloc(cnt,sizeof(tp)))
#define REALOC(tp,d,cnt) ((tp*)_realoc(d,sizeof(tp)*(cnt)))
#define SWAP(a,b,T) {T __TMP = a; a = b ; b = __TMP;}