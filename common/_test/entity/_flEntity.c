
#include"_flEntity.h"

static bool _test_flentNew(){
    flentCC_t cc = 1;
    flEntity *contP = flentNew(cc, NULL, 1);
    
    flEntity* ent = flentNew(cc, contP, 2);

    if(ent->ccode != cc || ent->_con != contP || ent->ui2D || ent->ui3D || 
        ent->_cin->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        ent->_cout->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        ent->props || ent->components->capacity != 2 || ent->tick ||
        contP->components->length != 1){
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

//Multiplier entity
typedef struct{
    flNumber_t product;
}_multiplierProps;

typedef struct{
    flEntity * adder;
    flEntity * multiplier;
}_opResultAccumulatorProps;


bool _flentRunTests(){
    _test_flentNew();

    return true;
}