#include<stdio.h>

#include"../fl.h"
#include"_flArray.h"

void onError(const char* errLog){
    printf("%s", errLog);
    flCleanup();
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    flerrSetCallback(onError);
    flerrSetLogBufferSize(1024);
    flInit();

    if(_flarrRunTests()){
        printf("\n$_flarrRunTest: TEST OK\n");
    }

    flCleanup();
    return 0;    
}