
#include"flEntity.h"
#include"flentXenv.h"

//--DEFAULT FUNCTIONS--

//Default tick handler
static void flentdfnTick(flEntity* ent, flentXenv* xenv){
    return;
}

//Default system command handler
static void flentdfnHscmd(flentsyc_t cmd, void *args, void* rvalDest){
    return;
}

//--


#define _flentInitializeIOports(ent, portCount) _flentSetIOports(ent, flarrNew(portCount, sizeof(flentIOport*)))

flEntity* flentNew(flentXenv* xenv, int initialPortCount){
    flEntity* ent = flmemMalloc(sizeof(flEntity));

    _flentEnableTick(ent);
    flentDisableAutoTick(ent);
    
    flentSetCcode(ent, 0);

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
    ent->hscmd(flentsycCLEANUP, ent, NULL);

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

flentIOport* flentFindPortByID(flEntity* ent, flentiopID_t portID, flentiopType_t portType){
    if(!ent->ioports) return NULL;

    for(int i = 0; i<ent->ioports->length; i++){
        flentIOport* port = *(flentIOport**)flarrGet(ent->ioports, i);
        if( port && port->id == portID && port->type == portType) return port;
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

        ent->hscmd(flentsycIOPORT_ADD, port, NULL);//Notify target entity of the new port to be added

        flentIOport** ploc = flentFindPort(ent, NULL);//check for any available slots

        if(ploc) *ploc = port;
        else return flarrPush(ent->ioports, &port)? true : false;
    }

    return true;
}

void flentRemovePort(flEntity* ent, flentIOport* port){
    flentIOport** ploc = flentFindPort(ent, port);
    if(ploc){
        ent->hscmd(flentsycIOPORT_REMOVE, *ploc, NULL);//Notify target entity

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
        flentIOport* port = *(flentIOport**)flarrGet(ent->ioports, i);
        if(port && cb) cb(port, cbArgs);
    }
}

void _flentClearAllPortsObuf(flEntity* ent){
    if(!ent->ioports) return;
    
    for(int i = 0; i<ent->ioports->length; i++){
        flentIOport* port = *(flentIOport**)flarrGet(ent->ioports, i);
        if(port->type == flentiopTYPE_OUTPUT && !port->isBusy && 
            _flentiopObufGetDataType(port) != flentiopDTYPE_NIL) flentiopClear(port);
    }   
}

#undef _flentInitializeIOports