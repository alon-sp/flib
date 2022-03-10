#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include"../flConstants.h"

#include"../container/flArray.h"
#include"flEntDid.h"

typedef struct flEntity flEntity;

typedef void (*flentTick_tf)(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs);

/**
 * @brief
 * -Each instance of this interface is the brain of a given process or activity.
 */
struct flEntity{
  /**
   * @brief The classification code of this entity
   *
   */
  const flentCC_t ccode;

  /**
   * @brief The 2D ui or form of this entity.
   */
  const flEntity * const ui2D;

  /**
   * @brief The 3D ui or form of this entity.
   */
  const flEntity * const ui3D;

  /**
   * @brief The controller of this entity(component)
   */
  const flEntity * const _con;
  /**
   * @brief Input from controller to this entity(component)
   * @note This is an array of bytes: 
   * ->the first ${sizeof(uint8_t)} bytes is the mode of the input data.
   * ->the second ${sizeof(flentDataID_t)} bytes is the id of the input data.
   * ->the interpretation of the remaining set of bytes depends on the entity/component involve.
   */
  const flArray * const _cin;
  /**
   * @brief Output to controller from this entity(component)
   * This is an array of bytes: 
   * ->the first ${sizeof(uint8_t)} bytes is the mode of the output data.
   * ->the second ${sizeof(flentDataID_t)} bytes is the id of the output data.
   * ->the interpretation of the remaining set of bytes depends on the entity/component involve.
   */
  const flArray * const _cout;

  /**
   * @brief All other specific properties of this entity apart from the ones 
   * specified by the interface.
   * @note This is the only property an entity should perform clean up operation
   * on when it receives the CLEANUP system command. This should exclude any inner properties
   * that were registered as components of this entity.
   */
  const void * const props;

  /**
   * @brief An array of pointers to all entities INCLUDING($.ui2D and $.ui3D) that are
   * components of this entity.(IE this entity is the controller of these entities).
   * @note If this entity has any entity whose pointer is not included in this array, 
   * that entity will not be consider as component entity. And hence operations such as ticking and 
   * clean up that are performed on component entities will not be perform on such entity.
   */
  const flArray * const components;

  /**
   * @brief This method gets called by the ticker during every time frame.
   * It's intended that an entity update it's state only during this function call
   * @param self
   * @param ct The time in milliseconds of the latest call of the tick method of the ticker
   * @param dt The time difference in milliseconds of the latest call and the previous 
   * call of the tick method of the ticker
   * @param syscmd Command passed from system to this entity. This is how CLEANUP and other 
   * system command are passed to each entity along a given controller-component(s).
   * @param syscmdArgs Arguments for the given system command.
   */
  void (* const tick)(flEntity* self, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs);
  
};

/**
 * @brief A structure for holding arguments of the tick method of an entity
 */
typedef struct{
  flEntity* self;
  flInt_t ct;
  flInt_t dt;
  int8_t syscmd;
  const void* syscmdArgs;
}flEntityTickMethodArg;

/*----------SETTERS----------*/
/**
 * @brief Set the 2d ui entity of this entity
 * @note If $entP->ui2D is already set, this function has no effect.
 * @param entP 
 * @param ui2dP 
 */
bool flentSetUi2D(flEntity* entP, flEntity* ui2dP);

/**
 * @brief Set the 3D ui entity of this entity.
 * @note If $entP->ui3D is already set, this function has no effect.
 * @param entP 
 * @param ui3dP 
 */
bool flentSetUi3D(flEntity* entP, flEntity* ui3dP);

#define _flentSetCcode(entPtr, _ccode)     *( (flentCC_t*)(&entPtr->ccode) )    = _ccode

#define _flentSetUi2D(entPtr, _ui2D)       *( (flEntity**)(&entPtr->ui2D) )     = _ui2D
#define _flentSetUi3D(entPtr, _ui3D)       *( (flEntity**)(&entPtr->ui3D) )     = _ui3D

#define _flentSetCon(entPtr, con_)          *( (flEntity**)(&entPtr->_con) )     = con_
#define _flentSetCin(entPtr, cin_)          *( (flArray **)(&entPtr->_cin) )     = cin_
#define _flentSetCout(entPtr, cout_)        *( (flArray **)(&entPtr->_cout) )    = cout_

#define  flentSetProps(entPtr, _props)     *( (void    **)(&entPtr->props) )    = _props

#define _flentSetComponents(entPtr, _comps)*( (flArray **)(&entPtr->components) ) = _comps

#define  flentSetTick(entPtr, _tick)       *( (flentTick_tf *)(&entPtr->tick) ) = _tick

/*----------FLENTITY UTILITY FUNCTIONS----------*/

/**
 * @brief Create and initialize a new $flEntity interface
 * @param ccode The classification code of the new entity
 * @param contP Pointer to the controller of the new entity
 * @param initialCompCount The initial number of components the new entity will have.
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentCC_t ccode, flEntity* contP, int initialCompCount);

/**
 * @brief Add the given component pointer($compP) to the given controller($contP) if it
 * doesn't already exist. It also link the controller to the component.
 * @param contP Pointer to the controller of the component to be added
 * @param compP the component pointer to be added
 * @return A pointer to the added component pointer or existing component pointer | 
 * NULL if the component pointer could not be added.
 * @note The returned pointer is guarantee to be valid till the next call to this function
 */
const flEntity** flentAddCompPtr(flEntity* contP, const flEntity* compP);

/**
 * @brief Remove the given component pointer from the given controller.
 * After this operation, the compoent pointer to by $compP is no longer link with the given controller.
 * @param contP pointer to the controller
 * @param compP the componet pointer to be remove
 */
//void flentRemoveCompPtr(flEntity* contP, const flEntity* compP);

/**
 * @brief Apply the callback $funcToApply to each entity in the controller-component(s) tree
 * @note The function is apply to the head(controller) entity first before
 * the children and it's apply to the children in an ascending order of their indexes.
 * @param contP Pointer to the head entity or node to transverse
 * @param funcToApply A callback to be called on each entity in the tree
 * @param funcArgs Other arguments for the callback function
 * @param lstack An array of pointers to flEntity to be use for transversing the tree
 * since this method does not use recursion.
 * @note if $funcToApply is NULL and $funcArgs is not NULL, this function assumes that
 * the user wants to apply the tick method of each entity and hence will interpret 
 * $funcArgs as a pointer to $flEntityTickMethodArg object.
 * @note if both $funcToApply and $funcArgs are NULL, this function does nothing.
 * @note if $lstack is NULL, this function uses it's internal static stack making it
 * multi-threading unsafe.
 */
void flentForeach(const flEntity* contP, void (*funcToApply)(flEntity*, void*), void* funcArgs, flArray* lstack);

/**
 * @brief Call the tick method of each entity in the controller-component(s) tree
 * 
 * @param contP Pointer to the head entity or controller to transverse
 * @param ct The latest time in ms before this function was called
 * @param dt The time difference in milliseconds of the latest call and the previous 
 * call of this function
 * @param lstack An array of flEntity to be use for transversing the tree
 * since this function does not use recursion.
 * @note This function is a friendly wrapper for $flentForeach @see $flentForeach
 */
void flentTick(const flEntity* contP, flInt_t ct, flInt_t dt, int8_t syscmd, const void* syscmdArgs, flArray* lstack);

/**
 * @brief Causes all entities on the controller-component(s) tree to perform a cleanup operation.
 * @param contPP 
 * @param lstack @see $flentForeach
 */
void flentFree(flEntity** contPP, flArray* lstack);

#endif//FLENTITYHEADERH_INCLUDED