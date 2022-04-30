#ifndef FLTYPESH_INCLUDED
#define FLTYPESH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

//fl number types
typedef uint8_t flbyt_t;
typedef int64_t flint_t;
typedef float flflt_t;
typedef float flnum_t;

typedef uint32_t flentcco_t;

//fl function pointers types
typedef void (*flvod_tf)(void* args, flbyt_t* rvalDest);


typedef struct flEntity flEntity;
typedef struct flentIOport flentIOport;
typedef struct flentXenv flentXenv;


typedef uint16_t flentiopID_t;

#endif