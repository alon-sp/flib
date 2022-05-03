
#include"flentstd.h"

static void  flentstdOperPlusHscmd(flentsyc_t cmd, void *args, void* rvalDest){

}

static void  flentstdOperPlusTick(flEntity* plent, flentXenv* xenv){
    flentIOport* inpA = flentFindPortByID(plent, flentstdOPERPLUS_IN_A, flentiopTYPE_INPUT);
    flentIOport* inpB = flentFindPortByID(plent, flentstdOPERPLUS_IN_B, flentiopTYPE_INPUT);
    flentIOport* outp = flentFindPortByID(plent, flentstdOPERPLUS_OUT, flentiopTYPE_OUTPUT);
    
    flentiopDtype_t inpAdtype = flentiopGetDataType(inpA);
    flentiopDtype_t inpBdtype = flentiopGetDataType(inpB);
    if( inpAdtype == flentiopDTYPE_NIL || inpBdtype  == flentiopDTYPE_NIL){
        return;
    }

    flint_t intOpA, intOpB, intSum;
    flnum_t numOpA, numOpB, numSum;

    if(inpAdtype == inpBdtype){

    }else{

    }
}

flEntity* flentstdOperPlusNew(flentXenv* xenv){
    flEntity* plusEnt = flentNew(xenv, 3);
    flentSetTick(plusEnt, flentstdOperPlusTick);
    flentSetHscmd(plusEnt, flentstdOperPlusHscmd);

    flentAddPort( plusEnt, flentiopNew(flentstdOPERPLUS_IN_A, flentiopTYPE_INPUT, 2) );
    flentAddPort( plusEnt, flentiopNew(flentstdOPERPLUS_IN_B, flentiopTYPE_INPUT, 2) );
    flentAddPort( plusEnt, flentiopNew(flentstdOPERPLUS_OUT, flentiopTYPE_OUTPUT, 2) );

    return plusEnt;
}