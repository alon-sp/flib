#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

#include<stdlib.h>

#include"flConstants.h"
#include"flError.h"
#include"./container/flArray.h"

#define flmemMalloc(nbytes) malloc(nbytes)
#define flmemFree(mptr) free(mptr)
#define flmemRealloc(mptr, nbytes) realloc(mptr, nbytes)

bool flInit();
void flCleanup();

#endif//FLHEADERH_INCLUDED