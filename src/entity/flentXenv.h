#ifndef FLENTXENVHEADERH_INCLUDED
#define FLENTXENVHEADERH_INCLUDED

#include"flTypes.h"
#include"flArray.h"

struct flentXenv{
  flArray * const entities;
  #define _flentxevSetEntities(xenv, ents) *( (flArray **)(&(xenv)->entities) ) = ents

  //The time in milliseconds of the latest call of the tick method
  const flint_t tickCT;
  #define  _flentxevSetTickCT(xenv, ct) *( (flint_t*)(&(xenv)->tickCT) ) = ct

  //The time difference in milliseconds of the latest call and the previous 
  //call of the tick method
  const flint_t tickDT;
  #define  _flentxevSetTickDT(xenv, dt) *( (flint_t*)(&(xenv)->tickDT) ) = dt

};

/**
 * @brief Create a new execution environment
 *  @param initialEntityCount The initial number of entities
 */
flentXenv* flentxevNew(flint_t initialEntityCount);

/**
 * @brief Destroy/delete the given environment.
 * @param xenv 
 * @param freeEnts If true, all entities within this environment will also be deleted.
 */
void flentxevFree(flentXenv* xenv, bool freeEnts);

/**
 * @brief To be called every time frame to run the environment.
 * @param xenv
 * @param ct The latest time before this function was called
 * @param dt The time difference of the latest call and the previous call of this function.
 * @note if $ct and $dt are zero, $flMillis will be use to obtain these values.
 */
void flentxevTick(flentXenv* xenv, flint_t ct, flint_t dt);

/**
 * @brief Add the given entity($ent) to the environment($xenv) if it($ent) doesn't
 * exist in the environment.
 * @param xenv
 * @param ent
 * @return true if the entity alread existed or was added | false otherwise
 */
bool flentxevAddEntity(flentXenv* xenv, flEntity* ent);

/**
 * @brief Unlink and remove the given entity($ent) from the environment($xenv) if
 * found.
 * @note This method does not delete the given entity
 * @param xenv
 * @param ent
 */
void flentxevRemoveEntity(flentXenv* xenv, flEntity* ent);

#endif