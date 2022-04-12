
#include"fl.h"

#include"flEntity.h"

/*----------flentIOdata functions----------*/

void flentiodPuts(flentIOdata* iod, flentdmo_t mode, flentdid_t id, void* dataPtr, flint_t dataSize){
    flentiodPutMode(iod, mode);
    flentiodPutID(iod, id);
    flentiodPutData(iod, dataPtr, dataSize);
}

/*----------flentIOport functions----------*/

flentIOport* flentiopNew(flentipn_t ipname, flentIOport* targetPort, bool hasOutputBuffer){
    flentIOport* iop = flmemMalloc(sizeof(flentIOport));

    flentIOdata* outputBuf = NULL;
    if(hasOutputBuffer){
        outputBuf = flentiodNew(flentiodDATA_BUFFER_MIN_SIZE, sizeof(flbyt_t)); 
    }

    if(outputBuf){
        //Write default output value
        flentiodClear(outputBuf, flentdmoNIL);
    }

    _flentiopSetObuf(iop, outputBuf);

    flentiopSetName(iop, ipname);
    flentiopSetEntity(iop, NULL);
    flentiopSetProps(iop, NULL);

    
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

#define _flentInitializeIOports(ent, portCount) _flentSetIOports(ent, flarrNew(portCount, sizeof(flentIOport*)))

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

    if(xenv) flentxevAddEntity(xenv, ent);

    return ent;
}

void flentFree(flEntity* ent, flentXenv* xenv){
    if(!ent) return;

    //Send cleanup command to entity before destroying it.
    ent->hscmd(flentsycCLEANUP, NULL);

    //remove entiy from environment before destroying
    if(xenv) flentxevRemoveEntity(xenv, ent);

    //unlink and free all ports
    if(ent->ioports){
        for(int i = 0; i < ent->ioports->length; i++){
            flentIOport* iop = *( (flentIOport**)flarrGet(ent->ioports, i) );
            flentiopFree(iop);
        }

        //clean up ioports
        flarrFree(ent->ioports);
        _flentSetIOports(ent, NULL);
    }

    //clean up entity
    flmemFree(ent);
}

flentIOport* flentFindPortByName(flEntity* ent, flentipn_t portName){
    if(!ent->ioports) return NULL;

    for(int i = 0; i<ent->ioports->length; i++){
        flentIOport** ploc = (flentIOport**)flarrGet(ent->ioports, i);
        if( *ploc && (*ploc)->name == portName ) return *ploc;
    }

    return NULL;
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
        flentIOport** ploc = (flentIOport**)flarrGet(ent->ioports, i);
        if(*ploc == port) return ploc;
    }
    
    return NULL;
}

bool flentAddPort(flEntity* ent, flentIOport* port){
    if(!port) return false;

    if(!ent->ioports) _flentInitializeIOports(ent, 2);

    if(port) flentiopSetEntity(port, ent);//just incase

    if(!flentFindPort(ent, port)){

        ent->hscmd(flentsycIOPORT_ADD, port);//Notify target entity of the new port to be added

        flentIOport** ploc = flentFindPort(ent, NULL);//check for any available slots

        if(ploc) *ploc = port;
        else return flarrPush(ent->ioports, &port)? true : false;
    }

    return true;
}

void flentRemovePort(flEntity* ent, flentIOport* port){
    flentIOport** ploc = flentFindPort(ent, port);
    if(ploc){
        ent->hscmd(flentsycIOPORT_REMOVE, *ploc);//Notify target entity

        flentiopSetEntity(*ploc, NULL);
        *ploc = NULL;
    }
}

void flentDeletePort(flEntity* ent, flentIOport* port){
    flentRemovePort(ent, port);
    flentiopFree(port);
}

void flentForEachPort(flEntity* ent, flentForEachPortCb_tf cb, void* cbArgs){
    if(!ent->ioports) return;
    
    for(int i = 0; i<ent->ioports->length; i++){
        flentIOport** ploc = (flentIOport**)flarrGet(ent->ioports, i);
        if(*ploc && cb) cb(*ploc, cbArgs);
    }
}

/*----------flentXenv functions---------*/

flentXenv* flentxevNew(flint_t initialEntityCount){
    flentXenv* xenv = flmemMalloc(sizeof(flentXenv));

    if(!initialEntityCount) initialEntityCount = 2;
    _flentxevSetEntities( xenv, flarrNew(initialEntityCount, sizeof(flEntity*) ) );

    _flentxevSetTickCT(xenv, 0);
    _flentxevSetTickDT(xenv, 0);

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

/**
 * @brief Find $ent in $xenv
 * @param xenv
 * @param ent
 * @return The location of $ent in $xenv if found | NULL
 */
static flEntity** flentxevFindEntity(flentXenv* xenv, flEntity* ent){
    for(int i = 0; i < xenv->entities->length; i++){
        flEntity** eloc = (flEntity**)flarrGet(xenv->entities, i);
        if(*eloc == ent) return eloc;
    }

    return NULL;
}

void flentxevTick(flentXenv* xenv, flint_t ct, flint_t dt){
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

bool flentxevAddEntity(flentXenv* xenv, flEntity* ent){
    if(!ent) return false;

    if( !flentxevFindEntity(xenv, ent) ){
        flEntity** eloc = flentxevFindEntity(xenv, NULL);
        if(eloc) *eloc = ent;
        else return flarrPush(xenv->entities, &ent)? true : false;
    }

    return true;
}

void flentxevRemoveEntity(flentXenv* xenv, flEntity* ent){
    flEntity** eloc = flentxevFindEntity(xenv, ent);
    if(eloc){
        *eloc = NULL;
    }
}

#undef _flentInitializeIOports