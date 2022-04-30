
#include"_flEntity.h"

/*-----TEST IOport-----*/
static bool runPortReadAndWriteTest(){
    flentIOport* inp = flentiopNew(1, flentiopTYPE_INPUT);
    flentIOport* outp = flentiopNew(2, flentiopTYPE_OUTPUT);
    flentiopLink(inp, outp);
    flentiopPut(outp, flentiopDTYPE_STR, "Hello", strlen("Hello")+1);

    flentiopDtype_t dtype = flentiopGetDataType(inp);
    const char* hellostr = (const char*)flentiopGetData(inp);

    bool status = ( dtype == flentiopDTYPE_STR && strcmp("Hello", hellostr) == 0 );

    flentiopFree(inp);
    flentiopFree(outp);

    return status;
}

bool _flentRunTests(){

    if(runPortReadAndWriteTest()){
        printf("\n_runPortReadAndWriteTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runPortReadAndWriteTest)");
    }

    return true;
}