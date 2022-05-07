#include"flentIOport.h"
#include"fl.h"
#include"flEntity.h"

#define _flentiopClear(port)\
    _flarrSetLength(port->_obuf, _flentiopOBUF_DTYPE_INDEX+sizeof(flentiopDtype_t));\
    _flentiopObufSetDataType(port, flentiopDTYPE_NIL);

#define _flentiopOmInitLports(omPort)\
    _flentiopSetlinkedPort(omPort, /**@noted*/(flentIOport*)flarrNew(2, sizeof(flentIOport*)) )

#define _flentiopOmGetLportsLen(omPort) ( ((flArray*)(omPort)->_linkedPort)->length )
#define _flentiopOmGetLport(omPort, index) ( *(flentIOport**)_flarrGet((flArray*)(omPort)->_linkedPort, index) )

static flentIOport** flentiopOmFindPort(flentIOport* omPort, flentIOport* inPort){
    flArray* linkedPorts = (flArray*)omPort->_linkedPort;
    for(int i = 0; i<linkedPorts->length; i++){
        flentIOport** ploc = (flentIOport**)_flarrGet(linkedPorts, i);
        if(*ploc == inPort) return ploc;
    }

    return NULL;
}

static void flentiopOmAddPort(flentIOport* omPort, flentIOport* inPort){
    if(!flentiopOmFindPort(omPort, inPort)){
        flentIOport** nullLoc = flentiopOmFindPort(omPort, NULL);
        if(nullLoc) *nullLoc = inPort;
        else flarrPush((flArray*)omPort->_linkedPort, &inPort);
    }
}

static void flentiopOmRemovePort(flentIOport* omPort, flentIOport* inPort){
    flentIOport** ploc = flentiopOmFindPort(omPort, inPort);
    if(ploc) *ploc = NULL;
}

flentIOport* flentiopNew(flentiopID_t id, flentiopType_t type, flentiopDTC_t dataTypeCount){
    flentIOport* iop = flmemMalloc(sizeof(flentIOport));

    flArray* obuf = NULL;

    if(type != flentiopTYPE_INPUT){
        obuf = flarrNew(_flentiopOBUF_PRE_DATA_META_SIZE+sizeof(void*), sizeof(flbyt_t)); 
    }

    _flentiopSetObuf(iop, obuf);
    if(obuf){
        //Write default output value
        _flentiopClear(iop);
    }

    _flentiopSetlinkedPort(iop, NULL);
    if(type == flentiopTYPE_OM) _flentiopOmInitLports(iop);

    _flentiopSetIsBusy(iop, false);
    flentiopSetEntity(iop, NULL);
    flentiopSetID(iop, id);
    flentiopSetDataTypeCount(iop, dataTypeCount);
    _flentiopSetType(iop, type);
    flentiopSetProps(iop, NULL);

    return iop;
}

void flentiopFree(flentIOport* iop){
    if(!iop) return;

    flentiopUnlink(iop);

    if(iop->_obuf){
        flarrFree(iop->_obuf);
        _flentiopSetObuf(iop, NULL);
    }

    flmemFree(iop);
}

//Check whether the given input port accepts all data types of the given output port
static bool _flentiopAcceptAllDtype(flentIOport* inPort, flentIOport* outPort){
    if( !(inPort->dataTypeCount && outPort->dataTypeCount) ) return true;

    bool opStatus = true;

    if(inPort->entity && outPort->entity){
        for(flentiopDataType_t i = 0; i < outPort->dataTypeCount; i++){
            
            flentsycEntIoportNthDtypeArg dtypeArg = {.port = outPort, .n = i};
            flentiopDtype_t dtype = flentiopDTYPE_NIL;
            outPort->entity->hscmd(flentsycgetENT_IOPORT_NTH_DTYPE, &dtypeArg, &dtype);
            if(dtype == flentiopDTYPE_NIL) continue;

            bool acceptStatus = false;
            flentsycEntIoportAcceptDtypeArg acceptArg = {.port = inPort, .dtype = dtype};
            inPort->entity->hscmd(flentsycgetENT_IOPORT_ACCEPT_DTYPE, &acceptArg, &acceptStatus);
            if(!acceptStatus){
                opStatus = false;
                break;
            }
        }
    }

    return opStatus;
}

bool flentiopLink(flentIOport* port1, flentIOport* port2){
    if(!(port1 && port2)) return false;

    if(flentiopTypeToBasicType(port1->type) == flentiopTypeToBasicType(port2->type)) goto HANDLE_INCOMPATIBLE_PORT;

    flentIOport* inPort =  (port1->type == flentiopTYPE_INPUT)? port1 : port2;
    flentIOport* outPort = (port1->type == flentiopTYPE_INPUT)? port2 : port1;

    if(!_flentiopAcceptAllDtype(inPort, outPort)) goto HANDLE_INCOMPATIBLE_PORT;

    _flentiopSetlinkedPort(inPort, outPort);
    
    if(outPort->type == flentiopTYPE_OUTPUT && !outPort->_linkedPort){
        _flentiopSetlinkedPort(outPort, inPort);

    }else if(outPort->type == flentiopTYPE_OUTPUT && outPort->_linkedPort != inPort){
        //upgrade $outPort to flentiopTYPE_OM type port
        flentIOport* existingLinkedPort = outPort->_linkedPort;
        _flentiopSetType(outPort, flentiopTYPE_OM);
        _flentiopOmInitLports(outPort);

        flentiopOmAddPort(outPort, existingLinkedPort);
    }

    if(outPort->type == flentiopTYPE_OM) flentiopOmAddPort(outPort, inPort);

    return true;

HANDLE_INCOMPATIBLE_PORT:{
    char* port1Name = FLSTR("");
    if(port1->entity) port1->entity->hscmd(flentsycgetIOPORT_NAME, port1, &port1Name);
    char* port2Name = FLSTR("");
    if(port2->entity) port2->entity->hscmd(flentsycgetIOPORT_NAME, port2, &port2Name);

    flArray* errLog = flarrNew(strlen(port1Name)+strlen(port2Name)+16, sizeof(char));

    flerrHandle(flarrstrPushs(errLog, 4, FLSTR("\nICMport: "), port1Name, FLSTR(" !-> "), port2Name));

    flarrFree(errLog);
}

    return false;
}

flentIOport* flentiopUnlink(flentIOport* iop){
    if( !(iop && iop->_linkedPort) ) return NULL;
    if(iop->type == flentiopTYPE_OM) return NULL;

    flentIOport* linkedPort = iop->_linkedPort;
    if(linkedPort->type == flentiopTYPE_OM) flentiopOmRemovePort(linkedPort, iop);
    else _flentiopSetlinkedPort(linkedPort, NULL);

    _flentiopSetlinkedPort(iop, NULL);

    return linkedPort;
}

//--

static void flentiopHandleInvalidOperation(flentIOport* port, const char* operNameStr, const char* portAdjStr){
    char* portName = FLSTR("");
    if(port->entity){
        port->entity->hscmd(flentsycgetIOPORT_NAME, port, &portName);
    }
    flArray* errLog = flarrNew(strlen(portName)+strlen(operNameStr)+strlen(portAdjStr)+16, sizeof(char));
    flerrHandle(flarrstrPushs(errLog, 6, FLSTR("\nINVop:"), operNameStr, FLSTR(" on ") , portAdjStr ,FLSTR(" port: "), portName));
    flarrFree(errLog);
}

//--Functions and micros for writing to port--
//--------------------------------------------

static bool _flentiopPerformDefaultPreWriteOps(flentIOport* iop, const char* callerNameStr, flentiopType_t newDataType, const void* newDataPtr){
    if( iop->type == flentiopTYPE_INPUT || iop->isBusy){
        const char* adjStr = iop->isBusy? FLSTR("busy") : FLSTR("input");
        flentiopHandleInvalidOperation(iop, callerNameStr, adjStr);
        return false;
    }


    if(iop->type == flentiopTYPE_OM) {
        for(int i = 0; i < _flentiopOmGetLportsLen(iop); i++){
            flentIOport* lp = _flentiopOmGetLport(iop, i);
            if(lp && lp->entity) _flentEnableTick(lp->entity);
        }
    }else{
        if(iop->_linkedPort && iop->_linkedPort->entity)_flentEnableTick(iop->_linkedPort->entity);
    }

    if(newDataType == flentiopDTYPE_DPTR){
        flentiopDptr* dptr = *(flentiopDptr**)newDataPtr;
        _flentiopDptrIncLsp(dptr);
    }

    if(_flentiopObufGetDataType(iop) == flentiopDTYPE_DPTR){
        flentiopDptr* dptr = *(flentiopDptr**)_flentiopObufGetData(iop);
        _flentiopDptrDecLsp(dptr);
        if(dptr->_lsp <= 0){
            dptr->_donecb(dptr);
        }
    }
    return true;
}

void flentiopPut(flentIOport* port, flentiopDtype_t dtype, const void* dataPtr, size_t dataSize){
    if( !_flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopPut"), dtype, dataPtr) ) return;

    _flarrSetLength(port->_obuf, _flentiopOBUF_DATA_INDEX+dataSize);
    _flentiopObufSetDataType(port, dtype);
    _flarrPuts(port->_obuf, _flentiopOBUF_DATA_INDEX, dataPtr, dataSize);
}

void flentiopPutb(flentIOport* port, const void* bytesPtr, size_t bytesSize){
    if( 
        !_flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopPutb"), 
        *(flentiopDtype_t*)bytesPtr, (char*)bytesPtr + sizeof(flentiopDtype_t)) ) return;

    _flarrSetLength(port->_obuf, bytesSize);
    _flarrPuts(port->_obuf, 0, bytesPtr, bytesSize);
}

void flentiopClear(flentIOport* port){
    if( !_flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopClear"), flentiopDTYPE_NIL, NULL) ){
        return;
    }

    _flentiopClear(port);
}

bool flentiopSetIsBusy(flentIOport* port, bool bval){
    if(flentiopTypeToBasicType(port->type) == flentiopTYPE_OUTPUT){
        flentiopHandleInvalidOperation(port, "flentiopSetIsBusy", "output");
    }

    if(port->isBusy == bval) return true;
    
//!!incompleted function!!
//------------------------
    if(port->type == flentiopTYPE_OM){
        if(bval && !port->isBusy){

        }else if(!bval && port->isBusy){

        }
    }else{
        _flentiopSetIsBusy(port, bval);
        if( port->_linkedPort){
            _flentiopSetIsBusy(port->_linkedPort, bval);
            if(port->_linkedPort->entity) _flentEnableTick(port->_linkedPort->entity);
        }
    }

    return true;
}

//--Functions and micros for reading from port--
//----------------------------------------------
#define _flentiopPerformDefaultPreReadChecks(port, operNameStr, returnVal)\
    if(port->isBusy || port->type == flentiopTYPE_OUTPUT){\
        const char* adjstr = port->isBusy? "busy" : "output";\
        flentiopHandleInvalidOperation(port, FLSTR(operNameStr), FLSTR(adjstr));\
        return returnVal;\
    }

void* flentiopGetBuf(flentIOport* port){
    _flentiopPerformDefaultPreReadChecks(port, "flentiopGetBuf", NULL)

    return port->_linkedPort? _flentiopObufGetBufptr(port->_linkedPort) : NULL;
}

size_t flentiopGetBufSize(flentIOport* port){
    _flentiopPerformDefaultPreReadChecks(port, "flentiopGetBufSize", 0)

    return port->_linkedPort? _flentiopObufGetBufSize(port->_linkedPort) : 0;
}

flentiopDtype_t flentiopGetDataType(flentIOport* port){
    _flentiopPerformDefaultPreReadChecks(port, "flentiopGetDataType", flentiopDTYPE_NIL)

    return port->_linkedPort? _flentiopObufGetDataType(port->_linkedPort) : flentiopDTYPE_NIL;
}

void* flentiopGetData(flentIOport* port){
    _flentiopPerformDefaultPreReadChecks(port, "flentiopGetData", NULL)

    return port->_linkedPort? _flentiopObufGetData(port->_linkedPort) : NULL; 
}

size_t flentiopGetDataSize(flentIOport* port){
    _flentiopPerformDefaultPreReadChecks(port, "flentiopGetDataSize", 0)

    return port->_linkedPort? _flentiopObufGetDataSize(port->_linkedPort) : 0;
}

#define _flentiopPerformDefaultPreOutputReadChecks(port, operNameStr, returnVal)\
    if(port->isBusy || port->type == flentiopTYPE_INPUT){\
        const char* adjstr = port->isBusy? "busy" : "input";\
        flentiopHandleInvalidOperation(port, FLSTR(operNameStr), FLSTR(adjstr));\
        return returnVal;\
    }

flentiopDtype_t flentiopGetOutputDataType(flentIOport* port){
    _flentiopPerformDefaultPreOutputReadChecks(port, "flentiopGetOutputDataType", flentiopDTYPE_NIL)

    return _flentiopObufGetDataType(port);
}

void* flentiopGetOutputData(flentIOport* port){
    _flentiopPerformDefaultPreOutputReadChecks(port, "flentiopGetOutputData", NULL)

    return _flentiopObufGetData(port);
}

size_t flentiopGetOutputDataSize(flentIOport* port){
    _flentiopPerformDefaultPreOutputReadChecks(port, "flentiopGetOutputDataSize", 0)

    return _flentiopObufGetDataSize(port);
}

#undef _flentiopClear
#undef _flentiopOmInitLports
#undef _flentiopIsComp
#undef _flentiopPerformDefaultPreWriteOps
#undef _flentiopPerformDefaultPreReadChecks
#undef _flentiopPerformDefaultPreOutputReadChecks