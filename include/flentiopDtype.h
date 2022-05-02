#ifndef FLENTIOPDTYPEHEADERH_INCLUDED
#define FLENTIOPDTYPEHEADERH_INCLUDED

#include<stdint.h>
#include<stdbool.h>
#include<stddef.h>

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
typedef int32_t flentiopDlsp_t;
typedef struct{
    void* data;
    size_t dataSize;

    void * const buf_;
    const size_t bufSize_;

    flentiopDlsp_t* const _lsp;
    const bool _lspIsFrozen;
    
    #define flentiopDptrSetLspPtr(dptr, lspPtr)\
        *(flentiopDlsp_t**)(&(dptr)._lspPtr) = (flentiopDlsp_t*)lspPtr

    #define flentiopDptrSetLsp(dptr, lspVal) ( *(dptr)._lspPtr = lspVal )

    #define _flentiopDptrGetLsp(dptr) (*(dptr)._lspPtr)

    #define flentiopDptrIncLsp(dptr) flentiopDptrSetLsp(dptr, _flentiopDptrGetLsp(dptr)+1)

    #define flentiopDptrDecLsp(dptr) flentiopDptrSetLsp(dptr, _flentiopDptrGetLsp(dptr)-1)

    #define flentiopDptrInit(dataPtr, dataSize, lspPtr)\
        {.data = dataPtr, .size = dataSize, ._lspPtr = (flentiopDlsp_t*)lspPtr}
}flentiopDptr;

#endif
