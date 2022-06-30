#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flTypes.h"
#include"flArray.h"
#include"fllog.h"

void* flmemMalloc(flint_t nbytes);

void flmemFree(void* mptr);

#ifdef _FLIB_DEBUG_
    uint32_t flmemGetTotalFlmemFreeCalls();
    uint32_t flmemGetTotalFlmemMallocCalls();
#endif

void* flmemRealloc(void* mptr, flint_t nbytes);

/*----------Error handling functions----------*/
void flerrSetCallback(void (*errorCallback)(const char*));

void flerrHandle(const char* errstr);

/*----------utilities----------*/
#define flcopSet(cop, copValue, copType) ( *(copType*)&(cop) = copValue )

#endif//FLHEADERH_INCLUDED
