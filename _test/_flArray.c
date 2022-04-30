
#include<stdio.h>

#include"fl.h"
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
    flarrFree(foos);

    printf("\n_flarrTestPush: TEST OK");

    return true;
}

static bool _flarrTestSetLength(){
    flArray* nums = flarrNew(10, sizeof(flint_t));
    flarrSetLength(nums, 0);
    flarrSetLength(nums, 5);
    if(nums->length != 5){
        flerrHandle("\nTESf _flarrTestSetLength: Test Failed !1");
    }

    flarrFree(nums);

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

    flarrFree(str);

    printf("\n_flarrTestPut: TEST OK");

    return true;
}

static bool _flarrTestPuts(){
    flArray* str = flarrNew(10, sizeof(char));
    flarrSetLength(str, 4);

    flarrPuts(str, 0, "Hello", strlen("Hello")+1 );

    if(strcmp("Hello", (char*)flarrGet(str, 0)) != 0){
        flerrHandle("\nTESf _flarrTestPuts: Test Failed !1");
    }

    flarrPuts(str, 1, "EL", strlen("EL") );
    if(strcmp("HELlo", (char*)flarrGet(str, 0)) != 0){
        flerrHandle("\nTESf _flarrTestPuts: Test Failed !2");
    }

    flarrFree(str);

    printf("\n_flarrTestPuts: TEST OK");

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

    flarrFree(str);

    printf("\n_flarrstrTestPush: TEST OK");

    return true;
}

static bool _flarrstrTestPushs(){
    flArray* str = flarrNew(3, sizeof(char));

    if(strcmp(
        flarrstrPushs(str, 4,  "Hello", " world", " from", " flarrstr"), 
                               "Hello world from flarrstr") != 0 ){
        flerrHandle("\nTESf _flarrstrTestPush: Test Failed !1");
    }

    flarrFree(str);

    printf("\n_flarrstrTestPushs: TEST OK");

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

    flarrFree(str);

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

    flarrFree(intArr);

    printf("\n_flarrTestPushs: TEST OK");

    return true;

}

static bool _flarrTestGets(){
    flArray* intArr = flarrNew(0, sizeof(int));
    int nums[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    flarrPushs(intArr, nums, sizeof(nums) / sizeof(*nums));

    int numDest[3];
    flint_t elemCount = flarrGets(intArr, 1, 3, numDest);
    if(!(elemCount == 3 && numDest[0] == 1 && numDest[1] == 2 && numDest[2] == 3)){
        flerrHandle("\nTESf _flarrTestGets: Test Failed !1");
    }

    elemCount = flarrGets(intArr, 9, 3, numDest);  
    if(!(elemCount == 2 && numDest[0] == 9 && numDest[1] == 10 && numDest[2] == 3)){
        flerrHandle("\nTESf _flarrTestGets: Test Failed !2");
    }

    flarrFree(intArr);

    printf("\n_flarrTestGets: TEST OK");

    return true;
}

static bool _flarrTestShift(){
    flArray* intArr = flarrNew(0, sizeof(int));
    int nums[5] = {1, 2, 3, 4, 5};
    int numDest[5];
    flarrPushs(intArr, nums, sizeof(nums) / sizeof(*nums));

    flarrShift(intArr, -2);//perform left shift
    flarrGets(intArr, 0, 5, numDest);
    if( !(numDest[0] == 3 && numDest[1] == 4 && numDest[2] == 5 && 
          numDest[3] == 4 && numDest[4] == 5) ){
        flerrHandle("\nTESf _flarrTestShift: Test Failed !1");
    }

    flarrShift(intArr, 3);//perform right shift
    flarrGets(intArr, 0, 5, numDest);
    if( !(numDest[0] == 3 && numDest[1] == 4 && numDest[2] == 5 && 
          numDest[3] == 3 && numDest[4] == 4) ){
        flerrHandle("\nTESf _flarrTestShift: Test Failed !2");
    }

    flarrFree(intArr);

    printf("\n_flarrTestShift: TEST OK");

    return true;
}

static bool _flarrTestShiftAndFit(){
    flArray* intArr = flarrNew(0, sizeof(int));
    int nums[5] = {1, 2, 3, 4, 5};
    int numDest[7];
    flarrPushs(intArr, nums, sizeof(nums) / sizeof(*nums));

    flarrShiftAndFit(intArr, 2);//perform right shift and fit
    flarrGets(intArr, 0, 7, numDest);
    if( !(intArr->length == 7 && numDest[0] == 1 && numDest[1] == 2 && numDest[2] == 1 && 
          numDest[3] == 2 && numDest[4] == 3 && numDest[5] == 4 && numDest[6] == 5 ) ){
        flerrHandle("\nTESf _flarrTestShiftAndFit: Test Failed !1");
    }

    flarrShiftAndFit(intArr, -4);//perform left shift and fit
    flarrGets(intArr, 0, 3, numDest);
    if( !(intArr->length == 3 && numDest[0] == 3 && numDest[1] == 4 && numDest[2] == 5) ){
        flerrHandle("\nTESf _flarrTestShiftAndFit: Test Failed !2");
    }

    flarrFree(intArr);

    printf("\n_flarrTestShiftAndFit: TEST OK");

    return true;
}

bool _flarrRunTests(){
    _flarrTestPush();
    _flarrTestSetLength();
    _flarrTestPut();
    _flarrstrTestPush();
    _flarrstrTestPushs();
    _flarrstrTestPop();
    _flarrTestPushs();
    _flarrTestGets();
    _flarrTestShift();
    _flarrTestShiftAndFit();
    _flarrTestPuts();

    return true;
}