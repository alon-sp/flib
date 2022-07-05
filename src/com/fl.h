#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flTypes.h"
#include"flArray.h"
#include"fllog.h"

#define flmmMalloc(nbytes) flmemMalloc(nbytes)
#define flmmFree(ptr) flmemFree(ptr)
#define flmmRealloc(mptr, nbytes) flmemRealloc(mptr, nbytes)

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

#define flutImplNew(funcName, objType, objType_)\
objType* funcName(){\
    objType* obj = flmmMalloc(sizeof(objType));\
    if(!obj) return NULL;\
    *obj = (objType){objType_};\
    return obj;\
}

#endif//FLHEADERH_INCLUDED
