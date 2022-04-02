#include"fl.h"

void* flmemMalloc(flint_t nbytes){
    void* buf = malloc(nbytes);
    if(!buf){
        flerrHandle("\nMEMf flmemMalloc");
    }

    return buf;
}

void* flmemRealloc(void* mptr, flint_t nbytes){
    void* buf = realloc(mptr, nbytes);
    if(!buf){
        flmemFree(mptr);
        flerrHandle("\nMEMf flmemRealloc");
    }

    return buf;
}