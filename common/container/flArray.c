
#include"fl.h"
#include"flArray.h"

inline static void _flarrSetLength(flArray* arr, flint_t _length){
    *( (flint_t*)(&arr->length) ) = _length;
}

inline static void _flarrSetCapacity(flArray* arr, flint_t _capacity){
    *( (flint_t*)(&arr->capacity) ) = _capacity;
}

inline static void _flarrSetElemSize(flArray* arr, flint_t _elemSize){
    *( (flint_t*)(&arr->elemSize) ) = _elemSize;
}

inline static void _flarrSetData(flArray* arr, void* _data){
    *( (void**)(&arr->data) ) = _data;
}

bool flarrAllocCapacity(flArray* arr, flint_t newCapacity){
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

bool flarrSetLength(flArray* arr, flint_t newLength){
    if(newLength < 0) newLength = 0;

    if(newLength >= arr->capacity && !flarrAllocCapacity(arr, newLength)) return false;
    
    _flarrSetLength(arr, newLength);

    return true;
}

flArray* flarrNew(flint_t initialCapacity, flint_t sizeOfDatatype){
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

void flarrFree(flArray* flarr){
    if(!flarr) return;

    if(flarr->data){
        flmemFree(flarr->data);
        _flarrSetData(flarr, NULL);
    }

    flmemFree(flarr);
}

void* flarrPush(flArray* flarr, const void* dataBytesPtr){
    if(flarr->length >= flarr->capacity){
        if(!flarrAllocCapacity(flarr, flarr->capacity*2)) return NULL;
    }
    _flarrPut(flarr, flarr->length, dataBytesPtr);
    _flarrSetLength(flarr, flarr->length+1);

    return _flarrGet(flarr, flarr->length-1);
}

void* flarrPushs(flArray* flarr, const void* dataBytesPtr, flint_t elemCount){
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

void flarrPops(flArray* flarr, flint_t n){
    if(n < 0){
        flarrShift(flarr, n);
    }else{
        flarrSetLength(flarr, flarr->length-n);
    }
}

void* flarrGet(flArray* flarr, flint_t index){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrGet !1");
        return NULL;
    }
    return _flarrGet(flarr, index);
}

flint_t flarrGets(flArray* flarr, flint_t index, flint_t count, void* destBuf){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrGet !1");
        return 0;
    }

    if( index+count > flarr->length ) count = flarr->length-index;

    memcpy( destBuf, _flarrGet(flarr, index), count*flarr->elemSize );

    return count;
}

inline static void shiftLeft(flArray* flarr, flint_t count){
    flint_t srcElemCount = flarr->length - count;
    if(srcElemCount <= 0) return;
    
    memcpy(_flarrGet(flarr, 0), _flarrGet(flarr, count), srcElemCount*flarr->elemSize);
}

inline static void shiftRight(flArray* flarr, flint_t count){
    flint_t srcElemCount = flarr->length - count;
    if(srcElemCount <= 0) return;

    memcpy(_flarrGet(flarr, count), _flarrGet(flarr, 0), srcElemCount*flarr->elemSize);
}

void flarrShift(flArray* flarr, flint_t n){
    if( n < 0) shiftLeft(flarr, -1*n);
    else shiftRight(flarr, n);
}

void flarrShiftAndFit(flArray* flarr, flint_t n){
    if( n < 0){
        flint_t count = -1*n;
        shiftLeft(flarr, count);
        flarrSetLength(flarr, flarr->length-count);
    }else{
        flarrSetLength(flarr, flarr->length+n);
        shiftRight(flarr, n);
    }
}

// void* flarrFind(flArray* flarr, const void * dataBytesPtr){
//     for(flint_t i = 0; i<flarr->length; i++){
//         if(memcmp(dataBytesPtr, _flarrGet(flarr, i), flarr->elemSize) == 0 ){
//             return _flarrGet(flarr, i);
//         }
//     }

//     return NULL;
// }

void* flarrPut(flArray* flarr, flint_t index, const void* dataBytesPtr){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrPut !1");
        return NULL;
    }

    _flarrPut(flarr, index, dataBytesPtr);

    return _flarrGet(flarr, index);
}

void* flarrPuts(flArray* flarr, flint_t index, const void* dataBytesPtr, flint_t count){
    if(index < 0) index = 0;

    if(index >= flarr->length){
        flerrHandle("\nIOB flarrPuts !1");
        return NULL;
    }

    flint_t overflowCount = ( index+count > flarr->length )? index+count - flarr->length : 0;
    if(overflowCount) count = count - overflowCount;

    _flarrPuts(flarr, index, dataBytesPtr, count);
    if(overflowCount) flarrPushs(flarr, (flbyt_t*)dataBytesPtr + count*flarr->elemSize, overflowCount);

    return _flarrGet(flarr, index);
}

/*----------STRING PROCESSING UTILS----------*/
const char* flarrstrPush(flArray* chArr, const char* strv){

    flarrPushs(chArr, strv, strlen(strv)+1/*include null char*/);

    _flarrSetLength(chArr, chArr->length-1);/*exclude the null character*/

    return _flarrstrCstr(chArr);
}

char flarrstrPop(flArray* chArr){
    char c = *((char*)_flarrPop(chArr));
    _flarrPut(chArr, chArr->length, "");

    return c;
}

const char* flarrstrCstr(flArray* chArr){
    return _flarrstrCstr(chArr);
}