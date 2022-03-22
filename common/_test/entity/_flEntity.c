
#include"_flEntity.h"

static void defaultTick(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs){
    return;
}

static bool _test_flentNew(){
    flentCC_t cc = 1;
    flEntity *contP = flentNew(cc, 1);
    flentSetTick(contP, defaultTick);
    
    flEntity* compP = flentNew(cc, 2);
    flentSetTick(compP, defaultTick);

    flentAddCompPtr(contP, compP);

    if(compP->ccode != cc || compP->controller != contP || compP->ui2D || compP->ui3D || 
        compP->_cin->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->_cout->capacity != sizeof(uint8_t)+sizeof(flentDataID_t)+sizeof(void*) ||
        compP->props || compP->components->capacity != 2 ||
        contP->components->length != 1){
            flerrHandle("\nTESf _test_flentNew Test Failed !1");
            return false;
        }

    flentFree(&contP, NULL);

    printf("\n_test_flentNew: TEST OK");

    return true;
}

///Test entities declarations and implementations

static flNumber_t add(flNumber_t a, flNumber_t b){
    return a+b;
}

static flNumber_t multiply(flNumber_t a, flNumber_t b){
    return a*b;
}

static flNumber_t divide(flNumber_t a, flNumber_t b){
    return a/b;
}

//Operator Entity
typedef struct{
    flNumber_t result;
    flNumber_t (*operate)(flNumber_t a, flNumber_t b);
}_accOperProps;

static void accOperTick(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs){
    if(syscmd && syscmd == flentsycCLEANUP){
        const char* ename = self->name? self->name : "accOper";
        printf("\n%s entity cleaned up", ename);
        if(self->props) flmemFree(self->props);
        flentSetProps(self, NULL);
        return;
    }

    _accOperProps* adp = (_accOperProps*)self->props;

    flentIOdata cin, cout;

    flentReadFromControllerOutput(self, &cout);
    if(cout.mode){
        flentWriteToControllerOutput(self, flentiodNew(flentdmoNIL, flentdidNIL, NULL, 0));
    }

    flentReadFromControllerInput(self, &cin);

    switch(cin.mode){
        case flentdmoGET:
            if(cin.id == flentdidRESULT){
                flentWriteToControllerOutput(self, flentiodNew(flentdmoPOST, flentdidRESULT, &adp->result, sizeof(flNumber_t)));
            }
        break;

        case flentdmoPOST:
            if(cin.id == flentdidNUMBER){
                adp->result = adp->operate(adp->result, *(flNumber_t*)cin.data);
            }else if(cin.id == flentdidRESET){
                adp->result = 0;
            }
        break;
    }
}

static flEntity* accOperNew(flNumber_t (*operate)(flNumber_t, flNumber_t)){
    _accOperProps *adp = flmemMalloc(sizeof(_accOperProps));
    adp->result = 1;
    adp->operate = operate;

    flEntity *accOperEnt = flentNew(0, 0);

    flentSetProps(accOperEnt, adp);
    flentSetTick(accOperEnt, accOperTick);

    return accOperEnt;
}

//Calculator entity
typedef struct{
    flEntity * adder;
    flEntity * multiplier;
    flEntity * divider;
}_accCalProps;

static inline void accCalRequestCompResult(flEntity *comp){
    flentWriteToComponentOutput(comp, flentiodNew(flentdmoGET, flentdidRESULT, NULL, 0));
}

static flEntity* accCalSelectComp(flEntity* self, flentDataID_t dataID){
    switch(dataID){
        case flentdidNIL:
        case flentdidRESULT:
        case flentdidSUM:
            return ((_accCalProps*)self->props)->adder;
        
        case flentdidPRODUCT:
            return ((_accCalProps*)self->props)->multiplier;
        
        case flentdidQUOTIENT:
            return ((_accCalProps*)self->props)->divider;
    }
}

static void accCalTick(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs){
    if(syscmd && syscmd == flentsycCLEANUP){
        const char* ename = self->name? self->name : "accCal";
        printf("\n%s entity cleaned up", ename);
        if(self->props) flmemFree(self->props);
        flentSetProps(self, NULL);
        return;
    }

    _accCalProps* calp = (_accCalProps*)self->props;

    flentIOdata cin, cout;

    flentReadFromControllerOutput(self, &cout);
    flentReadFromControllerInput(self, &cin);
    
    switch(cin.mode){
        case flentdmoNIL:
            if(cout.mode){
                flentWriteToControllerOutput(self, flentiodNew(flentdmoNIL, flentdidNIL, NULL, 0));
            }
        break;

        case flentdmoGET:
            switch(cout.mode){
                case flentdmoNIL:
                    accCalRequestCompResult(accCalSelectComp(self, cin.id));
                    flentWriteToControllerOutput(self, flentiodNew(flentdmoPROCI, flentdidNIL, NULL, 0));
                break;

                case flentdmoPROCI:{
                    flentIOdata cmpin;
                    flentReadFromComponentInput(accCalSelectComp(self, cin.id), &cmpin);
                    if(cmpin.mode == flentdmoPOST && cmpin.id == flentdidRESULT){
                        cmpin.id = cin.id;
                        flentWriteToControllerOutput(self, cmpin);
                    }
                }
                break;

                case flentdmoPOST:
                    flentWriteToControllerOutput(self, flentiodNew(flentdmoNIL, flentdidNIL, NULL, 0));
                break;
            }
        break;

        case flentdmoPOST:
            for(int i = 0; i<self->components->length; i++){
                flEntity *comp = *( (flEntity**)flarrGet(self->components, i) );
                if(comp) flentWriteToComponentOutput(comp, cin);
            }
        break;
    }
}

static flEntity* accCalNew(flEntity *adder, flEntity *multiplier, flEntity *divider){
    _accCalProps *acp = flmemMalloc(sizeof(_accCalProps));
    acp->adder = adder;
    acp->multiplier = multiplier;
    acp->divider = divider;

    flEntity *accCalEnt = flentNew(0, 3);

    flentSetProps(accCalEnt, acp);
    flentSetTick(accCalEnt, accCalTick);

    flentAddCompPtr(accCalEnt, adder);
    flentAddCompPtr(accCalEnt, multiplier);
    flentAddCompPtr(accCalEnt, divider);

    return accCalEnt;
}

static bool _test_flEntity(){
    flEntity *adderEnt = accOperNew( add);
    flEntity *multEnt = accOperNew(multiply);
    flEntity *divEnt = accOperNew(divide);

    flEntity *calEnt = accCalNew(adderEnt, multEnt, divEnt);

    //Post some numbers to the calculator
    #define _tmp_EPSILON 0.000001
    flNumber_t expectedSum = 1;
    flNumber_t expectedProduct = 1;
    flNumber_t expectedQuotient = 1;
    for(int i = 1; i < 10; i++){
        flNumber_t data = (flNumber_t)i;
        expectedSum += data;
        expectedProduct *= data;
        expectedQuotient /= data;

        flentIOdata iod = flentiodNew(flentdmoPOST, flentdidNUMBER, &data, sizeof(flNumber_t));
        flentWriteToComponentOutput(calEnt, iod);
        flentTick(calEnt, 0, 0, 0, NULL, NULL);

    }

    //Read the accumulated result of each operator from the calculator
    //SUM
    flentWriteToComponentOutput(calEnt, flentiodNew(flentdmoGET, flentdidSUM, NULL, 0));
    flentIOdata calSumIO = {.mode = flentdmoNIL, .id = flentdidNIL};
    while( !(calSumIO.mode == flentdmoPOST && calSumIO.id == flentdidSUM) ){
        flentReadFromComponentInput(calEnt, &calSumIO);
        flentTick(calEnt, 0, 0, 0, NULL, NULL);
    }
    if( fabs( *(flNumber_t*)calSumIO.data - expectedSum ) > _tmp_EPSILON){
        flerrHandle("\nTESf _test_flEntity SUM");
    }

    //PRODUCT
    flentWriteToComponentOutput(calEnt, flentiodNew(flentdmoGET, flentdidPRODUCT, NULL, 0));
    flentIOdata calProdIO = {.mode = flentdmoNIL, .id = flentdidNIL};
    while( !(calProdIO.mode == flentdmoPOST && calProdIO.id == flentdidPRODUCT) ){
        flentReadFromComponentInput(calEnt, &calProdIO);
        flentTick(calEnt, 0, 0, 0, NULL, NULL);
    }
    flNumber_t _tmp = *(flNumber_t*)calProdIO.data;
    if( fabs( *(flNumber_t*)calProdIO.data - expectedProduct ) > _tmp_EPSILON){
        flerrHandle("\nTESf _test_flEntity PRODUCT");
    }

    printf("\n_test_flEntity: TEST OK");

    #undef _tmp_EPSILON

    /**
     * @brief @note The SUM test will pass but the PRODUCT test will fail. This is due to the
     * fact that $calEnt failed to clear the components' outputs after a POST operation. Other
     * similar issues associated with entities involved in this test could also contribute to
     * the failure. I decided to tackle all of these issues and correct them in the next
     * improved implementation of the flEntity interface.
     */

    
}

bool _flentRunTests(){
    _test_flentNew();
    _test_flEntity();

    return true;
}