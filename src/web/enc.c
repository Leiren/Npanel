
#include "enc.h"
extern char* cEnc(char *);
extern char* cDec(char *);

char * encrypt(char * text){
    return cEnc(text);
}
char * decrypt(char * text){
    return cDec(text);

}