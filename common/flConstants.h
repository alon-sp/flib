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
 * @brief Command for requesting a particular data.
 */
#define flentdmoGET                        1

/**
 * @brief Command to indicate that a component is still processing an input.
 * 
 */
#define flentdmoPROCI                      2
/**
 * @brief command for updating a particular data, intended to be use 
 * by parent/controller entities only. 
 */
#define flentdmoSET                        3
/**
 * @brief command for posting notification, intended to be use by both controller and component entity.
 */
#define flentdmoPOST                       4

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