
#include"_flEntity.h"


/*-----TEST ENTITY:strAdderEnt-----*/
typedef struct _strAdderEnt _strAdderEnt;

static const char* strAdderEntReadInput(flentIOport* port){
    flentIOdata iod = flentiopRead(port);
    if(iod.mode == flentdmoPOST && iod.id == flentdidSTRING){
        return (const char*)iod.data;
    }
    return "";
}

static void strAdderEntTick(flEntity* stradder, flentXenv* xenv){
    const char* input1str = strAdderEntReadInput(flentFindPortByName(stradder, flentipn1));
    const char* input2str = strAdderEntReadInput(flentFindPortByName(stradder, flentipn2));

    //concatenate the two strings and write them to the output
    flentIOport* output = flentFindPortByName(stradder, flentipn3);
    flentiopWrite(output, flentiodNew( flentdmoPOST, flentdidSTRING, (void*)input1str, strlen(input1str)) );
    flentiopAppendData(output, input2str, strlen(input2str)+1/*include null char*/);
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
    flentiopWrite(inp1, flentiodNew(flentdmoPOST, flentdidSTRING, "a", strlen("a")+1));
    flentiopWrite(inp2, flentiodNew(flentdmoPOST, flentdidSTRING, "b", strlen("b")+1));
    flentiopWrite(inp3, flentiodNew(flentdmoPOST, flentdidSTRING, "c", strlen("c")+1));
    flentiopWrite(inp4, flentiodNew(flentdmoPOST, flentdidSTRING, "d", strlen("d")+1));

    //Run the environment at least 2 times and read the output of the network.
    flentxevTick(xenv, 0, 0);
    flentxevTick(xenv, 0, 0);

    flentIOdata iod = flentiopReadOutput(adderNetOut);
    bool status = ( iod.mode == flentdmoPOST && iod.id == flentdidSTRING && strcmp("abcd", (const char*)iod.data) == 0 );

    //cleanup
    flentxevFree(xenv, true);

    return status;
}

bool _flentRunTests(){
    if(!runStrAdderTest()){
        flerrHandle("\nTESf _flentRunTests: Test Failed !1(rflarrNewunStrAdderTest)");
    }
    return true;
}