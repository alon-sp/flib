#ifndef FLMHEADERH_INCLUDED
#define FLMHEADERH_INCLUDED
#include<stdint.h>
#include<stdbool.h>
#include<string.h>//for memcpy
#include"raymath.h"

#define flmEPSILON 0.000001f

bool Vector3CompareEqual(Vector3 v1, Vector3 v2);
bool Vector2CompareEqual(Vector2 v1, Vector2 v2);
#define Vector3MultiplyScalar(v, s) ( (Vector3){(v).x*(s), (v).y*(s), (v).z*(s)} )

typedef struct flmhOrthonormalBasis flmhOrthonormalBasis;

struct flmhOrthonormalBasis{
    const Vector3 pos;
    const Vector3 x, y, z;

#define _flmhobSetVector3Prop(ob, prop, propVal) (  *(Vector3*)&(ob)->prop = propVal  )
#define flmhobSetPos(ob, val) _flmhobSetVector3Prop(ob, pos, val)
#define _flmhobSetX(ob, val) _flmhobSetVector3Prop(ob, x, val)
#define _flmhobSetY(ob, val) _flmhobSetVector3Prop(ob, y, val)
#define _flmhobSetZ(ob, val) _flmhobSetVector3Prop(ob, z, val)
};

#define flmhobInit() {  .pos = (Vector3){0, 0, 0}, \
    .x = (Vector3){1, 0, 0}, .y = (Vector3){0, 1, 0}, .z = (Vector3){0, 0, 1}  }

#define flmhobNew() ( (flmhOrthonormalBasis)flmhobInit() )

void flmhobCopy(flmhOrthonormalBasis* dest, flmhOrthonormalBasis src);

flmhOrthonormalBasis flmhobNewPTU(Vector3 pos, Vector3 target, Vector3 up);

void flmhobOrthonormalize(flmhOrthonormalBasis* ob);

float16 _flmhobGetTransform(flmhOrthonormalBasis* ob);
#define flmhobGetTransform(ob) _flmhobGetTransform(ob).v

float16 _flmhobGetViewTransform(flmhOrthonormalBasis* ob);
#define flmhobGetViewTransform(ob) _flmhobGetViewTransform(ob).v

//Perform a local x-axis rotation
void flmhobRotateX(flmhOrthonormalBasis* ob, float angle);
//Perform a local y-axis rotation
void flmhobRotateY(flmhOrthonormalBasis* ob, float angle);
//Perform a local z-axis rotation
void flmhobRotateZ(flmhOrthonormalBasis* ob, float angle);

void flmhobRotate(flmhOrthonormalBasis* ob, Vector3 axis, float angle);
#define flmhobRotateGX(ob, angle) flmhobRotate(ob, (Vector3){1, 0, 0}, angle)
#define flmhobRotateGY(ob, angle) flmhobRotate(ob, (Vector3){0, 1, 0}, angle)
#define flmhobRotateGZ(ob, angle) flmhobRotate(ob, (Vector3){0, 0, 1}, angle)

void flmhobOrbitXZ(flmhOrthonormalBasis* ob, float radius, float angle);
void flmhobOrbitYZ(flmhOrthonormalBasis* ob, float radius, float angle);
void flmhobOrbitXY(flmhOrthonormalBasis* ob, float radius, float angle);

void flmhobOrbitLR(flmhOrthonormalBasis* ob, float radius, float angle);
#define flmhobOrbitUD(ob, radius, angle) flmhobOrbitYZ(ob, radius, angle)

#endif