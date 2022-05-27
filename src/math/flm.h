#ifndef FLMHEADERH_INCLUDED
#define FLMHEADERH_INCLUDED
#include<stdint.h>
#include<stdbool.h>
#include"raymath.h"

#define flmEPSILON 0.000001f

bool Vector3CompareEqual(Vector3 v1, Vector3 v2);
bool Vector2CompareEqual(Vector2 v1, Vector2 v2);

#endif