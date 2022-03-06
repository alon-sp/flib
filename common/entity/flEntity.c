
#include"../fl.h"

#include"flEntity.h"

flEntity* flentNew(flentCC_t ccode, flEntity* controller, int initialCompCount){
    flEntity* ent = flmemMalloc(sizeof(flEntity));
    if(!ent){
        flerrHandle("\nMEMf flentNew");
        return NULL;
    }

    _flentSetCcode(ent, ccode);

    //Create two $flArray with initial capacity large enough to be able to
    //store the data mode and ID and a pointer
    flArray* cinArr = flarrNew(sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*), sizeof(uint8_t));
    flArray* coutArr = flarrNew(sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*), sizeof(uint8_t));

    _flentSetCin(ent, cinArr);
    _flentSetCout(ent, coutArr);

    _flentSetUi2D(ent, NULL);
    _flentSetUi3D(ent, NULL);

    if(initialCompCount){
        _flentSetComponents(ent, flarrNew(initialCompCount, sizeof(flEntity*)));
    }

    flentSetProps(ent, NULL);
    flentSetTick(ent, NULL);

    flentAddCompPtr(controller, ent);

    return ent;
}

const flEntity** flentAddCompPtr(flEntity* controller, const flEntity* compPtr){
    if( !(controller && compPtr) ) return false;

    if(!controller->components){
        _flentSetComponents(controller, flarrNew(2, sizeof(flEntity*)));
    }

    //Check for existance of the component to be added in the controller and any available free slot.
    int freeSlotIndex = -1;
    for(int i = 0; i<controller->components->length; i++){
        const flEntity** eptr = (flEntity**)flarrGet(controller->components, i);
        if(*eptr == compPtr) return eptr;
        else if(!*eptr && freeSlotIndex < 0) freeSlotIndex = i;
    }

    //Link component with controller
    _flentSetCon(compPtr, controller);

    if(freeSlotIndex >= 0) return flarrPut(controller->components, freeSlotIndex, compPtr);
    
    return flarrPush(controller->components, compPtr);
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