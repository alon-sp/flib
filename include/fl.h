#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flUtil.h"
#include"flError.h"
#include"flArray.h"
#include"flEntity.h"

void* flmemMalloc(flint_t nbytes);

#define flmemFree(mptr) free(mptr)

void* flmemRealloc(void* mptr, flint_t nbytes);

#define flMillis() 0

#define fltmeMillis() flMillis()

#endif//FLHEADERH_INCLUDED
