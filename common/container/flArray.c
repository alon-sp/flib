
#include"../fl.h"
#include"flArray.h"

inline static void _flarrSetLength(flArray* arr, flInt_t _length){
    *( (flInt_t*)(&arr->length) ) = _length;
}

inline static void _flarrSetCapacity(flArray* arr, flInt_t _capacity){
    *( (flInt_t*)(&arr->capacity) ) = _capacity;
}

inline static void _flarrSetElemSize(flArray* arr, flInt_t _elemSize){
    *( (flInt_t*)(&arr->elemSize) ) = _elemSize;
}

inline static void _flarrSetData(flArray* arr, void* _data){
    *( (void**)(&arr->data) ) = _data;
}

bool flarrAllocCapacity(flArray* arr, flInt_t newCapacity){
    if(newCapacity <= 0) newCapacity = 1;

    void* newData = flmemRealloc(arr->data, newCapacity*arr->elemSize);
    if(!newData){
        flerrHandle("\nMEMf flarrAllocCapacity !1");
        return false;
    }

    _flarrSetCapacity(arr, newCapacity);
    if(arr->length > newCapacity) _flarrSetLength(arr, newCapacity);
    _flarrSetData(arr, newData);

    return true;
}

bool flarrSetLength(flArray* arr, flInt_t newLength){
    if(newLength < 0) newLength = 0;

    if(newLength >= arr->capacity && !flarrAllocCapacity(arr, newLength)) return NULL;
    
    _flarrSetLength(arr, newLength);
}

flArray* flarrNew(flInt_t initialCapacity, flInt_t sizeOfDatatype){
    flArray* arr = (flArray*)flmemMalloc(sizeof(flArray));

    if(!arr){
        flerrHandle("\nMEMf flarrNew !1");
        return NULL;
    }

    _flarrSetLength(arr, 0);
    _flarrSetCapacity(arr, 0);
    _flarrSetElemSize(arr, sizeOfDatatype);
    _flarrSetData(arr, NULL);

    if(!initialCapacity){
        return arr;
    }

    if(!flarrAllocCapacity(arr, initialCapacity)){
        flmemFree(arr);
        return NULL;
    }

    return arr;
}

void flarrFree(flArray** flarr){
    if(!(flarr && *flarr)) return;

    if((*flarr)->data){
        flmemFree((*flarr)->data);
        _flarrSetData(*flarr, NULL);
    }

    flmemFree(*flarr);
    *flarr = NULL;
}

void* flarrPush(flArray* flarr, const void* dataBytesPtr){
    if(flarr->length >= flarr->capacity){
        if(!flarrAllocCapacity(flarr, flarr->capacity*2)) return NULL;
    }
    _flarrPut(flarr, flarr->length, dataBytesPtr);
    _flarrSetLength(flarr, flarr->length+1);

    return _flarrGet(flarr, flarr->length-1);
}

void* flarrPushs(flArray* flarr, const void* dataBytesPtr, flInt_t elemCount){
    if(flarr->length+elemCount >= flarr->capacity){
        if(!flarrAllocCapacity(flarr, flarr->capacity*2 + elemCount)) return NULL;
    }
    _flarrPuts(flarr, flarr->length, dataBytesPtr, elemCount);
    _flarrSetLength(flarr, flarr->length+elemCount);

    return _flarrGet(flarr, flarr->length-elemCount);
}

void* flarrPop(flArray* flarr){
    return _flarrPop(flarr);
}

void* flarrGet(flArray* flarr, flInt_t index){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrGet !1");
        return NULL;
    }
    return _flarrGet(flarr, index);
}

// void* flarrFind(flArray* flarr, const void * dataBytesPtr){
//     for(flInt_t i = 0; i<flarr->length; i++){
//         if(memcmp(dataBytesPtr, _flarrGet(flarr, i), flarr->elemSize) == 0 ){
//             return _flarrGet(flarr, i);
//         }
//     }

//     return NULL;
// }

void* flarrPut(flArray* flarr, flInt_t index, const void* dataBytesPtr){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrPut !1");
        return NULL;
    }

    _flarrPut(flarr, index, dataBytesPtr);

    return _flarrGet(flarr, index);
}

/*----------STRING PROCESSING UTILS----------*/
const char* flarrStrPush(flArray* chArr, const char* strv){

    flarrPushs(chArr, strv, strlen(strv)+1/*include null char*/);

    _flarrSetLength(chArr, chArr->length-1);/*exclude the null character*/

    return _flarrStrCstr(chArr);
}

char flarrStrPop(flArray* chArr){
    char c = *((char*)_flarrPop(chArr));
    _flarrPut(chArr, chArr->length, "");

    return c;
}

const char* flarrStrCstr(flArray* chArr){
    return _flarrStrCstr(chArr);
}