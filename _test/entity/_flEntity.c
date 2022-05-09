
#include"_flEntity.h"

/*-----TEST IOport-----*/
static bool runPortReadAndWriteTest(){
    flentIOport* inp = flentiopNew(1, flentiopTYPE_INPUT, 0);
    flentIOport* outp = flentiopNew(2, flentiopTYPE_OUTPUT, 0);
    flentiopLink(inp, outp);

    const char* origHstr = "Hello World!";
    flentiopPut(outp, flentiopDTYPE_STR, origHstr, strlen(origHstr)+1);

    flentiopDtype_t dtype = flentiopGetDataType(inp);
    const char* hellostr = (const char*)flentiopGetData(inp);

    bool status = ( dtype == flentiopDTYPE_STR && 
                strlen(origHstr)+1 == flentiopGetDataSize(inp) &&
                strcmp(origHstr, hellostr) == 0 );

    flentiopFree(inp);
    flentiopFree(outp);

    return status;
}

static bool runFlentstdBAoperTest(){
    bool status = false;

    flentXenv* xenv = flentxevNew(2);

    flEntity* adder = flentstdAddNew(xenv);
    flentIOport* adderOutput = flentFindPortByID(adder, flentstdADD_OUT, flentiopTYPE_OUTPUT);

    flentIOport* operAport = flentiopNew(0, flentiopTYPE_OUTPUT, 0);
    flentIOport* operBport = flentiopNew(1, flentiopTYPE_OUTPUT, 0);
    flentiopLink(operAport, flentFindPortByID(adder, flentstdADD_IN_A, flentiopTYPE_INPUT));
    flentiopLink(operBport, flentFindPortByID(adder, flentstdADD_IN_B, flentiopTYPE_INPUT));

    flint_t intOperA = 2;
    flint_t intOperB = 3;
    flentiopPut(operAport, flentiopDTYPE_INT, &intOperA, sizeof(flint_t));
    flentiopPut(operBport, flentiopDTYPE_INT, &intOperB, sizeof(flint_t));

    flentxevTick(xenv, 0, 0);
    status = flentiopGetOutputDataType(adderOutput) == flentiopDTYPE_INT && 
             *(flint_t*)flentiopGetOutputData(adderOutput) == intOperA+intOperB;
    flentxevTick(xenv, 0, 0);
    status = status && flentiopGetOutputDataType(adderOutput) == flentiopDTYPE_NIL;

    flentiopFree(operAport);
    flentiopFree(operBport);
    flentxevFree(xenv, true);

    return status;
}

static bool runFlentstdBytsToDptrTest(){
    bool status = false;

    flentXenv* xenv = flentxevNew(2);
    
    flEntity* btdEnt2 = flentstdBytsToDptrNew(xenv);
    flEntity* btdEnt1 = flentstdBytsToDptrNew(xenv);
    flentIOport* out1 = flentFindPortByID(btdEnt1, flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT);
    flentIOport* out2 = flentFindPortByID(btdEnt2, flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT);

    flentIOport* dataPort = flentiopNew(0, flentiopTYPE_OUTPUT, 0);
    int data = 64;

    flentiopLink(dataPort, flentFindPortByID(btdEnt1, flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT) );
    flentiopLink(flentFindPortByID(btdEnt1, flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT),
            flentFindPortByID(btdEnt2, flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT) );

    flentiopPut(dataPort, flentiopDTYPE_BYTS, &data, sizeof(int));

    flentxevTick(xenv, 0, 0);
    status = (  flentiopGetOutputDataType(out1) == flentiopDTYPE_DPTR &&
            *(int*) (*(flentiopDptr**)flentiopGetOutputData(out1))->data == data &&
            (*(flentiopDptr**)flentiopGetOutputData(out1))->dataSize == sizeof(int)  ) &&
            flentiopGetOutputDataType(out2) == flentiopDTYPE_NIL;

    flentxevTick(xenv, 0, 0);
    status = status && (  flentiopGetOutputDataType(out2) == flentiopDTYPE_DPTR &&
            *(int*) (*(flentiopDptr**)flentiopGetOutputData(out2))->data == data &&
            (*(flentiopDptr**)flentiopGetOutputData(out2))->dataSize == sizeof(int)  ) &&
            flentiopGetOutputDataType(out1) == flentiopDTYPE_NIL;

    flentiopFree(dataPort);
    flentxevFree(xenv, true);

    return status;
}

bool _flentRunTests(){

    if(runPortReadAndWriteTest()){
        printf("\nrunPortReadAndWriteTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runPortReadAndWriteTest)");
    }

    if(runFlentstdBAoperTest()){
        printf("\nrunFlentstdBAoperTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runFlentstdBAoperTest)");
    }

    if(runFlentstdBytsToDptrTest()){
        printf("\nrunFlentstdBytsToDptrTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runFlentstdBytsToDptrTest)");
    }

    return true;
}