
#include "enc.h"
extern char* cEnc(char *);
extern char* cDec(char *,int len);

char * encrypt(char * text){
    return cEnc(text);
}
char * decrypt(char * text,int len){
    return cDec(text,len);

}