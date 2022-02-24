#ifndef FLERRORHEADERH_INCLUDED
#define FLERRORHEADERH_INCLUDED

#include"flConstants.h"

void flerrSetCallback(void (*errorCallback)(const char*));

void flerrHandle(const char* errstr);

#endif