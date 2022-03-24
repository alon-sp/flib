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
typedef struct flIOport flIOport;

/**
 * @brief Represent a port of an entity
 * 
 */
typedef struct{
/**
 * @brief 
 * This is an array of bytes: 
 * ->the first ${sizeof(uint8_t)} bytes is the mode of the output data.
 * ->the second ${sizeof(flentDataID_t)} bytes is the id of the output data.
 * ->the interpretation of the remaining set of bytes depends on the entity involved.
 */
  flArray * const _obuf;

  flEntity * const _entity;

  const flentIOportName_t name;

  flIOport * const _linkedPort;
}flIOport;

/**
 * @brief A utility structure for decoding $flIOport output buffer.
 * 
 */
typedef struct{
  int8_t mode;
  flentDataID_t id;
  void* data;
  flInt_t size;
}flentIOdata;

flentIOdata flentiodNew(int8_t dataMode, flentDataID_t dataId, void* data, flInt_t dataSize);

typedef void  (*flentTick_tf)(flEntity* self, void* args);
typedef void* (*flentGetopp_tf)(int optionalPropID);

/**
 * @brief
 * -A mesh data structure of computational nodes
 * -Each instance of this interface is the brain of a given process or activity.
 */
struct flEntity{
  /**
   * @brief The classification code of this entity
   *
   */
  const flentCC_t ccode;

  /**
   * @brief A c-string representing the name of this entity
   */
  const char* const name;

  /**
   * @brief All other specific properties of this entity apart from the ones 
   * specified by the interface.
   * @note This pointer and all associated resources should be managed entirely by the entity.
   * This should exclude any resources that are associated with ports that have been added 
   * to this entity; these and all other resources/properties of this interface are owned and
   * managed by the system(flEntity module).
   */
  void * const props;

  /**
   * @brief An array of pointers to all ports of this entity.
   */
  flArray * const ioports;

  /**
   * @brief This method gets called by the ticker during every time frame.
   * It's intended that an entity update it's state only during this function call
   * @param self
   * @param args:flEntityTickMethodArg -> Arguments passed from the ticker to this entity.
   */
  void (* const tick)(flEntity* self, void* args);
  
  /**
   * @brief This method should be implemented to get optional properties of
   * this interface that have been defined on this entity.
   * @param optionalPropID The id of the optional property to retrieve.
   */
  void* (* const getOpp)(int optionalPropID);
  
};

/**
 * @brief A structure for holding additional arguments of the tick method of an entity
 */
typedef struct{

  //The time in milliseconds of the latest call of the tick method of the ticker
  flInt_t ct;

  //The time difference in milliseconds of the latest call and the previous 
  //call of the tick method of the ticker
  flInt_t dt;

  //Command passed from system to the target entity
  int8_t syscmd;

  //Arguments for the given system command.
  const void* syscmdArgs;

}flEntityTickMethodArg;

/*----------SETTERS----------*/

/**
 * @brief Set the name of this entity
 * @param ent 
 * @param namestr 
 */
bool flentSetName(flEntity* ent, const char* namestr);

#define  flentSetCcode(ent, _ccode)      *( (flentCC_t*)(&ent->ccode) )    = _ccode

#define _flentSetName(ent, _namestr)     *( (char**)(&ent->name)      )    = _namestr

#define  flentSetProps(ent, _props)      *( (void    **)(&ent->props) )    = _props

#define _flentSetIOports(ent, _ioports)  *( (flArray **)(&ent->ioports) )  = _ioports

#define  flentSetTick(ent, _tick)        *( (flentTick_tf *)(&ent->tick) ) = _tick

#define  flentSetGetOpp(ent, oppG)       *( (flentGetopp_tf *)(&ent->getOpp) ) = oppG


/*----------FLENTITY FUNCTIONS----------*/

/**
 * @brief Create and initialize a new $flEntity interface and add it to the given environment
 * if it($xenv) is not NULL.
 * @param xenv The environment in which the new entity will be executed
 * @param ccode The classification code of the new entity
 * @param initialPortCount The initial number of io ports the new entity will have.
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentXenv *xenv, flentCC_t ccode, int initialPortCount);

#endif//FLENTITYHEADERH_INCLUDED