#ifndef FLENTIOPDTYPEHEADERH_INCLUDED
#define FLENTIOPDTYPEHEADERH_INCLUDED

#include<stdint.h>
#include<stdbool.h>
#include<stddef.h>

#include"flTypes.h"

typedef int8_t flentiopDataType_t;
typedef int8_t flentiopDtype_t;

typedef uint8_t flentiopDataTypeCount_t;
typedef uint8_t flentiopDTC_t;

#define flentiopDTYPE_NIL       0
#define flentiopDTYPE_BOOL      120
#define flentiopDTYPE_BYTS      121
#define flentiopDTYPE_INT       122
#define flentiopDTYPE_NUM       123
#define flentiopDTYPE_STR       124
#define flentiopDTYPE_JSON      125

#define flentiopDTYPE_DPTR      126
typedef struct flentiopDptr flentiopDptr;
struct flentiopDptr{
    void* const data;
    #define flentiopDptrSetData(dp, _dataPtr, _dataSize) do{\
        ( *(void**)(&(dp)->data) = _dataPtr );\
        ( *(size_t*)(&(dp)->dataSize) = _dataSize );\
    }while(0)

    const size_t dataSize;

    void * const _props;
    #define _flentiopDptrSetProps(dp, props) ( *(void**)(&(dp)->_props) = props )

    flEntity * const _srcEnt;

    const int _lsp;
    void (* const _donecb)(flentiopDptr* dp);

    #define _flentiopDptrSetLsp(dp, lsp) ( *(int*)( &(dp)->_lsp ) = lsp )

    #define _flentiopDptrIncLsp(dp) _flentiopDptrSetLsp(dp, (dp)->_lsp+1)

    #define _flentiopDptrDecLsp(dp) _flentiopDptrSetLsp(dp, (dp)->_lsp-1)

    #define _flentiopDptrInit(srcEnt, dataPtr, _dataSize, donecb)\
        {\
            .data = dataPtr, .dataSize = _dataSize,\
            ._props = NULL,\
            ._srcEnt = srcEnt, ._lsp = 0, ._donecb = donecb\
        }
};

#endif
