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
#define flentiopTYPE_OM      3/*This is an output port that support multiple input ports*/

#define flentiopTypeToBasicType(type)\
  ((type) == flentiopTYPE_INPUT? flentiopTYPE_INPUT : flentiopTYPE_OUTPUT)

/**
 * @brief Model a port of an entity
 * 
 */
struct flentIOport{
 /**
 * @note if this pointer is NULL, this port is an input only port and as such any attempt to
 * write to it will result in error
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
  //On the other hand if it's an OUTPUT port => this port is not available for writing.
  //Any write or read operation on any such port will result in error
  const bool isBusy;
  #define _flentiopSetIsBusy(iop, bval) *( (bool*)(&(iop)->isBusy) ) = bval

  flEntity * const entity;
  #define flentiopSetEntity(iop, ent) *( (flEntity**)(&(iop)->entity) ) = ent

  const flentiopID_t id;
  #define flentiopSetID(iop, _id) *( (flentiopID_t*)(&(iop)->id) ) = _id

  //For port of type flentiopTYPE_MO, this pointer is  a pointer to an flArray of
  //pointers to flentIOport
  flentIOport * const _linkedPort;
  #define _flentiopSetlinkedPort(iop, lport) *( (flentIOport**)(&(iop)->_linkedPort) ) = lport

  //The total number of data types this port can accept(is compatible with)
  //A value of 0(zero) implies that this port accept all data types and emit no data type
  //This value should be use for debugging/testing purposes only as it prevents proper
  //compability checking.
  const flentiopDTC_t dataTypeCount;
  #define flentiopSetDataTypeCount(iop, dtc) *( (flentiopDTC_t*)(&(iop)->dataTypeCount) ) = dtc

  //The type of port: whether input, output or om
  const flentiopType_t type;
  #define _flentiopSetType(iop, typ) *( (flentiopType_t*)(&(iop)->type) ) = typ

  //User defined properties associated with this port
  //This pointer and all associated resources is managed by the entity/user and as
  //such should be cleaned when $flentsycCLEANUP command is received by the entity.
  void * const props;
  #define flentiopSetProps(iop, _props) *( (void**)(&(iop)->props) ) = _props
};

/**
 * @brief Create a new port
 * @param id The id of this port
 * @param type The type of port to create:(flentiopTYPE_INPUT, flentiopTYPE_OUTPUT)
 * @param dataTypeCount The total number of data types this port can support. A value of zero(0)
 * implies this port can accept all data types whilst a negative value implies this port
 * is for debuggin/testing purpose and as such it's compatible with all ports.
 * @return Pointer to the newly created port.
 */
flentIOport* flentiopNew(flentiopID_t id, flentiopType_t type, flentiopDTC_t dataTypeCount);

/**
 * @brief unlink $iop and clean up the memory associated with it
 * 
 * @param iop A pointer that was obtained through $flentiopNew
 */
void flentiopFree(flentIOport* iop);

/**
 * @brief Link the two given ports if they are compatible
 * IF either of $port1 or $port2 is of type flentiopTYPE_OUTPUT and already has a linkage
 * different from the new link to be created, the said port is upgraded to flentiopTYPE_OM
 * type port and the new link is added to the existing one.
 * 
 * IF either of $port1 or $port2 is of type flentiopTYPE_OM and the new link to be created
 * doesn't already exist, the new link is added to the existing ones. 
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

/**
 * @brief Set the busy status of the given input port to the given boolean value
 * if possible. THE CALLER must always CHECK the returned value of this function
 * in order to avoid error arising from reading from a busy port.
 * @param inputPort a port of type flentiopTYPE_INPUT
 * @param bval
 * @return true if after the call $inputPort is equal to $bval | false if it wasn't possible
 * to change the busy status of $inputPort to the desired.
 */
bool flentiopSetIsBusy(flentIOport* inputPort, bool bval);

//--Functions and micros for reading from port--
//----------------------------------------------

void* flentiopGetBuf(flentIOport* port);
size_t flentiopGetBufSize(flentIOport* port);
flentiopDtype_t flentiopGetDataType(flentIOport* port);
void* flentiopGetData(flentIOport* port);
size_t flentiopGetDataSize(flentIOport* port);

flentiopDtype_t flentiopGetOutputDataType(flentIOport* port);
void* flentiopGetOutputData(flentIOport* port);
size_t flentiopGetOutputDataSize(flentIOport* port);

#endif