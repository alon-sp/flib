#ifndef FLSOKEADERH_INCLUDED
#define FLSOKEADERH_INCLUDED

#include<mongoose.h>

#include"fl.h"

//->flsk
//==========================================================================================
typedef uint32_t flskID_t;

typedef struct flskMessage flskMessage;
typedef struct flSocket flSocket;

typedef void (* flskRun_tf)(flSocket* sok); 
#define flskRun(sok) (sok)->_run(sok)

typedef size_t (* flskSend_tf)(flSocket* sok, flskMessage* msg);
#define flskSend(sok, msg) (sok)->_send(sok, msg)

typedef void (* flskClosec_tf)(flSocket* sok, flskMessage* msg);
#define flskClosec(sok, msg) (sok)->_closec(sok, msg)

typedef void (* flskFree_tf)(flSocket* sok);
#define flskFree(sok) (sok)->_free(sok)

typedef void (* flskRecvCb_tf)(flSocket* sok, flskMessage msg);

struct flSocket{
    flskRun_tf _run;
    flskSend_tf _send;
    flskClosec_tf _closec;
    flskFree_tf _free;

    flskRecvCb_tf recvCb;//User-defined callback function

    void* _;//api specific properties
};
#define flSocket_ ._run = NULL, ._send = NULL, ._closec = NULL, ._free = NULL, .recvCb = NULL

struct flskMessage{
    void* _;//api specific properties
    const uint8_t* data;
    size_t dataLen;
};
#define flskMessage_ ._ = NULL, .data = NULL, .dataLen = 0

#define flskGetDeviceKey(sok, uint8Buffer) ( *(flskID_t*)(uint8Buffer) )
#define flskGetMsgHeader(sok, uint8Buffer) ( *(flskID_t*)(uint8Buffer+sizeof(flskID_t)) )

//->dvws
//==========================================================================================
flSocket* flskdvwsNew(const char* rootDir, const char* url, flskRecvCb_tf recvCb);

#endif