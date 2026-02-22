#ifndef __EPRINTF
#define __EPRINTF

#include <stdint.h>

//This list may be supplemented with new formats if needed
#define EPRINTF_FORMAT_LIST\
 X(s)\
 X(c)\
 X(i)\
 X(x)\
 X(b)

#ifdef EPRINTF_IMPLEMENTATION

#define X(a) a
static uint8_t a##_format_handler(char* s);
#undef X

static inline uint8_t s_format_handler(char* s){return 0;}
static inline uint8_t c_format_handler(char* s){return 0;}
static inline uint8_t i_format_handler(char* s){return 0;}
static inline uint8_t x_format_handler(char* s){return 0;}
static inline uint8_t b_format_handler(char* s){return 0;}



#endif
#endif