#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"common/flConstants.h"
#include"common/util/flUtil.h"
#include"common/flError.h"
#include"common/container/flArray.h"
#include"common/entity/flEntity.h"

void* flmemMalloc(flint_t nbytes);

#define flmemFree(mptr) free(mptr)

void* flmemRealloc(void* mptr, flint_t nbytes);

#define flMillis() 0

#define fltmeMillis() flMillis()

#endif//FLHEADERH_INCLUDED
