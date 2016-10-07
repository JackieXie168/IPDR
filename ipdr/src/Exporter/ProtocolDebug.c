/** @file Exporter/ProtocolDebug.c
 *
 * Functions for displaying debug information related to the protocol
 * (e.g., dumping messages in human-readable form).
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ProtocolDebug.c,v 1.27 2005/05/11 12:38:34 dmitryk Exp $
 *
 */

#include <IPDR/Config.h>
const char ProtocolDebug_rcsid[] = "@(#) $Id: ProtocolDebug.c,v 1.27 2005/05/11 12:38:34 dmitryk Exp $"  IPDR_COPYRIGHT_STR;

#include <Exporter/ProtocolDebug.h>

const char ProtocolDebug_h_rcsid[] = PROTOCOL_DEBUG_H_RCSID;

#include <Exporter/ExporterMsgs.h>
#include <Exporter/Templates.h>
#include <Common/Protocol.h>
#include <Common/CommonUtil.h>

const char *ProtocolDebug_GetMessageName(unsigned char MessageID);

typedef struct IPDR_tag_MessageIDString{
	unsigned char	id;
	char			str[32];
}IPDR_MessageIDString;

#ifdef DEPRECATED
static
void ProtocolDebug_PrintTemplateChangeBlocks(Log *Logger, const TemplateChangeBlockHeader *TCB,
                                             int NumChanges, IPDR_DebugLevel Lvl)
{
    int i, j;
    const KeyBlock *KB;

    for (i = 0; i < NumChanges; i++) {
        Log_OutputStr1(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                       "... TemplateID: %d", IPDR_NTOHS(TCB->TemplateID));
        KB = (const KeyBlock *) ((const char *) TCB + sizeof(TemplateChangeBlockHeader));
        for (j = 0; j < IPDR_NTOHS(TCB->NumKeys); j++) {
            Log_OutputStr2(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                           "...   KeyID: %d, Attrs: 0x%x",
                           (int)IPDR_NTOHL(KB->FieldID), IsEnabled(KB));
            KB++;
        }
        TCB = (const TemplateChangeBlockHeader *) KB;
    }
}

static
void ProtocolDebug_PrintTemplateBlocks(Log *Logger, const TemplateBlockHeader *TB,
                                       int NumTemplates, IPDR_DebugLevel Lvl)
{
#ifdef NOTDONEYET
    int i, j;
    const KeyBlock *KB;
    unsigned short Len;
    for (i = 0; i < NumTemplates; i++)
	{
        Log_OutputStr1(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                       "... TemplateID: %d", IPDR_NTOHS(TB->TemplateID));
        Len = Util_Pad32(IPDR_NTOHS(TB->DescriptionLength));
        KB = (const KeyBlock *) ((const char *)  TB + (sizeof(TemplateBlockHeader) + Len));
        for (j = 0; j < IPDR_NTOHS(TB->NumKeys); j++) {
            Log_OutputStr3(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                           "...   KeyID: %d, KeyType: 0x%x, Attrs: %d",
                           (int)IPDR_NTOHL(KB->KeyID),
                           IPDR_NTOHS(KB->KeyTypeID),
                           KB->Enabled);
            KB++;
        }
        TB = (const TemplateBlockHeader *) KB;
    }
#endif
}

static
void ProtocolDebug_PrintDataMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
    const DataMessage *H = (const DataMessage *) Msg;
	Log_OutputStr4(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "... TemplateID: %u, Flags: 0x%x, ConfigurationID: %d, DSN: %llu",
                   IPDR_NTOHS(H->TemplateID),
                   H->Flags,
                   IPDR_NTOHS(H->ConfigurationID),
                   IPDR_NTOHLL(H->sequenceNum));
}

static
void ProtocolDebug_PrintACKMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
    const DataACKMessage *DAM = (const DataACKMessage *) Msg;
    Log_OutputStr2(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "... DSN: %llu, ConfigurationID: %d",
                   IPDR_NTOHLL(DAM->sequenceNum),
                   IPDR_NTOHS(DAM->ConfigurationID));
}

static
void ProtocolDebug_PrintTmplDataMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
    const TmplDataMessage *TDM = (const TmplDataMessage *) Msg;
    Log_OutputStr2(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "... ConfigurationID: %d, Flags: 0x%x",
                   IPDR_NTOHS(TDM->ConfigurationID), TDM->Flags);
#ifdef NOTDONEYET
    if (Lvl >= IPDR_LEVEL_DEBUG) {
        ProtocolDebug_PrintTemplateBlocks(Logger, (const TemplateBlockHeader *) (TDM + 1) ,
                                          ntohl(TDM->NumTemplates), Lvl);
    }
#endif
}

static
void ProtocolDebug_PrintTmplDataACKMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
#ifdef NOTDONEYET
    const TmplDataACKMessage *TDAM = (const TmplDataACKMessage *) Msg;
    Log_OutputStr1(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "... ConfigurationID: %d",
                   TDAM->ConfigurationID);
    if (Lvl >= IPDR_LEVEL_DEBUG) {
        ProtocolDebug_PrintTemplateChangeBlocks(Logger, (const TemplateChangeBlockHeader *) (TDAM + 1),
                                                IPDR_NTOHS(TDAM->NumOfChanges), Lvl);
    }
#endif
}


static
void ProtocolDebug_PrintFinalTmplDataACKMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
    /*const FinalTemplateDataACKMessage *TDAM = (const FinalTemplateDataACKMessage *) Msg;*/
    Log_OutputStr(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "...");
}
#endif

const char *ProtocolDebug_GetMessageName(unsigned char MessageID)
{
	static IPDR_MessageIDString MessageIDStrings[] = {
			{0x00u, "nil"},
			{IPDR_MSG_FLOW_START, "FLOW_START"},
			{0x02u, "nil"},
			{IPDR_MSG_FLOW_STOP, "FLOW_STOP"},
			{0x04u, "nil"},
			{IPDR_MSG_CONNECT, "CONNECT"},
			{IPDR_MSG_CONNECT_RESPONSE, "CONNECT_RESPONSE"},
			{IPDR_MSG_DISCONNECT, "DISCONNECT"},
			{IPDR_MSG_SESSION_START, "SESSION_START"},
			{IPDR_MSG_SESSION_STOP, "SESSION_STOP"},
			{0x0au, "nil"},
			{0x0bu, "nil"},
			{0x0cu, "nil"},
			{0x0du, "nil"},
			{0x0eu, "nil"},
			{0x0fu, "nil"},
			{IPDR_MSG_TEMPLATE_DATA, "TEMPLATE_DATA"},
			{0x11u, "nil"},
			{0x12u, "nil"},
			{IPDR_MSG_FINAL_TEMPLATE_DATA_ACK, "FINAL_TEMPLATE_DATA_ACK"},
			{IPDR_MSG_GET_SESSIONS, "GET_SESSIONS"},
			{IPDR_MSG_GET_SESSIONS_RESPONSE, "GET_SESSIONS_RESPONSE"},
			{IPDR_MSG_GET_TEMPLATES, "GET_TEMPLATES"},
			{IPDR_MSG_GET_TEMPLATES_RESPONSE, "GET_TEMPLATES_RESPONSE"},
			{0x18u, "nil"},
			{0x19u, "nil"},
			{IPDR_MSG_MODIFY_TEMPLATE, "MODIFY_TEMPLATE"},
			{IPDR_MSG_MODIFY_TEMPLATE_RESPONSE, "MODIFY_TEMPLATE_RESPONSE"},
			{0x1cu, "nil"},
			{IPDR_MSG_START_NEGOTIATION, "START_NEGOTIATION"},
			{IPDR_MSG_START_NEGOTIATION_REJECT, "START_NEGOTIATION_REJECT"},
			{0x1fu, "nil"},
			{IPDR_MSG_DATA, "DATA"},
			{IPDR_MSG_DATA_ACK, "DATA_ACK"},
			{0x22u, "nil"},
			{IPDR_MSG_ERROR, "ERROR"},
			{0x24u, "nil"},
			{0x25u, "nil"},
			{0x26u, "nil"},
			{0x27u, "nil"},
			{0x28u, "nil"},
			{0x29u, "nil"},
			{0x2au, "nil"},
			{0x2bu, "nil"},
			{0x2cu, "nil"},
			{0x2du, "nil"},
			{0x2eu, "nil"},
			{0x2fu, "nil"},
			{0x30u, "nil"},
			{0x31u, "nil"},
			{0x32u, "nil"},
			{0x33u, "nil"},
			{0x34u, "nil"},
			{0x35u, "nil"},
			{0x36u, "nil"},
			{0x37u, "nil"},
			{0x38u, "nil"},
			{0x39u, "nil"},
			{0x3au, "nil"},
			{0x3bu, "nil"},
			{0x3cu, "nil"},
			{0x3du, "nil"},
			{0x3eu, "nil"},
			{0x3fu, "nil"},
			{IPDR_MSG_KEEP_ALIVE, "KEEP_ALIVE"}
		};

	static char buf[64];
	sprintf(buf, "%.24s [%03d]", ((MessageID > IPDR_MSG_KEEP_ALIVE)?("nil"):(MessageIDStrings[MessageID].str)), MessageID);
	return buf;
}

static
void ProtocolDebug_HandleMessage(Log *Logger, const char *Msg, IPDR_DebugLevel Lvl)
{
#ifdef NOTYETDONE
	const MessageHeader *H = (const MessageHeader *) Msg;

    void (*MessagePrintFunc)(Log *Logger_, const char *Msg_, IPDR_DebugLevel Lvl_);

    switch (H->MessageID) {
    case IPDR_MSG_FLOW_START:				MessagePrintFunc = NULL; break;
    case IPDR_MSG_FLOW_STOP:				MessagePrintFunc = NULL; break;
    case IPDR_MSG_MODIFY_TEMPLATE:			MessagePrintFunc = NULL; break;
    case IPDR_MSG_FINAL_TEMPLATE_DATA_ACK:	MessagePrintFunc = NULL; break;
    case IPDR_MSG_START_NEGOTIATION:		MessagePrintFunc = NULL; break;
    case IPDR_MSG_GET_SESSIONS:				MessagePrintFunc = NULL; break;
    case IPDR_MSG_GET_TEMPLATES:			MessagePrintFunc = NULL; break;
    case IPDR_MSG_DATA_ACK:					MessagePrintFunc = NULL; break;

    case IPDR_MSG_CONNECT:					MessagePrintFunc = NULL; break;
    case IPDR_MSG_CONNECT_RESPONSE:			MessagePrintFunc = NULL; break;
    case IPDR_MSG_DISCONNECT:				MessagePrintFunc = NULL; break;
    case IPDR_MSG_ERROR:					MessagePrintFunc = NULL; break;
    case IPDR_MSG_KEEP_ALIVE:				MessagePrintFunc = NULL; break;

    case IPDR_MSG_SESSION_START:			MessagePrintFunc = NULL; break;
    case IPDR_MSG_SESSION_STOP:				MessagePrintFunc = NULL; break;
    case IPDR_MSG_TEMPLATE_DATA:			MessagePrintFunc = NULL; break;
    case IPDR_MSG_MODIFY_TEMPLATE_RESPONSE:	MessagePrintFunc = NULL; break;
    case IPDR_MSG_START_NEGOTIATION_REJECT:	MessagePrintFunc = NULL; break;
    case IPDR_MSG_GET_SESSIONS_RESPONSE:	MessagePrintFunc = NULL; break;
    case IPDR_MSG_GET_TEMPLATES_RESPONSE:	MessagePrintFunc = NULL; break;
    case IPDR_MSG_DATA:						MessagePrintFunc = NULL; break;
	default:                             	MessagePrintFunc = NULL; break;
   }

    if (MessagePrintFunc != NULL) {
        MessagePrintFunc(Logger, Msg, Lvl);
    } else {
      Log_OutputStr1(Logger, PROTOCOL_MODULE_ID, Lvl,
                    "ProtocolDebug_HandleMessage",
                     "...messageId=0x%04x..", H->MessageID);
    }
#endif
}

IPDR_Status ProtocolDebug_PrintOutgoingMessage(Log *Logger,
                                                Connection Conn,
                                                const char *Msg,
                                                IPDR_DebugLevel Lvl)
{
    MessageHeader Header ;

	memcpy( &Header, Msg, sizeof(MessageHeader));

	Log_OutputStr5(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "--> Outgoing message - %.98s,\n\t\t\t\t\tMsg: %.99s [0x%02X], Session: %d, Length: %lu",
                   TL_GetConnectionInfoStr(Conn),
                   ProtocolDebug_GetMessageName(Header.MessageID),
				   Header.MessageID,
                   Header.SessionID,
                   (unsigned long)(IPDR_NTOHL(Header.MessageLength)));

    if (IPDR_NTOHL(Header.MessageLength) >= sizeof(MessageHeader)) {
        ProtocolDebug_HandleMessage(Logger, Msg, Lvl);
    } else {
        Log_OutputStr(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                      "... [bad message length]");
    }

    return IPDR_OK;
}

IPDR_Status ProtocolDebug_PrintIncomingMessage(Log *Logger,
                                                Connection Conn,
                                                const char *Msg,
                                                IPDR_DebugLevel Lvl)
{
    const MessageHeader *Header = (const MessageHeader *) Msg;
    const char* connStr = TL_GetConnectionInfoStr(Conn);
    const char* msgName = ProtocolDebug_GetMessageName(Header->MessageID);
    unsigned long msgLength = (long unsigned)IPDR_NTOHL(Header->MessageLength);

    Log_OutputStr5(Logger, PROTOCOL_MODULE_ID, Lvl, "",
                   "<-- Incoming message - %.98s,\n\t\t\t\t\tMsg: %.99s [0x%02X], Session: %d, Length: %lu",
                   connStr,
                   msgName,
				   Header->MessageID,
                   Header->SessionID,
                   msgLength);

    ProtocolDebug_HandleMessage(Logger, Msg, Lvl);

    return IPDR_OK;
}

/* The following is if-defed out because it isn't currently used; it
   might also not be correct, although it probably is. */

#if 0

static
IPDR_Status ProtocolDebug_PrintDataMessageDetails(Log *Logger,
                                                   Session *S,
                                                   IPDR_TemplateHandle TH,
                                                   char *Msg)
{
#ifdef STD_FILE_OPERATIONS_SUPPORT
    Template *Tmpl = (Template *) S->ActiveContext->Templates.Data[TH];
    Key *K;
    int Offset = sizeof(DataMessageHeader);
    char i8;
    unsigned char ui8;
    short i16;
    unsigned short ui16;
    long i32;
    unsigned long ui32, ui32_2;
#ifndef _MSC_VER
    long long i64;
    unsigned long long ui64;
#else
    hyper i64;
    unsigned hyper ui64;
#endif

    char *str;
    int i;
    float f;
    double d;
    IPDR_IPAddress ip;
    TL_PABuf paBuf;

    if (Tmpl == NULL)
        return IPDR_OK;

    for (i = 0; i < Tmpl->Keys.Length; i++) {
        K = (Key *) Tmpl->Keys.Data[i];
        if (K->Attributes & IPDR_KEY_DISABLED)
            continue;

        switch (K->KeyTypeID) {

        case XDR_TYPE_BOOLEAN:
            i8 = *((char *) & Msg[Offset]);
            printf("%c,", i8);
            Offset++;
            break;
        case XDR_TYPE_UBYTE:
            ui8 = *((unsigned char *) & Msg[Offset]);
            printf("%c,", ui8);
            Offset++;
            break;
        case XDR_TYPE_BYTE:
            i8 = *((char *) & Msg[Offset]);
            printf("%c,", i8);
            Offset++;
            break ;
        case XDR_TYPE_USHORT:
            ui16 = *((unsigned short *) & Msg[Offset]);
            printf("%hu,", ui16);
            Offset += 2;
            break;
        case XDR_TYPE_SHORT:
            i16 = *((short *) & Msg[Offset]);
            printf("%hd,", i16);
            Offset += 2;
            break;
        case XDR_TYPE_UINT:
            ui32 = *((unsigned long *) & Msg[Offset]);
            printf("%lu,", ui32);
            Offset += 4;
            break;
        case XDR_TYPE_INT:
            i32 = *((long *) & Msg[Offset]);
            printf("%ld,", i32);
            Offset += 4;
            break;
        case XDR_TYPE_ULONG:
#ifndef _MSC_VER
            ui64 = *((unsigned long long *) & Msg[Offset]);
#else
            ui64 = *((unsigned hyper *) & Msg[Offset]);
#endif
            printf("%llu,", ui64);
            Offset += 8;
            break;
        case XDR_TYPE_LONG:
#ifndef _MSC_VER
            i64 = *((long long *) & Msg[Offset]);
#else
            i64 = *((hyper *) & Msg[Offset]);
#endif
            printf("%lld,", i64);
            Offset += 8;
            break;
        case XDR_TYPE_FLOAT:
            f = *((float *) & Msg[Offset]);
            printf("%f,", f);
            Offset += 4;
            break;
        case XDR_TYPE_STRING:
            d = *((double *) & Msg[Offset]);
            printf("%f,", d);
            Offset += 8;
            break;

        case XDR_TYPE_STRING:
            break;

        case XDR_TYPE_STRING:
            str = Msg + Offset;
            printf("%.199s,", str);
            Offset += strlen(str) + 1;
            break;

        case XDR_TYPE_STRING:
            break;

        case XDR_TYPE_STRING:
            break;

        case XDR_TYPE_IPV4ADDR:
            ip = *((IPDR_IPAddress *) & Msg[Offset]);
            printf("%.30s,", TL_PrintAddress(ip), paBuf);
            Offset += 4;
            break;

        case XDR_TYPE_IPV6ADDR:
            printf(",");
            Offset += 16;
            break ;

        case XDR_TYPE_DATETIME:
            ui32 = *((unsigned long *) & Msg[Offset]);
            printf("time:%u,", ui32);
            Offset += 4;
            break ;

        case XDR_TYPE_TIMEMSEC:
            ui64 = *((unsigned long long *) & Msg[Offset]);
            printf(",[msec:%llu]", ui64);
            Offset += 8;
            break;

        case XDR_TYPE_TIMEUSEC:
            ui64 = *((unsigned long long *) & Msg[Offset]);
            printf(",[usec:%llu]", ui64);
            Offset += 8;
            break ;

        case XDR_TYPE_TIMEMSEC_32:
            ui32 = *((unsigned long *) & Msg[Offset]);
            ui32_2 = *((unsigned long *) & Msg[Offset+4]);
            printf(",msec[%u,%u]", ui32, ui32_2);
            Offset += 8;
            break;

        case XDR_TYPE_TIMEUSEC_32:
            ui32 = *((unsigned long *) & Msg[Offset]);
            ui32_2 = *((unsigned long *) & Msg[Offset+4]);
            printf(",usec[%u,%u]", ui32, ui32_2);
            Offset += 8;
            break;

        case  IPDR_TYPE_BLOB:
            break;
        }
    }

    printf("\n");

    return IPDR_OK;
#endif
}

#endif /* ProtocolDebug_PrintDataMessageDetails */
