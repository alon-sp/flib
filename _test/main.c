#include<stdio.h>

#define _FLIB_DEBUG_

#include"fl.h"

#include"_flArray.h"
#include"./entity/_flEntity.h"

void onError(const char* errLog){
    printf("%s", errLog);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    flerrSetCallback(onError);

    printf("\nRunning flArray tests\n----------");
    if(_flarrRunTests()){
        printf("\n$_flarrRunTest: TEST OK\n");
    }

    printf("\nRunning flEntity tests\n----------");
    if(_flentRunTests()){
        printf("\n$_flentRunTests: TEST OK\n");
    }

    printf("\n\n-----TotalFlmemMallocCalls: %u", flmemGetTotalFlmemMallocCalls());
    printf("\n-----TotalFlmemFreeCalls: %u\n", flmemGetTotalFlmemFreeCalls());
    if( flmemGetTotalFlmemMallocCalls() != flmemGetTotalFlmemFreeCalls()){
        onError("!!!!!Possible memory leaks!!!!!\n");
    }

    return 0;    
}