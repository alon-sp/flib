#include<stdio.h>

#include"../fl.h"
#include"_flArray.h"

void onError(const char* errLog){
    printf("%s", errLog);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    flerrSetCallback(onError);

    if(_flarrRunTests()){
        printf("\n$_flarrRunTest: TEST OK\n");
    }

    return 0;    
}