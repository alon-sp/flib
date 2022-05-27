
#define RAYMATH_IMPLEMENTATION

#include"flm.h"

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