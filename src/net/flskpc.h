#ifndef FLSKPCHEADERH_INCLUDED
#define FLSKPCHEADERH_INCLUDED

#include<stdint.h>

typedef uint32_t flskMsgheader_t;
#define flskMsg_HEADER_SIZE sizeof(flskMsgheader_t)
#define flskMsg_TIMEOUT (10*1000)

//->flsk default protocol
//==========================================================================================
/*
*Message bits layout
*--bits 0-3: protocol
*--bits 4-7: protocol specific flags
*--bits 8-15: src device id/key
*--bits 16-23 dest devie id/key
*--bits 24-31 msg id/type : cJSON(command description JSON), ACKR, ...
*/
#define flskp0HBitOffset_PROTOCOL        0
#define flskp0HBitOffset_FLAGS           4
#define flskp0HBitOffset_FLAGS_ACKR      (flskp0HBitOffset_FLAGS+0)
#define flskp0HBitOffset_SRC_ID          8/*source device id*/
#define flskp0HBitOffset_DEST_ID         16/*destination device id*/
#define flskp0HBitOffset_MSG_TYPE        24

#define flskp0HBitMask_PROTOCOL      ((flskMsgheader_t)0x0000000f)
#define flskp0HBitMask_FLAGS         ((flskMsgheader_t)0x000000f0)
#define flskp0HBitMask_FLAGS_ACKR    ((flskMsgheader_t)0x00000010)
#define flskp0HBitMask_SRC_ID        ((flskMsgheader_t)0x0000ff00)
#define flskp0HBitMask_DEST_ID       ((flskMsgheader_t)0x00ff0000)
#define flskp0HBitMask_MSG_TYPE      ((flskMsgheader_t)0xff000000)

#define flskp0SetProtocol(msgBuffer, proto)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(proto)) << flskp0HBitOffset_PROTOCOL  )
#define flskp0SetFlags(msgBuffer, flags)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(flags)) << flskp0HBitOffset_FLAGS  )
#define flskp0SetFlagsAckr(msgBuffer, boolVal)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(boolVal)) << flskp0HBitOffset_FLAGS_ACKR  )
#define flskp0SetSrcID(msgBuffer, srcID)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(srcID)) << flskp0HBitOffset_SRC_ID  )
#define flskp0SetDestID(msgBuffer, destID)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(destID)) << flskp0HBitOffset_DEST_ID  )
#define flskp0SetMsgType(msgBuffer, msgID)(\
    *(flskMsgheader_t*)(msgBuffer) |= ((flskMsgheader_t)(msgID)) << flskp0HBitOffset_MSG_TYPE  )

#define flskp0GetProtocol(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_PROTOCOL ) >> flskp0HBitOffset_PROTOCOL  )
#define flskp0GetFlags(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_FLAGS ) >> flskp0HBitOffset_FLAGS  )
#define flskp0GetFlagsAckr(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_FLAGS_ACKR ) >> flskp0HBitOffset_FLAGS_ACKR  )
#define flskp0GetSrcID(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_SRC_ID ) >> flskp0HBitOffset_SRC_ID  )
#define flskp0GetDestID(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_DEST_ID ) >> flskp0HBitOffset_DEST_ID  )
#define flskp0GetMsgType(msgBuffer)(\
    ( (*(flskMsgheader_t*)(msgBuffer)) & flskp0HBitMask_MSG_TYPE ) >> flskp0HBitOffset_MSG_TYPE  )

#define flskp0MsgID_CJSON         1

#endif