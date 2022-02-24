
#include"../fl.h"

#include"flEntity.h"

flEntity* flentNew(flentCC_t ccode, const flEntity* env){
    flEntity* ent = flmemMalloc(sizeof(flEntity));
    if(!ent){
        flerrHandle("\nMEMf flentNew");
        return NULL;
    }

    _flentSetCcode(ent, ccode);
    _flentSetEnv(ent, env);

    _flentSetUi2D(ent, NULL);
    _flentSetUi3D(ent, NULL);
    _flentSetUinbuf(ent, NULL);
    _flentSetProps(ent, NULL);
    _flentSetEntPtrs(ent, NULL);
    _flentSetTick(ent, NULL);
    _flentSetInput(ent, NULL);

    return ent;
}

void flentForeach(const flEntity* env, void (*funcToApply)(flEntity*, void*), void* funcArgs, flArray* lstack){
    static flArray* _defaultStack = NULL;

    //Check args
    if(!(funcToApply || funcArgs)) return;

    //Initialize stack
    if(!lstack){
        if(!_defaultStack) _defaultStack = flarrNew(16, sizeof(flEntity*));
        lstack = _defaultStack;
    }
    flInt_t iniLstackLength = lstack->length;

    //Apply callback and push children entities to stack
    flarrPush(lstack, env);
    while(lstack->length != iniLstackLength){
        flEntity* cent = (flEntity*)flarrPop(lstack);
        if(cent){
            //Push all children of the current entity($cent) to the stack
            ///@note It's the responsibility of the caller to ensure that children entities will
            ///still be valid after applying callback to parent.
            if(cent->entPtrs){
                for(flInt_t j = 0; j < cent->entPtrs->length; j++){
                    flarrPush(lstack, flarrGet(cent->entPtrs, j));
                }
            }

            ///Apply callback. @note It's important that we push children to stack
            ///before applying callback. This is to prevent us from accessing an invalid
            ///object(entity) which may result from applying the callback.
            if(funcToApply) funcToApply(cent, funcArgs);
            else //Perform default action : tick
                cent->tick( cent, *( ((flInt_t*)funcArgs)+0 ), *( ((flInt_t*)funcArgs)+1 ) );
        }
    }
}

void flentTick(const flEntity* env, flInt_t ct, flInt_t dt, flArray* lstack){
    flInt_t argbuf[2] = {ct, dt};

    flentForeach(env, NULL, argbuf, lstack);
}

static void freeEntity(flEntity* ent, void* otherArgs){
    ent->input(ent, flentimoCLEANUP, flentipnNIL, NULL, NULL);

    if(ent->uinbuf) flarrFree(&ent->uinbuf);
    if(ent->entPtrs) flarrFree(&ent->entPtrs);

    _flentSetUinbuf(ent, NULL);
    _flentSetEntPtrs(ent , NULL);
    _flentSetTick(ent, NULL);
    _flentSetInput(ent, NULL);

    flmemFree(ent);
}

void flentFree(flEntity** env, flArray* lstack){
    flentForeach(*env, freeEntity, NULL, lstack);

    *env = NULL;
}