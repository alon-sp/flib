#ifndef FLCONSTANTSH_INCLUDED
#define FLCONSTANTSH_INCLUDED

#include<stdint.h>
#include<stdbool.h>

//fl number types
typedef int64_t flInt_t;
typedef float flFloat_t;

typedef uint32_t flentCC_t;

//fl entity

///@note flentimo -> fl Entity Input Mode 

/**
 * @brief Command for retrieving a target property's value.
 */
#define flentimoGET                        1
/**
 * @brief command for setting a target property's value, intended to be use 
 * by parent entities only. 
 */
#define flentimoSET                        2
/**
 * @brief command for posting notification, intended to be use by both parent and child entity.
 */
#define flentimoPOST                       3
/**
 * @brief This command should cause an entity to perform cleanup operation on custom properties
 * excluding custom properties that have been registered as children entities. 
 */
#define flentimoCLEANUP                     4

#endif