#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"common/flConstants.h"
#include"common/flError.h"
#include"common/container/flArray.h"
#include"common/entity/flEntity.h"

#define flmemMalloc(nbytes) malloc(nbytes)
#define flmemFree(mptr) free(mptr)
#define flmemRealloc(mptr, nbytes) realloc(mptr, nbytes)

#define flMillis() 0

#define fltmeMillis() flMillis()

#endif//FLHEADERH_INCLUDED
