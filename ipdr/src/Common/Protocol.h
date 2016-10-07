/** @file Common/Protocol.h
 *
 * Macros, structures defining the IPDR protocol.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Protocol.h,v 1.36 2005/05/29 14:50:08 dmitryk Exp $
 *
 * @note DSN handling also requires compareDSN in CommonUtil.c
 */
#ifndef _IPDR_PROTOCOL_H_
#define _IPDR_PROTOCOL_H_

#include <IPDR/Config.h>
#define PROTOCOL_H_RCSID "@(#) $Id: Protocol.h,v 1.36 2005/05/29 14:50:08 dmitryk Exp $" IPDR_COPYRIGHT_STR

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h> /* for IPDR_DSN_TYPE */
#if defined(GLOBAL_RENAME)
#define compareDSN     IPDR__compareDSN
#define Protocol_rcsid IPDR__Protocol_rcsid
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse the Emacs's automatic indentation */
#endif

#define IPDR_TRANSPORT_TCP     1
#define IPDR_TRANSPORT_SCTP    2

#define IPDR_INVALID_DSN(x)   0 /**< for 31-bit DSN: ((x) == (IPDR_DSN_TYPE)(-1)) */
#define IPDR_INITIAL_DSN      0
#define IPDR_MAX_DSN          0xffffffffu /* 4294967295u */
#define IPDR_VALID_DSN_WINDOW (IPDR_MAX_DSN / 100) /**< Not an error for re-sending a DSN this much older than the expected DSN */

#if 1

/* The following are for unsigned DSN, depending on
   wrap-around after 0xffffffffu */

#define IPDR_ASSIGN_AND_INCR_DSN(dsnPlus1, dsn)      \
    { (dsnPlus1) = (dsn); (dsnPlus1)++; }

#define IPDR_INCR_DSN(dsn)                     \
    { (dsn)++; }

#else

/* The following are for signed DSN (no longer used)
   which requires IPDR_MAX_DSN to be 0x7fffffff */

#define IPDR_ASSIGN_AND_INCR_DSN(dsnPlus1, dsn)       \
    { (dsnPlus1) = (dsn); IPDR_INCR_DSN(dsnPlus1); }

#define IPDR_ASSIGN_AND_DECR_DSN(dsnMinus1, dsn)      \
    { (dsnMinus1) = (dsn); IPDR_DECR_DSN(dsnMinus1); }

#define IPDR_INCR_DSN(dsn)                     \
    if ((dsn) >= IPDR_MAX_DSN) {               \
        (dsn) = IPDR_INITIAL_DSN;              \
    } else {                                    \
        (dsn)++;                                \
        IPDR_HANDLE_DSN_OVERFLOW(dsn);         \
    }

#define IPDR_DECR_DSN(dsn)                     \
    if ((dsn) <= IPDR_INITIAL_DSN) {           \
        (dsn) = IPDR_MAX_DSN;                  \
    } else {                                    \
        (dsn)--;                                \
        IPDR_HANDLE_DSN_UNDERFLOW(dsn);        \
    }

#define IPDR_HANDLE_DSN_RANGE(dsn, newDSN)                     \
    if ((dsn) < IPDR_INITIAL_DSN || (dsn) > IPDR_MAX_DSN) {   \
        (dsn) = newDSN;                                         \
    }

#define IPDR_HANDLE_DSN_UNDERFLOW(dsn) IPDR_HANDLE_DSN_RANGE(dsn, IPDR_MAX_DSN)
#define IPDR_HANDLE_DSN_OVERFLOW( dsn) IPDR_HANDLE_DSN_RANGE(dsn, IPDR_INITIAL_DSN)

#endif

enum IPDR_compareDSN_t {
    IPDR_compareDSN_less = 0,
    IPDR_compareDSN_equal = 1,
    IPDR_compareDSN_greater = 2,
    IPDR_compareDSN_unknown = 3
};

/* Return IPDR_compareDSN_less/equal/greater/unknown from the
   comparison, using the window (IPDR_compareDSN_unknown means not
   comparable because outside the window) */

enum IPDR_compareDSN_t compareDSN(IPDR_DSN_TYPE d1, IPDR_DSN_TYPE d2);

#define IPDR_FIXED_LENGTH_MASK    0x4000u
#define IPDR_MAX_BASE_TYPES       0x1000u

#define IPDR_BIG_ENDIAN_FLAG      0x01u
#define IPDR_TEMPLATE_STATUS_FLAG 0x0001u

#define IPDR_IS_FIXED_LENGTH(Type) (! (((Type) & IPDR_FIXED_LENGTH_MASK)))
#define IPDR_BASE_KEY_TYPE(Type) ((Type) & 0x0fff)
#define IPDR_MESSAGE_LENGTH(Msg) (IPDR_NTOHL((MessageHeader *) (Msg))->MessageLength)

/* it changed in IPDR no need o fsync flag only duplicate flag need
#define IPDR_SYNC_FLAG            0x01u
#define IPDR_DUPLICATE_FLAG       0x02u
*/
#define IPDR_DUPLICATE_FLAG       0x01u

#define IPDR_PROTOCOL_VERSION   2

#define IPDR_MSG_FLOW_START					0x01u /* << */
#define IPDR_MSG_FLOW_STOP					0x03u /* << */
#define IPDR_MSG_MODIFY_TEMPLATE			0x1au /* << */
#define IPDR_MSG_FINAL_TEMPLATE_DATA_ACK	0x13u /* << */
#define IPDR_MSG_START_NEGOTIATION			0x1du /* << */
#define IPDR_MSG_GET_SESSIONS				0x14u /* << */
#define IPDR_MSG_GET_TEMPLATES				0x16u /* << */
#define IPDR_MSG_DATA_ACK					0x21u /* << */

#define IPDR_MSG_CONNECT					0x05u /* >> << */
#define IPDR_MSG_CONNECT_RESPONSE			0x06u /* >> << */
#define IPDR_MSG_DISCONNECT					0x07u /* >> << */
#define IPDR_MSG_ERROR						0x23u /* >> << */
#define IPDR_MSG_KEEP_ALIVE					0x40u /* >> << */

#define IPDR_MSG_SESSION_START				0x08u /* >> */
#define IPDR_MSG_SESSION_STOP				0x09u /* >> */
#define IPDR_MSG_TEMPLATE_DATA				0x10u /* >> */
#define IPDR_MSG_MODIFY_TEMPLATE_RESPONSE	0x1bu /* >> */
#define IPDR_MSG_START_NEGOTIATION_REJECT	0x1eu /* >> */
#define IPDR_MSG_GET_SESSIONS_RESPONSE		0x15u /* >> */
#define IPDR_MSG_GET_TEMPLATES_RESPONSE		0x17u /* >> */
#define IPDR_MSG_DATA						0x20u /* >> */


/* For the various numeric fields, we have special "opaque" types ...
 * the values are kept in network byte order, so they need macros to
 * set and extract (IPDR_HTONS, IPDR_HTONL, IPDR_NTOHS,
 * IPDR_NTOHL, all in Defs.h).
 *
 * Because we know that 8-bit values are always safe in whatever
 * order (big- or little-endian), we've been a bit lazy and haven't
 * used the special types for those fields.
 */

typedef struct { unsigned char  v8;  } ProtocolUnsigned8;  /* <inttypes.h> uint8_t */
typedef struct {   signed char  v8;  } ProtocolSigned8;    /* <inttypes.h>  int8_t */
typedef struct { unsigned short v16; } ProtocolUnsigned16; /* <inttypes.h> uint16_t */
typedef struct {   signed short v16; } ProtocolSigned16;   /* <inttypes.h>  int16_t */
typedef struct { unsigned long  v32; } ProtocolUnsigned32; /* <inttypes.h> uint32_t */
                                                           /* same as IPDR_IPAddress */
typedef struct {   signed long  v32; } ProtocolSigned32;   /* <inttypes.h>  int32_t */
typedef struct { unsigned long long  v64; } ProtocolUnsigned64;
typedef struct {   signed long long  v64; } ProtocolSigned64;

/** The common header in all of the IPDR messages. */
typedef struct
{
    unsigned char      Version;
    unsigned char      MessageID;
    unsigned char      SessionID;    /* ... 2 fields switched. */
    unsigned char      MessageFlags; /* XXX protocol document has these ... */
    ProtocolUnsigned32 MessageLength;
} __attribute__((packed))
MessageHeader;

/** Connect message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned32 Addr;
    ProtocolUnsigned16 Port;
	ProtocolUnsigned32 Capabilities;
	ProtocolUnsigned32 keepAliveInterval;
    ProtocolUnsigned32 vendorIDLength;
} __attribute__((packed))
ConnectMessage;

typedef struct
{
	MessageHeader      Header;
	ProtocolUnsigned32 Capabilities;
	ProtocolUnsigned32 keepAliveInterval;
	ProtocolUnsigned32 vendorIDLength;
} __attribute__((packed))
ConnectResponseMessage;

typedef struct
{
	MessageHeader      Header;
} __attribute__((packed))
DisconnectMessage;

/** error message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned32 Timestamp;
    ProtocolUnsigned16 ErrorCode;
    ProtocolUnsigned32 DescrLength;
} __attribute__((packed))
ErrorMessage;

/** Flow Start Message */
typedef struct
{
	MessageHeader      Header;
} __attribute__((packed))
FlowStartMessage;

/** Session Start Message */
typedef struct {
	MessageHeader 		Header;
	ProtocolUnsigned32 	exporterBootTime; /**< boot time of Exporter(in seconds from epoch time)*/
	ProtocolUnsigned64 	firstRecordSequenceNumber; /**< first sequence number to be expected */
	ProtocolUnsigned64 	droppedRecordCount; /**< number of records dropped in gap situations */
	unsigned char		primary;			/**< indication that the Collector is the primary */
	ProtocolUnsigned32 	ackTimeInterval; 	/**< the maximum time between acks from Collector (in second units) */
	ProtocolUnsigned32 	ackSequenceInterval; /**< the maximum number of unacknowledged records */
	char 				documentId[16]; 	/**< the UUID associated with the info being sent in this Session */
} __attribute__((packed))
SessionStartMessage ;

/** Flow Stop Message */
typedef struct
{
	MessageHeader      Header;
	ProtocolUnsigned16 reasonCode;
	ProtocolUnsigned32 reasonInfoLength;
} __attribute__((packed))
FlowStopMessage;

/** Session Stop Message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 reasonCode;			/**< Stop Session Reason Code */
	ProtocolUnsigned32 reasonInfoLength;	/**< Stop Session Reason Descripsion (UTF8String) */
} __attribute__((packed))
SessionStopMessage;

/** template data message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 ConfigurationID;
    unsigned char  	   Flags;
    ProtocolUnsigned32 NumTemplates;
} __attribute__((packed))
TmplDataMessage;

/** get templates message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 RequestID;
} __attribute__((packed))
GetTemplatesMessage;

/** get templates response message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 RequestID;
	ProtocolUnsigned16 ConfigurationID;
    ProtocolUnsigned32 NumTemplates;
} __attribute__((packed))
GetTemplatesResponseMessage;

/** get session message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 RequestID;
} __attribute__((packed))
GetSessionsMessage;

/** get sessions response message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 RequestID;
    ProtocolUnsigned32 NumSessions;
} __attribute__((packed))
GetSessionsResponseMessage;

/** Modify Template Message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 ConfigurationID;
    unsigned char      Flags;
    ProtocolUnsigned32 NumOfTemplates;
} __attribute__((packed))
ModifyTemplateMessage;

/** Modify Template Response Message  */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 ConfigurationID;
    unsigned char      Flags;
    ProtocolUnsigned32 NumOfTemplates;
} __attribute__((packed))
ModifyTemplateResponseMessage;

/** Final Template Data Message */
typedef struct
{
    MessageHeader      Header;
} __attribute__((packed))
FinalTemplateDataACKMessage;

/** Start Negotiation Message */
typedef struct
{
    MessageHeader      Header;
} __attribute__((packed))
StartNegotiationMessage;

/** Start Negotiation Reject Message */
typedef struct
{
    MessageHeader      Header;
} __attribute__((packed))
StartNegotiationRejectMessage;

/** data message */
typedef struct
{
    MessageHeader      Header;
    ProtocolUnsigned16 TemplateID;
    ProtocolUnsigned16 ConfigurationID;
    unsigned char      Flags;           /**< IPDR_SYNC_FLAG, IPDR_DUPLICATE_FLAG */
    ProtocolUnsigned64 sequenceNum;
	ProtocolUnsigned32 dataLength;
} __attribute__((packed))
DataMessage;

/** data ACK message */
typedef struct
{
	MessageHeader		Header;
    ProtocolUnsigned16	ConfigurationID;
	ProtocolUnsigned64	sequenceNum;
} __attribute__((packed))
DataACKMessage;

/** Keep Alive message */
typedef struct
{
    MessageHeader		Header;
} __attribute__((packed))
KeepAliveMessage;

/** template block component in a message
 * @see TmplDataMessageHeader
 */
typedef struct
{
    ProtocolUnsigned16 TemplateID;
	/**< UTFString8 Schema name */
	/**< UTFString8 Type name */
	/**< <Field Descriptor> fields */
} __attribute__((packed))
TemplateBlockHeader;

/** key block component in a message
 * @see TemplateBlockHeader
 */
typedef struct
{
    ProtocolUnsigned32 KeyTypeID;
    ProtocolUnsigned32 FieldID;
	/**< UTFString8 Field Name */
	/**< ProtocolUnsigned8 isEnabled;*/
} __attribute__((packed))
KeyBlock;

/** extended key block component in a message
 * @see TemplateBlockHeader
 */
typedef struct
{
    ProtocolUnsigned32 KeyID;
    ProtocolUnsigned16 KeyTypeID;
    ProtocolUnsigned16 KeyNameLength;
    ProtocolUnsigned16 KeyLabelLength;
    ProtocolUnsigned16 KeyHelpLength;
    ProtocolUnsigned32 KeyAttributes;   /* IPDR_KEY_DISABLED */
    /* XXX the protocol document shows KeyAttributes after all the
     * names, which is wrong. */
} __attribute__((packed))
ExtendedKeyBlock;

/** template change block within a template message */
typedef struct
{
    ProtocolUnsigned16 TemplateID;
    ProtocolUnsigned16 NumKeys;
} __attribute__((packed))
TemplateChangeBlockHeader;


/** sesion block within get sessions message */
typedef struct
{
    unsigned char      SessionID;
    unsigned char      Reserved1;
    ProtocolUnsigned32 SessionNameLength;
    ProtocolUnsigned32 SessionDescriptionLength;
	ProtocolUnsigned32 ackTimeInterval; 	/**< the maximum time between acks from Collector (in second units) */
	ProtocolUnsigned32 ackSequenceInterval; /**< the maximum number of unacknowledged records */
} __attribute__((packed))
SessionBlock;

/** Error codes */
enum ErrorCode {

   /* Values of 0-255 are reserved for standard error codes */
    KeepAliveExpired = 0x0,
    MessageInvalidForCapabilities = 0x1,
    MessageInvalidForState = 0x2,
    MessageDecodeError = 0x3,
    ProcessTerminating = 0x4,

    BrokenTemplate = 0x8000,
    CollectorNotConfigured = 0x8001,
    InvalidConnectionState = 0x8002,
	TemplateDataAckExpired = 0x8003,
	CollectorNotInSession  = 0x8004,
    ErrorCodeUnknown = 0xEFFF
};

/* protocol info request message (initial UDP negotiation) */
typedef struct
{
    ProtocolUnsigned32 ServerAddress;
    ProtocolUnsigned32 ServerBootTime;
    char MSG[4];                      /* Must be 'C', 'R', 'A', 'N' for version 1 and 'IPDR' for version 2*/
}
ProtocolInfoRequestMessage;

/** protocol info block within protocol info response */
typedef struct
{
    ProtocolUnsigned32 TransportType;
    ProtocolUnsigned32 ProtocolVersion;
    ProtocolUnsigned16 PortNumber;
    ProtocolUnsigned16 Reserved;
} __attribute__((packed))
ProtocolInfoBlock;

/** protocol info response message */
typedef struct
{
    ProtocolInfoBlock  DefaultProtocol;
    ProtocolUnsigned32 AdditionalProtocolsCount;
} __attribute__((packed))
ProtocolInfoResponseMessage;

#ifdef __cplusplus
}
#endif

#endif
