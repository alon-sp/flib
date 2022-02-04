
#include"flError.h"
#include"./container/flArray.h"

static void (*flerrOnError)(const char* LogBuffer) = NULL;

static flArray* LogBuffer = NULL;
static int MaxLogBufferLength = 64;//size in bytes

static void flerrPutlog(const char* errstr){
    if(!(LogBuffer || errstr)) return;

    //Write one character at a time until the buffer is full
    //This is to prevent the internal buffer of $LogBuffer from being resize
    int i = 0;
    int errstrLen = strlen(errstr);
    while(LogBuffer->length+1 < MaxLogBufferLength && i < errstrLen) {
        flarrPush(LogBuffer, errstr+i++);
    }
    
    flarrPut(LogBuffer, LogBuffer->length, "\0");
}

void flerrSetCallback(void (*errorCallback)(const char*)){
    flerrOnError = errorCallback;
}

void flerrSetLogBufferSize(int logsize){
    if(LogBuffer) return;
    MaxLogBufferLength = logsize;
}

bool flerrInit(){
    if(!LogBuffer){
        LogBuffer = flarrNew(MaxLogBufferLength, sizeof(char));
        if(!LogBuffer) return false;
    }
    return true;
}

void flerrCleanup(){
    flarrFree(&LogBuffer);
}

void flerrPushStr(const char* errstr){
    if( !(LogBuffer || flerrInit()) ) return;

    flerrPutlog(errstr);
}

void flerrHandle(const char* errstr){
    flerrPushStr(errstr);

    if(flerrOnError) flerrOnError((char*)flarrGet(LogBuffer, 0));
}