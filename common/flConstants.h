#ifndef FLCONSTANTSH_INCLUDED
#define FLCONSTANTSH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

//fl number types
typedef unsigned long int flUlint_t;
typedef int flInt_t;
typedef unsigned int flUint_t;
typedef float flFloat_t;

typedef uint32_t flentCC_t;

//fl entity
#define flenticoGET                1
#define flenticoSET                2
#define flenticoPOST               3
#define flenticoCLEANUP_EC         4

#endif