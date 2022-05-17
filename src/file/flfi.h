#ifndef FLFIHEADERH_INCLUDED
#define FLFIHEADERH_INCLUDED

#include<stdio.h>

#include<errno.h>
extern int errno;

#include"fl.h"

const char* flfiRead(const char* filePath, flLog** errlogPD);


#define flfiFree(fileContentPtr) flmemFree((char*)fileContentPtr)

#endif