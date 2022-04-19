#ifndef FLCONSTANTSH_INCLUDED
#define FLCONSTANTSH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

//fl number types
typedef uint8_t flbyt_t;
typedef int64_t flint_t;
typedef float flflt_t;
typedef float flnum_t;

typedef uint32_t flentcco_t;

//fl function pointers types
typedef void* (*flvod_tf)(void* args);

///fl entity
//----------

typedef flbyt_t flentdmo_t;
///@note flentdmo -> fl Entity Data Mode 

/**
 * @brief Represent the absence of any data.
 * 
 */
#define flentdmoNIL                        0

/**
 * @brief mode for requesting data.
 */
#define flentdmoGET                        1

/**
 * @brief mode for posting data/notification
 */
#define flentdmoPOST                       2

/**
 * @brief mode for posting pointer to data
 */
#define flentdmoPOSTDP                     3


typedef int8_t flentsyc_t;
///SYC -> system command

/**
 * @brief Represent the absence of any system command.
 * 
 */
#define flentsycNIL                        0

//This command is sent whenever an entity is about to be destroyed.
//An entity on receving this command must perform cleanup operation on custom props as required.
///@arg void
#define flentsycCLEANUP                    1

//This command is a notification that is sent whenever a new port is about to be
//added to an entity
///@arg flentIOport* : Pointer to the port to be added
#define flentsycIOPORT_ADD                 2

//This command is a notification that is sent whenever a port is about to be
//remove from an entity.
///@arg flentIOport* : Pointer to the port to be removed
#define flentsycIOPORT_REMOVE              3

#endif