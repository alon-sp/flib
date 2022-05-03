#ifndef FLENTIOPDTYPEHEADERH_INCLUDED
#define FLENTIOPDTYPEHEADERH_INCLUDED

#include<stdint.h>
#include<stdbool.h>
#include<stddef.h>

#include"flTypes.h"

typedef uint8_t flentiopDataType_t;
typedef uint8_t flentiopDtype_t;

typedef int8_t flentiopDataTypeCount_t;
typedef int8_t flentiopDTC_t;

#define flentiopDTYPE_NIL       0
#define flentiopDTYPE_BOOL      201
#define flentiopDTYPE_BYTS      202
#define flentiopDTYPE_INT       203
#define flentiopDTYPE_NUM       204
#define flentiopDTYPE_STR       205
#define flentiopDTYPE_JSON      206

#define flentiopDTYPE_DPTR      207
typedef struct flentiopDptr flentiopDptr;
struct flentiopDptr{
    void* data;
    size_t dataSize;

    void * const _srcBuf;
    const size_t _srcBufSize;
    flEntity * const _srcEnt;

    const int _lsp;
    void (* const _donecb)(flentiopDptr* dp);

    #define _flentiopDptrSetLsp(dp, lsp) ( *(int*)( &(dp)->_lsp ) = lsp )

    #define _flentiopDptrIncLsp(dp) _flentiopDptrSetLsp(dp, (dp)->_lsp+1)

    #define _flentiopDptrDecLsp(dp) _flentiopDptrSetLsp(dp, (dp)->_lsp-1)

    #define _flentiopDptrInit(srcEnt, dataPtr, dataSize, donecb)\
        {\
            .data = dataPtr, .dataSize = dataSize,\
            ._srcBuf = dataPtr, ._srcBufSize = dataSize,\
            ._srcEnt = srcEnt, ._lsp = 0, ._donecb = donecb\
        }

    #define _flentiopDptrUpdate(dp, newDataPtr)\
        do{\
            (dp)->data = (void*)( (char*)(newDataPtr) + ((char*)(dp)->_srcBuf - (char*)(dp)->data) );\
            *(void**)(&(dp)->_srcBuf) = (newDataPtr);\
        }while(0)
};

#endif
