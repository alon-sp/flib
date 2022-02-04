#ifndef FLARRHEADERH_INCLUDED
#define FLARRHEADERH_INCLUDED

#include"../flConstants.h"
#include<string.h>

/**
 * @brief A simple dynamic array container
 */
typedef struct{
    const flInt_t length;//The current number of elements in the array
    const flInt_t capacity;//The number of elements this array can hold without resizing it's buffer
    const flInt_t elemSize;//Size in bytes per element of $.data
    void* const data;
}flArray;

/**
 * @brief 
 * Create a new $flArray
 * @param initialCapacity The initial capacity of the array
 * @param sizeOfDatatype The size in bytes of the data type of this array
 * @return flArray* on success | NULL if memory allocation fails
 */
flArray* flarrNew(flInt_t initialCapacity, flInt_t sizeOfDatatype);

void flarrFree(flArray** flarr);

/**
 * @brief Resize the internal data buffer of $flarr to $flarr->elemSize * $newCapacity
 * @note if memory could not be allocated for the new capacity, $flarr remains
 * unaffected(ie it's length, capacity and data buffer remain unchanged)
 * @note if resizing is successful and $flarr->length is greater than $newCapacity,
 * $flarr->length -> $newCapacity
 * @param flarr 
 * @param newCapacity 
 * @return true if memory allocation was successful | false otherwise
 */
bool flarrAllocCapacity(flArray* flarr, flInt_t newCapacity);

/**
 * @note if $newLength is greater than $flarr current capacity, flarrAllocCapacity is called
 * to allocate memory for the desired length.
 * @param flarr 
 * @param newLength 
 * @return true if length was set | false otherwise
 */
bool flarrSetLength(flArray* flarr, flInt_t newLength);

/**
 * @brief Read $flarr->elemSize bytes from $dataBytesPtr and write it at $flarr[$flarr->length]
 * and increment $flarr->length
 * @param flarr 
 * @param dataBytesPtr 
 * @return void* pointer to the written data in $flarr | NULL if push failed
 */
void* flarrPush(flArray* flarr, const void* dataBytesPtr);

/**
 * @brief Pop the element at $flarr[$flarr->length] and decrement $flarr->length
 * 
 * @param flarr 
 * @return pointer to the poped element | NULL if $flarr->length was already 0
 */
void* flarrPop(flArray* flarr);

/**
 * @brief Get the element at the given index
 * @param flarr 
 * @param index 
 * @return pointer to the element at the given index | NULL if index >= $flarr->length
 */
void* flarrGet(flArray* flarr, flInt_t index);

/**
 * @brief Write the element at the given index
 * 
 * @param flarr 
 * @param index 
 * @param dataBytesPtr 
 * @return pointer to the written element at the given index | NULL if index >= $flarr->length 
 */
void* flarrPut(flArray* flarr, flInt_t index, const void* dataBytesPtr);

/**
 * @brief These micros are not intended to be use directly; if they must be use,
 * they must be use with extreme care.
 * @note The first argument($flarr) to these micros should not be an expression with
 * side effect because it get evaluated more than once.
 * @note Unlike their wrapper functions above, these micros are not error aware, they assume
 * every thing is fine. For instance, they assume all indexes are valid per the operation involve
 */

#define _flarrPush(flarr, dataBytesPtr)\
    do{\
        if(flarr->length >= flarr->capacity){\
            if(!flarrAllocCapacity(flarr, flarr->capacity*2)) break;\
        }\
        _flarrPut(flarr, flarr->length, dataBytesPtr);\
        flarrSetLength(flarr, flarr->length+1);\
    } while (0)

#define _flarrPop(flarr)\
    ( ( (flarr)->length && flarrSetLength(flarr, (flarr)->length-1) )?\
        _flarrGet(flarr, flarr->length) : NULL )

#define _flarrGet(flarr, index) ( ((char*)(flarr)->data)+ (index)*(flarr)->elemSize )

#define _flarrPut(flarr, index, dataBytesPtr) memcpy( _flarrGet(flarr, index), dataBytesPtr, flarr->elemSize )

/*----------STRING PROCESSING UTILS----------*/
/**
 * @brief Assume $chArr is a character array and append $strv to this array
 * beginning at $chArr->length. 
 * After this operation $chArr->length is updated such that $chArr[$chArr->length] equals
 * the NULL character.
 * @param chArr 
 * @param strv 
 * @return pointer to the resulting c-string | NULL if error(memory related) occur
 */
const char* flarrStrPush(flArray* chArr, const char* strv);

/**
 * @brief 
 * same as {(const char*)flarrGet(chArr, 0)}
 * @param chArr 
 * @return c-string
 */
const char* flarrStrCstr(flArray* chArr);

/**
 * @brief Pop the character at $chArr->length-1 and NULL terminate the resulting string
 * 
 * @param chArr 
 * @return the popped character
 */
char flarrStrPop(flArray* chArr);

#define _flarrStrCstr(flarr) _flarrGet(flarr, 0)

#endif