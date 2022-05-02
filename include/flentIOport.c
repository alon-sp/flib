#include"flentIOport.h"
#include"fl.h"
#include"flEntity.h"

flentIOport* flentiopNew(flentiopID_t id, flentiopType_t type){
    flentIOport* iop = flmemMalloc(sizeof(flentIOport));

    flArray* obuf = NULL;

    if(type != flentiopTYPE_INPUT){
        obuf = flarrNew(_flentiopOBUF_PRE_DATA_META_SIZE+sizeof(void*), sizeof(flbyt_t)); 
    }

    _flentiopSetObuf(iop, obuf);
    if(obuf){
        //Write default output value
        flentiopClear(iop);
    }

    _flentiopSetIsBusy(iop, false);
    flentiopSetEntity(iop, NULL);
    flentiopSetID(iop, id);
    _flentiopSetlinkedPort(iop, NULL);
    flentiopSetDataTypeCount(iop, -1);
    flentiopSetType(iop, type);
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

//Check whether the given port accepts all data types of the other port($portO)
static bool _flentiopAcceptAllDtype(flentIOport* port, flentIOport* portO){
    if(!port->dataTypeCount) return true;

    bool opStatus = false;

    if(port->entity && portO->entity){
        for(flentiopDataType_t i = 1; i <= portO->dataTypeCount; i++){
            flentsycEntIoportNthDtypeArg dtypeArg = {.port = portO, .n = i};
            flentiopDtype_t dtype = flentiopDTYPE_NIL;
            portO->entity->hscmd(flentsycgetENT_IOPORT_NTH_DTYPE, &dtypeArg, &dtype);

            bool acceptStatus = false;
            flentsycEntIoportAcceptDtypeArg acceptArg = {.port = port, .dtype = dtype};
            port->entity->hscmd(flentsycgetENT_IOPORT_ACCEPT_DTYPE, &acceptArg, &acceptStatus);
            if(!acceptStatus){
                opStatus = false;
                break;
            }
            opStatus = true;
        }
    }

    return opStatus;
}

static bool flentiopIscomp(flentIOport* pA, flentIOport* pB){
    if( pA->type == pB->type ) return false;

    if(pA->dataTypeCount < 0 || pB->dataTypeCount < 0) return true;

    return pA->type == flentiopTYPE_INPUT? 
        _flentiopAcceptAllDtype(pA, pB) : _flentiopAcceptAllDtype(pB, pA) ;
}

bool flentiopLink(flentIOport* port1, flentIOport* port2){
    if( !flentiopIscomp(port1, port2) ){
        char* port1Name = FLSTR("");
        if(port1->entity) port1->entity->hscmd(flentsycgetIOPORT_NAME, port1, &port1Name);
        char* port2Name = FLSTR("");
        if(port2->entity) port2->entity->hscmd(flentsycgetIOPORT_NAME, port2, &port2Name);

        flArray* errLog = flarrNew(strlen(port1Name)+strlen(port2Name)+16, sizeof(char));

        flerrHandle(flarrstrPushs(errLog, 4, FLSTR("ICMport: "), port1Name, FLSTR(" !-> "), port2Name));

        flarrFree(errLog);

        return false;
    }

    if(port1)_flentiopSetlinkedPort(port1, port2);
    if(port2)_flentiopSetlinkedPort(port2, port1);

    return true;
}

flentIOport* flentiopUnlink(flentIOport* iop){
    if( !(iop && iop->_linkedPort) ) return NULL;

    flentIOport* linkedPort = iop->_linkedPort;

    _flentiopSetlinkedPort(linkedPort, NULL);
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
    flerrHandle(flarrstrPushs(errLog, 6, FLSTR("INVop:"), operNameStr, FLSTR(" on ") , portAdjStr ,FLSTR(" port: "), portName));
    flarrFree(errLog);
}

//--Functions and micros for writing to port--
//--------------------------------------------

#define _flentiopPerformDefaultPreWriteOps(iop, callerNameStr)\
  if( (iop)->type == flentiopTYPE_INPUT || (iop)->isBusy){\
      const char* adjStr = (iop)->isBusy? FLSTR("busy") : FLSTR("input");\
      flentiopHandleInvalidOperation(iop, callerNameStr, adjStr);\
      return;\
  }\
  if((iop)->_linkedPort && (iop)->_linkedPort->entity) _flentEnableTick((iop)->_linkedPort->entity)

void flentiopPut(flentIOport* port, flentiopDtype_t dtype, const void* dataPtr, size_t dataSize){
    _flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopPut"));

    _flarrSetLength(port->_obuf, _flentiopOBUF_DATA_INDEX+dataSize);
    _flentiopObufSetDataType(port, dtype);
    _flarrPuts(port->_obuf, _flentiopOBUF_DATA_INDEX, dataPtr, dataSize);
}

void flentiopPutb(flentIOport* port, const void* bytesPtr, size_t bytesSize){
    _flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopPutb"));

    _flarrSetLength(port->_obuf, _flentiopOBUF_DATA_INDEX+bytesSize);
    _flarrPuts(port->_obuf, 0, bytesPtr, bytesSize);
}

void flentiopClear(flentIOport* port){
    _flentiopPerformDefaultPreWriteOps(port, FLSTR("flentiopClear"));

    _flarrSetLength(port->_obuf, _flentiopOBUF_DTYPE_INDEX+sizeof(flentiopDtype_t));
    _flentiopObufSetDataType(port, flentiopDTYPE_NIL);
}

void flentiopSetIsBusy(flentIOport* port, bool bval){
    if(port->type == flentiopTYPE_OUTPUT){
        flentiopHandleInvalidOperation(port, "flentiopSetIsBusy", "output");
    }
    _flentiopSetIsBusy(port, bval);
    if( port->_linkedPort){
        _flentiopSetIsBusy(port->_linkedPort, bval);
        if(port->_linkedPort->entity) _flentEnableTick(port->_linkedPort->entity);
    }
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

#undef _flentiopIsComp
#undef _flentiopPerformDefaultPreWriteOps