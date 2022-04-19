#ifndef FLENTITYHEADERH_INCLUDED
#define FLENTITYHEADERH_INCLUDED

#include<stdio.h> //for sprintf
#include<string.h>

#include"../flConstants.h"

#include"../container/flArray.h"
#include"flEntDid.h"
#include"flEntIpn.h"

typedef struct flEntity flEntity;
typedef struct flentXenv flentXenv;
typedef struct flentIOport flentIOport;

typedef flArray flentIOdata;

#define flentiodNew(initialCapacity, sizeOfDatatype) flarrNew(initialCapacity, sizeOfDatatype)

/*--------------------flentIOdata--------------------*/
/*
*The default data layout of an IO data is as shown:
*|<--sizeof(flbyt_t)-->|<--sizeof(flentdid_t)-->|<--buffer len -. -->|
*|--data mode----------|--data id---------------|--data--------------|
*
*If data mode : flentdmoPOSTDP, the data is layout as shown below
*|<--sizeof(flbyt_t)-->|<--sizeof(flentdid_t)-->|<--sizeof(flint_t)-->|<--sizeof(void*)-->|<--sizeof( flentiodDoneCb_tf )-->|<--buffer len -. -->|
*|--data mode----------|--data id---------------|--data size----------|--data pointer-----|--done callback------------------|--callback args-----|
*/
#define flentiodDATA_BUFFER_MIN_SIZE ( sizeof(flbyt_t) + sizeof(flentdid_t) + sizeof(void*) )

#define flentiodMODE_INDEX             0
#define flentiodID_INDEX             ( flentiodMODE_INDEX + sizeof(flbyt_t)              )
#define flentiodDATA_INDEX           ( flentiodID_INDEX + sizeof(flentdid_t)             )
#define flentiodPD_SIZE_INDEX          flentiodDATA_INDEX
#define flentiodPD_PTR_INDEX         ( flentiodPD_SIZE_INDEX + sizeof(flint_t)           )
#define flentiodPD_DONECB_INDEX      ( flentiodPD_PTR_INDEX + sizeof(void*)                 )
#define flentiodPD_DONECB_ARGS_INDEX ( flentiodPD_DONECB_INDEX + sizeof(flvod_tf)  )

#define flentiodGetBuffer(iod)           ( (flbyt_t*)((iod)->data)                        )
#define flentiodGetBufferSize(iod)       ( iod->length                                    )
#define flentiodGetModeLoc(iod)          ( flentiodGetBuffer(iod) + flentiodMODE_INDEX    )
#define flentiodGetIDloc(iod)            ( flentiodGetBuffer(iod) + flentiodID_INDEX      )
#define flentiodGetDataLoc(iod)          ( flentiodGetBuffer(iod) + flentiodDATA_INDEX    )
#define flentiodGetPdSizeLoc(iod)          flentiodGetDataLoc(iod)
#define flentiodGetPdPtrLoc(iod)         ( flentiodGetBuffer(iod) + flentiodPD_PTR_INDEX     )
#define flentiodGetPdDonecbLoc(iod)      ( flentiodGetBuffer(iod) + flentiodPD_DONECB_INDEX  )
#define flentiodGetPdDonecbArgLoc(iod)   ( flentiodGetBuffer(iod) + flentiodPD_DONECB_ARGS_INDEX   )

#define flentiodMode(iod)          (  *flentiodGetModeLoc(iod)                            )
#define flentiodID(iod)            (  *( (flentdid_t*)flentiodGetIDloc(iod) )             )
#define flentiodData(iod)             flentiodGetDataLoc(iod)
#define flentiodSize(iod)          (  (iod)->length - flentiodDATA_INDEX                  )
#define flentiodPdSize(iod)        (  *( (flint_t*)flentiodGetPdSizeLoc(iod) )            )
#define flentiodPdPtr(iod)         (  *( (void**)flentiodGetPdPtrLoc(iod) )                 )
#define flentiodPdDonecb(iod)      (  *( (flvod_tf*)flentiodGetPdDonecbLoc(iod) )  )
#define flentiodPdDonecbArgs(iod)     flentiodGetPdDonecbArgLoc(iod)

#define flentiodAppend(iod, dataPtr, dataSize) flarrPushs( iod, dataPtr, dataSize )

#define flentiodPutMode(iod, mode)              ( flentiodMode(iod) = mode               )
#define flentiodPutID(iod, dataId)              ( flentiodID(iod) = dataId               )
#define flentiodPutData(iod, dataPtr, dataSize) ( flarrSetLength(iod, flentiodDATA_INDEX)? flentiodAppend(iod, dataPtr, dataSize) : NULL )
#define flentiodPutPdSize(iod, postdpSize)      ( flentiodPdSize(iod) = postdpSize  )
#define flentiodPutPdPtr(iod, dataPtr)          ( flentiodPdPtr(iod) = dataPtr             )
#define flentiodPutPdDoneCb(iod, doneCb)        ( flentiodPdDonecb(iod) = doneCb           )
#define flentiodPutPdDoneCbArgs(iod, adPtr, adSize) ( flarrSetLength(iod, flentiodPD_DONECB_ARGS_INDEX)? flentiodAppend(iod, adPtr, adSize) : NULL )

#define flentiodClear(iod, dataMode) ( flarrSetLength(iod, flentiodDATA_INDEX)? flentiodPutMode(iod, dataMode) : flentdmoNIL )

/**
 * @brief Write the value of the given arguments into $iod
 * @param iod 
 * @param mode 
 * @param id 
 * @param dataPtr 
 * @param dataSize 
 */
void flentiodPuts(flentIOdata* iod, flentdmo_t mode, flentdid_t id, const void* dataPtr, flint_t dataSize);

/**
 * @brief  Write the given arguments into $iod
 * 
 * @param iod 
 * @param id The id of the data
 * @param dataPtr The pointer to be written to $iod
 * @param dataSize The size in bytes of the data pointer to by($dataPtr)
 * @param cb A function to be call when the user of the given data pointer($dataPtr) is
 * done with it.
 * @param cbArgPtr A pointer to data to be given to the callback as argument
 * @param cbArgSize The size in bytes of the data pointer to by $cbArgPtr
 */
void flentiodPdPuts(flentIOdata* iod, flentdid_t id, void* dataPtr, flint_t dataSize, flvod_tf cb, void* cbArgPtr, flint_t cbArgSize);

#define flentiodCopy(destIOD, srcIOD) ( flarrSetLength(destIOD, 0)? flarrPuts(destIOD, 0, srcIOD->data, srcIOD->length) : NULL )

typedef void  (*flentTick_tf)(flEntity* self, flentXenv* xenv);
typedef void* (*flentHscmd_tf)(flentsyc_t cmd, void *args);

/**
 * @brief
 * -A mesh-like data structure for modeling real world entities and how they interact with each other.
 * -Each instance of this interface can be view as a high level sequential logic circuit.
 */
struct flEntity{

  //The classification code of this entity
  const flentcco_t ccode;
  #define  flentSetCcode(ent, _ccode) *( (flentcco_t*)(&(ent)->ccode) ) = _ccode

  //A c-string representing the name of this entity
  const char* const name;
  #define _flentSetName(ent, _namestr) *( (char**)(&(ent)->name) ) = _namestr

  /**
   * @brief User defined properties(ie all other additional properties of this entity 
   * apart from the ones specified by the interface).
   * @note This pointer and all associated resources should be managed entirely by the entity.
   * This should exclude any resources that are associated with any ports
   * of this entity; these and all other resources/properties of this interface are own by
   * the sytem and are automatically cleaned up when $flentFree is called on this entity.
   */
  void * const props;
  #define  flentSetProps(ent, _props) *( (void**)(&(ent)->props) ) = _props

  //An array of pointers to all input-output pairs ports of this entity.
  flArray * const ioports;
  #define _flentSetIOports(ent, _ioports) *( (flArray **)(&(ent)->ioports) ) = _ioports
  #define _flentGetPort(ent, index) ( *(flentIOport**)_flarrGet((ent)->ioports, index) )
  #define flentGetPort(ent, index) ( *(flentIOport**)flarrGet((ent)->ioports, index) )

  /**
   * @brief This method gets called by the ticker during every time frame if tick is enable.
   * It's intended that an entity update it's state only during this function call
   * @param self
   * @param xenv The execution environment of this entity.
   */
  void (* const tick)(flEntity* self, flentXenv* xenv);
  #define flentSetTick(ent, _tick) *( (flentTick_tf *)(&(ent)->tick) ) = _tick

  const bool tickEnable;
  #define flentEnableTick(ent)  *( (bool*)(&(ent)->tickEnable) )  = true
  #define flentDisableTick(ent) *( (bool*)(&(ent)->tickEnable) )  = false
  
  /**
   * @brief This method should be implemented to handle commands from system(flEntity module).
   * $flentsycCLEANUP command is one important such command that is send whenever $flentFree
   * is called on an entity. The purpose is to allow the entity to perform cleanup operation on
   * user defined properties before it can be destroy.
   * @param cmd The system command
   * @param args Arguments for the given commands
   * @return The result of executing the given command
   */
  void* (* const hscmd)(flentsyc_t cmd, void *args);
  #define flentSetHscmd(ent, sch) *( (flentHscmd_tf *)(&(ent)->hscmd) ) = sch
  
};


/*----------FLENTITY FUNCTIONS----------*/

/**
 * @brief Set the name of this entity
 * @param ent 
 * @param namestr 
 */
bool flentSetName(flEntity* ent, const char* namestr);

/**
 * @brief Create and initialize a new $flEntity interface. The newly created entity is added
 * to the given execution environment($xenv) if it($xenv) is not NULL.
 * @param xenv The environment in which the new entity will be executed
 * @param ccode The classification code of the new entity
 * @param initialPortCount The initial number of io ports the new entity will have.
 * @return Pointer to the newly created interface(struct) | NULL if an error(MEMORY) occur.
 */
flEntity* flentNew(flentXenv* xenv, flentcco_t ccode, int initialPortCount);

/**
 * @brief Free the memory associated with the given entity
 * @note send $flentsycCLEANUP command to the given entity before destroying it.
 * @param ent 
 * @param xenv The execution environment of the given entity
 */
void flentFree(flEntity* ent, flentXenv* xenv);

/**
 * @brief find the first port encounter in $ent with the given name($portName)
 * @param ent 
 * @param portName 
 * @return Pointer to the port with the given name | NULL if no port was found.
 */
flentIOport* flentFindPortByName(flEntity* ent, flentipn_t portName);

/**
 * @brief Link and add the given port to $ent
 * @note If $port already exist in $ent, only linking operation will happen
 * @param ent 
 * @param port
 * @return true if $port already existed or was added | false otherwise 
 */
bool flentAddPort(flEntity* ent, flentIOport* port);

/**
 * @brief Remove the given port from $ent if found
 * @note This method does not free the memory associated with the port
 * @param ent 
 * @param port 
 */
void flentRemovePort(flEntity* ent, flentIOport* port);

/**
 * @brief Remove the given port if found and free the memory associated with it($port)
 * @param ent 
 * @param port 
 */
void flentDeletePort(flEntity* ent, flentIOport* port);

typedef void (*flentForEachPortCb_tf)(flentIOport* port, void* args);

/**
 * @brief Apply the given callback to each port of the given entity.
 * @param ent
 * @param cb The callback to be called on each port
 * @param cbArgs additional arguments for the callback function
 */
void flentForEachPort(flEntity* ent, flentForEachPortCb_tf cb, void* cbArgs);

struct flentXenv{
  flArray * const entities;
  #define _flentxevSetEntities(xenv, ents) *( (flArray **)(&(xenv)->entities) ) = ents

  //The time in milliseconds of the latest call of the tick method
  const flint_t tickCT;
  #define  _flentxevSetTickCT(xenv, ct) *( (flint_t*)(&(xenv)->tickCT) ) = ct

  //The time difference in milliseconds of the latest call and the previous 
  //call of the tick method
  const flint_t tickDT;
  #define  _flentxevSetTickDT(xenv, dt) *( (flint_t*)(&(xenv)->tickDT) ) = dt

};

/**
 * @brief Create a new execution environment
 *  @param initialEntityCount The initial number of entities
 */
flentXenv* flentxevNew(flint_t initialEntityCount);

/**
 * @brief Destroy/delete the given environment.
 * @param xenv 
 * @param freeEnts If true, all entities within this environment will also be deleted.
 */
void flentxevFree(flentXenv* xenv, bool freeEnts);

/**
 * @brief To be called every time frame to run the environment.
 * @param xenv
 * @param ct The latest time before this function was called
 * @param dt The time difference of the latest call and the previous call of this function.
 * @note if $ct and $dt are zero, $flMillis will be use to obtain these values.
 */
void flentxevTick(flentXenv* xenv, flint_t ct, flint_t dt);

/**
 * @brief Add the given entity($ent) to the environment($xenv) if it($ent) doesn't
 * exist in the environment.
 * @param xenv
 * @param ent
 * @return true if the entity alread existed or was added | false otherwise
 */
bool flentxevAddEntity(flentXenv* xenv, flEntity* ent);

/**
 * @brief Unlink and remove the given entity($ent) from the environment($xenv) if
 * found.
 * @note This method does not delete the given entity
 * @param xenv
 * @param ent
 */
void flentxevRemoveEntity(flentXenv* xenv, flEntity* ent);


/*--------------------flentIOport--------------------*/
/**
 * @brief Represent a port of an entity
 * 
 */
struct flentIOport{
/**
 * @brief 
 * @see $flentIOdata for interpretation
 * @note if this pointer is NULL, this port is an input only port and as such any attempt to
 * write to it will result in error(most likely segmentation fault)
 */
  flentIOdata * const _obuf;
  #define _flentiopSetObuf(iop, obuf) *( (flentIOdata**)(&(iop)->_obuf) ) = obuf

  /**
   * @brief Whether the current output of this port has been processed by it's linked port.
   * This flag is on(true) by default; a linked port's entity can disable this flag to let
   * the entity associated with this port knows that it's still processing the lastest output
   * before this flag was disabled and hence it's currently not watching this port's output.
   */
  const bool outputProcessed;
  #define _flentiopSetOutputProcessed(iop, bval) *( (bool*)&(iop)->outputProcessed ) = bval
  
  #define flentiopSetInputProcessed(iop, bval) do{\
    if((iop)->_linkedPort && (iop)->_linkedPort->entity) flentEnableTick((iop)->_linkedPort->entity);\
    if((iop)->_linkedPort) _flentiopSetOutputProcessed((iop)->_linkedPort, bval);\
  }while(0)

  flEntity * const entity;
  #define flentiopSetEntity(iop, ent) *( (flEntity**)(&(iop)->entity) ) = ent

  const flentipn_t name;
  #define flentiopSetName(iop, _name) *( (flentipn_t*)(&(iop)->name) ) = _name

  flentIOport * const _linkedPort;
  #define _flentiopSetlinkedPort(iop, lport) *( (flentIOport**)(&(iop)->_linkedPort) ) = lport

  //User defined properties associated with this port
  //This pointer and all associated resources is managed by the entity/user and as
  //such should be cleaned when $flentsycCLEANUP command is received by the entity.
  void * const props;
  #define flentiopSetProps(iop, _props) *( (void**)(&(iop)->props) ) = _props
};

/**
 * @brief Create a new port
 * @param ipname The name of this port
 * @param targetPort? Port to link the newly created port with
 * @param hasOutputBuffer Whether this particular port has an output buffer. If this value
 * is false, this port is an input only port and hence any write operation to it will result
 * in error.
 * @return Pointer to the newly created port.
 */
flentIOport* flentiopNew(flentipn_t ipname, flentIOport* targetPort, bool hasOutputBuffer);

#define flentiopNewIOport(ipname, targetPort) flentiopNew(ipname, targetPort, true)
#define flentiopNewInputPort(ipname, srcPort) flentiopNew(ipname, srcPort, false)

/**
 * @brief unlink $iop and clean up the memory associated with it
 * 
 * @param iop A pointer that was obtained through $flentiopNew
 */
void flentiopFree(flentIOport* iop);

/**
 * @brief Create a connection between the two given ports
 * 
 * @param port1 
 * @param port2 
 */
void flentiopLink(flentIOport* port1, flentIOport* port2);

/**
 * @brief Break the connection between the given port and it's target port
 * @param iop 
 * @return pointer to the linked port of $iop before the unlink operation
 */
flentIOport* flentiopUnlink(flentIOport* iop);

#define _flentiopCheckPort(iop) (iop && (iop)->_obuf)

//Micros for reading input data of a given port
#define flentiopGetInputMode(iop)     (  _flentiopCheckPort((iop)->_linkedPort)? flentiodMode((iop)->_linkedPort->_obuf) : flentdmoNIL )
#define flentiopMo(iop)                   flentiopGetInputMode(iop)

#define flentiopGetInputID(iop)       (  flentiodID((iop)->_linkedPort->_obuf)      )
#define flentiopID(iop)                  flentiopGetInputID(iop)

#define flentiopGetInputData(iop)     (  flentiodData((iop)->_linkedPort->_obuf)    )
#define flentiopDt(iop)                  flentiopGetInputData(iop)

#define flentiopGetInputSize(iop)     (  flentiodSize((iop)->_linkedPort->_obuf)    )
#define flentiopSz(iop)                  flentiopGetInputSize(iop)

#define flentiopGetInputPdSize(iop)   (  flentiodPdSize((iop)->_linkedPort->_obuf)    )
#define flentiopPdSz(iop)                flentiopGetInputPdSize(iop)

#define flentiopGetInputPdPtr(iop)    (  flentiodPdPtr((iop)->_linkedPort->_obuf)    )
#define flentiopPtr(iop)                 flentiopGetInputPdPtr(iop)

#define flentiopGetInputPdDonecb(iop) (  flentiodPdDonecb((iop)->_linkedPort->_obuf)    )
#define flentiopCb(iop)                  flentiopGetInputPdDonecb(iop)

#define flentiopGetInputPdDonecbArgs(iop) (  flentiodPdDonecbArgs((iop)->_linkedPort->_obuf)    )
#define flentiopArg(iop)                 flentiopGetInputPdDonecbArgs(iop)

//Micros for writing data to the output of a given port
#define flentiopPuts(iop, mode, id, dataPtr, dataSize) do{\
    if((iop)->_linkedPort && (iop)->_linkedPort->entity) flentEnableTick((iop)->_linkedPort->entity);\
    flentiodPuts((iop)->_obuf, mode, id, dataPtr, dataSize);\
}while(0)

#define flentiopPdPuts(iop, id, dataPtr, dataSize, cb, cbArgPtr, cbArgSize) do{\
    if((iop)->_linkedPort && (iop)->_linkedPort->entity) flentEnableTick((iop)->_linkedPort->entity);\
    flentiodPdPuts((iop)->_obuf, id, dataPtr, dataSize, cb, cbArgPtr, cbArgSize);\
}while(0)

#define flentiopClear(iop, dataMode) do{\
    if((iop)->_linkedPort && (iop)->_linkedPort->entity) flentEnableTick((iop)->_linkedPort->entity);\
    flentiodClear((iop)->_obuf, dataMode);\
}while(0)
           
#define flentiopAppend(iop, dataPtr, dataSize)  flentiodAppend((iop)->_obuf, dataPtr, dataSize)

#define flentiopCopyOutput(destIop, srcIop) do{\
  if((destIop)->_linkedPort && (destIop)->_linkedPort->entity) flentEnableTick((destIop)->_linkedPort->entity);\
  flentiodCopy(destIop->_obuf, srcIop->_obuf);\
}while(0)

#define flentiopCopyInput(destIop, srcIop)\
  if(srcIop->_linkedPort)flentiopCopyOutput(destIop, srcIop->_linkedPort)

//Micros for reading output data of a given port
#define flentiopGetOutputMode(iop)     (  _flentiopCheckPort(iop)? flentiodMode((iop)->_obuf) : flentdmoNIL )
#define flentiopOmo(iop)                  flentiopGetOutputMode(iop)

#define flentiopGetOutputID(iop)       (  flentiodID((iop)->_obuf)      )
#define flentiopOid(iop)                  flentiopGetOutputID(iop)

#define flentiopGetOutputData(iop)     (  flentiodData((iop)->_obuf)    )
#define flentiopOdt(iop)                  flentiopGetOutputData(iop)

#define flentiopGetOutputSize(iop)     (  flentiodSize((iop)->_obuf)    )
#define flentiopOsz(iop)                  flentiopGetOutputSize(iop)

#define flentiopGetOutputPdSize(iop)   (  flentiodPdSize((iop)->_obuf)    )
#define flentiopOdpSz(iop)                flentiopGetOutputPdSize(iop)

#define flentiopGetOutputPdPtr(iop)    (  flentiodPdPtr((iop)->_obuf)    )
#define flentiopOptr(iop)                flentiopGetOutputPdPtr(iop)

#define flentiopGetOutputPdDonecb(iop) (  flentiodPdDonecb((iop)->_obuf)    )
#define flentiopOcb(iop)                  flentiopGetOutputPdDonecb(iop)

#define flentiopGetOutputPdDonecbArgs(iop) (  flentiodPdDonecbArgs((iop)->_obuf)    )
#define flentiopOarg(iop)                 flentiopGetOutputPdDonecbArgs(iop)

/*--------------------STANDARD ENTITIES--------------------*/
flEntity* flentstdDataToPtrNew(flentXenv* xenv);

#endif//FLENTITYHEADERH_INCLUDED