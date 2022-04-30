#ifndef FLENTIOPORTH_INCLUDED
#define FLENTIOPORTH_INCLUDED

#include"flConstants.h"
#include"flTypes.h"
#include"flError.h"
#include"flArray.h"
#include"flentiopDtype.h"

typedef uint8_t flentiopType_t;
#define flentiopTYPE_INPUT   1
#define flentiopTYPE_OUTPUT  2

/**
 * @brief Model a port of an entity
 * 
 */
struct flentIOport{
 /**
 * @note if this pointer is NULL, this port is an input only port and as such any attempt to
 * write to it will result in error(most likely segmentation fault)
  *The default data layout of this buffer is as shown:
  *|<--sizeof(flentiopDtype_t)-->|<--buffer len -. -->|
  *|--data type------------------|--data--------------|
 */
  flArray * const _obuf;
  #define _flentiopOBUF_PRE_DATA_META_SIZE sizeof(flentiopDtype_t)
  #define _flentiopOBUF_DTYPE_INDEX 0
  #define _flentiopOBUF_DATA_INDEX _flentiopOBUF_PRE_DATA_META_SIZE

  #define _flentiopObufGetBufptr(iop) ( (iop)->_obuf->data )
  #define _flentiopObufGetBufSize(iop) ( (iop)->_obuf->length )

  #define _flentiopObufGetDataType(iop) *(flentiopDtype_t*)_flarrGet((iop)->_obuf, _flentiopOBUF_DTYPE_INDEX) 

  #define _flentiopObufSetDataType(iop, dtype) *(flentiopDtype_t*)_flarrGet(iop->_obuf, _flentiopOBUF_DTYPE_INDEX) = dtype

  #define _flentiopObufGetData(iop) _flarrGet((iop)->_obuf, _flentiopOBUF_DATA_INDEX)

  #define _flentiopObufGetDataSize(iop) ((iop)->_obuf->length - _flentiopOBUF_PRE_DATA_META_SIZE)
  
  #define _flentiopSetObuf(iop, obuf) *( (flArray**)(&(iop)->_obuf) ) = obuf

  //If this is an INPUT port and this flag is true => this port is currently not accepting input
  //On the other hand if it's an OUTPUT or IO port => this port is not available for writing and
  //as such any write operation will result in error.
  const bool isBusy;
  #define _flentiopSetIsBusy(iop, bval) *( (bool*)(&(iop)->isBusy) ) = bval

  flEntity * const entity;
  #define flentiopSetEntity(iop, ent) *( (flEntity**)(&(iop)->entity) ) = ent

  const flentiopID_t id;
  #define flentiopSetID(iop, _id) *( (flentiopID_t*)(&(iop)->id) ) = _id

  flentIOport * const _linkedPort;
  #define _flentiopSetlinkedPort(iop, lport) *( (flentIOport**)(&(iop)->_linkedPort) ) = lport

  //The total number of data types this port can accept(is compatible with)
  //A value of 0(zero) implies all data types and a value less 0 implies this port is
  //a test/debug port and as such is assumed to be compatible with all other ports.
  const flentiopDTC_t dataTypeCount;
  #define flentiopSetDataTypeCount(iop, dtc) *( (flentiopDTC_t*)(&(iop)->dataTypeCount) ) = dtc

  //The type of port: whether input, output or io
  const flentiopType_t type;
  #define flentiopSetType(iop, typ) *( (flentiopType_t*)(&(iop)->type) ) = typ

  //User defined properties associated with this port
  //This pointer and all associated resources is managed by the entity/user and as
  //such should be cleaned when $flentsycCLEANUP command is received by the entity.
  void * const props;
  #define flentiopSetProps(iop, _props) *( (void**)(&(iop)->props) ) = _props
};

/**
 * @brief Create a new port
 * @param id The id of this port
 * @param type The type of port to create:(flentiopTYPE_INPUT, flentiopTYPE_OUTPUT, flentiopTYPE_IO)
 * @return Pointer to the newly created port.
 */
flentIOport* flentiopNew(flentiopID_t id, flentiopType_t type);

/**
 * @brief unlink $iop and clean up the memory associated with it
 * 
 * @param iop A pointer that was obtained through $flentiopNew
 */
void flentiopFree(flentIOport* iop);

/**
 * @brief Link the two given ports ensuring that the two ports are compatible
 * @param port1 
 * @param port2 
 */
bool flentiopLink(flentIOport* port1, flentIOport* port2);

/**
 * @brief Break the connection between the given port and it's target port
 * @param iop 
 * @return pointer to the linked port of $iop before the unlink operation
 */
flentIOport* flentiopUnlink(flentIOport* iop);

//--Functions and micros for writing to port--
//--------------------------------------------

/**
 * @brief 
 * @param port A port of type OUTPUT or IO
 * @param dtype 
 * @param dataPtr 
 * @param dataSize 
 */
void flentiopPut(flentIOport* port, flentiopDtype_t dtype, const void* dataPtr, size_t dataSize);

void flentiopPutb(flentIOport* port, const void* bytesPtr, size_t bytesSize);

#define flentiopAppendData(port, dataPtr, dataSize) if((port)->_obuf) flarrPushs(port->_obuf, dataPtr, dataSize)

/**
 * @brief Clear the output buffer of the given port
 * @param port 
 */
void flentiopClear(flentIOport* port);

//--Functions and micros for reading from port--
//----------------------------------------------

void* flentiopGetBuf(flentIOport* port);
size_t flentiopGetBufSize(flentIOport* port);
flentiopDtype_t flentiopGetDataType(flentIOport* port);
void* flentiopGetData(flentIOport* port);
size_t flentiopGetDataSize(flentIOport* port);

#endif