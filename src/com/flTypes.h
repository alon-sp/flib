#ifndef FLTYPESH_INCLUDED
#define FLTYPESH_INCLUDED

#include<stdio.h>
#include<inttypes.h>
#include<stdbool.h>

//fl number types
typedef uint8_t flbyt_t;

typedef uint32_t flstt_t;
#define _flsttSetType(flstruct, type) ( *(flstt_t*)&((flstruct)->_type) =  type )

typedef int64_t flint_t;
#define flintToStr(destBuf, _flint) sprintf(destBuf, "%" PRId64, _flint)

typedef float flflt_t;

typedef float flnum_t;
#define flnumToStr(destBuf, _flnum) sprintf(destBuf, "%f", _flnum)

//fl function pointers types
typedef void (*flvod_tf)(void);

#endif