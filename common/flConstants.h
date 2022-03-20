#ifndef FLCONSTANTSH_INCLUDED
#define FLCONSTANTSH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

//fl number types
typedef int64_t flInt_t;
typedef float flFloat_t;
typedef float flNumber_t;

typedef uint32_t flentCC_t;

///fl entity
//----------

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