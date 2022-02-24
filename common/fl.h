#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flError.h"
#include"./container/flArray.h"
#include"./entity/flEntity.h"

#define flmemMalloc(nbytes) malloc(nbytes)
#define flmemFree(mptr) free(mptr)
#define flmemRealloc(mptr, nbytes) realloc(mptr, nbytes)

#endif//FLHEADERH_INCLUDED