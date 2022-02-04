#include"fl.h"

bool flInit(){
    if(!flerrInit()) return false;
    return true;
}

void flCleanup(){
    flerrCleanup();
}