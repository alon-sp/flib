
#include"../fl.h"
#include"flArray.h"

inline static void _flarrSetLength(flArray* arr, flInt_t length){
    flInt_t* arrLenPtr = (flInt_t*)(&arr->length);
    *arrLenPtr = length;
}

inline static void _flarrSetCapacity(flArray* arr, flInt_t capacity){
    flInt_t* arrCapPtr = (flInt_t*)(&arr->capacity);
    *arrCapPtr = capacity;
}

inline static void _flarrSetElemSize(flArray* arr, flInt_t elemSize){
    flInt_t* arrElemSizePtr = (flInt_t*)(&arr->elemSize);
    *arrElemSizePtr = elemSize;
}

inline static void _flarrSetData(flArray* arr, void* data){
    void** arrDataptr = (void*)(&arr->data);
    *arrDataptr = data;
}

bool flarrAllocCapacity(flArray* arr, flInt_t newCapacity){
    if(newCapacity <= 0) newCapacity = 1;

    void* newData = flmemRealloc(arr->data, newCapacity*arr->elemSize);
    if(!newData){
        flerrHandle("\nMEMf flarrNew !1");
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
    uint32_t strvLenPlusNullChar = strlen(strv)+1;
    if(chArr->length+strvLenPlusNullChar >= chArr->capacity){
        if( !flarrAllocCapacity(chArr,  chArr->capacity*2 + strvLenPlusNullChar) ) return NULL;
    }
    
    char* strvDest = _flarrGet(chArr, chArr->length);
    for(int i = 0; i < strvLenPlusNullChar; i++) *(strvDest+i) = *(strv+i);
    
    //Set the length of the string such that chArr->data[chArr->length] = '\0'
    _flarrSetLength(chArr, chArr->length+strvLenPlusNullChar-1);

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