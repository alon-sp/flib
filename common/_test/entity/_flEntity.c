
#include"_flEntity.h"

static void defaultTick(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs){
    return;
}

static bool _test_flentNew(){
    flentCC_t cc = 1;
    flEntity *contP = flentNew(cc, NULL, 1);
    flentSetTick(contP, defaultTick);
    
    flEntity* compP = flentNew(cc, contP, 2);
    flentSetTick(compP, defaultTick);

    if(compP->ccode != cc || compP->_con != contP || compP->ui2D || compP->ui3D || 
        compP->_cin->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->_cout->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->props || compP->components->capacity != 2 || compP->tick ||
        contP->components->length != 1){
            flerrHandle("\nTESf _test_flentNew Test Failed !1");
            return false;
        }

    flentFree(&contP, NULL);

    return true;
}

///Test entities declarations and implementations

//Adder Entity
typedef struct{
    flNumber_t sum;
}_adderProps;

static void adderTick(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs){
    if(syscmd && syscmd == flentsycCLEANUP){
        printf("\nAdder entity cleaned up");
        if(self->props) flmemFree(self->props);
        flentSetProps(self, NULL);
        return;
    }
}

//Multiplier entity
typedef struct{
    flNumber_t product;
}_multiplierProps;

//Calculator entity
typedef struct{
    flEntity * adder;
    flEntity * multiplier;
}_calculatorProps;


bool _flentRunTests(){
    _test_flentNew();

    return true;
}