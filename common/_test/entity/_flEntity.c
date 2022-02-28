
#include"_flEntity.h"

static bool _test_flentNew(){
    flentCC_t cc = 1;
    flEntity env, *envPtr = &env;
    
    flEntity* ent = flentNew(cc, envPtr);

    if(ent->ccode != cc || ent->env != envPtr || ent->ui2D || ent->ui3D || ent->uinbuf
        || ent->props || ent->entPtrs || ent->tick || ent->input){
            flerrHandle("\nTESf _test_flentNew Test Failed !1");
            return false;
        }

    flmemFree(ent);

    return true;
}

///Test entities declarations and implementations

//Adder Entity
typedef struct{
    flNumber_t sum;
}_adderProps;

static void* _adderInputs(flEntity* self, int8_t inputMode, flInt_t propid, const void* propv, flEntity* callerEnt){
    switch (inputMode){
        case flentimoGET:
            if(propid == flentipnSUM) return &( ((_adderProps*)self->props)->sum );
        break;

        case flentimoPOST:
            if(propid == flentipnNUMBER){
                
            }
        break;
    
        default:
        break;
    }
}

//Multiplier entity
typedef struct{
    flNumber_t product;
}_multiplierProps;

typedef struct{
    flEntity * adder;
    flEntity * multiplier;
}_opResultAccumulatorProps;

static bool _test_flentForeach(){
    
}