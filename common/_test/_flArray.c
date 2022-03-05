
#include<stdio.h>
#include"../container/flArray.h"
#include"../flError.h"
#include"_flArray.h"

typedef struct{
    int inum;
    double dbl;
}_foo;

static bool _flarrTestPush(){
    flArray* foos = flarrNew(2, sizeof(_foo));
    int foosMaxLength = 10;

    for(int i = 0; i<foosMaxLength; i++){
        _foo f = {.inum = i};
        _foo* fptr = (_foo*)flarrPush(foos, &f);
    }

    for(int i = 0; i<foosMaxLength; i++){
        _foo* fptr = (_foo*)flarrGet(foos, i);
        if( !(fptr->inum == i) ){
            flerrHandle("\nTESf _flarrRunTest:flarrGet Test Failed !1");
            return false;
        }
    }
    flarrFree(&foos);

    printf("\n_flarrTestPush: TEST OK");

    return true;
}

static bool _flarrTestSetLength(){
    flArray* nums = flarrNew(10, sizeof(flInt_t));
    flarrSetLength(nums, 0);
    flarrSetLength(nums, 5);
    if(nums->length != 5){
        flerrHandle("\nTESf _flarrTestSetLength: Test Failed !1");
    }

    flarrFree(&nums);

    printf("\n_flarrTestSetLength: TEST OK");

    return true;
}

static bool _flarrTestPut(){
    flArray* str = flarrNew(10, sizeof(char));
    flarrSetLength(str, 10);

    flarrPut(str, 0, "H");
    flarrPut(str, 1, "e");
    flarrPut(str, 2, "l");
    flarrPut(str, 3, "l");
    flarrPut(str, 4, "o");
    flarrPut(str, 5, "");

    if(strcmp("Hello", (char*)flarrGet(str, 0)) != 0){
        flerrHandle("\nTESf _flarrTestPut: Test Failed !1");
    }

    flarrFree(&str);

    printf("\n_flarrTestPut: TEST OK");

    return true;
}

static bool _flarrstrTestPush(){
    flArray* str = flarrNew(5, sizeof(char));
    flarrstrPush(str, "Hello");
    flarrstrPush(str, " world");
    flarrstrPush(str, " from");
    flarrstrPush(str, " flarrstr");
    if(strcmp(flarrstrCstr(str), "Hello world from flarrstr") != 0){
        flerrHandle("\nTESf _flarrstrTestPush: Test Failed !1");
    }

    flarrFree(&str);

    printf("\n_flarrstrTestPush: TEST OK");

    return true;
}

static bool _flarrstrTestPop(){
    flArray* str = flarrNew(5, sizeof(char));
    flarrstrPush(str, "olleH");
    char helloStr[5+1];
    
    int i = 0;
    while( str->length ) helloStr[i++] = flarrstrPop(str);

    helloStr[strlen("Hello")] = '\0';

    if(strcmp(helloStr, "Hello") != 0){
        flerrHandle("\nTESf _flarrstrTestPop: Test Failed !1");
    }

    flarrFree(&str);

    printf("\n_flarrstrTestPop: TEST OK");

    return true;

}

static bool _flarrTestPushs(){
    flArray* intArr = flarrNew(0, sizeof(int));
    int nums[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    flarrPushs(intArr, nums, sizeof(nums) / sizeof(*nums));
    if(intArr->length != sizeof(nums) / sizeof(*nums)){
        flerrHandle("\nTESf _flarrTestPushs: Test Failed !1");
    }

    for(int i = 0; i<intArr->length; i++){
        if( *( (int*)flarrGet(intArr, i) ) != nums[i]){
            flerrHandle("\nTESf _flarrTestPushs: Test Failed !2");
        }
    }

    flarrFree(&intArr);

    printf("\n_flarrTestPushs: TEST OK");

    return true;

}

bool _flarrRunTests(){
    _flarrTestPush();
    _flarrTestSetLength();
    _flarrTestPut();
    _flarrstrTestPush();
    _flarrstrTestPop();
    _flarrTestPushs();
}