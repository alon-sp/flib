
#include"fl.h"
#include"flentXenv.h"
#include"flEntity.h"

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
        if(!ent) continue;

        _flentClearAllPortsObuf(ent);
        if(ent->_tick || ent->autoTick){
            ent->tick(ent, xenv);
            _flentDisableTick(ent);
        }
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