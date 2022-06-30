#include"fl.h"

#ifdef _FLIB_DEBUG_
    static uint32_t totalFlmemMallocCalls = 0;
    static uint32_t totalFlmemFreeCalls = 0;
#endif

void* flmemMalloc(flint_t nbytes){
    void* buf = malloc(nbytes);
    if(!buf){
        flerrHandle("\nMEMf flmemMalloc");
    }

#ifdef _FLIB_DEBUG_
    totalFlmemMallocCalls++;
#endif

    return buf;
}

void flmemFree(void* mptr){
    if(!mptr) return;

    free(mptr);

#ifdef _FLIB_DEBUG_
    totalFlmemFreeCalls++;
#endif
}

#ifdef _FLIB_DEBUG_
uint32_t flmemGetTotalFlmemMallocCalls(){
    return totalFlmemMallocCalls;
}
#endif

#ifdef _FLIB_DEBUG_
uint32_t flmemGetTotalFlmemFreeCalls(){
    return totalFlmemFreeCalls;
}
#endif

void* flmemRealloc(void* mptr, flint_t nbytes){
    void* buf = realloc(mptr, nbytes);
    if(!buf){
        flmemFree(mptr);
        flerrHandle("\nMEMf flmemRealloc");
    }

#ifdef _FLIB_DEBUG_
    if(!mptr) totalFlmemMallocCalls++;
#endif

    return buf;
}

//----------Error handling functions---------*/

/**
 * @brief This callback function is called whenever a critical error occur in any function or module
 * 
 */
static void (*flerrOnError)(const char* errstr) = NULL;

void flerrSetCallback(void (*errorCallback)(const char*)){
    flerrOnError = errorCallback;
}

void flerrHandle(const char* errstr){
    if(flerrOnError) flerrOnError(errstr);
}