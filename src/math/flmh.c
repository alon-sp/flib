
#define RAYMATH_IMPLEMENTATION

#include"flmh.h"

bool flmhv3CompareEqual(flmhVector3 v1, flmhVector3 v2){
    return
        fabs( v1.x - v2.x ) <= flmEPSILON && 
        fabs( v1.y - v2.y ) <= flmEPSILON && 
        fabs( v1.z - v2.z ) <= flmEPSILON  ;
}

bool flmhv2CompareEqual(flmhVector2 v1, flmhVector2 v2){
    return
        fabs( v1.x - v2.x ) <= flmEPSILON && 
        fabs( v1.y - v2.y ) <= flmEPSILON ;
}

flmhOrthonormalBasis flmhobNewPTU(flmhVector3 pos, flmhVector3 target, flmhVector3 up){

    flmhOrthonormalBasis ob = { .pos = pos, .y = up, .z = flmhv3Subtract(target, pos) };

    flmhobOrthonormalize(&ob);

    return ob;
}

void flmhobOrthonormalize(flmhOrthonormalBasis* ob){
    ob->z =  flmhv3Normalize(ob->z);
    ob->x =  flmhv3Normalize(flmhv3CrossProduct(ob->y, ob->z)) ;
    ob->y =   flmhv3Normalize(flmhv3CrossProduct(ob->z, ob->x));
}

flmhMatrix flmhobGetTransform(flmhOrthonormalBasis* ob){
    flmhVector3 X = ob->x, Y = ob->y, Z = ob->z, T = ob->pos;

    return (flmhMatrix){
        X.x,  Y.x,  Z.x,  T.x,
        X.y,  Y.y,  Z.y,  T.y,
        X.z,  Y.z,  Z.z,  T.z,
          0,    0,    0,    1
    };
}

flmhMatrix flmhobGetViewTransform(flmhOrthonormalBasis* ob){

    #define _fltDot(v1, v2) ((v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z)

    flmhVector3 X = ob->x, Y = ob->y, Z = ob->z, T = ob->pos;

    //This is the result of translating the basis to the origin, performing an inverse(transpose)
    //transform on the orthogonal matrix form by the basis and writing the result(row major)

    return (flmhMatrix){
         -X.x,  -X.y,  -X.z,   _fltDot(X, T),
          Y.x,   Y.y,   Y.z,  -_fltDot(Y, T),
         -Z.x,  -Z.y,  -Z.z,   _fltDot(Z, T),
            0,     0,     0,               1
    };

    #undef _fltDot
}

void flmhobRotateX(flmhOrthonormalBasis* ob, float angle){
    flmhMatrix rmat = flmhmtRotate(ob->x, angle);
    ob->y =  flmhv3Transform(ob->y, rmat);
    ob->z =  flmhv3Transform(ob->z, rmat);
}

void flmhobRotateY(flmhOrthonormalBasis* ob, float angle){
    flmhMatrix rmat = flmhmtRotate(ob->y, angle);
    ob->x =  flmhv3Transform(ob->x, rmat);
    ob->z =  flmhv3Transform(ob->z, rmat);
}

void flmhobRotateZ(flmhOrthonormalBasis* ob, float angle){
    flmhMatrix rmat = flmhmtRotate(ob->z, angle);
    ob->x =  flmhv3Transform(ob->x, rmat);
    ob->y =  flmhv3Transform(ob->y, rmat);
}

void flmhobRotate(flmhOrthonormalBasis* ob, flmhVector3 axis, float angle){
    flmhMatrix rmat = flmhmtRotate(axis, angle);
    ob->x =  flmhv3Transform(ob->x, rmat);
    ob->y =  flmhv3Transform(ob->y, rmat);
    ob->z =  flmhv3Transform(ob->z, rmat);
}

void flmhobOrbitXZ(flmhOrthonormalBasis* ob, float radius, float angle){
    flmhVector3 center = flmhv3Subtract(ob->pos, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));

    flmhobRotateY(ob, angle);
    ob->pos =  flmhv3Add(center, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));
}

void flmhobOrbitYZ(flmhOrthonormalBasis* ob, float radius, float angle){
    flmhVector3 center = flmhv3Subtract(ob->pos, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));

    flmhobRotateX(ob, angle);
    ob->pos =  flmhv3Add(center, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));
}

void flmhobOrbitXY(flmhOrthonormalBasis* ob, float radius, float angle){
    flmhVector3 center = flmhv3Subtract(ob->pos, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));

    flmhobRotateZ(ob, angle);
    ob->pos = flmhv3Add(center, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));
}

void flmhobOrbitLR(flmhOrthonormalBasis* ob, float radius, float angle){
    flmhVector3 sphereCenter = flmhv3Subtract(ob->pos, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));
    flmhobRotateGY(ob, angle);
    ob->pos = flmhv3Add(sphereCenter, flmhv3Negate(flmhv3MultiplyScalar(ob->z, radius)));
}