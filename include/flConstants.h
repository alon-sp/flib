#ifndef FLCONSTANTSH_INCLUDED
#define FLCONSTANTSH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

#include"flentiopDtype.h"
#include"flTypes.h"

// #ifdef _FLIB_EXP_EMBEDDED
//     #define flentMAX_NAME_LENGTH 16/*characters maximum*/
//     #define flentiopMAX_NAME_LENGTH 16
// #else
//     #define flentMAX_NAME_LENGTH 64/*characters maximum*/
//     #define flentiopMAX_NAME_LENGTH 64
// #endif

#define FLSTR(str) str

typedef int8_t flentsyc_t;
///SYC -> system command

#define flentsycNIL                           0

//This command is sent whenever an entity is about to be destroyed.
//An entity on receving this command must perform cleanup operation on custom props as required.
///@arg flEntity* : Pointer to the target entity
///@return void
#define flentsycCLEANUP                       1

//This command is a notification that is sent whenever a new port is about to be
//added to an entity
///@arg flentIOport* : Pointer to the port to be added
///@return void
#define flentsycIOPORT_ADD                    2

//This command is a notification that is sent whenever a port is about to be
//remove from an entity.
///@arg flentIOport* : Pointer to the port to be removed
///@return void
#define flentsycIOPORT_REMOVE                 3

///This command is sent to request the name of a port
///@arg flentIOport* : Pointer to the port whose name is being requested for
///@return const char*
#define flentsycgetIOPORT_NAME                4

///This command is sent to request the Nth data type that is accepted by the given port
///@arg flentsycEntIoportNthDtypeArg*
///@return flentiopDataTypeCount_t
#define flentsycgetENT_IOPORT_NTH_DTYPE       5
typedef struct{
    flentIOport* port;
    flentiopDataTypeCount_t n;
}flentsycEntIoportNthDtypeArg;

///This command is sent to query whether the given port accept the given data type
///@arg flentsycEntIoportAcceptDtypeArg*
///@return bool
#define flentsycgetENT_IOPORT_ACCEPT_DTYPE    6
typedef struct{
    flentIOport* port;
    flentiopDtype_t dtype;
}flentsycEntIoportAcceptDtypeArg;

#endif