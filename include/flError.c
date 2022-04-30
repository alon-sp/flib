
#include"flError.h"

/**
 * @brief This callback function is called whenever an error occur in any function or module
 * 
 */
static void (*flerrOnError)(const char* errstr) = NULL;

void flerrSetCallback(void (*errorCallback)(const char*)){
    flerrOnError = errorCallback;
}

void flerrHandle(const char* errstr){
    if(flerrOnError) flerrOnError(errstr);
}