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

///fl entity
//----------

typedef int8_t flentdmo_t;
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
 * @brief mode to indicate that an entity is still processing an input.
 * 
 */
#define flentdmoPROCI                      2

/**
 * @brief mode for posting data/notification
 */
#define flentdmoPOST                       4

//mode for posting pointer to data
#define flentdmoPOSTDP                     5

typedef int8_t flentsyc_t;
///SYC -> system command

/**
 * @brief Represent the absence of any system command.
 * 
 */
#define flentsycNIL                        0

/**
 * @brief This command should cause an entity to perform cleanup operation on custom properties
 * excluding custom properties that have been registered as components.
 */
#define flentsycCLEANUP                    1

#endif