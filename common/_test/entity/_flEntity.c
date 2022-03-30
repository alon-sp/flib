
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

}

flEntity* strAdderEntNew(flentXenv* xenv){
    flEntity* ent = flentNew(xenv, 0, 0);
    flentIOport* input1 = flentiopNewInputPort(flentipnONE, NULL);
    flentIOport* input2 = flentiopNewInputPort(flentipnTWO, NULL);
    flentIOport* output = flentiopNewOutputport(flentipnTHREE, NULL);
    
    flentAddPort(ent, input1);
    flentAddPort(ent, input2);
    flentAddPort(ent, output);

    flentSetTick(ent, strAdderEntTick);

    return ent;
}

bool _flentRunTests(){

    return true;
}