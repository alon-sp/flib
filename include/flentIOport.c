#include"flentIOport.h"
#include"fl.h"
#include"flEntity.h"

static bool _flentiopOmSetIsBusy(flentIOport* port, bool bval);

#define _flentiopClear(port) do{\
    _flarrSetLength(port->_obuf, _flentiopOBUF_DTYPE_INDEX+sizeof(flentiopDtype_t));\
    _flentiopObufSetDataType(port, flentiopDTYPE_NIL);\
}while(0)

struct omLinkedPort{
    flArray* ports;//An array of pointers to all linked ports

    //This is the port responsible for activating the lastest busy status of the 
    //target flentiopTYPE_OM type port.
    flentIOport* busyPort;
    #define _flentiopOmSetBusyPort(omPort, bport)\
        ((struct omLinkedPort*)omPort->_linkedPort)->busyPort = bport

    #define _flentiopOmGetBusyPort(omPort) ((struct omLinkedPort*)omPort->_linkedPort)->busyPort
};

static void _flentiopOmInitLport(flentIOport* omPort){
    struct omLinkedPort* omLport = flmemMalloc(sizeof(struct omLinkedPort));
    omLport->ports = flarrNew(2, sizeof(flentIOport*));
    omLport->busyPort = NULL;
    _flentiopSetlinkedPort(omPort, /**@noted*/(void*)omLport );
}

#define _flentiopOmFreeLport(omPort) do{\
    if(omPort->_linkedPort) flmemFree( (struct omLinkedPort*)omPort->_linkedPort );\
    _flentiopSetlinkedPort(omPort, NULL);\
}while(0)

#define _flentiopOmGetPorts(omPort) ( ((struct omLinkedPort*)(omPort)->_linkedPort)->ports )

#define _flentiopOmGetPortsLen(omPort) (  _flentiopOmGetPorts(omPort)->length  )

#define _flentiopOmGetPortLocAt(omPort, index)\
    (flentIOport**)_flarrGet( _flentiopOmGetPorts(omPort), index)

#define _flentiopOmGetPortAt(omPort, index) *_flentiopOmGetPortLocAt(omPort, index)

static flentIOport** flentiopOmFindPort(flentIOport* omPort, flentIOport* inPort){
    for(int i = 0; i < _flentiopOmGetPortsLen(omPort); i++){
        flentIOport** ploc = _flentiopOmGetPortLocAt(omPort, i);
        if(*ploc == inPort) return ploc;
    }

    return NULL;
}

static void flentiopOmAddPort(flentIOport* omPort, flentIOport* inPort){
    if(!flentiopOmFindPort(omPort, inPort)){
        flentIOport** nullLoc = flentiopOmFindPort(omPort, NULL);
        if(nullLoc) *nullLoc = inPort;
        else flarrPush(_flentiopOmGetPorts(omPort), &inPort);
    }
}

static void flentiopOmRemovePort(flentIOport* omPort, flentIOport* inPort){
    flentIOport** ploc = flentiopOmFindPort(omPort, inPort);

    if(ploc && *ploc == _flentiopOmGetBusyPort(omPort)) _flentiopOmSetIsBusy(*ploc, false);

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

    _flentiopSetType(iop, type);
    _flentiopSetlinkedPort(iop, NULL);
    if(type == flentiopTYPE_OM) _flentiopOmInitLport(iop);

    _flentiopSetIsBusy(iop, false);
    flentiopSetEntity(iop, NULL);
    flentiopSetID(iop, id);
    flentiopSetDataTypeCount(iop, dataTypeCount);
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

    if(iop->type == flentiopTYPE_OM) _flentiopOmFreeLport(iop);

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

    if(inPort->_linkedPort) flentiopUnlink(inPort);
    _flentiopSetlinkedPort(inPort, outPort);
    
    if(outPort->type == flentiopTYPE_OUTPUT && !outPort->_linkedPort){
        _flentiopSetlinkedPort(outPort, inPort);

    }else if(outPort->type == flentiopTYPE_OUTPUT && outPort->_linkedPort != inPort){
        //upgrade $outPort to flentiopTYPE_OM type port
        flentIOport* existingLinkedPort = outPort->_linkedPort;
        _flentiopSetType(outPort, flentiopTYPE_OM);
        _flentiopOmInitLport(outPort);

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
    
    if(iop->type == flentiopTYPE_OM){
        for(int i = 0; i < _flentiopOmGetPortsLen(iop); i++){
            flentiopUnlink(_flentiopOmGetPortAt(iop, i));
        }
        return NULL;
    }

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
        for(int i = 0; i < _flentiopOmGetPortsLen(iop); i++){
            flentIOport* lp = _flentiopOmGetPortAt(iop, i);
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

    if(_flentiopOBUF_DATA_INDEX+dataSize != port->_obuf->length){
        flarrSetLength(port->_obuf, _flentiopOBUF_DATA_INDEX+dataSize);
    }
    _flentiopObufSetDataType(port, dtype);
    _flarrPuts(port->_obuf, _flentiopOBUF_DATA_INDEX, dataPtr, dataSize);
}

void flentiopPutb(flentIOport* port, const void* bytesPtr, size_t bytesSize){
    if( 
        !_flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopPutb"), 
        *(flentiopDtype_t*)bytesPtr, (char*)bytesPtr + sizeof(flentiopDtype_t)) ) return;

    if(bytesSize != port->_obuf->length) flarrSetLength(port->_obuf, bytesSize);
    _flarrPuts(port->_obuf, 0, bytesPtr, bytesSize);
}

void flentiopClear(flentIOport* port){
    if( !_flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopClear"), flentiopDTYPE_NIL, NULL) ){
        return;
    }

    _flentiopClear(port);
}

bool _flentiopOmSetIsBusy(flentIOport* port, bool bval){
    flentIOport* omPort = port->_linkedPort;
    if(bval && !omPort->isBusy){

        _flentiopOmSetBusyPort(omPort, port);
        _flentiopSetIsBusy(omPort, bval);

        //Mark all linked ports of this OM port as busy. This will prevent them from
        //processing the same input data multiple times
        for(int i = 0; i < _flentiopOmGetPortsLen(omPort); i++){
            flentIOport* lp = _flentiopOmGetPortAt(omPort, i);
            if(lp) _flentiopSetIsBusy(lp, bval);
        }
    }else if(!bval && omPort->isBusy){
        if(port != _flentiopOmGetBusyPort(omPort)) 
            return false;//The busy status should only be turn off by the same port that activated it

        _flentiopOmSetBusyPort(omPort, NULL);
        _flentiopSetIsBusy(omPort, bval);

        //Disable the busy status of all linked ports of this OM port and enable their
        //corresponding tick flags.
        for(int i = 0; i < _flentiopOmGetPortsLen(omPort); i++){
            flentIOport* lp = _flentiopOmGetPortAt(omPort, i);
            if(lp)_flentiopSetIsBusy(lp, bval);
            if(lp && lp->entity) _flentEnableTick(lp->entity);
        }
    }

    return true;
}

bool flentiopSetIsBusy(flentIOport* port, bool bval){
    if(flentiopTypeToBasicType(port->type) == flentiopTYPE_OUTPUT){
        flentiopHandleInvalidOperation(port, "flentiopSetIsBusy", "output");
    }

    if(port->isBusy == bval) return true;
    
    if(port->_linkedPort && port->_linkedPort->type == flentiopTYPE_OM){
        return _flentiopOmSetIsBusy(port, bval);
    }else{
        _flentiopSetIsBusy(port, bval);
        if( port->_linkedPort){
            _flentiopSetIsBusy(port->_linkedPort, bval);
            if(!bval && port->_linkedPort->entity) _flentEnableTick(port->_linkedPort->entity);
        }
    }

    return true;
}

//--Functions and micros for reading from port--
//----------------------------------------------
#define _flentiopPerformDefaultPreReadChecks(port, operNameStr, returnVal)\
    if(port->isBusy || flentiopTypeToBasicType(port->type) == flentiopTYPE_OUTPUT){\
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
#undef _flentiopOmSetBusyPort
#undef _flentiopOmGetBusyPort
#undef _flentiopOmFreeLport
#undef _flentiopOmGetPorts
#undef _flentiopOmGetPortsLen
#undef _flentiopOmGetPortLocAt
#undef _flentiopOmGetPortAt
#undef _flentiopIsComp
#undef _flentiopPerformDefaultPreWriteOps
#undef _flentiopPerformDefaultPreReadChecks
#undef _flentiopPerformDefaultPreOutputReadChecks