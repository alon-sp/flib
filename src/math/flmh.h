#ifndef FLMHEADERH_INCLUDED
#define FLMHEADERH_INCLUDED
#include<stdint.h>
#include<stdbool.h>
#include<string.h>//for memcpy
#include"raymath.h"

#define flmEPSILON 0.000001f

typedef Vector2 flmhVector2;

typedef Vector3 flmhVector3;
#define flmhv3Zero() Vector3Zero()
#define flmhv3Negate(v) Vector3Negate(v)
#define flmhv3Add(v1, v2) Vector3Add(v1, v2)
#define flmhv3Subtract(v1, v2) Vector3Subtract(v1, v2)
#define flmhv3CrossProduct(v1, v2) Vector3CrossProduct(v1, v2)
#define flmhv3Normalize(v) Vector3Normalize(v)
#define flmhv3Transform(v, mat) Vector3Transform(v, mat)

typedef Matrix flmhMatrix;
#define flmhmtIdentity() MatrixIdentity()
#define flmhmtValuePtr(mat) MatrixToFloat(mat)
#define flmhmtRotate(axis, angle) MatrixRotate(axis, angle)
#define flmhmtRotateX(angle) MatrixRotateX(angle)
#define flmhmtRotateY(angle) MatrixRotateY(angle)
#define flmhmtRotateZ(angle) MatrixRotateZ(angle)
#define flmhmtPerspective(fovy, aspect, near, far) MatrixPerspective(fovy, aspect, near, far)

bool flmhv3CompareEqual(flmhVector3 v1, flmhVector3 v2);
bool flmhv2CompareEqual(flmhVector2 v1, flmhVector2 v2);
#define flmhv3MultiplyScalar(v, s) ( (flmhVector3){(v).x*(s), (v).y*(s), (v).z*(s)} )

typedef struct flmhOrthonormalBasis flmhOrthonormalBasis;

struct flmhOrthonormalBasis{
    flmhVector3 pos;
    flmhVector3 x, y, z;
};

#define flmhob_ .pos = (flmhVector3){0, 0, 0}, \
    .x = (flmhVector3){1, 0, 0}, .y = (flmhVector3){0, 1, 0}, .z = (flmhVector3){0, 0, 1}

#define flmhobNew() ( (flmhOrthonormalBasis){flmhob_} )

flmhOrthonormalBasis flmhobNewPTU(flmhVector3 pos, flmhVector3 target, flmhVector3 up);

void flmhobOrthonormalize(flmhOrthonormalBasis* ob);

flmhMatrix flmhobGetTransform(flmhOrthonormalBasis* ob);

flmhMatrix flmhobGetViewTransform(flmhOrthonormalBasis* ob);

//Perform a local x-axis rotation
void flmhobRotateX(flmhOrthonormalBasis* ob, float angle);
//Perform a local y-axis rotation
void flmhobRotateY(flmhOrthonormalBasis* ob, float angle);
//Perform a local z-axis rotation
void flmhobRotateZ(flmhOrthonormalBasis* ob, float angle);

void flmhobRotate(flmhOrthonormalBasis* ob, flmhVector3 axis, float angle);
#define flmhobRotateGX(ob, angle) flmhobRotate(ob, (flmhVector3){1, 0, 0}, angle)
#define flmhobRotateGY(ob, angle) flmhobRotate(ob, (flmhVector3){0, 1, 0}, angle)
#define flmhobRotateGZ(ob, angle) flmhobRotate(ob, (flmhVector3){0, 0, 1}, angle)

void flmhobOrbitXZ(flmhOrthonormalBasis* ob, float radius, float angle);
void flmhobOrbitYZ(flmhOrthonormalBasis* ob, float radius, float angle);
void flmhobOrbitXY(flmhOrthonormalBasis* ob, float radius, float angle);

void flmhobOrbitLR(flmhOrthonormalBasis* ob, float radius, float angle);
#define flmhobOrbitUD(ob, radius, angle) flmhobOrbitYZ(ob, radius, angle)

#endif