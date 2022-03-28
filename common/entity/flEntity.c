
#include"../fl.h"

#include"flEntity.h"

/*----------flentIOport functions----------*/

flentIOport* flentiopNew(flentipn_t ipname, flEntity* ent, flentIOport* targetPort){
    flentIOport* iop = flmemMalloc(sizeof(flentIOport));

    flArray* outputBuffer = flarrNew(sizeof(flbyt_t)+sizeof(flentdid_t)+sizeof(void*), sizeof(flbyt_t));
    _flentiopSetObuf(iop, outputBuffer);

    flentiopSetName(iop, ipname);
    flentiopSetEntity(iop, ent);

    
    flentiopLink(iop, targetPort);

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

//--DEFAULT FUNCTIONS--

//Default tick handler
static void flentdfnTick(flEntity* ent, flentXenv* xenv){
    return;
}

//Default system command handler
static void* flentdfnHscmd(flentsyc_t cmd, void *args){
    return NULL;
}

//--

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

    if(xenv) xenv->addEntity(xenv, ent);

    return ent;
}

void flentFree(flEntity* ent, flentXenv* xenv){
    //remove entiy from environment before destroying
    if(xenv) xenv->removeEntity(xenv, ent);

    //unlink and free all ports
    if(ent->ioports){
        for(int i = 0; i < ent->ioports->length; i++){
            flentIOport* iop = *( (flentIOport**)flarrGet(ent->ioports, i) );
            flentiopFree(iop);
        }

        //clean up ioports
        flarrFree(ent->ioports);
    }

    //clean up entity
    flmemFree(ent);
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

    if(port) flentiopSetEntity(port, ent);//just incase

    if(!flentFindPort(ent, port)){
        flentIOport** ploc = flentFindPort(ent, NULL);//check for any available slots

        if(ploc) *ploc = port;
        else return flarrPush(ent->ioports, port)? true : false;
    }

    return true;
}

void flentRemovePort(flEntity* ent, flentIOport* port){
    flentIOport** ploc = flentFindPort(ent, port);
    if(ploc){
        flentiopUnlink(*ploc);//This operation is necessary because a port without an entity is invalid.
        flentiopSetEntity(*ploc, NULL);
        *ploc = NULL;
    }
}

void flentDeletePort(flEntity* ent, flentIOport* port){
    flentRemovePort(ent, port);
    flentiopFree(port);
}

/*----------flentXenv functions---------*/

//--DEFAULT FUNCTIONS--

static void flentxevdfnTick(flentXenv* xenv, flint_t ct, flint_t dt){
    if(ct || dt){
        _flentxevSetTickCT(xenv, ct);
        _flentxevSetTickDT(xenv, dt);
    }else{
        flint_t prevTime = xenv->tickCT;
        _flentxevSetTickCT(xenv, flMillis());
        _flentxevSetTickDT(xenv, xenv->tickCT - prevTime);
    }

    for(int i = 0; i < xenv->entities->length; i++){
        flEntity* ent = *( (flEntity**)flarrGet(xenv->entities, i) );
        if(ent && ent->tickEnable) ent->tick(ent, xenv);
    }
}

static flEntity** flentxevdfnFindEntity(flentXenv* xenv, flEntity* ent){
    for(int i = 0; i < xenv->entities->length; i++){
        flEntity** eloc = (flEntity**)flarrGet(xenv->entities, i);
        if(*eloc == ent) return eloc;
    }

    return NULL;
}

static bool flentxevdfnAddEntity(flentXenv* xenv, flEntity* ent){
    if( !xenv->_findEntity(xenv, ent) ){
        flEntity** eloc = xenv->_findEntity(xenv, NULL);
        if(eloc) *eloc = ent;
        else return flarrPush(xenv->entities, ent)? true : false;
    }

    return true;
}

static void flentxevdfnRemoveEntity(flentXenv* xenv, flEntity* ent){
    flEntity** eloc = xenv->_findEntity(xenv, ent);
    if(eloc){
        *eloc = NULL;
    }
}

flentXenv* flentxevNew(flint_t initialEntityCount){
    flentXenv* xenv = flmemMalloc(sizeof(flentXenv));

    if(!initialEntityCount) initialEntityCount = 2;
    _flentxevSetEntities( xenv, flarrNew(initialEntityCount, sizeof(flentXenv*) ) );

    _flentxevSetTickCT(xenv, 0);
    _flentxevSetTickDT(xenv, 0);

    _flentxevSetTick(xenv, flentxevdfnTick);
    _flentxevSetFindEntity(xenv, flentxevdfnFindEntity);
    _flentxevSetAddEntity(xenv, flentxevdfnAddEntity);
    _flentxevSetRemoveEntity(xenv, flentxevdfnRemoveEntity);

    return xenv;
}

void flentxevFree(flentXenv* xenv, bool freeEnts){
    if(!xenv) return;

    if(freeEnts){
        for(int i = 0; i < xenv->entities->length; i++){
            flentFree( *( (flEntity**)flarrGet(xenv->entities, i) ), xenv );
        }
    }

    flarrFree(xenv->entities);
    _flentxevSetEntities(xenv, NULL);

    flmemFree(xenv);
}

#undef _flentInitializeIOports