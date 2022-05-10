
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

static bool runFlentiopTypeMoTest(){
    bool status = false;

    flentXenv* xenv = flentxevNew(2);

    flEntity* adder1 = flentstdAddNew(xenv);
    flEntity* adder2 = flentstdAddNew(xenv);
    flEntity* adder3 = flentstdAddNew(xenv);
    flentIOport* adder3Out = flentFindPortByID(adder3, flentstdADD_OUT, flentiopTYPE_OUTPUT);

    //Set up a network to evaluate the expression ( (2+2)+(2+3) )
    flentIOport* operPort = flentiopNew(0, flentiopTYPE_OUTPUT, 0);
    flentIOport* adder2InBOperPort = flentiopNew(1, flentiopTYPE_OUTPUT, 0);

    flentiopLink(operPort, flentFindPortByID(adder1, flentstdADD_IN_A, flentiopTYPE_INPUT));
    flentiopLink(operPort, flentFindPortByID(adder1, flentstdADD_IN_B, flentiopTYPE_INPUT));
    flentiopLink(operPort, flentFindPortByID(adder2, flentstdADD_IN_A, flentiopTYPE_INPUT));
    flentiopLink(adder2InBOperPort, flentFindPortByID(adder2, flentstdADD_IN_B, flentiopTYPE_INPUT));
    flentiopLink(flentFindPortByID(adder1, flentstdADD_OUT, flentiopTYPE_OUTPUT), 
                 flentFindPortByID(adder3, flentstdADD_IN_A, flentiopTYPE_INPUT));
    flentiopLink(flentFindPortByID(adder2, flentstdADD_OUT, flentiopTYPE_OUTPUT), 
                 flentFindPortByID(adder3, flentstdADD_IN_B, flentiopTYPE_INPUT));

    int operPortData = 2;
    flentiopPut(operPort, flentiopDTYPE_INT, &operPortData, sizeof(operPortData));
    int adder2InBOperPortData = 3;
    flentiopPut(adder2InBOperPort, flentiopDTYPE_INT, &adder2InBOperPortData, sizeof(adder2InBOperPortData));

    //Activate the busy status of one port and check to ensure that it affect only connected ports
    flentiopSetIsBusy(flentFindPortByID(adder1, flentstdADD_IN_A, flentiopTYPE_INPUT), true);
    status = flentFindPortByID(adder1, flentstdADD_IN_A, flentiopTYPE_INPUT)->isBusy &&
             flentFindPortByID(adder1, flentstdADD_IN_B, flentiopTYPE_INPUT)->isBusy &&
             flentFindPortByID(adder2, flentstdADD_IN_A, flentiopTYPE_INPUT)->isBusy &&
             !flentFindPortByID(adder2, flentstdADD_IN_B, flentiopTYPE_INPUT)->isBusy;
    
    //Attempt deactivating the busy status set above using a different connected port
    status = status && !flentiopSetIsBusy(flentFindPortByID(adder1, flentstdADD_IN_B, flentiopTYPE_INPUT), false);

    //Attempt deactivating the busy status set above using the port that activated it
    status = status && flentiopSetIsBusy(flentFindPortByID(adder1, flentstdADD_IN_A, flentiopTYPE_INPUT), false);

    //Execute the network once and ensure that the result is as expected
    flentxevTick(xenv, 0, 0);
    status = status && *(int*)flentiopGetOutputData(adder3Out) == 
            ( (operPortData+operPortData)+(operPortData+adder2InBOperPortData) );

    flentiopFree(operPort);
    flentiopFree(adder2InBOperPort);

    flentxevFree(xenv, true);

    return status;
}

static bool runFlentstdToStrTest(){
    flentXenv* xenv = flentxevNew(1);
    flEntity* toStrEnt = flentstdToStrNew(xenv);

    flentIOport* dataInPort = flentiopNew(0, flentiopTYPE_OUTPUT, 0);
    flentiopLink(dataInPort, flentFindPortByID(toStrEnt, flentstdTOSTR_IN, flentiopTYPE_INPUT));

    flentIOport* toStrEntOutPort = flentFindPortByID(toStrEnt, flentstdTOSTR_OUT, flentiopTYPE_OUTPUT);

    bool status;
    //Convert a flentiopDTYPE_NUM type data to string
    flnum_t num = 32.63459873f;
    flentiopPut(dataInPort, flentiopDTYPE_NUM, &num, sizeof(num));
    flentxevTick(xenv, 0, 0);
    //check if result is as expected
    char numStrBuf[25];
    flnumToStr(numStrBuf, num);
    status = flentiopGetOutputDataType(toStrEntOutPort) == flentiopDTYPE_STR &&
            strcmp(numStrBuf, (const char*)flentiopGetOutputData(toStrEntOutPort)) == 0;

    //Convert a flentiopDTYPE_DPTR type data to string
    char* strData = "My very very very very very very very long Hello World Greeting Message";
    char strHexData[ (strlen(strData)*2) + 1];
    for(int i = 0; i<strlen(strData); i++){
        char hexBuf[5];
        sprintf(hexBuf, "%02X", *(strData+i));

        strHexData[(i*2)+0] = hexBuf[0];
        strHexData[(i*2)+1] = hexBuf[1];
        if(i+1 == strlen(strData)) strHexData[(i*2)+2] = '\0';
    }

    flentiopDptr dptr = _flentiopDptrInit(NULL, strData, strlen(strData), NULL);
    flentiopDptr* dp = &dptr;
    flentiopPut(dataInPort, flentiopDTYPE_DPTR, &dp, sizeof(dp));
    flentxevTick(xenv, 0, 0);
    //check if result is as expected
    status = status && strlen(strHexData) == strlen(strData)*2 &&
            strlen(strHexData)+1 == flentiopGetOutputDataSize(toStrEntOutPort) &&
            flentiopGetOutputDataType(toStrEntOutPort) == flentiopDTYPE_STR &&
            strcmp(strHexData, (const char*)flentiopGetOutputData(toStrEntOutPort)) == 0;

    flentiopFree(dataInPort);
    flentxevFree(xenv, true);
    return status;
}

static bool runFlentstdPrintsTest(){
    flentXenv* xenv = flentxevNew(1);
    flEntity* printer = flentstdPrintsNew(xenv);
    flentIOport* printerDataInPort = flentiopNew(0, flentiopTYPE_OUTPUT, 0);

    char* strData = "\n--runFlentstdPrintsTest: !!Hello new World!!--";
    flentiopPut(printerDataInPort, flentiopDTYPE_STR, strData, strlen(strData)+1);
    flentiopLink(printerDataInPort, flentFindPortByID(printer, flentstdPRINTS_IN, flentiopTYPE_INPUT));

    //run entity and check console for desire output.
    flentxevTick(xenv, 0, 0);

    flentiopFree(printerDataInPort);
    flentxevFree(xenv, true);
    return true;
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

    if(runFlentiopTypeMoTest()){
        printf("\nrunFlentiopTypeMoTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runFlentiopTypeMoTest)");
    }

    if(runFlentstdToStrTest()){
        printf("\nrunFlentstdToStrTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runFlentstdToStrTest)");
    }

    if(runFlentstdPrintsTest()){
        printf("\nrunFlentstdPrintsTest: TEST OK");
    }else{
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runFlentstdPrintsTest)");
    }

    return true; 
}