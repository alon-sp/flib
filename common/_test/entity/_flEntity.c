
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

    if(compP->ccode != cc || compP->controller != contP || compP->ui2D || compP->ui3D || 
        compP->_cin->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->_cout->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->props || compP->components->capacity != 2 ||
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

    _adderProps* adp = (_adderProps*)self->props;

    flentIOdata cin, cout;

    flentReadFromControllerOutput(self, &cout);
    if(cout.mode){
        flentWriteToControllerOutput(self, flentiodNew(flentdmoNIL, flentdidNIL, NULL, 0));
    }

    flentReadFromControllerInput(self, &cin);

    switch(cin.mode){
        case flentdmoGET:
            if(cin.id == flentdidSUM){
                flentWriteToControllerOutput(self, flentiodNew(flentdmoPOST, flentdidSUM, &adp->sum, sizeof(flNumber_t)));
            }
        break;

        case flentdmoPOST:
            if(cin.id == flentdidNUMBER){
                adp->sum += *(flNumber_t*)cin.data;
            }else if(cin.id == flentdidRESET){
                adp->sum = 0;
            }
        break;
    }
}

static flEntity* adderNew(flEntity *controller){
    _adderProps *adp = flmemMalloc(sizeof(_adderProps));
    flEntity *adderEnt = flentNew(0, controller, 0);

    flentSetProps(adderEnt, adp);

    return adderEnt;
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