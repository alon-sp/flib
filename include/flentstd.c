
#include"fl.h"
#include"flentstd.h"

/*----------BASIC ARITHMETIC OPERATORS----------*/
//-----------------------------------------------
static void  flentstdBAoperHscmd(flentsyc_t cmd, void *args, void* rvalDest){
    switch(cmd){
        case flentsycgetENT_NAME:{
            //to be implemented
        }
        break;

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
        if( !flentiopSetIsBusy(inpA, false) ) return;\
        if( !flentiopSetIsBusy(inpB, false) ) return;\
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

static void  flentstdBytsToDptrHscmd(flentsyc_t cmd, void *args, void* rvalDest){
    switch(cmd){
        case flentsycgetENT_NAME:
            *(char**)rvalDest = FLSTR("stdBytsToDptr");
        break;

        case flentsycgetIOPORT_NAME:{
            flentIOport* port = (flentIOport*)args;
            if(port->id == flentstdBYTSTODPTR_IN) *(char**)rvalDest = FLSTR("IN");
            else if(port->id == flentstdBYTSTODPTR_OUT) *(char**)rvalDest = FLSTR("OUT");
        }
        break;

        case flentsycgetENT_IOPORT_NTH_DTYPE:{
            flentsycEntIoportNthDtypeArg* darg = (flentsycEntIoportNthDtypeArg*)args;
            if(darg->n == 0) *(flentiopDtype_t*)rvalDest = flentiopDTYPE_DPTR;
            else if(darg->n == 1) *(flentiopDtype_t*)rvalDest = flentiopDTYPE_BYTS;
        }
        break;

        case flentsycgetENT_IOPORT_ACCEPT_DTYPE:{
            flentsycEntIoportAcceptDtypeArg* darg = (flentsycEntIoportAcceptDtypeArg*)args;
            if(darg->dtype == flentiopDTYPE_DPTR ||
                (darg->port->id == flentstdBYTSTODPTR_IN && darg->dtype == flentiopDTYPE_BYTS) )
                     *(bool*)rvalDest = true;
        }
        break;

        case flentsycCLEANUP:{
            flEntity* ent = (flEntity*)args;
            flentiopDptr* dptr = (flentiopDptr*)ent->props;
            if(dptr){
                if(dptr->_props){
                    flarrFree((flArray*)dptr->_props);
                    _flentiopDptrSetProps(dptr, NULL);
                }

                flmemFree(dptr);
                flentSetProps(ent, NULL);
            }
        }
        break;
    }
}

static void flentstdBytsToDptrDonecb(flentiopDptr* dptr){
    if(!(dptr && dptr->_props)) return;

    flarrSetLength((flArray*)dptr->_props, 0);
    flentiopSetIsBusy(flentFindPortByID(dptr->_srcEnt, flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT), false);
}

static void flentstdBytsToDptrTick(flEntity* ent, flentXenv* xenv){
    flentIOport* input = flentFindPortByID(ent, flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT);
    flentIOport* output = flentFindPortByID(ent, flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT);
    flentiopDptr* dptr = (flentiopDptr*)ent->props;
    flArray* dBuf = (flArray*)dptr->_props;

    if(output->isBusy || (dBuf && dBuf->length) ){
        flentiopSetIsBusy(input, true);
        return;
    }else if(input->isBusy) if( !flentiopSetIsBusy(input, false) ) return;

    flentiopDtype_t inDtype = flentiopGetDataType(input);
    if(inDtype == flentiopDTYPE_DPTR){
        flentiopPutb(output, flentiopGetBuf(input), flentiopGetBufSize(input));
    }else if(inDtype == flentiopDTYPE_BYTS){
        if(!dBuf){
            dBuf = flarrNew( flentiopGetDataSize(input), sizeof(flbyt_t) );
            _flentiopDptrSetProps( dptr, dBuf );
        }

        flarrSetLength(dBuf, 0);
        flarrPushs(dBuf, flentiopGetData(input), flentiopGetDataSize(input));
        flentiopDptrSetData(dptr, dBuf->data, dBuf->length);

        flentiopPut(output, flentiopDTYPE_DPTR, &dptr, sizeof(dptr));
    }
}

flEntity* flentstdBytsToDptrNew(flentXenv* xenv){
    flEntity* ent = flentNew(xenv, 2);

    flentiopDptr _data = _flentiopDptrInit(ent, NULL, 0, flentstdBytsToDptrDonecb);
    flentiopDptr* dbuf = flmemMalloc(sizeof(flentiopDptr));
    memcpy(dbuf, &_data, sizeof(flentiopDptr));
    flentSetProps(ent, dbuf);

    flentSetTick(ent, flentstdBytsToDptrTick);
    flentSetHscmd(ent, flentstdBytsToDptrHscmd);

    flentAddPort( ent, flentiopNew(flentstdBYTSTODPTR_IN, flentiopTYPE_INPUT, 2) );
    flentAddPort(ent, flentiopNew(flentstdBYTSTODPTR_OUT, flentiopTYPE_OUTPUT, 1) );

    return ent;
}

/*----------flentiopDTYPE TO flentiopDTYPE_STR (ToStr)----------------------*/
//---------------------------------------------------------------------------

static void  flentstdToStrHscmd(flentsyc_t cmd, void *args, void* rvalDest){
    switch(cmd){
        case flentsycgetENT_NAME:
            *(char**)rvalDest = FLSTR("stdToStr");
        break;

        case flentsycgetIOPORT_NAME:{
            flentIOport* port = (flentIOport*)args;
            if(port->id == flentstdTOSTR_IN) *(char**)rvalDest = FLSTR("IN");
            else if(port->id == flentstdTOSTR_OUT) *(char**)rvalDest = FLSTR("OUT");
        }
        break;

        case flentsycgetENT_IOPORT_NTH_DTYPE:{
            flentsycEntIoportNthDtypeArg* arg = (flentsycEntIoportNthDtypeArg*)args;
            if(arg->port->id == flentstdTOSTR_OUT && arg->n == 0) 
                *(flentiopDtype_t*)rvalDest = flentiopDTYPE_STR;
        }
        break;

        case flentsycgetENT_IOPORT_ACCEPT_DTYPE:{
            flentsycEntIoportAcceptDtypeArg* arg = (flentsycEntIoportAcceptDtypeArg*)args;
            if(arg->port->id == flentstdTOSTR_OUT){
                if(arg->dtype == flentiopDTYPE_STR) *(bool*)rvalDest = true;
            }else *(bool*)rvalDest = true;
        }
        break;
    }
}

static void flentstdToStrTick(flEntity* ent, flentXenv* xenv){
    flentIOport* input = flentFindPortByID(ent, flentstdTOSTR_IN, flentiopTYPE_INPUT);
    flentIOport* output = flentFindPortByID(ent, flentstdTOSTR_OUT, flentiopTYPE_OUTPUT);

    if(output->isBusy){
        flentiopSetIsBusy(input, true);
        return;
    }else if(input->isBusy) if(!flentiopSetIsBusy(input, false)) return;

    switch(flentiopGetDataType(input)){
        case flentiopDTYPE_BOOL:
            flentiopPut(output, flentiopDTYPE_STR, *(bool*)flentiopGetData(input)? "1":"0", 2);
        break;

        case flentiopDTYPE_INT:{
            char numBuf[25];
            flintToStr(numBuf, *(flint_t*)flentiopGetData(input));
            flentiopPut(output, flentiopDTYPE_STR, numBuf, strlen(numBuf)+1 );
        }
        break;

        case flentiopDTYPE_NUM:{
            char numBuf[25];
            flnumToStr(numBuf, *(flnum_t*)flentiopGetData(input));
            flentiopPut(output, flentiopDTYPE_STR, numBuf, strlen(numBuf)+1 );
        }
        break;

        case flentiopDTYPE_STR:
        case flentiopDTYPE_JSON:
            flentiopPut(output, flentiopDTYPE_STR, flentiopGetData(input), flentiopGetDataSize(input));
        break;

        default:{
            flbyt_t* bytes = flentiopGetData(input);
            size_t bytesSize = flentiopGetDataSize(input);
            if(flentiopGetDataType(input) == flentiopDTYPE_DPTR){
                flentiopDptr* dp = *(flentiopDptr**)bytes;
                bytes = dp->data;
                bytesSize = dp->dataSize;
            }

            //not mandatory but it ensures that the capacity of the output buffer is expanded if needed
            flentiopEnsureObufCapacity(output, sizeof(flentiopDtype_t) + (bytesSize*2) + 1);

            flentiopPut(output, flentiopDTYPE_STR, NULL, 0);
            for(size_t i = 0; i < bytesSize; i++){
                char hexBuf[5];
                sprintf(hexBuf, "%02X ", *(bytes+i));
                flentiopAppendData(output, hexBuf, i+1 == bytesSize? strlen(hexBuf)+1 : strlen(hexBuf));
            }
        }
        break;
    }
}

flEntity* flentstdToStrNew(flentXenv* xenv){
    flEntity* ent = flentNew(xenv, 2);

    flentSetTick(ent, flentstdToStrTick);
    flentSetHscmd(ent, flentstdToStrHscmd);

    flentAddPort( ent, flentiopNew(flentstdTOSTR_IN, flentiopTYPE_INPUT, 0) );
    flentAddPort(ent, flentiopNew(flentstdTOSTR_OUT, flentiopTYPE_OUTPUT, 1) );

    return ent;
}