#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include"../flConstants.h"

#include"../container/flArray.h"

typedef struct flEntity flEntity;

typedef void (*flentTick_tf)(flEntity* self, flUlint_t ct, flUlint_t dt);
typedef const void* (*flentInput_tf)(flEntity* self, int8_t inputType, flInt_t propid, const void* propv, flEntity* callerEnt);

struct flEntity{
  /**
   * @brief The classification code of this entity
   * 
   */
  const flEntCC_t ccode;

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
   * Because entities are intended to update their internal state(s) 
   * only during the tick function execution; any input (such as SET, POST) which may
   * cause an entity to alter it's internal state must be store temporarily and
   * handle during the tick function execution. This buffer($uinbuf) serve this purpose. 
   * The memory layout of this buffer when it's not empty is as follow:
   * uinbuf                                                    uinbuf.length
   * |-----IN1 Len-----|-----IN2 Len-----|...|-----INN Len-----|
   * IN1B              IN2B                  INNB      
   * 
   * IN1B                                                                                                        IN1 Len
   * |-callerEntityPtr:flEntity*-|-type:int8_t-[|-propid:flInt_t-|-propvSize:flInt_t-|-propv:propvSize bytes-]|
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
  void (* const tick)(flEntity* self, flUlint_t ct, flUlint_t dt);

  /**
   * @brief Provide a means of communicating with this entity from the outside world.
   * It's intended that communication take place between this entity and it's parent(environment),
   * or between this entity and it's children or between this entity and other procedures but
   * not between this entity and any of it's siblings directly.
   * @param self This entity itself
   * @param inputType The type of input: whether GET, SET, POST, or others
   * @param propid The id or name of the property or command
   * @param propv The value or argument of this property or command
   * @param callerEnt The entity that called this method. It's value can be NULL when necessary
   * @return Result of the operation | NULL
   */
  const void* (* const input)(flEntity* self, int8_t inputType, flInt_t propid, const void* propv, flEntity* callerEnt);
  
};

#endif//FLENTITYHEADERH_INCLUDED