/** @fileIPDRsimMIB.cpp
 *
 *  Implementation of an SNMP agent, using the public-domain CMU AgentX code.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: IPDRsimMIB.cpp,v 1.12 2005/03/29 09:46:07 dmitryk Exp $
 *
 * The public-domain codee can be obtained from
 * ftp://ftp.net.cmu.edu/pub/snmp/cmu-snmp-V1.10.tar.gz (or later),
 * http://www.net.cmu.edu/projects/snmp/,
 * ftp://ftp.net.cmu.edu/projects/snmp
 */

#include <IPDR/Config.h>
const char IPDRsimMIB_rcsid[] = "@(#) $Id: IPDRsimMIB.cpp,v 1.12 2005/03/29 09:46:07 dmitryk Exp $" IPDR_COPYRIGHT_STR

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <agentx.h> /* From the CMU AgentX library. */

#include <IPDR/IPDR.h>

/** global: remember the value of the Exporter Handle
 *  (because the CMU code doesn't have provision for
 *  passing around a global parameter).
 */
static IPDR_ExporterHandle g_ExporterH;

agentx_errortype_t IPDRAgent_Get(int Nsubids, u_int *subids,
                                  agentx_varbind_t *varbind, int map)
{
    IPDR_OID O;
    int i;
    const char *Value;
    IPDR_Status Status;
    unsigned int Type;
    long long l;
    IPDR_BOOL Persistent, Writable;

    /* build a IPDR_OID corresponding to the received subids */
    O.Length = Nsubids;
    for (i = 0; i < Nsubids; i++) {
        O.OIDArray[i] = subids[i];
    }

    Status = IPDR_MIBGet(g_ExporterH, & O, & Value, & Type, &Persistent, &Writable);
    if (FAILURE(Status)) {
        return agentx_noSuchName;
    }

    if (Value == NULL) {
        varbind->type = agentx_Null;
        return agentx_noError;
    }

    switch (Type) {
    case XDR_TYPE_IPV4ADDR:
        varbind->type = agentx_IpAddress;
        i = inet_addr(Value);
        agentx_ostrcpy(& (varbind->data.ostr_data), 4, (u_char *) & i);
        break;
    case XDR_TYPE_BOOLEAN:
        varbind->type = agentx_Integer;
        varbind->data.int_data = atoi(Value);
        break;
    case XDR_TYPE_INT:
    case XDR_TYPE_UINT:
        varbind->type = agentx_Integer;
        varbind->data.int_data = atoi(Value);
        break;
    case XDR_TYPE_LONG:
    case XDR_TYPE_ULONG:
        l = /*atoll(Value)*/ strtoll(Value, (char **)NULL, 10);
        varbind->type = agentx_Counter64;
        memcpy(& (varbind->data.counter64), & l, sizeof(long long));
        break;
    case XDR_TYPE_STRING:
        varbind->type = agentx_OctetString;
        agentx_ostrcpy(& (varbind->data.ostr_data), strlen(Value), (u_char*) Value);
        break;
    }

    return agentx_noError;
}

int IPDRAgent_GetNext(agentx_oid_t *oid, int base_len, int map)
{
    IPDR_Status Status;
    IPDR_OID O, NextOID;
    int i;
    const char *Value;
    unsigned int Type;
    IPDR_BOOL Persistent, Writable;

    /* build a IPDR_OID corresponding to the received subids */
    O.Length = oid->Nsubid - base_len;
    for (i = 0; i < oid->Nsubid - base_len; i++)
        O.OIDArray[i] = oid->subids[i + base_len];

    /* if the get next is inclusive, check if the current oid exists within
       the MIB */
    if (oid->include == 1) {
        Status = IPDR_MIBGet(g_ExporterH, & O, & Value, & Type, &Persistent, &Writable);
        if (SUCCESS(Status))
            return AGENTX_OK;
    }

    Status = IPDR_MIBGetNext(g_ExporterH, & O, & NextOID);
    if (FAILURE(Status)) {
        return AGENTX_NOTFOUND;
    }

    /* copy the result to the oid */
    oid->Nsubid = NextOID.Length + base_len;
    for (i = 0; i < NextOID.Length; i++) {
        oid->subids[i + base_len] = NextOID.OIDArray[i];
    }

    return AGENTX_OK;
}

agentx_errortype_t IPDRAgent_Test(int Nsubids, u_int *subids,
                                   agentx_varbind_t *varbind, int map)
{
    char Buf[1024];
    unsigned char *Addr;
    long long l;
    IPDR_OID O;
    int i;

    /* build a IPDR_OID corresponding to the received subids */
    O.Length = Nsubids;
    for (i = 0; i < Nsubids; i++) {
        O.OIDArray[i] = subids[i];
    }

    switch (varbind->type) {
    case agentx_Integer:
        sprintf(Buf, "%d", varbind->data.int_data);
        break;
    case agentx_OctetString:
        memcpy(Buf, varbind->data.ostr_data.data, varbind->data.ostr_data.len);
        Buf[varbind->data.ostr_data.len] = '\0';
        break;
    case agentx_IpAddress:
        Addr = (unsigned char *) varbind->data.ostr_data.data;
        sprintf(Buf, "%d.%d.%d.%d", Addr[0], Addr[1], Addr[2], Addr[3]);
        break;
    case agentx_Counter64:
        l = *((long long *) & (varbind->data));
        sprintf(Buf, "%lld", l);
        break;
    default:
        return agentx_wrongType;
    }

    if (FAILURE(IPDR_MIBSet(g_ExporterH, & O, Buf))) {
        return agentx_notWritable;
    }

    return agentx_noError;
}

agentx_errortype_t IPDRAgent_Commit(int Nsubids, u_int *subids,
                                     agentx_varbind_t *varbind, int map)
{
    return agentx_noError;
}

agentx_errortype_t IPDRAgent_Undo(int Nsubids, u_int *subids,
                                   agentx_varbind_t *varbind, int map)
{
    return agentx_noError;
}

agentx_errortype_t IPDRAgent_Clean(int Nsubids, u_int *subids,
                                    agentx_varbind_t *varbind, int map)
{
    return agentx_noError;
}

void IPDRAgent_Initialize(IPDR_ExporterHandle C)
{
    int s, reg, map;
    agentx_oid_t oid;

    g_ExporterH = C;

    if (agentx_init(NULL) != TRUE) {
        fprintf(stderr, "Unable to initialize AgentX\n");
        exit(1);
    }

    oid.subids = agentx_newoid(IPDR_MIB_ROOT_OID_LENGTH,
                               IPDR_MIB_ROOT_OID_INTS /* 1, 3, 6, 1, 4, 4316, 2*/);
    oid.include = 0;
    oid.Nsubid = IPDR_MIB_ROOT_OID_LENGTH - 1;

    s = agentx_open(60 /* seconds timeout */, & oid, "crane");
    if (s == AGENTX_NOTRUNNING) {
        fprintf(stderr, "Unable to open AgentX session\n");
        exit(1);
    }

    /* agentx_setdebug(10); */

    reg = agentx_register(s, 0, 0, 1, IPDR_MIB_ROOT_OID_LENGTH,
                          agentx_newoid(IPDR_MIB_ROOT_OID_LENGTH,
                                        IPDR_MIB_ROOT_OID_INTS /* 1, 3, 6, 1, 4, 4316, 2 */));

    map = agentx_mapget(reg, 0, NULL, IPDRAgent_Get, IPDRAgent_GetNext);
    agentx_mapset(map, IPDRAgent_Test, IPDRAgent_Commit,
                  IPDRAgent_Undo, IPDRAgent_Clean);
}
