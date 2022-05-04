
#include"fl.h"
#include"flentstd.h"

/*----------BASIC ARITHMETIC OPERATORS----------*/
//-----------------------------------------------
static void  flentstdBAoperHscmd(flentsyc_t cmd, void *args, void* rvalDest){
    switch(cmd){
        case flentsycgetIOPORT_NAME:{
            flentIOport* port = (flentIOport*)args;
            if(port->id == flentstdBINOPER_IN_A) *(char**)rvalDest = FLSTR("A");
            else if(port->id == flentstdBINOPER_IN_B) *(char**)rvalDest = FLSTR("B");
            else if(port->id == flentstdBINOPER_OUT) *(char**)rvalDest = FLSTR("OUT");
        }
        break;

        case flentsycgetENT_IOPORT_NTH_DTYPE:{
            flentsycEntIoportNthDtypeArg* arg = (flentsycEntIoportNthDtypeArg*)args;
            if(arg->n == 0) *(flentiopDtype_t*)rvalDest = flentiopDTYPE_INT;
            else if(arg->n == 1) *(flentiopDtype_t*)rvalDest = flentiopDTYPE_NUM;
        }
        break;

        case flentsycgetENT_IOPORT_ACCEPT_DTYPE:{
            flentsycEntIoportAcceptDtypeArg* arg = (flentsycEntIoportAcceptDtypeArg*)args;
            if(arg->dtype == flentiopDTYPE_INT || arg->dtype == flentiopDTYPE_NUM)
                *(bool*)rvalDest = true;
        }
        break;
    }
}

#define _flentstdBAoperDefineFuncs(operName, intEvalExp, numEvalExp)\
static void  flentstd##operName##Tick(flEntity* baoEnt, flentXenv* xenv){\
    flentIOport* inpA = flentFindPortByID(baoEnt, flentstdBINOPER_IN_A, flentiopTYPE_INPUT);\
    flentIOport* inpB = flentFindPortByID(baoEnt, flentstdBINOPER_IN_B, flentiopTYPE_INPUT);\
    flentIOport* outp = flentFindPortByID(baoEnt, flentstdBINOPER_OUT, flentiopTYPE_OUTPUT);\
    if(outp->isBusy){\
        flentiopSetIsBusy(inpA, true);\
        flentiopSetIsBusy(inpB, true);\
        return;\
    }else if(inpA->isBusy || inpB->isBusy){\
        flentiopSetIsBusy(inpA, false);\
        flentiopSetIsBusy(inpB, false);\
    }\
\
    flentiopDtype_t inpAdtype = flentiopGetDataType(inpA);\
    flentiopDtype_t inpBdtype = flentiopGetDataType(inpB);\
    if( inpAdtype == flentiopDTYPE_NIL || inpBdtype  == flentiopDTYPE_NIL){\
        return;\
    }\
\
    if(inpAdtype == inpBdtype && inpAdtype == flentiopDTYPE_INT){\
        flint_t intOpA = *(flint_t*)flentiopGetData(inpA);\
        flint_t intOpB = *(flint_t*)flentiopGetData(inpB);\
\
        flint_t intResult = intEvalExp;\
        flentiopPut(outp, flentiopDTYPE_INT, &intResult, sizeof(flint_t));\
    }else{\
        flnum_t numOpA = (inpAdtype == flentiopDTYPE_NUM)? \
                *(flnum_t*)flentiopGetData(inpA) :\
                (flnum_t)*(flint_t*)flentiopGetData(inpA);\
\
        flnum_t numOpB = (inpBdtype == flentiopDTYPE_NUM)? \
                *(flnum_t*)flentiopGetData(inpB) :\
                (flnum_t)*(flint_t*)flentiopGetData(inpB);\
\
        flnum_t numResult = numEvalExp;\
        flentiopPut(outp, flentiopDTYPE_NUM, &numResult, sizeof(flnum_t));\
    }\
\
}\
\
flEntity* flentstd##operName##New(flentXenv* xenv){\
    flEntity* operName##Ent = flentNew(xenv, 3);\
    flentSetTick(operName##Ent, flentstd##operName##Tick);\
    flentSetHscmd(operName##Ent, flentstdBAoperHscmd);\
\
    flentAddPort( operName##Ent, flentiopNew(flentstdBINOPER_IN_A, flentiopTYPE_INPUT, 2) );\
    flentAddPort( operName##Ent, flentiopNew(flentstdBINOPER_IN_B, flentiopTYPE_INPUT, 2) );\
    flentAddPort( operName##Ent, flentiopNew(flentstdBINOPER_OUT, flentiopTYPE_OUTPUT, 2) );\
\
    return operName##Ent;\
}

_flentstdBAoperDefineFuncs(Add, intOpA+intOpB, numOpA+numOpB)

_flentstdBAoperDefineFuncs(Sub, intOpA-intOpB, numOpA-numOpB)

_flentstdBAoperDefineFuncs(Mult, intOpA*intOpB, numOpA*numOpB)

_flentstdBAoperDefineFuncs(Div, intOpA/intOpB, numOpA/numOpB)

_flentstdBAoperDefineFuncs(Mod, intOpA%intOpB, (flnum_t)fmod(numOpA, numOpB))

_flentstdBAoperDefineFuncs(Pow, (flint_t)pow(intOpA, intOpB), (flnum_t)pow(numOpA, numOpB))

/*----------flentiopDTYPE_BYTS TO flentiopDTYPE_DPTR (BytsToDptr)----------*/
//---------------------------------------------------------------------------
typedef struct{
    flArray* dptrs;
    uint8_t dptrTotalAllocatedLen;
    uint8_t dptrsMaxLen;
    flArray* buf;
}flentstdBytsToDptrProps;

static void flentstdBytsToDptrDonecb(flentiopDptr* dptr){

}

static void flentstdBytsToDptrTick(flEntity* ent, flentXenv* xenv){
    flentIOport* input = flentFindPortByID(ent, flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT);
    flentIOport* output = flentFindPortByID(ent, flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT);
    flentstdBytsToDptrProps* entProps = (flentstdBytsToDptrProps*)ent->props;

    if(output->isBusy || entProps->dptrs->length >= entProps->dptrsMaxLen){
        flentiopSetIsBusy(input, true);
        return;
    }else if(input->isBusy) flentiopSetIsBusy(input, false);

    flentiopDtype_t inDtype = flentiopGetDataType(input);
    if(inDtype == flentiopDTYPE_DPTR){
        flentiopPutb(output, flentiopGetBuf(input), flentiopGetBufSize(input));
    }else if(inDtype == flentiopDTYPE_BYTS){
        if(!entProps->buf){
            entProps->dptrs = flarrNew(entProps->dptrsMaxLen, sizeof(flentstdBytsToDptrProps));
            entProps->buf = flarrNew(flentiopGetDataSize(input), sizeof(flbyt_t));
        }

        void* bufOldDataPtr = entProps->buf->data;
        void* dataLoc = flarrPushs(entProps->buf, flentiopGetData(input), flentiopGetDataSize(input));
        void* bufNewDataPtr = entProps->buf->data;
        if(bufOldDataPtr != bufNewDataPtr){
            for(int i = 0; i<entProps->dptrs->length; i++){
                flentiopDptr* dptr = (flentiopDptr*)flarrGet(entProps->dptrs, i);
                _flentiopDptrUpdate( dptr, (char*)bufNewDataPtr + ((char*)bufOldDataPtr - (char*)dptr->_srcBuf) );
            }
        }

        flentiopDptr newDptr  = _flentiopDptrInit(ent, dataLoc, flentiopGetDataSize(input), flentstdBytsToDptrDonecb);

    }

    //uncompleted
}