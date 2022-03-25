
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

#undef _flentInitializeIOports