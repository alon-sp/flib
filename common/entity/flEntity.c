
#include"../fl.h"

#include"flEntity.h"

bool flentSetUi2D(flEntity* entP, flEntity* ui2dP){
    if(entP->ui2D) return false;

    flEntity** ePP = flentAddCompPtr(entP, ui2dP);
    _flentSetUi2D(entP, ePP? *ePP : NULL);

    return ePP? true : false;
}

bool flentSetUi3D(flEntity* entP, flEntity* ui3dP){
    if(entP->ui3D) return false;

    flEntity** ePP = flentAddCompPtr(entP, ui3dP);
    _flentSetUi3D(entP, ePP? *ePP : NULL);

    return ePP? true : false;
}

flEntity* flentNew(flentCC_t ccode, flEntity* contP, int initialCompCount){
    flEntity* entP = flmemMalloc(sizeof(flEntity));
    if(!entP){
        flerrHandle("\nMEMf flentNew");
        return NULL;
    }

    _flentSetCcode(entP, ccode);

    //Create two $flArray with initial capacity large enough to be able to
    //store the data mode and ID and a pointer
    flArray* cinArr = flarrNew(sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*), sizeof(uint8_t));
    flArray* coutArr = flarrNew(sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*), sizeof(uint8_t));

    _flentSetCin(entP, cinArr);
    _flentSetCout(entP, coutArr);

    _flentSetUi2D(entP, NULL);
    _flentSetUi3D(entP, NULL);

    if(initialCompCount){
        _flentSetComponents(entP, flarrNew(initialCompCount, sizeof(flEntity*)));
    }else{ _flentSetComponents(entP, NULL); }

    flentSetProps(entP, NULL);
    flentSetTick(entP, NULL);

    flentAddCompPtr(contP, entP);

    return entP;
}

const flEntity** flentAddCompPtr(flEntity* contP, const flEntity* compP){
    if( !(contP && compP) ) return NULL;

    if(!contP->components){
        _flentSetComponents(contP, flarrNew(2, sizeof(flEntity*)));
    }

    //Check for existance of the component to be added in the controller and any available free slot.
    int freeSlotIndex = -1;
    for(int i = 0; i<contP->components->length; i++){
        const flEntity** entPP = (flEntity**)flarrGet(contP->components, i);
        if(*entPP == compP) return entPP;
        else if(!*entPP && freeSlotIndex < 0) freeSlotIndex = i;
    }

    //Link component with controller
    _flentSetCon(compP, contP);

    if(freeSlotIndex >= 0) return flarrPut(contP->components, freeSlotIndex, compP);
    
    return flarrPush(contP->components, compP);
}

// void flentRemoveCompPtr(flEntity* contP, const flEntity* compP){
//     if( !(contP && compP) ) return;

//     for(int i = 0; i<contP->components->length; i++){
//         const flEntity** entPP = (flEntity**)flarrGet(contP->components, i);
//         if(*entPP == compP){
//             flarrPut(contP->components, entPP - (flEntity**)flarrGet(contP->components, 0), NULL);
//         }
//     }
// }

void flentForeach(const flEntity* contP, void (*funcToApply)(flEntity*, void*), void* funcArgs, flArray* lstack){
    static flArray* _defaultStack = NULL;

    //Check args
    if(!(funcToApply || funcArgs)) return;

    //Initialize stack
    if(!lstack){
        if(!_defaultStack) _defaultStack = flarrNew(16, sizeof(flEntity*));
        lstack = _defaultStack;
    }
    flInt_t iniLstackLength = lstack->length;

    //Loop through all entities and apply callback
    flarrPush(lstack, contP);
    while(lstack->length != iniLstackLength){
        flEntity* centP = *(flEntity**)flarrPop(lstack);//centP -> controller entity pointer
        if(centP){
            //Push all components of the current controller entity($cent) to the stack
            ///@note It's the responsibility of the caller to ensure that components entities will
            ///still be valid after applying callback to their controller.
            if(centP->components){
                for(flInt_t j = 0; j < centP->components->length; j++){
                    flarrPush(lstack, *(flEntity**)flarrGet(centP->components, j));
                }
            }

            ///Apply callback. @note It's important that we push components to stack
            ///before applying callback. This is to prevent us from accessing an invalid
            ///object(controller entity) which may result from applying the callback.
            if(funcToApply) funcToApply(centP, funcArgs);
            else{ //Perform default action : apply tick method
                flEntityTickMethodArg* tmargP = (flEntityTickMethodArg*) funcArgs;
                centP->tick( centP, tmargP->ct, tmargP->dt, tmargP->syscmd, tmargP->syscmdArgs);
            }
        }
    }
}

void flentTick(const flEntity* contP, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs, flArray* lstack){
    flEntityTickMethodArg tmarg = {.self = NULL, .ct = ct, .dt = dt, .syscmd = syscmd, .syscmdArgs = syscmdArgs};

    flentForeach(contP, NULL, &tmarg, NULL);
}

static void freeEntity(flEntity* ent, void* otherArgs){
    ent->tick(ent, 0, 0, flentsycCLEANUP, NULL);

    ///@note $ent->ui2D and $ent->ui3D are components of the current entity($ent) and hence
    ///this function will be called on them later.
    
    ///@note At this stage $ent->_con no longer exist(ie this function has been called on it)

    if(ent->_cin) flarrFree(&ent->_cin);
    if(ent->_cout) flarrFree(&ent->_cout);

    //At this stage all components' pointers have been pushed to the stack and this function
    //will be called on them later; hence it's safe to destroy this array.
    if(ent->components) flarrFree(&ent->components);

    flmemFree(ent);
}

void flentFree(flEntity** contPP, flArray* lstack){
    flentForeach(*contPP, freeEntity, NULL, lstack);

    *contPP = NULL;
}