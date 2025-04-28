#include "common.h"

#define logicalNameLen                 24
#define physicalNameLen                12
#define MaxUnitLineLen                 39
#define MaxRAMLineLen                  16

/*  System Controller Message  */
#define msgSCInfoRequest               0x01
#define msgSCInfoResponse              0x02
#define msgSCConnectRequest            0x03
#define msgSCConnectResponse           0x04
#define msgSCInboundBridgedData        0x05
#define msgSCInboundAck                0x06
#define msgSCOutboundRequest           0x07
#define msgSCOutboundAck               0x08
#define msgSCInboundUnsolicited        0x09
#define msgSCAddrChangeRequest         0x0A
#define msgSCStatsRequest              0x0B
#define msgSCStatsResponse             0x0C
#define msgSCStatsClearRequest         0x0D
#define msgSCTableRequest              0x0E
#define msgSCTableResponse             0x0F
#define msgSCTableClearRequest         0x10

/*  Outbound Bridged Data Messages  0x07 */
#define msgMsgToTagNoBeacon            0x82
#define msgMsgToTagBeacon              0x83
#define msgSensorPingRequest           0x84
#define msgSetIrtInfoRequest           0x85
#define msgForceIrtInfoRequest         0x86
#define msgCassetteIDofTag             0x87

/*  Inbound Bridged Data Messages  0x05 */
#define msgMsgFromTag                  0x02
#define msgPingResponse                0x04
#define msgIrtOverrun                  0x05
#define msgIrtOutboundAck              0x06
#define msgScannerNotResponding        0x07

/*  Msgs From Tags  0x02 */
#define msgTwoWayReport                0x01
#define msgIrtInq                      0x02
#define msgTagRead                     0x03
#define msgOneWayReport                0x04
#define msgIrtAlignPing                0x05
#define msgBarCodeScanReport           0x06
#define msgOneWayCassetteReport        0x07


/*  Msgs To Tags   0x83 */
#define msgDisplayString               0x81   /* variable length */
#define msgTagAck                      0x84
#define msgBeaconWithData              0x85
#define msgResponseCmd                 0x87
#define msgExtSleepCommand             0x89
#define msgReadRAM                     0x8A
#define msgWriteRAM                    0x8B
#define msgTagReset                    0x8C   /* variable length */
#define msgCassetteAssign              0x8D  
#define msgTagProgramVariables         0x8E

#define msgRespBroadcastCmd            0x99   /* pseudo message */
#define msgTypeVersionRequest          0x98   /* pseudo message */
#define msgQZDisplayString             0x97   /* pseudo message  v2.5 */
#define msgTransitMsg                  0x92   /* pseudo message  v2.5 */

#define BIG_ENDIAN                     'B'
#define LITTLE_ENDIAN                  'l'

#define findLogicalType                 0
#define findPhysicalType                1
#define logicalType                     0
#define physicalType                    1


#define msgTypeAssociateUnit            20
#define msgTypeDisassociateUnit         21
#define msgTypePTLUnit                  22
#define msgTypeLTPUnit                  23
#define msgTypeListDevices              24
#define msgTypeQuerySwebStats           25
#define msgTypeClearSwebStats           26
#define msgTypeAssociateSensor          27
#define msgTypeDisassociateSensor       28
#define msgTypePTLSensor                29
#define msgTypeLTPSensor                30
#define msgTypeDisplayMsg               31
#define msgTypeQuestion                 32
#define msgTypeReadMemory               33
#define msgTypeWriteMemory              34
#define msgTypeConnectRequest           35
#define msgTypeCloseRequest             36
#define msgTypeListSensors              37
#define msgTypeTagReset                 38
#define msgTypeListUnits                39
#define msgTypeQueryUnitLocType         40
#define msgTypeLockUnit                 41
#define msgTypeReleaseUnit              42
#define msgTypeQueryDeviceInfo          43
#define msgTypeQuerySensorLoc           44
#define msgTypeQueryUnitLoc             45
#define msgTypeQueryServerInfo          46
#define msgTypeSetEventMask             47
#define msgTypeDeleteSensor             49
#define msgTypeDeleteUnit               50
#define msgTypeChangeMode               51
#define msgTypeUpdateLocation           52
#define msgTypeShutdownRequest          53
#define msgTypeGetIrtInfo               54
#define msgTypeSetIrtInfo               55
#define msgTypeReconfigRequest          57
#define msgTypeWriteToLog               58
#define msgTypeEventNotify              59
#define msgTypeSensorMotion             60
#define msgTypeSensorMove               61
#define msgTypeQuerySensorType          62
#define msgTypeListSensorsByDevice      63
#define msgTypeSetUnitCategory          64
#define msgTypeClearUnitCategory        65
#define msgTypeSetSensorCategory        66
#define msgTypeClearSensorCategory      67
#define msgTypeGetSensorCategory        68
#define msgTypeGetUnitCategory          69
#define msgTypeClearEventMask           70
#define msgTypeWriteToAnomaly           71
#define msgTypeCreateSensor             72
#define msgTypeTagResetForce            73
#define msgTypeListUnitsByCategory      74
#define msgTypeListSensorsByCategory    75
#define msgTypeCreateDevice             76
#define msgTypeCreateUnit               77
#define msgTypeDeleteDevice             78
#define msgTypeGetUnitInfo              79
#define msgTypeSetUnitInfo              80
#define msgTypeQuerySwebTable           81
#define msgTypeClearSwebTable           82
#define msgTypeTagExtSleep              83
#define msgTypeAssociateSlave           84
#define msgTypeDisassociateSlave        85
#define msgTypeCreateCassette           86
#define msgTypeCreateWafer              87
#define msgTypeDeleteCassette           88
#define msgTypeDeleteWafer              89
#define msgTypePTLCassette              90
#define msgTypeLTPCassette              91
#define msgTypePTLWafer                 92
#define msgTypeLTPWafer                 93
#define msgTypeListCassettesByCategory  94
#define msgTypeListWafersByCategory     95
#define msgTypeGetCassetteCategory      96
#define msgTypeGetWaferCategory         97
#define msgTypeSetCassetteCategory      98
#define msgTypeSetWaferCategory         99
#define msgTypeClearCassetteCategory    100
#define msgTypeClearWaferCategory       101
#define msgTypeAssociateCassette        102
#define msgTypeDisassociateCassette     103
#define msgTypeAssociateWafer           104
#define msgTypeDisassociateWafer        105
#define msgTypeListCassettesByUnit      106
#define msgTypeListWafersByCassette     107
#define msgTypeListCassettes            108
#define msgTypeSetDataField             109
#define msgTypeGetDataField             110
#define msgTypeUpdateCassetteLocation   111
#define msgTypeUpdateWaferLocation      112
#define msgTypeListUnitsBySensor        113
#define msgTypeListClients              114
#define msgTypeSetServerInfo            115
#define msgTypeUpdateSensorLocation     116
#define msgTypeGetSlaveTag              117
#define msgTypeCassetteAssign           118
#define msgTypeOutOfViewMessage         119
#define msgTypeSelectLots               120

#define QueryByUnitLogical              1
#define QueryByUnitPhysical             2
#define QueryByUnitCategory             3

#define maxClientMessageLen             60000 

#define BLOCKSIZE                       52000
#define MAX_DISPLAY_LINES               32

#define pad1  (sizeof(osLong) - ((logicalNameLen+1)  % sizeof(osLong)))
#define pad2  (sizeof(osLong) - ((physicalNameLen+1) % sizeof(osLong)))
#define pad7  (sizeof(osLong) - ((MaxRAMLineLen+1) % sizeof(osLong)))

#define LogicalNamesPerBlock (BLOCKSIZE / sizeof(rLogicalName))

#define unitBit             1<<0
#define unitLogicalBit      1<<1
#define unitTypeBit         1<<2
#define sensorLogicalBit    1<<3
#define sensorBit           1<<4
#define sensorTypeBit       1<<5

typedef long            osLong;
typedef int             osInt;
typedef short int       osShort;
typedef unsigned long   osULong;
typedef unsigned int    osUInt;
typedef unsigned short  osUShort;
typedef char            osChar;
typedef unsigned char   osUChar;
typedef char            osBool;

typedef struct _rLogicalNameTag {
      osChar name[logicalNameLen+1+pad1];
      osUChar unitTransit; /* v2.5 NEW */
      } rLogicalName;
      
typedef struct _rConnectRequestTag {
      osUShort msgLen;
      osUChar  msgType;
      osChar   byteOrder;
      osChar   bitOrder;
      osUChar  pad_1[3];
      osChar   name[logicalNameLen+1+pad1];
      }rConnectRequest;

typedef struct _rConnectReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osShort  result;
      osShort  major;
      osShort  minor;
      osChar   byteOrder;
      osChar   bitOrder;
      osShort  point;
      }rConnectReply;
      
typedef struct _rSimpleRequestTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      }rSimpleRequest;

typedef struct _rSimpleReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  numItems;
      osShort  result;
      osUChar  pad[2];
      }rSimpleReply;  
      
typedef struct _rQuerySensorRequestTag{
      osUShort msgLen;
      osUChar  msgType;
      osUChar  requestType;    /* physical or logical */
      osUChar  responseType;   /* physical or logical */
      osUChar  pad[2];
      osUChar  lastFlag;
      osLong   pingTime;
      osLong   numItems;
      rLogicalName   nameList;
      } rQuerySensorRequest;

typedef struct _rQueryInfoTag {
      osChar  unitID[physicalNameLen+1+pad2];
      osChar  unitName[logicalNameLen+1+pad1];
      osChar  sensorID[physicalNameLen+1+pad2];
      osChar  sensorName[logicalNameLen+1+pad1];
      osShort unittype;     /* one way/ two way */
      osLong  updateTime;
      osLong  moveTime;
      osLong  motionTime;
      osShort unitCategory;
      osShort sensorCategory;
      osUChar unitTransit; /* v2.5 NEW */
      } rQueryInfoRec;

#define QueryRecPerBlock (BLOCKSIZE / sizeof(rQueryInfoRec))
      
typedef struct _rQuerySensorReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  lastFlag;
      osInt    result;
      osInt    totalNum;
      osInt    numItems;
      rQueryInfoRec   responseMsg;
      }rQuerySensorReply;
      
typedef struct _rReadRAMRequestTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osInt    period;
      osInt    addr;
      osChar   unitName[logicalNameLen+1+pad1];
      }rReadRAMRequest;

typedef struct _rReadRAMReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osShort  result;
      osUChar  pad_1[2];
      osInt    addr;
      osUChar  data[MaxRAMLineLen+1+pad7];
      }rReadRAMReply;
      
typedef struct ClientEventTag {
  osUChar buffer[maxClientMessageLen];
  } rClientMsg;
  
typedef struct _rGenericRequestTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osInt    itemType;
      osChar   physicalID[physicalNameLen+1+pad2];
      osChar   logicalName[logicalNameLen+1+pad1];
      osChar   altID[physicalNameLen+1+pad2];
      osLong   period;
      }rGenRequest;

typedef struct _rGenericReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osInt    result;
      osChar   physicalID[physicalNameLen+1+pad2];
      osChar   logicalName[logicalNameLen+1+pad1];
      osChar   altID[physicalNameLen+1+pad2];
      }rGenReply;
      
typedef struct _rQueryUnitLocTag{
      osUShort msgLen;
      osUChar  msgType;
      osUChar  requestType;    /* physical or logical */
      osUChar  responseType;   /* physical or logical */
      osUChar  pad[2];
      osBool   lastFlag;
      osLong   pingTime;
      osLong   numItems;
      rLogicalName nameList[LogicalNamesPerBlock];
      }rQueryUnitLocRequest;

typedef struct _rQueryUnitReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  lastFlag;
      osInt    result;
      rLogicalName   responseMsg[LogicalNamesPerBlock];
      }rQueryUnitLocReply;
      
typedef struct _rQueryInfoRequestTag{
      osUShort msgLen;
      osUChar  msgType;
      osUChar  requestType;    /* physical or logical */
      osUChar  responseType;   /* physical or logical */
      osUChar  pad[2];
      osUChar  lastFlag;
      osLong   pingTime;
      osLong   numItems;
      osUChar  logicalName[logicalNameLen+1];
      } rQueryInfoRequest;

typedef struct _rQueryInfoReplyTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  lastFlag;
      osInt    result;
      osInt    totalNum;
      osInt    numItems;
      rQueryInfoRec   responseMsg;
      }rQueryInfoReply;
      
typedef struct _rPostLineRequestTag {
      osUShort msgLen;
      osUChar  msgType;
      osUChar  pad;
      osInt    line;
      osBool   confirm;
      osUChar  pad_1[3];
      osLong   period;
      osChar   unitName[logicalNameLen+1+pad1];
      osChar   msg[MaxUnitLineLen+1+2];
      }rPostLineRequest;
      
typedef struct _BCRTHREADINFO {
    int sock;
    pthread_t msg_tid;
    pthread_attr_t attr;
} BCRTHREADINFO;      