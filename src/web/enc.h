#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
char * encrypt(char * text);
char * decrypt(char * text,int len);
#ifdef __cplusplus
}
#endif