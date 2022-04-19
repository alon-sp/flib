
#include"_flEntity.h"

/*-----TEST IOport-----*/
static bool runPortReadAndWriteTest(){
    flentIOport* ioPort = flentiopNewIOport(flentipn1, NULL);
    
    flentiopPuts(ioPort, flentdmoPOST, flentdidSTRING, "Hello", strlen("Hello")+1);

    flentdmo_t mode = flentiopGetOutputMode(ioPort);
    flentdid_t id = flentiopGetOutputID(ioPort);
    const char* hellostr = (const char*)flentiopGetOutputData(ioPort);

    bool status = ( mode == flentdmoPOST &&  id == flentdidSTRING && strcmp("Hello", hellostr) == 0 );

    flentiopFree(ioPort);

    return status;
}

static bool runPdPortReadAndWriteTest(){
    flentIOport* ioPort = flentiopNewIOport(flentipn1, NULL);

    int inum = 27;
    flvod_tf _cb = NULL;
    int _cbArg = 12;

    flentiopPdPuts(ioPort, flentdidDATA, &inum, sizeof(inum), _cb, &_cbArg, sizeof(_cbArg));

    flentdmo_t mode = flentiopGetOutputMode(ioPort);
    flentdid_t id = flentiopGetOutputID(ioPort);
    int data = *(int*)flentiopGetOutputPdPtr(ioPort);
    int dataSize = flentiopGetOutputPdSize(ioPort);
    flvod_tf cb = flentiopGetOutputPdDonecb(ioPort);
    int cbArg = *(int*)flentiopGetOutputPdDonecbArgs(ioPort);
    
    bool status = (mode == flentdmoPOSTDP && id == flentdidDATA && data == inum && dataSize == sizeof(inum)
                  && cb == _cb && cbArg == _cbArg);
    
    flentiopFree(ioPort);

    return status;
}

/*-----TEST ENTITY:strAdderEnt-----*/
typedef struct _strAdderEnt _strAdderEnt;

static const char* strAdderEntReadInput(flentIOport* port){
    if(flentiopGetInputMode(port) == flentdmoPOST && flentiopGetInputID(port) == flentdidSTRING){
        return (const char*)flentiopGetInputData(port);
    }
    return "";
}

static void strAdderEntTick(flEntity* stradder, flentXenv* xenv){
    const char* input1str = strAdderEntReadInput(flentFindPortByName(stradder, flentipn1));
    const char* input2str = strAdderEntReadInput(flentFindPortByName(stradder, flentipn2));

    //concatenate the two strings and write them to the output
    flentIOport* output = flentFindPortByName(stradder, flentipn3);
    flentiopPuts( output, flentdmoPOST, flentdidSTRING, (void*)input1str, strlen(input1str) );
    flentiopAppend(output, input2str, strlen(input2str)+1/*include null char*/);
}

flEntity* strAdderEntNew(flentXenv* xenv){
    flEntity* ent = flentNew(xenv, 0, 0);
    flentIOport* input1 = flentiopNewInputPort(flentipn1, NULL);
    flentIOport* input2 = flentiopNewInputPort(flentipn2, NULL);
    flentIOport* output = flentiopNewIOport(flentipn3, NULL);
    
    flentAddPort(ent, input1);
    flentAddPort(ent, input2);
    flentAddPort(ent, output);

    flentSetTick(ent, strAdderEntTick);

    return ent;
}

static bool runStrAdderTest(){

    flentXenv* xenv = flentxevNew(1);

    //Create some strAdder entities
    flEntity* adder1 = strAdderEntNew(xenv);
    flEntity* adder2 = strAdderEntNew(xenv);
    flEntity* adder3 = strAdderEntNew(xenv);

    //Connect adders
    flentiopLink(flentFindPortByName(adder1, flentipn3), flentFindPortByName(adder3, flentipn1));
    flentiopLink(flentFindPortByName(adder2, flentipn3), flentFindPortByName(adder3, flentipn2));

    //Create some ports for sending inputs to the above entities
    flentIOport* inp1 = flentiopNewIOport(flentipn1, NULL);
    flentIOport* inp2 = flentiopNewIOport(flentipn2, NULL);
    flentIOport* inp3 = flentiopNewIOport(flentipn3, NULL);
    flentIOport* inp4 = flentiopNewIOport(flentipn4, NULL);

    //link entities with input and ouput ports
    flentiopLink(inp1, flentFindPortByName(adder1, flentipn1));
    flentiopLink(inp2, flentFindPortByName(adder1, flentipn2));
    flentiopLink(inp3, flentFindPortByName(adder2, flentipn1));
    flentiopLink(inp4, flentFindPortByName(adder2, flentipn2));

    flentIOport* adderNetOut = flentFindPortByName(adder3, flentipn3);

    //Set up some inputs for the network
    flentiopPuts(inp1, flentdmoPOST, flentdidSTRING, "a", strlen("a")+1);
    flentiopPuts(inp2, flentdmoPOST, flentdidSTRING, "b", strlen("b")+1);
    flentiopPuts(inp3, flentdmoPOST, flentdidSTRING, "c", strlen("c")+1);
    flentiopPuts(inp4, flentdmoPOST, flentdidSTRING, "d", strlen("d")+1);

    //Run the environment at least 2 times and read the output of the network.
    flentxevTick(xenv, 0, 0);
    flentxevTick(xenv, 0, 0);

    bool status = ( flentiopOmo(adderNetOut) == flentdmoPOST && 
                    flentiopOid(adderNetOut) == flentdidSTRING && 
                    strcmp("abcd", (const char*)flentiopOdt(adderNetOut)) == 0 );

    //cleanup
    flentxevFree(xenv, true);

    return status;
}

bool _flentRunTests(){

    if(!runPortReadAndWriteTest()){
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(runPortReadAndWriteTest)");
    }

    if(!runPdPortReadAndWriteTest()){
        flerrHandle("\nTESf _flentRunTests: Test Failed !2(runPdPortReadAndWriteTest)");
    }

    if(!runStrAdderTest()){
        flerrHandle("\nTESf _flentRunTests: Test Failed !3(runStrAdderTest)");
    }

    return true;
}