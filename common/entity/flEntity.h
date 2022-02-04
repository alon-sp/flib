#ifndef FLENTITYH_INCLUDED
#define FLENTITYH_INCLUDED

#include"../flConstants.h"

#include"../container/flArray.h"

typedef struct flEntity flEntity;

typedef void (* flentPretick_tf)(flEntity* self);

typedef void (* flentTick_tf)(flEntity* self, flUlint_t ct, flUlint_t dt);
//typedef void (*fleMsgFromEntity_tf)()

struct flEntity{
  const int8_t etype;

  const flEntity * const ui2d;
  const flEntity * const ui3d;
  const flEntity * const env;

  void (* const pretick)(flEntity* self);

  void (* const tick)(flEntity* self, flUlint_t ct, flUlint_t dt);

  const void * const currentTickInputs;
  const void * const _nextTickInputs;

  const void* const props;

  const flArray* const entities;
};


#endif//FLENTITYH_INCLUDED