#ifndef FLENTSTDHEADERH_INCLUDE
#define FLENTSTDHEADERH_INCLUDE

#include"math.h"

#include"flEntity.h"
#include"flentXenv.h"

#define flentstdBINOPER_IN_A       0
#define flentstdBINOPER_IN_B       1
#define flentstdBINOPER_OUT        2

flEntity* flentstdAddNew(flentXenv* xenv);
#define flentstdADD_IN_A flentstdBINOPER_IN_A
#define flentstdADD_IN_B flentstdBINOPER_IN_B
#define flentstdADD_OUT flentstdBINOPER_OUT

flEntity* flentstdSubNew(flentXenv* xenv);
#define flentstdSUB_IN_A flentstdBINOPER_IN_A
#define flentstdSUB_IN_B flentstdBINOPER_IN_B
#define flentstdSUB_OUT flentstdBINOPER_OUT

flEntity* flentstdMultNew(flentXenv* xenv);
#define flentstdMULT_IN_A flentstdBINOPER_IN_A
#define flentstdMULT_IN_B flentstdBINOPER_IN_B
#define flentstdMULT_OUT flentstdBINOPER_OUT

flEntity* flentstdDivNew(flentXenv* xenv);
#define flentstdDIV_IN_A flentstdBINOPER_IN_A
#define flentstdDIV_IN_B flentstdBINOPER_IN_B
#define flentstdDIV_OUT flentstdBINOPER_OUT

flEntity* flentstdModNew(flentXenv* xenv);
#define flentstdMOD_IN_A flentstdBINOPER_IN_A
#define flentstdMOD_IN_B flentstdBINOPER_IN_B
#define flentstdMOD_OUT flentstdBINOPER_OUT

flEntity* flentstdPowNew(flentXenv* xenv);
#define flentstdPOW_IN_A flentstdBINOPER_IN_A
#define flentstdPOW_IN_B flentstdBINOPER_IN_B
#define flentstdPOW_OUT flentstdBINOPER_OUT

flEntity* flentstdBytsToDptrNew(flentXenv* xenv, uint8_t maxDptrs);
#define flentstdBYTSTODPTR_IN  0
#define flentstdBYTSTODPTR_OUT 1

#endif