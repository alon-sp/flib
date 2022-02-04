#ifndef FLERRORHEADERH_INCLUDED
#define FLERRORHEADERH_INCLUDED

#include"flConstants.h"

/**
 * @brief This callback function is called whenever an error occur in any function or module
 * 
 */
extern void (*flOnError)(const char* errorLog);

void flerrSetCallback(void (*errorCallback)(const char*));

void flerrSetLogBufferSize(int logsize);

bool flerrInit();

void flerrCleanup();

void flerrPushStr(const char* errstr);

void flerrPushInt(int num);

void flerrHandle(const char* errstr);

#endif