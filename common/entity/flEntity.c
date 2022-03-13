
#include"../fl.h"

#include"flEntity.h"

bool flentSetUi2D(flEntity* entP, flEntity* ui2dP){
    if(entP->ui2D) return false;

    flEntity** ePP = (flEntity**)flentAddCompPtr(entP, ui2dP);
    _flentSetUi2D(entP, ePP? *ePP : NULL);

    return ePP? true : false;
}

bool flentSetUi3D(flEntity* entP, flEntity* ui3dP){
    if(entP->ui3D) return false;

    flEntity** ePP = (flEntity**)flentAddCompPtr(entP, ui3dP);
    _flentSetUi3D(entP, ePP? *ePP : NULL);

    return ePP? true : false;
}

#define _flentdioReportError(e1P, e2P, funcName){\
        flArray* errBuf = flarrNew(64+sizeof(flentCC_t)*4, sizeof(char));\
        flarrstrPush(errBuf, "\nACCv ");\
        flarrstrPush(errBuf, funcName);\
        flarrstrPush(errBuf, " : ");\
        flentccoToStr(e1P->ccode, errBuf);\
        flarrPush(errBuf, " ");\
        flentccoToStr(e2P->ccode, errBuf);\
\
        flerrHandle(flarrstrCstr(errBuf));\
\
        flarrFree(errBuf);\
}

void flentWriteToComponentOutput(flEntity* contP, flEntity* compP, int8_t dataMode, flentDataID_t dataId, void* data, flInt_t dataSize){
    if(compP->_con != contP){
        _flentdioReportError(contP, compP, "flentWriteToComponentOutput")
    }

    flarrSetLength(compP->_cin, 0);

    flarrPush(compP->_cin, &dataMode);
    flarrPushs(compP->_cin, &dataId, sizeof(flentDataID_t));
    flarrPushs(compP->_cin, data, dataSize);
}

void flentWriteToControllerOutput(flEntity* compP, flEntity* contP, int8_t dataMode, flentDataID_t dataId, void* data, flInt_t dataSize){
    if(compP->_con != contP){
        _flentdioReportError(compP, contP, "flentWriteToControllerOutput")
    }

    flarrSetLength(compP->_cout, 0);

    flarrPush(compP->_cout, &dataMode);
    flarrPushs(compP->_cout, &dataId, sizeof(flentDataID_t));
    flarrPushs(compP->_cout, data, dataSize);

}

void flentReadFromComponentOutput(flEntity* contP, flEntity* compP, int8_t* dataModeP, flentDataID_t* dataIdP, void** dataP, flInt_t* dataSizeP){
    
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
        const flEntity** entPP = (const flEntity**)flarrGet( (flArray*)contP->components, i);
        if(*entPP == compP) return entPP;
        else if(!*entPP && freeSlotIndex < 0) freeSlotIndex = i;
    }

    //Link component with controller
    _flentSetCon(compP, contP);

    if(freeSlotIndex >= 0) return flarrPut( (flArray*)contP->components, freeSlotIndex, compP);
    
    return flarrPush( (flArray*)contP->components, compP);
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
            //Push all components of the current controller entity($centP) to the stack
            ///@note It's the responsibility of the caller to ensure that components entities will
            ///still be valid after applying callback to their controller.
            if(centP->components){
                for(flInt_t j = 0; j < centP->components->length; j++){
                    flarrPush(lstack, *(flEntity**)flarrGet( (flArray*)centP->components, j));
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

    if(ent->_cin) flarrFree( (flArray**) &ent->_cin );
    if(ent->_cout) flarrFree( (flArray**)&ent->_cout);

    //At this stage all components' pointers have been pushed to the stack and this function
    //will be called on them later; hence it's safe to destroy this array.
    if(ent->components) flarrFree( (flArray**) &ent->components);

    flmemFree(ent);
}

void flentFree(flEntity** contPP, flArray* lstack){
    flentForeach(*contPP, freeEntity, NULL, lstack);

    *contPP = NULL;
}

void flentccoToStr(flentCC_t ccode, flArray* charArr){
    char numstrBuf[12];
    sprintf(numstrBuf, "%u", ccode);
    flarrstrPush(charArr, numstrBuf);
}