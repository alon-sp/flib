#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include<stdio.h> //for sprintf
#include<string.h>

#include"../flConstants.h"

#include"../container/flArray.h"
#include"flEntDid.h"
#include"flEntIpn.h"

typedef struct flEntity flEntity;
typedef struct flentXenv flentXenv;
typedef struct flentIOport flentIOport;
typedef struct flentIOdata flentIOdata;

/**
 * @brief Represent a port of an entity
 * 
 */
struct flentIOport{
/**
 * @brief 
 * This is an array of bytes: 
 * ->the first ${sizeof(uint8_t)} bytes is the mode of the output data.
 * ->the second ${sizeof(flentdid_t)} bytes is the id of the output data.
 * ->the interpretation of the remaining set of bytes depends on the entity involved.
 */
  flArray * const _obuf;
  #define _flentiopSetObuf(iop, obuf) *( (flArray**)(&iop->_obuf) ) = obuf

  flEntity * const entity;
  #define flentiopSetEntity(iop, ent) *( (flEntity**)(&iop->entity) ) = ent

  const flentipn_t name;
  #define flentiopSetName(iop, _name) *( (flentipn_t*)(&iop->name) ) = _name

  flentIOport * const _linkedPort;
  #define _flentiopSetlinkedPort(iop, lport) *( (flentIOport**)(&iop->_linkedPort) ) = lport

};

/**
 * @brief Create a new port
 * @param ipname The name of this port
 * @param ent The associated entity of the new port
 * @param targetPort? Port to link the newly created port with
 * @return Pointer to the newly created port.
 */
flentIOport* flentiopNew(flentipn_t ipname, flEntity* ent, flentIOport* targetPort);

/**
 * @brief Create a connection between the two given ports
 * 
 * @param port1 
 * @param port2 
 */
void flentiopLink(flentIOport* port1, flentIOport* port2);

/**
 * @brief Break the connection between the given port and it's target port
 * @param port 
 * @return pointer to the linked port of $port before the unlink operation
 */
flentIOport* flentiopUnlink(flentIOport* port);

/**
 * @brief A utility structure for decoding $flentIOport output buffer.
 * 
 */
struct flentIOdata{
  int8_t mode;
  flentdid_t id;
  void* data;
  flint_t size;
};

flentIOdata flentiodNew(int8_t dataMode, flentdid_t dataId, void* data, flint_t dataSize);

typedef void  (*flentTick_tf)(flEntity* self);
typedef void* (*flentsch_tf)(flentsyc_t cmd, void *args);

/**
 * @brief
 * -A mesh data structure of computational nodes
 * -Each instance of this interface is the brain of a given process or activity.
 */
struct flEntity{

  //The classification code of this entity
  const flentcco_t ccode;
  #define  flentSetCcode(ent, _ccode) *( (flentcco_t*)(&ent->ccode) ) = _ccode

  //The environment in which the new entity will be executed
  flentXenv * const xenv;
  #define _flentSetXenv(ent, _xenv) *( (flentXenv **)(&ent->xenv) ) = _xenv

  //A c-string representing the name of this entity
  const char* const name;
  #define _flentSetName(ent, _namestr) *( (char**)(&ent->name) ) = _namestr

  /**
   * @brief All other specific properties of this entity apart from the ones 
   * specified by the interface.
   * @note This pointer and all associated resources should be managed entirely by the entity.
   * This should exclude any resources that are associated with ports that have been added 
   * to this entity; these and all other resources/properties of this interface are own by
   * the sytem and are automatically cleaned up when $flentFree is called on this entity.
   */
  void * const props;
  #define  flentSetProps(ent, _props) *( (void**)(&ent->props) ) = _props

  //An array of pointers to all ports of this entity.
  flArray * const ioports;
  #define _flentSetIOports(ent, _ioports) *( (flArray **)(&ent->ioports) ) = _ioports

  /**
   * @brief This method gets called by the ticker during every time frame if tick is enable.
   * It's intended that an entity update it's state only during this function call
   * @param self
   */
  void (* const tick)(flEntity* self);
  #define flentSetTick(ent, _tick) *( (flentTick_tf *)(&ent->tick) ) = _tick

  const bool tickEnable;
  #define flentEnableTick(ent)  *( (bool*)(&ent->tickEnable) )  = true
  #define flentDisableTick(ent) *( (bool*)(&ent->tickEnable) )  = false
  
  /**
   * @brief This method should be implemented to handle commands from system(flEntity module)
   * @param cmd The system command
   * @param args Arguments for the given commands
   * @return The result of executing the given command
   */
  void* (* const hscmd)(flentsyc_t cmd, void *args);
  #define flentSetHscmd(ent, sch) *( (flentsch_tf *)(&ent->hscmd) ) = sch
  
};


/*----------FLENTITY FUNCTIONS----------*/

/**
 * @brief Set the name of this entity
 * @param ent 
 * @param namestr 
 */
bool flentSetName(flEntity* ent, const char* namestr);

/**
 * @brief Create and initialize a new $flEntity interface. The newly created entity is added
 * to the given execution environment($xenv) if it($xenv) is not NULL.
 * @param xenv The environment in which the new entity will be executed
 * @param ccode The classification code of the new entity
 * @param initialPortCount The initial number of io ports the new entity will have.
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentXenv* xenv, flentcco_t ccode, int initialPortCount);

/**
 * @brief 
 * 
 * @param ent 
 */
void flentFree(flEntity* ent);

struct flentXenv{
  flArray * const entities;

  //The time in milliseconds of the latest call of the tick method
  const flint_t tickCT;

  //The time difference in milliseconds of the latest call and the previous 
  //call of the tick method
  const flint_t tickDT;

  void (* const tick)(flentXenv* xenv, flint_t ct, flint_t dt);

  bool (* const addEntity)(flentXenv* xenv, flEntity* ent);
  bool (* const removeEntity)(flentXenv* xenv, flEntity* ent);

  flint_t (* const millis)();
};

#endif//FLENTITYHEADERH_INCLUDED