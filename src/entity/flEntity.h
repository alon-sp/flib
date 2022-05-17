#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include<stdio.h> //for sprintf
#include<string.h>

#include"fl.h"
#include"flentConstants.h"
#include"flentIOport.h"

typedef void  (*flentTick_tf)(flEntity* self, flentXenv* xenv);
typedef void  (*flentHscmd_tf)(flentsyc_t cmd, void *args, void* rvalDest);

/**
 * @brief
 * A flow based programming inspired data structure for modeling components with input
 * and output ports.
 * -Each instance of this interface can be view as a high level sequential logic circuit.
 */
struct flEntity{

  //The classification code of this entity
  const flentcco_t ccode;
  #define  flentSetCcode(ent, _ccode) *( (flentcco_t*)(&(ent)->ccode) ) = _ccode

  /**
   * @brief User defined properties(ie all other additional properties of this entity 
   * apart from the ones specified by the interface).
   * @note This pointer and all associated resources should be managed entirely by the entity.
   * This should exclude any resources that are associated with any ports
   * of this entity; these and all other resources/properties of this interface are own by
   * the sytem and are automatically cleaned up when $flentFree is called on this entity.
   */
  void * const props;
  #define  flentSetProps(ent, _props) *( (void**)(&(ent)->props) ) = _props

  //An array of pointers to all input-output pairs ports of this entity.
  flArray * const ioports;
  #define _flentSetIOports(ent, _ioports) *( (flArray **)(&(ent)->ioports) ) = _ioports
  #define _flentGetPort(ent, index) ( *(flentIOport**)_flarrGet((ent)->ioports, index) )
  #define flentGetPort(ent, index) ( *(flentIOport**)flarrGet((ent)->ioports, index) )

  /**
   * @brief This method gets called by the ticker during every time frame if
   * $._tick or $.autoTick is enable.
   * It's intended that an entity update it's state only during this function call
   * @param self
   * @param xenv The execution environment of this entity.
   */
  void (* const tick)(flEntity* self, flentXenv* xenv);
  #define flentSetTick(ent, tickf) *( (flentTick_tf *)(&(ent)->tick) ) = tickf

  const bool _tick;
  #define _flentEnableTick(ent)  *( (bool*)(&(ent)->_tick) )  = true
  #define _flentDisableTick(ent) *( (bool*)(&(ent)->_tick) )  = false

  const bool autoTick;
  #define flentEnableAutoTick(ent)  *( (bool*)(&(ent)->autoTick) )  = true
  #define flentDisableAutoTick(ent) *( (bool*)(&(ent)->autoTick) )  = false

  /**
   * @brief This method should be implemented to handle commands from system(flEntity module).
   * $flentsycCLEANUP command is one important such command that is send whenever $flentFree
   * is called on an entity. The purpose is to allow the entity to perform cleanup operation on
   * user defined properties before it can be destroy.
   * @param cmd The system command @see flConstants.h for various syc command description
   * @param args Arguments for the given commands
   * @param rvalDest Destination buffer for the result of executing the given command
   */
  void (* const hscmd)(flentsyc_t cmd, void *args, void* rvalDest);
  #define flentSetHscmd(ent, sch) *( (flentHscmd_tf *)(&(ent)->hscmd) ) = sch
  
};


/*----------FLENTITY FUNCTIONS----------*/

/**
 * @brief Create and initialize a new $flEntity interface. The newly created entity is added
 * to the given execution environment($xenv) if it($xenv) is not NULL.
 * @param xenv The environment in which the new entity will be executed
 * @param initialPortCount The initial number of io ports the new entity will have.
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentXenv* xenv, int initialPortCount);

/**
 * @brief Free the memory associated with the given entity
 * @note send $flentsycCLEANUP command to the given entity before destroying it.
 * @param ent 
 * @param xenv The execution environment of the given entity
 */
void flentFree(flEntity* ent, flentXenv* xenv);

/**
 * @brief find the first port encounter in $ent with the given id($portID) and type($portType)
 * @param ent 
 * @param portName 
 * @param portType
 * @return Pointer to the port with the given name | NULL if no port was found.
 */
flentIOport* flentFindPortByID(flEntity* ent, flentiopID_t portID, flentiopType_t portType);

/**
 * @brief Link and add the given port to $ent
 * @note If $port already exist in $ent, only linking operation will happen
 * @param ent 
 * @param port
 * @return true if $port already existed or was added | false otherwise 
 */
bool flentAddPort(flEntity* ent, flentIOport* port);

/**
 * @brief Remove the given port from $ent if found
 * @note This method does not free the memory associated with the port
 * @param ent 
 * @param port 
 */
void flentRemovePort(flEntity* ent, flentIOport* port);

/**
 * @brief Remove the given port if found and free the memory associated with it($port)
 * @param ent 
 * @param port 
 */
void flentDeletePort(flEntity* ent, flentIOport* port);

typedef void (*flentForEachPortCb_tf)(flentIOport* port, void* args);

/**
 * @brief Apply the given callback to each port of the given entity.
 * @param ent
 * @param cb The callback to be called on each port
 * @param cbArgs additional arguments for the callback function
 */
void flentForEachPort(flEntity* ent, flentForEachPortCb_tf cb, void* cbArgs);

/**
 * @brief Clear the output buffer associated with all OUTPUT and IO type ports
 * such that any read operation on those ports will result in data of type flentiopDTYPE_NIL
 * @param ent 
 */
void _flentClearAllPortsObuf(flEntity* ent);

#endif//FLENTITYHEADERH_INCLUDED