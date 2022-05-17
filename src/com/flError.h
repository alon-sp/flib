#ifndef FLERRORHEADERH_INCLUDED
#define FLERRORHEADERH_INCLUDED

#include"flConstants.h"
#include"stddef.h"

void flerrSetCallback(void (*errorCallback)(const char*));

void flerrHandle(const char* errstr);

#endif