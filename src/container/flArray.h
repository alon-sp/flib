#ifndef FLARRAYHEADERH_INCLUDED
#define FLARRAYHEADERH_INCLUDED

#include"flConstants.h"
#include"flTypes.h"
#include<string.h>
#include<stdarg.h>

/**
 * @brief A simple dynamic array container
 */
typedef struct{
    const flint_t length;//The current number of elements in the array
    #define _flarrSetLength(arr, _length) *( (flint_t*)(&arr->length) ) = _length

    const flint_t capacity;//The number of elements this array can hold without resizing it's buffer
    const flint_t elemSize;//Size in bytes per element of $.data
    void* const data;
}flArray;

/**
 * @brief 
 * Create a new $flArray
 * @param initialCapacity The initial capacity of the array
 * @param sizeOfDatatype The size in bytes of the data type of this array
 * @return flArray* on success | NULL if memory allocation fails
 */
flArray* flarrNew(flint_t initialCapacity, flint_t sizeOfDatatype);

void flarrFree(flArray* flarr);

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
bool flarrAllocCapacity(flArray* flarr, flint_t newCapacity);

/**
 * @note if $newLength is greater than $flarr current capacity, flarrAllocCapacity is called
 * to allocate memory for the desired length.
 * @param flarr 
 * @param newLength 
 * @return true if length was set | false otherwise
 */
bool flarrSetLength(flArray* flarr, flint_t newLength);

/**
 * @brief Read $flarr->elemSize bytes from $dataBytesPtr and write it to $flarr beginning
 * at $flarr[$flarr->length] and increment $flarr->length by 1
 * @param flarr 
 * @param dataBytesPtr 
 * @return void* pointer to the written data in $flarr | NULL if push failed
 */
void* flarrPush(flArray* flarr, const void* dataBytesPtr);

/**
 * @brief Read $elemCount * $flarr->elemSize bytes from $dataBytesPtr and write it to $flarr
 *  beginning at $flarr[$flarr->length] and increment $flarr->length by $elemCount
 * @param flarr 
 * @param dataBytesPtr 
 * @return void* pointer to the first element of $dataBytesPtr in $flarr | NULL if push failed
 */
void* flarrPushs(flArray* flarr, const void* dataBytesPtr, flint_t elemCount);

/**
 * @brief Pop the element at $flarr[$flarr->length] and decrement $flarr->length
 * 
 * @param flarr 
 * @return pointer to the poped element | NULL if $flarr->length was already 0
 */
void* flarrPop(flArray* flarr);

/**
 * @brief If $n is negative, perform a left shift and fit operation.
 * If $n is positive, pops $n elements of the given array
 * @param flarr 
 * @param n 
 */
void flarrPops(flArray* flarr, flint_t n);

/**
 * @brief Get the element at the given index
 * @param flarr 
 * @param index 
 * @return pointer to the element at the given index | NULL if index >= $flarr->length
 */
void* flarrGet(flArray* flarr, flint_t index);

/**
 * @brief Read $count elements of the given array($flarr) and write them to the given destination
 * ($destBuf) beginning at the given index($index) in the array.
 * @note if ${index + count} is greater than the array length, 
 * only (array length - $count) elements will be written to destination.
 * @note This function assume the given destination buffer is large enough to hold
 * $count elements of the given array.
 * @param flarr 
 * @param index 
 * @param count 
 * @param destBuf 
 * @return The total number of elements that were written to the given destination.
 */
flint_t flarrGets(flArray* flarr, flint_t index, flint_t count, void* destBuf);

/**
 * @brief Shift all elements of the array to either left(for negative values of $n)
 * or right(for positive values of $n) by $n elements.
 * @note Discards all elements whose new indexes are out of bound(ie either less than 0
 * or greater than the length of the array)
 * @note This function does not change the length of the given array.
 * @param flarr 
 * @param n 
 */
void flarrShift(flArray* flarr, flint_t n);

/**
 * @brief If $n is negative, shift all elements of the given array to the left by n,
 *  discards elements at negative indexes and resize the array(update it's length) to
 *  fit the new elements count(ie ${flarr->length -> flarr->length-|n|}).
 * If $n is positive, resize the given array(ie ${flarr->length -> flarr->length + n})
 *  and shift all elements of the array to the right by $n thereby maintaining the original
 *  elements of the array.
 * @param flarr 
 * @param n 
 */
void flarrShiftAndFit(flArray* flarr, flint_t n);

// /**
//  * @brief compare each element of $flarr against first $flarr->elemSize bytes of $dataBytesPtr
//  * for equality.
//  * @param flarr 
//  * @param dataBytesPtr 
//  * @return pointer to the first element of $flarr that is equal to $dataBytesPtr | NULL if none
//  */
// void* flarrFind(flArray* flarr, const void * dataBytesPtr);

/**
 * @brief Write the first $flarr->elemSize bytes of $dataBytesPtr at the given index in $flarr
 * @note Call the global error handler function if the given index is out of bound.
 * @param flarr 
 * @param index 
 * @param dataBytesPtr 
 * @return pointer to the written element at the given index | NULL if index >= $flarr->length 
 */
void* flarrPut(flArray* flarr, flint_t index, const void* dataBytesPtr);

/**
 * @brief Write the first ${count*flarr->elemSize} bytes of $dataBytesPtr
 * beginning at the given index in $flarr
 * @note Call the global error handler function if the given index is out of bound.
 * @note If ${count + index > flarr->length} the array length is expanded to accomodate
 * the added element(s).
 * @param flarr 
 * @param index 
 * @param dataBytesPtr 
 * @param count 
 * @return pointer to the first written element at the given index | NULL if index >= $flarr->length 
 */
void* flarrPuts(flArray* flarr, flint_t index, const void* dataBytesPtr, flint_t count);

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

#define _flarrGet(flarr, index) ( ((char*)(flarr)->data) + (index)*(flarr)->elemSize )

#define _flarrPut(flarr, index, dataBytesPtr) memcpy( _flarrGet(flarr, index), dataBytesPtr, flarr->elemSize )

#define _flarrPuts(flarr, index, dataBytesPtr, elemCount) memcpy( _flarrGet(flarr, index), dataBytesPtr, elemCount*flarr->elemSize )

/*----------STRING PROCESSING UTILS----------*/

#define flarrstrNewc(initialStrLen) flarrNew(initialStrLen, sizeof(char))

flArray* flarrstrNew(const char* str);

flArray* flarrstrNews(int argc, ...);

/**
 * @brief Assume $chArr is a character array and append $strv to this array
 * beginning at $chArr->length. 
 * After this operation $chArr->length is updated such that $chArr[$chArr->length] equals
 * the NULL character.
 * @param chArr 
 * @param strv 
 * @return pointer to the entire c-string | NULL if error(memory related) occur
 */
const char* flarrstrPush(flArray* chArr, const char* strv);

/**
 * @brief Assume $chArr is a character array and append $argc c-string to this array
 * beginning at $chArr->length.
 * After this operation $chArr->length is updated such that $chArr[$chArr->length] equals
 * the NULL character.
 * @param chArr 
 * @param argc Total number of c-string passed to this function 
 * @param ... 
 * @return  pointer to the entire c-string | NULL if error(memory related) occur
 */
const char* flarrstrPushs(flArray* chArr, int argc, ...);

/**
 * @brief 
 * same as {(const char*)flarrGet(chArr, 0)}
 * @param chArr 
 * @return c-string
 */
const char* flarrstrCstr(flArray* chArr);

/**
 * @brief Pop the character at $chArr->length-1 and NULL terminate the resulting string
 * 
 * @param chArr 
 * @return the popped character
 */
char flarrstrPop(flArray* chArr);

#define _flarrstrCstr(flarr) _flarrGet(flarr, 0)

#endif