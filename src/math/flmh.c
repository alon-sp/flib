
#define RAYMATH_IMPLEMENTATION

#include"flmh.h"

bool Vector3CompareEqual(Vector3 v1, Vector3 v2){
    return
        fabs( v1.x - v2.x ) <= flmEPSILON && 
        fabs( v1.y - v2.y ) <= flmEPSILON && 
        fabs( v1.z - v2.z ) <= flmEPSILON  ;
}

bool Vector2CompareEqual(Vector2 v1, Vector2 v2){
    return
        fabs( v1.x - v2.x ) <= flmEPSILON && 
        fabs( v1.y - v2.y ) <= flmEPSILON ;
}

void flmhobCopy(flmhOrthonormalBasis* dest, flmhOrthonormalBasis src){
    memcpy(dest, &src, sizeof(src));
}

flmhOrthonormalBasis flmhobNewPTU(Vector3 pos, Vector3 target, Vector3 up){

    flmhOrthonormalBasis ob = flmhobInit();
    flmhobSetPos(&ob, pos);
    _flmhobSetY(&ob, up);
    _flmhobSetZ(&ob, Vector3Subtract(target, pos) );

    flmhobOrthonormalize(&ob);

    return ob;
}

void flmhobOrthonormalize(flmhOrthonormalBasis* ob){
    _flmhobSetZ(ob, Vector3Normalize(ob->z));
    _flmhobSetX(ob, Vector3Normalize(Vector3CrossProduct(ob->y, ob->z))  );
    _flmhobSetY(ob,  Vector3Normalize(Vector3CrossProduct(ob->z, ob->x)) );
}

float16 _flmhobGetTransform(flmhOrthonormalBasis* ob){
    Vector3 X = ob->x, Y = ob->y, Z = ob->z, T = ob->pos;

    return (float16){.v = {
        X.x, X.y, X.z, 0,
        Y.x, Y.y, Y.z, 0,
        Z.x, Z.y, Z.z, 0,
        T.x, T.y, T.z, 1
    }};
}

float16 _flmhobGetViewTransform(flmhOrthonormalBasis* ob){

    #define _fltDot(v1, v2) ((v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z)

    Vector3 X = ob->x, Y = ob->y, Z = ob->z, T = ob->pos;

    //This is the result of translating the basis to the origin, performing an inverse(transpose)
    //transform on the orthogonal matrix form by the basis and writing the result using column
    //major odering.
    return (float16){.v = {
         -X.x,            Y.x,            -Z.x,            0,
         -X.y,            Y.y,            -Z.y,            0,
         -X.z,            Y.z,            -Z.z,            0,
         _fltDot(X, T),  -_fltDot(Y, T),  _fltDot(Z, T),   1
    }};

    #undef _fltDot
}

void flmhobRotateX(flmhOrthonormalBasis* ob, float angle){
    Matrix rmat = MatrixRotate(ob->x, angle);
    _flmhobSetY(ob, Vector3Transform(ob->y, rmat));
    _flmhobSetZ(ob, Vector3Transform(ob->z, rmat));
}

void flmhobRotateY(flmhOrthonormalBasis* ob, float angle){
    Matrix rmat = MatrixRotate(ob->y, angle);
    _flmhobSetX(ob, Vector3Transform(ob->x, rmat));
    _flmhobSetZ(ob, Vector3Transform(ob->z, rmat));
}

void flmhobRotateZ(flmhOrthonormalBasis* ob, float angle){
    Matrix rmat = MatrixRotate(ob->z, angle);
    _flmhobSetX(ob, Vector3Transform(ob->x, rmat));
    _flmhobSetY(ob, Vector3Transform(ob->y, rmat));
}

void flmhobRotate(flmhOrthonormalBasis* ob, Vector3 axis, float angle){
    Matrix rmat = MatrixRotate(axis, angle);
    _flmhobSetX(ob, Vector3Transform(ob->x, rmat));
    _flmhobSetY(ob, Vector3Transform(ob->y, rmat));
    _flmhobSetZ(ob, Vector3Transform(ob->z, rmat));
}

void flmhobOrbitXZ(flmhOrthonormalBasis* ob, float radius, float angle){
    Vector3 center = Vector3Subtract(ob->pos, Vector3Negate(Vector3MultiplyScalar(ob->z, radius)));

    flmhobRotateY(ob, angle);
    flmhobSetPos(ob, Vector3Add(center, Vector3Negate(Vector3MultiplyScalar(ob->z, radius))));
}

void flmhobOrbitYZ(flmhOrthonormalBasis* ob, float radius, float angle){
    Vector3 center = Vector3Subtract(ob->pos, Vector3Negate(Vector3MultiplyScalar(ob->z, radius)));

    flmhobRotateX(ob, angle);
    flmhobSetPos(ob, Vector3Add(center, Vector3Negate(Vector3MultiplyScalar(ob->z, radius))));
}

void flmhobOrbitXY(flmhOrthonormalBasis* ob, float radius, float angle){
    Vector3 center = Vector3Subtract(ob->pos, Vector3Negate(Vector3MultiplyScalar(ob->z, radius)));

    flmhobRotateZ(ob, angle);
    flmhobSetPos(ob, Vector3Add(center, Vector3Negate(Vector3MultiplyScalar(ob->z, radius))));
}