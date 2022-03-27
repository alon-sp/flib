
#include"../fl.h"

#include"flEntity.h"

/*----------DEFAULT FUNCTIONS---------*/

//Default tick handler
static void flentdfnTick(flEntity* ent){
    return;
}

//Default system command handler
static void* flentdfnHscmd(flentsyc_t cmd, void *args){
    return NULL;
}

/*----------flentIOport functions----------*/

flentIOport* flentiopNew(flentipn_t ipname, flEntity* ent, flentIOport* targetPort){
    flentIOport* iop = flmemMalloc(sizeof(flentIOport));

    flArray* outputBuffer = flarrNew(sizeof(flbyt_t)+sizeof(flentdid_t)+sizeof(void*), sizeof(flbyt_t));
    _flentiopSetObuf(iop, outputBuffer);

    flentiopSetName(iop, ipname);
    flentiopSetEntity(iop, ent);

    
    flentiopLink(iop, targetPort);
}

void flentiopFree(flentIOport* iop){
    if(!iop) return;

    if(iop->_obuf){
        flarrFree(iop->_obuf);
        _flentiopSetObuf(iop, NULL);
    }

    flmemFree(iop);
}

void flentiopLink(flentIOport* port1, flentIOport* port2){
    if(port1)_flentiopSetlinkedPort(port1, port2);
    if(port2)_flentiopSetlinkedPort(port2, port1);
}

flentIOport* flentiopUnlink(flentIOport* iop){
    if( !(iop && iop->_linkedPort) ) return NULL;

    flentIOport* linkedPort = iop->_linkedPort;

    _flentiopSetlinkedPort(linkedPort, NULL);
    _flentiopSetlinkedPort(iop, NULL);

    return linkedPort;
}

flentIOdata flentiodNew(int8_t dataMode, flentdid_t dataId, void* data, flint_t dataSize){
    flentIOdata iod = {.mode = dataMode, .id = dataId, .data = data, .size = dataSize};
    return iod;
}

void flentiodEncode(flentIOdata iod, flArray* destArrbuf){

    flarrSetLength( destArrbuf, 0 );

    flarrPush( destArrbuf, &iod.mode);
    flarrPushs( destArrbuf, &iod.id, sizeof(flentdid_t) );

    flbyt_t defaultData = 0;
    if(!iod.data){
        iod.data = &defaultData;
        iod.size = sizeof(flbyt_t);
    }
    flarrPushs( destArrbuf, iod.data, iod.size );

}

flentIOdata flentiodDecode(flArray* arrBuf){
    flentIOdata iod;

    flbyt_t* byteBuffer = (flbyt_t*)flarrGet( arrBuf, 0);

    iod.mode = *byteBuffer;
    iod.id = *( (flentdid_t*)(byteBuffer+sizeof(flbyt_t)) );
    iod.data = byteBuffer+sizeof(flbyt_t)+sizeof(flentdid_t);
    iod.size = arrBuf->length - ( sizeof(flbyt_t)+sizeof(flentdid_t) );

    return iod;
}

/*----------flEntity functions---------*/

bool flentSetName(flEntity* ent, const char* namestr){
    if(!namestr) namestr = "";

    char* destBuf = flmemRealloc( (char*)ent->name, ( strlen(namestr)+1 )*sizeof(char) );
    if(!destBuf){
        flerrHandle("\nMEMf flentSetName");
        return false;
    }

    strcpy(destBuf, namestr);
    _flentSetName(ent, destBuf);
    return true;
}

#define _flentInitializeIOports(ent, portCount) _flentSetIOports(ent, flarrNew(portCount, sizeof(flentIOport)))

flEntity* flentNew(flentXenv* xenv, flentcco_t ccode, int initialPortCount){
    flEntity* ent = flmemMalloc(sizeof(flEntity));

    flentEnableTick(ent);
    
    flentSetCcode(ent, ccode);
    _flentSetName(ent, NULL);

    flentSetProps(ent, NULL);

    if(initialPortCount){
        _flentInitializeIOports(ent, initialPortCount);
    }else _flentSetIOports(ent, NULL);

    flentSetTick(ent, flentdfnTick);

    flentSetHscmd(ent, flentdfnHscmd);

    _flentSetXenv(ent, xenv);
    if(xenv) xenv->addEntity(xenv, ent);
}

/**
 * @brief Search for $port in $ent
 * @param ent 
 * @param port 
 * @return pointer to $port in the given entity($ent) | NULL if $port was not found in $ent
 * @note if $port is NULL, the location of the first NULL port in $ent will be returned | NULL
 */
static flentIOport** flentFindPort(flEntity* ent, flentIOport* port){
    if(!ent->ioports) return NULL;
    
    for(int i = 0; i<ent->ioports->length; i++){
        flentIOport** p = (flentIOport**)flarrGet(ent->ioports, i);
        if(*p == port) return p;
    }
    
    return NULL;
}

bool flentAddPort(flEntity* ent, flentIOport* port){
    if(!ent->ioports) _flentInitializeIOports(ent, 2);
    
    flentIOport** ploc = flentFindPort(ent, port);

    if(ploc) *ploc = port;
    else return flarrPush(ent->ioports, port)? true : false;

    return true;
}

void flentRemovePort(flEntity* ent, flentIOport* port){
    flentIOport** ploc = flentFindPort(ent, port);
    if(ploc){
        flentiopSetEntity(*ploc, NULL);
        *ploc = NULL;
    }
}

void flentDeletePort(flEntity* ent, flentIOport* port){
    flentRemovePort(ent, port);
    flentiopFree(port);
}

#undef _flentInitializeIOports