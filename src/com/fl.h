#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flUtil.h"
#include"flError.h"
#include"flArray.h"
#include"flEntity.h"
#include"flentstd.h"

void* flmemMalloc(flint_t nbytes);

void flmemFree(void* mptr);

#ifdef _FLIB_DEBUG_
    uint32_t flmemGetTotalFlmemFreeCalls();
    uint32_t flmemGetTotalFlmemMallocCalls();
#endif

void* flmemRealloc(void* mptr, flint_t nbytes);

#define flMillis() 0

#define fltmeMillis() flMillis()

#endif//FLHEADERH_INCLUDED
