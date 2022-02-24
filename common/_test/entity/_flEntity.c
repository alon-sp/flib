
#include"_flEntity.h"

typedef struct{
    flInt_t sum;
}_adderProps;

typedef struct{
    flInt_t product;
}_multiplierProps;

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

static bool _test__flentNew(){
    flentCC_t cc = 1;
    flEntity env, *envPtr = &env;
    
    flEntity ent = _flentNew(cc, envPtr);

    if(ent.ccode != cc || ent.env != envPtr || ent.ui2D || ent.ui3D || ent.uinbuf
        || ent.props || ent.entPtrs || ent.tick || ent.input){
            flerrHandle("\nTESf _test__flentNew Test Failed !1");
            return false;
        }

    return true;
}

static bool _test_flentForeach(){
    
}