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
typedef struct flentIOdata flentIOdata;

/**
 * @brief Represent a port of an entity
 * 
 */
struct flentIOport{
/**
 * @brief 
 * This is an array of bytes: 
 * ->the first ${sizeof(uint8_t)} bytes is the mode of the output data.
 * ->the second ${sizeof(flentdid_t)} bytes is the id of the output data.
 * ->the interpretation of the remaining set of bytes depends on the entity involved.
 * @note if this pointer is NULL, this port is an input only port and as such any attempt to
 * write to it will result in error(most likely segmentation fault)
 */
  flArray * const _obuf;
  #define _flentiopSetObuf(iop, obuf) *( (flArray**)(&(iop)->_obuf) ) = obuf

  flEntity * const entity;
  #define flentiopSetEntity(iop, ent) *( (flEntity**)(&(iop)->entity) ) = ent

  const flentipn_t name;
  #define flentiopSetName(iop, _name) *( (flentipn_t*)(&(iop)->name) ) = _name

  flentIOport * const _linkedPort;
  #define _flentiopSetlinkedPort(iop, lport) *( (flentIOport**)(&(iop)->_linkedPort) ) = lport

};

/**
 * @brief Create a new port
 * @param inputOnly if true, no output buffer will be allocated for the newly created port
 * and as such any write operation on it will result in error.
 * @param ipname The name of this port
 * @param targetPort? Port to link the newly created port with
 * @return Pointer to the newly created port.
 */
flentIOport* flentiopNew(bool inputOnly, flentipn_t ipname, flentIOport* targetPort);

#define flentiopNewIOport(ipname, targetPort) flentiopNew(false, ipname, targetPort)
#define flentiopNewInputPort(ipname, srcPort) flentiopNew(true, ipname, srcPort)

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

/**
 * @brief 
 * Write the given io data($iodata) to the output buffer of the given port($iop)
 * @param iop 
 * @param iodata 
 */
void flentiopWrite(flentIOport* iop, flentIOdata iodata);

//Append the given data to the data output of the given port.
#define flentiopAppendData(iop, dataPtr, dataSize) flarrPushs( iop->_obuf, dataPtr, dataSize )

//Read the input of the given port
#define flentiopReadInput(iop) (iop->_linkedPort)?\
    flentiodDecode(iop->_linkedPort->_obuf): flentiodNew(flentdmoNIL, flentdidNIL, NULL, 0)

#define flentiopRead(iop) flentiopReadInput(iop)

//Read the output of the given port
#define flentiopReadOutput(iop) flentiodDecode(iop->_obuf)

/**
 * @brief A utility structure for decoding $flentIOport output buffer.
 * 
 */
struct flentIOdata{
  int8_t mode;
  flentdid_t id;
  void* data;
  flint_t size;
};

flentIOdata flentiodNew(int8_t dataMode, flentdid_t dataId, void* data, flint_t dataSize);

/**
 * @brief Encode the given I/O data struct into the destination array buffer($destArrbuf)
 * assuming $destArrbuf is an output buffer of a port @see flentIOport.
 * @note The destination buffer's length is reset to zero before the write operation.
 * @param iod 
 * @param destArrbuf 
 */
void flentiodEncode(flentIOdata iod, flArray* destArrbuf);

/**
 * @brief Decode the target buffer assuming it's an output buffer of a port @see flentIOport
 * 
 * @param arrBuf An output buffer of a port($flentIOport)
 * @return The decoded data
 * @note Pointers associated with the returned data struct are guaranteed to be valid 
 * till the next tick after the current tick(time frame)
 */
flentIOdata flentiodDecode(flArray* arrBuf);

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
   * @brief All other additional properties of this entity apart from the ones 
   * specified by the interface.
   * @note This pointer and all associated resources should be managed entirely by the entity.
   * This should exclude any resources that are associated with ports that have been added 
   * to this entity; these and all other resources/properties of this interface are own by
   * the sytem and are automatically cleaned up when $flentFree is called on this entity.
   */
  void * const props;
  #define  flentSetProps(ent, _props) *( (void**)(&(ent)->props) ) = _props

  //An array of pointers to all input-output pairs ports of this entity.
  flArray * const ioports;
  #define _flentSetIOports(ent, _ioports) *( (flArray **)(&(ent)->ioports) ) = _ioports

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
   * @brief This method should be implemented to handle commands from system(flEntity module)
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

#endif//FLENTITYHEADERH_INCLUDED