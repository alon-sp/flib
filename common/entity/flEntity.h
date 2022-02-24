#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include"../flConstants.h"

#include"../container/flArray.h"
#include"flEntIpn.h"

typedef struct flEntity flEntity;

typedef void (*flentTick_tf)(flEntity* self, flInt_t ct, flInt_t dt);
typedef const void* (*flentInput_tf)(flEntity* self, int8_t inputCmd, flInt_t propid, const void* propv, flEntity* callerEnt);

/*----------INTERFACE DESCRIPTION----------*/
struct flEntity{
  /**
   * @brief The classification code of this entity
   *
   */
  const flentCC_t ccode;

  /**
   * @brief The 2D ui or form of this entity.
   * @note All ui entities are defined with respect to their environment(parent) ui entity
   */
  const flEntity * const ui2D;

  /**
   * @brief The 3D ui or form of this entity.
   * @note All ui entities are defined with respect to their environment(parent) ui entity
   */
  const flEntity * const ui3D;

  /**
   * @brief The environment of this entity. 
   * @note Environment is analogous to parent
   */
  const flEntity * const env;

  /**
   * @brief uinbuf -> unhandled input buffer.
   * Because entities are intended to exist or update their internal state(s) 
   * only during the tick function execution; any input (such as SET, POST) which may
   * cause an entity to alter it's internal state must be store temporarily and
   * handle during the tick function execution. This buffer($uinbuf) serve this purpose. 
   * The memory layout of this buffer when it's not empty is as follow:
   * uinbuf                                                    uinbuf.length
   * |-----IN1 Len-----|-----IN2 Len-----|...|-----INN Len-----|
   * IN1B              IN2B                  INNB      
   * 
   * IN1B                                                                                                        IN1 Len
   * |-callerEntityPtr:flEntity*-|-mode:int8_t-[|-propid:flInt_t-|-propvSize:flInt_t-|-propv:propvSize bytes-]|
   * 
   * @note In most cases this entity will already be aware of the type and nature of the target
   * input property and hence $propvSize wouldn't be needed but it has been included 
   * just for those cases where this entity does not understand the given input. This will
   * make it possible for it to skip over this input and move to the next input.
   * @note It's intended for all inputs to be processed in one tick and the buffer reset.
   */
  const flArray * const uinbuf;

  /**
   * @brief All other additional properties of this entity apart from the ones 
   * specified by the interface.
   */
  const void * const props;

  /**
   * @brief An array of pointers to all entities INCLUDING($.ui2D and $.ui3D) that are
   * children of this entity.(IE this entity is considered as the environment of these entities).
   * @note If this entity has any entity whose pointer is not included in this array, 
   * that entity will not be consider as child entity. And hence operations such as ticking
   * that are performed on child's entity will not be perform on such entity.
   */
  const flArray * const entPtrs;

  /**
   * @brief This method gets called by the ticker during every time frame.
   * It's intended that an entity update it's state only during this function call
   * @param self
   * @param ct The time in milliseconds of the latest call of the tick method of the ticker
   * @param dt The time difference in milliseconds of the latest call and the previous 
   * call of the tick method of the ticker
   */
  void (* const tick)(flEntity* self, flInt_t ct, flInt_t dt);

  /**
   * @brief Provide a means of communicating with this entity from the outside world.
   * It's intended that communication take place between this entity and it's parent(environment),
   * or between this entity and it's children or between this entity and other procedures but
   * not between this entity and any of it's siblings directly.
   * @param self This entity itself
   * @param inputCmd The input command: whether GET, SET, POST, or CLEANUP
   * @param propid The id or name of the property or command
   * @param propv The value or argument of this property or command
   * @param callerEnt The entity that called this method. It's value can be NULL when necessary
   * @return Result of the operation | NULL
   */
  const void* (* const input)(flEntity* self, int8_t inputCmd, flInt_t propid, const void* propv, flEntity* callerEnt);
  
};

/*----------SETTERS----------*/
#define _flentSetCcode(entPtr, _ccode)     *( (flentCC_t*)(&entPtr->ccode) )   = _ccode

#define _flentSetUi2D(entPtr, _ui2D)       *( (flEntity**)(&entPtr->ui2D) )    = _ui2D
#define _flentSetUi3D(entPtr, _ui3D)       *( (flEntity**)(&entPtr->ui3D) )    = _ui3D
#define _flentSetEnv(entPtr, _env)         *( (flEntity**)(&entPtr->env) )     = _env
#define _flentSetUinbuf(entPtr, _uinbuf)   *( (flArray **)(&entPtr->uinbuf) )  = _uinbuf
#define _flentSetProps(entPtr, _props)     *( (void    **)(&entPtr->props) )   = _props
#define _flentSetEntPtrs(entPtr, _entPtrs) *( (flArray **)(&entPtr->entPtrs) ) = _entPtrs

#define _flentSetTick(entPtr, _tick)    *( (flentTick_tf *)(&entPtr->tick) )   = _tick
#define _flentSetInput(entPtr, _input)  *( (flentInput_tf*)(&entPtr->input) )  = _input

/*----------FLENTITY UTILITY FUNCTIONS----------*/

/**
 * @brief Create and initialize a new $flEntity interface
 * @param ccode The classification code of this entity
 * @param env The environment of this entity
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentCC_t ccode, const flEntity* env);


/**
 * @brief Apply the callback $funcToApply to each entity in the environment tree
 * @note The function is apply to the head(parent/environment) entity first before
 * the children and it's apply to the children in an ascending order of their indexes.
 * @param env The head entity or node to transverse
 * @param funcToApply A callback to be called on each entity in the tree
 * @param funcArgs Other arguments for the callback function
 * @param lstack An array of flEntity to be use for transversing the environment tree
 * since this method does not use recursion.
 * @note if $funcToApply is NULL and $funcArgs is not NULL, this function assumes that
 * the user wants to apply the tick method of each entity and hence will interpret 
 * $funcArgs as an array of 2 flInt_t (ie flInt_t funcArgs[2]); and pass their values
 * as the ct and dt arguments of the tick method.
 * @note if both $funcToApply and $funcArgs are NULL, this function does nothing.
 * @note if $lstack is NULL, this function uses it's internal static stack making it
 * multi-threading unsafe.
 */
void flentForeach(const flEntity* env, void (*funcToApply)(flEntity*, void*), void* funcArgs, flArray* lstack);

/**
 * @brief Call the tick method of each entity in the environment tree of $env
 * 
 * @param env The head entity or node to transverse
 * @param ct The latest time in ms before this function was called
 * @param dt The time difference in milliseconds of the latest call and the previous 
 * call of this function
 * @param lstack An array of flEntity to be use for transversing the environment tree
 * since this function does not use recursion.
 * @note This function is a friendly wrapper for $flentForeach @see $flentForeach
 */
void flentTick(const flEntity* env, flInt_t ct, flInt_t dt, flArray* lstack);

/**
 * @brief Causes all entities on the environment tree of {*env} to perform a cleanup operation.
 * @param env 
 * @param lstack @see $flentForeach
 */
void flentFree(flEntity** env, flArray* lstack);

#endif//FLENTITYHEADERH_INCLUDED