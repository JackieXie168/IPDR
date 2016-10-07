/** @file Exporter/Templates.c
 *
 * Implementation of template-related functions.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Templates.c,v 1.33 2005/04/21 11:30:21 dmitryk Exp $
 *
 * The file contains implementation of functions for generating
 * records using specific templates and for managing templates / keys.
 */

#include <IPDR/Config.h>
const char Templates_rcsid[] = "@(#) $Id: Templates.c,v 1.33 2005/04/21 11:30:21 dmitryk Exp $" ;

#include <Exporter/Templates.h>

const char Templates_h_rcsid[] = TEMPLATES_H_RCSID;

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <Common/Protocol.h>
#include <Common/MemoryManagement.h>
#include <Common/CommonUtil.h>
#include <Exporter/ExporterMsgs.h>

typedef int (*KeyLengthFunc)(const void *);

static int GetFixedLengthKeysSize(int baseType);
static KeyLengthFunc GetKeyLengthFunc(int baseType);

static IPDR_Status Template_CopyTemplate(IPDR_PlatformServices *PS,
                                          Template *Tmpl,
                                          Template **NewTmpl);


/**
 * Returns the length of a string key.
 *
 * @param Addr value address
 *
 * @return Key length
 */
static int
Template_GetStringLength(const void *Addr)
{
	if(Addr == NULL)
		return -1 ;
    return *((const int *) Addr) + sizeof(int);
}

#if 0 /* currently unused */

/**
 * Returns the length of a UTF8 string key.
 *
 * @param Addr value address
 *
 * @return Key length
 */
static
int Template_GetUTF8Length(const void *Addr)
{
    return *((const int *) Addr) + sizeof(int);
}

#endif /* currently unused */


#if 0 /* currently unused */

/**
 * Returns the length of a UTF16 string key.
 *
 * @param Addr value address
 *
 * @return Key length
 */
static int
Template_GetUTF16Length(const void *Addr)
{
    return *((const int *) Addr) + sizeof(int);
}

#endif /* currently unused */

#if 0 /* currently unused */

/**
 * Returns the length of a BLOB key.
 *
 * @param Addr value address
 *
 * @return Key length
 */
static int
Template_GetBLOBLength(const void *Addr)
{
    return *((const int *) Addr) + sizeof(int);
}

#endif /* currently unused */

/**
 * Checks if a template is modified given a a KeyStateVector
 *   (a bit vector containing key states (enabled / disabled)
 *
 * @param Tmpl template to check
 * @param NumKeys number of keys
 * @param KB array of key blocks containing updated attributes
 * @param IsModified true if the template is modified (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Template_IsModified(Template *Tmpl, int NumKeys,
                    const KeyBlock *KB, IPDR_BOOL *IsModified)
{
    int i, j;
    unsigned long FieldID;
    unsigned int Enabled;
    Key *K;

    *IsModified = IPDR_FALSE;

    for (i = 0; i < NumKeys; i++) {
        Enabled = IsEnabled(&KB[i]);
        FieldID = IPDR_NTOHL(KB[i].FieldID);
        for (j = 0; j < Tmpl->Keys.Length; j++) {
            K = (Key *) Tmpl->Keys.Data[j];
            if (K->KeyID == FieldID) {
                if (K->Enabled != Enabled) {
                    *IsModified = IPDR_TRUE;
                    return IPDR_OK;
                }
                break ;
            }
        }
    }

    return IPDR_OK;
}

/**
 * Create a copy of the template (the new template will reference the
 * same keys as the old one, and the ref count of the keys will be
 * increased).
 *
 * @param PS platform services
 * @param Tmpl original template
 * @param NewTmpl new template (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_CopyTemplate(IPDR_PlatformServices *PS,
                      Template *Tmpl,
                      Template **NewTmpl)
{
    IPDR_Status Status;
    Template *T;

    Status = Object_Allocate(PS, sizeof(Template), Template_Destructor,
                             (void **) & T);
    if (FAILURE(Status))
        return Status;

    Object_Duplicate(PS, Tmpl->SchemaName);
    Object_Duplicate(PS, Tmpl->TypeName);
    T->SchemaName = Tmpl->SchemaName;
    T->TypeName = Tmpl->TypeName;
    T->TemplateID = Tmpl->TemplateID;

    Status = DynamicArray_Copy(PS, & (Tmpl->Keys), & (T->Keys));
    if (FAILURE(Status)) {
        Object_Release(PS, T);
        return Status;
    }

    *NewTmpl = T;
    return IPDR_OK;
}

/**
 * Given a key state vector and a template, create a new template
 * based on the original one with the key state vector applied.
 *
 * @param S Session
 * @param Tmpl original template
 * @param NumKeys number of keys in vector
 * @param KB array of key blocks containing updated attributes
 * @param NewTmpl newly generated template (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Template_ModifyTemplate(Session *S,
                        Template *Tmpl,
                        int NumKeys,
                        const KeyBlock *KB,
                        Template **NewTmpl)
{
    int i, j;
    unsigned long FieldID, Enabled;
    IPDR_Status Status;
    Template *T;

    T = Tmpl;

    for (i = 0; i < NumKeys; i++) {
        Key *K = NULL;
        FieldID = IPDR_NTOHL(KB[i].FieldID);
        Enabled = IsEnabled(&KB[i]);
        for (j = 0; j < Tmpl->Keys.Length; j++) {
            K = (Key *) Tmpl->Keys.Data[j];
            if (K->KeyID == FieldID)
                break;
        }
        if (! K) {
            return IPDR_ERR_TEMPLATE_NOT_FOUND;
        }
        if (j == Tmpl->Keys.Length)
            continue;

        if (K->Enabled != Enabled) {
            Key *NewKey;
            if (T == Tmpl) {
                Status = Template_CopyTemplate(SESSION_PLATFORM, Tmpl, & T);
                if (FAILURE(Status))
                    return Status;
            }

            Status = Object_Allocate(SESSION_PLATFORM, sizeof(Key), Key_Destructor,
                                     (void **) & NewKey);
            if (FAILURE(Status)) {
                Object_Release(SESSION_PLATFORM, T);
                return Status;
            }

            Object_Duplicate(SESSION_PLATFORM, K->Name);
            Object_Duplicate(SESSION_PLATFORM, K->Label);
            Object_Duplicate(SESSION_PLATFORM, K->Help);

            NewKey->KeyID = K->KeyID;
            NewKey->KeyTypeID = K->KeyTypeID;
            NewKey->Name = K->Name;
            NewKey->Label = K->Label;
            NewKey->Help = K->Help;
            NewKey->Enabled = Enabled;
            NewKey->AccessType = K->AccessType;
            NewKey->ExtraData = K->ExtraData;
            NewKey->KeyAccessor = K->KeyAccessor;

            /* release the previous key */
            Object_Release(SESSION_PLATFORM, K);

            T->Keys.Data[j] = NewKey;
        }
    }

    /* in case no changes occured, use the same template */
    if (T == Tmpl)
        Object_Duplicate(SESSION_PLATFORM, T);
    else {
        Status = Template_Reset(S, T);
        if (FAILURE(Status)) {
            Object_Release(SESSION_PLATFORM, T);
            return Status;
        }
    }

    *NewTmpl = T;
    return IPDR_OK;
}

/**
 * Generate a record using a template containing both accessors and
 * offsets, with all fixed length keys
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */

static IPDR_Status
Template_BuildMixedFixedLengthRecord(ChunksManager *CMgr,
                                     Session *S,
                                     Template *T,
                                     const void *Data,
                                     BufferHandle *BH,
                                     unsigned long *RecordSize)
{
    int i;
    const void *Value;
    char *Ptr;
    Key *K;
    IPDR_Status Status;

    Status =  ChunksManager_AllocateBuffer(CMgr, T->FixedLengthSize, BH);
    if (FAILURE(Status))
        return Status;

    *RecordSize = T->FixedLengthSize;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (!K->Enabled)
            continue;
        if (K->AccessType == IPDR_ACCESS_BY_FUNCTION)
            ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
        else
            Value = (const char *) Data + (IPDR_Offset) K->KeyAccessor;

		/*memcpy(Ptr, Value, K->ValueLength);*/
		switch(K->KeyTypeID){
			case XDR_TYPE_INT:
			case XDR_TYPE_UINT:
			case XDR_TYPE_FLOAT:
			case XDR_TYPE_DATETIME:
			case XDR_TYPE_IPV4ADDR:
				{
					XDR_UINT tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htonl(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_UINT*)Ptr) = htonl(*((XDR_UINT*)Ptr));*/
				break;
			case XDR_TYPE_LONG:
			case XDR_TYPE_ULONG:
			case XDR_TYPE_DOUBLE:
			case XDR_TYPE_TIMEUSEC:
			case XDR_TYPE_TIMEMSEC:
			case XDR_TYPE_MACADDR:
				{
					XDR_ULONG tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htonll(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_ULONG*)Ptr) = htonll(*((XDR_ULONG*)Ptr));*/
				break;
			case XDR_TYPE_SHORT:
			case XDR_TYPE_USHORT:
				{
					XDR_USHORT tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htons(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_USHORT*)Ptr) = htons(*((XDR_USHORT*)Ptr));*/
				break;
			default:
				{
					memcpy(Ptr, Value, K->ValueLength);
				}
				break;
		};
        Ptr += K->ValueLength;
    }

    return IPDR_OK;
}

/**
 * Generate a record using a template containing only accessors with
 * all fixed length keys.
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_BuildFuncFixedLengthRecord(ChunksManager *CMgr,
                                    Session *S,
                                    Template *T,
                                    const void *Data,
                                    BufferHandle *BH,
                                    unsigned long *RecordSize)
{
    int i;
	const void *Value;
    char *Ptr;
    Key *K;
    IPDR_Status Status;

    Status =  ChunksManager_AllocateBuffer(CMgr, T->FixedLengthSize, BH);
    if (FAILURE(Status))
        return Status;

    *RecordSize = T->FixedLengthSize;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (!K->Enabled)
            continue;
        ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
		/*memcpy(Ptr, Value, K->ValueLength);*/
		switch(K->KeyTypeID){
			case XDR_TYPE_INT:
			case XDR_TYPE_UINT:
			case XDR_TYPE_FLOAT:
			case XDR_TYPE_DATETIME:
			case XDR_TYPE_IPV4ADDR:
				{
					XDR_UINT tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htonl(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_UINT*)Ptr) = htonl(*((XDR_UINT*)Ptr));*/
				break;
			case XDR_TYPE_LONG:
			case XDR_TYPE_ULONG:
			case XDR_TYPE_DOUBLE:
			case XDR_TYPE_TIMEUSEC:
			case XDR_TYPE_TIMEMSEC:
			case XDR_TYPE_MACADDR:
				{
					XDR_ULONG tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htonll(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_ULONG*)Ptr) = htonll(*((XDR_ULONG*)Ptr));*/
				break;
			case XDR_TYPE_SHORT:
			case XDR_TYPE_USHORT:
				{
					XDR_USHORT tv = 0;
					memcpy(&tv, Value, K->ValueLength);
					tv = htons(tv);
					memcpy(Ptr, &tv, K->ValueLength);
				}
				/**((XDR_USHORT*)Ptr) = htons(*((XDR_USHORT*)Ptr));*/
				break;
			default:
				{
					memcpy(Ptr, Value, K->ValueLength);
				}
				break;
		};
        Ptr += K->ValueLength;
    }

    return IPDR_OK;
}

/**
 * Generate a record using a template containing only
 *   offsets with all fixed length keys.
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_BuildOffsetFixedLengthRecord(ChunksManager *CMgr,
                                      Session *S,
                                      Template *T,
                                      const void *Data,
                                      BufferHandle *BH,
                                      unsigned long *RecordSize)
{
    int i;
    char *Ptr;
    IPDR_Status Status;

    Status =  ChunksManager_AllocateBuffer(CMgr, T->FixedLengthSize, BH);
    if (FAILURE(Status))
        return Status;

    *RecordSize = T->FixedLengthSize;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->NumRegions; i++) {
        memcpy(Ptr, (const char *) Data + T->CopyRegions[i].Offset,
               T->CopyRegions[i].Length);
        Ptr += T->CopyRegions[i].Length;
    }

    return IPDR_OK;
}

/**
 * Generate a record using a template containing both offsets and
 * accessors and variable length keys.
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_BuildMixedVarLengthRecord(ChunksManager *CMgr,
                                   Session *S,
                                   Template *T,
                                   const void *Data,
                                   BufferHandle *BH,
                                   unsigned long *RecordSize)
{
    unsigned int i, KeyLength;
    const void *Value;
    char *Ptr;
    Key *K;
    IPDR_Status Status;
	KeyLengthFunc	getKeyLengthCB ;

    *RecordSize = T->FixedLengthSize;

    /* calculate the record size */
    for (i = 0; i < T->VariableLengthKeys.Length; i++) {
        K = (Key *) T->VariableLengthKeys.Data[i];
        if (!K->Enabled)
            continue;
        if (K->AccessType == IPDR_ACCESS_BY_FUNCTION)
            ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
        else
            Value = (const char *) Data + (IPDR_Offset) K->KeyAccessor;
		getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
        *RecordSize += getKeyLengthCB(Value);
    }

    Status = ChunksManager_AllocateBuffer(CMgr, (*RecordSize), BH);
    if (FAILURE(Status))
        return Status;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (!K->Enabled)
            continue;
        if (K->AccessType == IPDR_ACCESS_BY_FUNCTION)
            ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
        else
            Value = (const char *) Data + (IPDR_Offset) K->KeyAccessor;
        if (IPDR_IS_FIXED_LENGTH(K->KeyTypeID)) {
			/*memcpy(Ptr, Value, K->ValueLength);*/
			switch(K->KeyTypeID){
				case XDR_TYPE_INT:
				case XDR_TYPE_UINT:
				case XDR_TYPE_FLOAT:
				case XDR_TYPE_DATETIME:
				case XDR_TYPE_IPV4ADDR:
					{
						XDR_UINT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonl(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_UINT*)Ptr) = htonl(*((XDR_UINT*)Ptr));*/
					break;
				case XDR_TYPE_LONG:
				case XDR_TYPE_ULONG:
				case XDR_TYPE_DOUBLE:
				case XDR_TYPE_TIMEUSEC:
				case XDR_TYPE_TIMEMSEC:
				case XDR_TYPE_MACADDR:
					{
						XDR_ULONG tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonll(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_ULONG*)Ptr) = htonll(*((XDR_ULONG*)Ptr));*/
					break;
				case XDR_TYPE_SHORT:
				case XDR_TYPE_USHORT:
					{
						XDR_USHORT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htons(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_USHORT*)Ptr) = htons(*((XDR_USHORT*)Ptr));*/
					break;
				default:
					{
						memcpy(Ptr, Value, K->ValueLength);
					}
					break;
			};
            Ptr += K->ValueLength;
        } else {
			getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
            KeyLength = getKeyLengthCB(Value);
            memcpy(Ptr, Value, KeyLength);
			if(KeyLength >= sizeof(XDR_INT))
			{
				XDR_INT tv = 0;
				memcpy(&tv, Ptr, sizeof(XDR_INT));
				tv = htonl(tv);
				memcpy(Ptr, &tv, sizeof(XDR_INT));
				/**((XDR_INT*)Ptr) = htonl(*((XDR_INT*)Ptr));*/
			}
            Ptr += KeyLength;
        }
    }

    return IPDR_OK;
}

/**
 * Generate a record using a template containing accessors and
 * variable length keys.
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_BuildFuncVarLengthRecord(ChunksManager *CMgr,
                                  Session *S,
                                  Template *T,
                                  const void *Data,
                                  BufferHandle *BH,
                                  unsigned long *RecordSize)
{
    unsigned int i, KeyLength;
    const void *Value;
    char *Ptr;
    Key *K;
    IPDR_Status Status;
	KeyLengthFunc	getKeyLengthCB ;

    *RecordSize = T->FixedLengthSize;

    /* calculate the record size */
    for (i = 0; i < T->VariableLengthKeys.Length; i++) {
        K = (Key *) T->VariableLengthKeys.Data[i];
        if (!K->Enabled)
            continue;
        ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
		getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
        *RecordSize += getKeyLengthCB(Value);
    }

    Status = ChunksManager_AllocateBuffer(CMgr, (*RecordSize), BH);
    if (FAILURE(Status))
        return Status;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (!K->Enabled)
            continue;
        ((IPDR_KeyAccessor) K->KeyAccessor)(Data, K->ExtraData, & Value);
        if (IPDR_IS_FIXED_LENGTH(K->KeyTypeID)) {
			/*memcpy(Ptr, Value, K->ValueLength);*/
			switch(K->KeyTypeID){
				case XDR_TYPE_INT:
				case XDR_TYPE_UINT:
				case XDR_TYPE_FLOAT:
				case XDR_TYPE_DATETIME:
				case XDR_TYPE_IPV4ADDR:
					{
						XDR_UINT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonl(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_UINT*)Ptr) = htonl(*((XDR_UINT*)Ptr));*/
					break;
				case XDR_TYPE_LONG:
				case XDR_TYPE_ULONG:
				case XDR_TYPE_DOUBLE:
				case XDR_TYPE_TIMEUSEC:
				case XDR_TYPE_TIMEMSEC:
				case XDR_TYPE_MACADDR:
					{
						XDR_ULONG tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonll(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_ULONG*)Ptr) = htonll(*((XDR_ULONG*)Ptr));*/
					break;
				case XDR_TYPE_SHORT:
				case XDR_TYPE_USHORT:
					{
						XDR_USHORT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htons(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_USHORT*)Ptr) = htons(*((XDR_USHORT*)Ptr));*/
					break;
				default:
					{
						memcpy(Ptr, Value, K->ValueLength);
					}
					break;
			};
            Ptr += K->ValueLength;
        } else {
			getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
            KeyLength = getKeyLengthCB(Value);
            memcpy(Ptr, Value, KeyLength);
			if(KeyLength >= sizeof(XDR_INT))
			{
				XDR_INT tv = 0;
				memcpy(&tv, Ptr, sizeof(XDR_INT));
				tv = htonl(tv);
				memcpy(Ptr, &tv, sizeof(XDR_INT));
				/**((XDR_INT*)Ptr) = htonl(*((XDR_INT*)Ptr));*/
			}
            Ptr += KeyLength;
        }
    }

    return IPDR_OK;
}

/**
 * Generate a record using a template containing offsets
 *   and variable length keys.
 *
 * @param CMgr chunks manager
 * @param S associated session
 * @param T template
 * @param Data data to generate record from
 * @param BH generated record (output)
 * @param RecordSize size of the genreated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Template_BuildOffsetVarLengthRecord(ChunksManager *CMgr,
                                    Session *S,
                                    Template *T,
                                    const void *Data,
                                    BufferHandle *BH,
                                    unsigned long *RecordSize)
{
    unsigned int i, KeyLength;
    const char *Value;
    char *Ptr;
    Key *K;
	KeyLengthFunc	getKeyLengthCB ;
    IPDR_Status Status;

    *RecordSize = T->FixedLengthSize;

    /* calculate the record size */
    for (i = 0; i < T->VariableLengthKeys.Length; i++) {
		K = (Key *) T->VariableLengthKeys.Data[i];
        if (!K->Enabled)
            continue;
        Value = (const char *) Data + (IPDR_Offset) K->KeyAccessor;
        getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
        *RecordSize += getKeyLengthCB(Value);
    }

    Status =  ChunksManager_AllocateBuffer(CMgr, (*RecordSize), BH);
    if (FAILURE(Status))
        return Status;

    Ptr = BH->Data + sizeof(DataMessage);
    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (!K->Enabled)
            continue;
        Value = (const char *) Data + (IPDR_Offset) K->KeyAccessor;
        if (IPDR_IS_FIXED_LENGTH(K->KeyTypeID)) {
			/*memcpy(Ptr, Value, K->ValueLength);*/
			switch(K->KeyTypeID){
				case XDR_TYPE_INT:
				case XDR_TYPE_UINT:
				case XDR_TYPE_FLOAT:
				case XDR_TYPE_DATETIME:
				case XDR_TYPE_IPV4ADDR:
					{
						XDR_UINT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonl(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_UINT*)Ptr) = htonl(*((XDR_UINT*)Ptr));*/
					break;
				case XDR_TYPE_LONG:
				case XDR_TYPE_ULONG:
				case XDR_TYPE_DOUBLE:
				case XDR_TYPE_TIMEUSEC:
				case XDR_TYPE_TIMEMSEC:
				case XDR_TYPE_MACADDR:
					{
						XDR_ULONG tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htonll(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_ULONG*)Ptr) = htonll(*((XDR_ULONG*)Ptr));*/
					break;
				case XDR_TYPE_SHORT:
				case XDR_TYPE_USHORT:
					{
						XDR_USHORT tv = 0;
						memcpy(&tv, Value, K->ValueLength);
						tv = htons(tv);
						memcpy(Ptr, &tv, K->ValueLength);
					}
					/**((XDR_USHORT*)Ptr) = htons(*((XDR_USHORT*)Ptr));*/
					break;
				default:
					{
						memcpy(Ptr, Value, K->ValueLength);
					}
					break;
			};
            Ptr += K->ValueLength;
        } else {
			getKeyLengthCB = GetKeyLengthFunc(K->KeyTypeID);
	        KeyLength = getKeyLengthCB(Value);
            memcpy(Ptr, Value, KeyLength);
			if(KeyLength >= sizeof(XDR_INT))
			{
				XDR_INT tv = 0;
				memcpy(&tv, Ptr, sizeof(XDR_INT));
				tv = htonl(tv);
				memcpy(Ptr, &tv, sizeof(XDR_INT));
				/**((XDR_INT*)Ptr) = htonl(*((XDR_INT*)Ptr));*/
			}
            Ptr += KeyLength;
        }
    }

    return IPDR_OK;
}

/**
 * Initialize global template related data structures.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Templates_Initialize(void)
{
	return IPDR_OK;
}

/**
 * Returns size of fixed length template field
*/
static int GetFixedLengthKeysSize(int baseType)
{
	switch(baseType)
	{
		case XDR_TYPE_INT:
		case XDR_TYPE_UINT:
		case XDR_TYPE_FLOAT:
		case XDR_TYPE_IPV4ADDR:
		case XDR_TYPE_DATETIME:
			return 4 ;
		case XDR_TYPE_LONG:
		case XDR_TYPE_ULONG:
		case XDR_TYPE_DOUBLE:
		case XDR_TYPE_TIMEUSEC:
		case XDR_TYPE_TIMEMSEC:
		case XDR_TYPE_MACADDR:
			return 8;
		case XDR_TYPE_BOOLEAN:
		case XDR_TYPE_BYTE:
		case XDR_TYPE_UBYTE:
			return 1;
		case XDR_TYPE_SHORT:
		case XDR_TYPE_USHORT:
			return 2;
		case XDR_TYPE_IPV6ADDR:
			return 16;
		case XDR_TYPE_UUID:
			return 20;
	}
	return 0;
}

/**
 * Returns function for calculating variable type lenght
*/
static KeyLengthFunc GetKeyLengthFunc(int baseType)
{
	switch(baseType)
	{
		case XDR_TYPE_BASE64:
		/*case XDR_TYPE_HEXBINARY:*/ /* -somehow they have the same number */
		case XDR_TYPE_STRING:
			return Template_GetStringLength;
	}
    return NULL;
}

/**
 * Delete a key
 * @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object reference
 */
void
Key_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
    Key *K = (Key *) Obj;

    Object_Release(PS, K->Name);
    Object_Release(PS, K->Label);
    Object_Release(PS, K->Help);
}

/**
 * Delete a template.
 * @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object reference
 */
void
Template_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
    Template *T = (Template *) Obj;

    Object_Release(PS, T->SchemaName);
    Object_Release(PS, T->TypeName);
    DynamicArray_Delete(PS, & (T->Keys));
    DynamicArray_Delete(PS, & (T->VariableLengthKeys));

    if (T->CopyRegions)
        Platform_MemoryFree(PS, T->CopyRegions);
}

/**
 * Add a key to a template.
 *
 * @param S Session
 * @param T template
 * @param KeyID key identifier
 * @param KeyTypeID key type identifier
 * @param Name key name
 * @param Label key label
 * @param Help key help string
 * @param Enabled key is enabled
 * @param AccessType accessor type (ACCESS_BY_OFFSET or by
 *                FUNCTION)
 * @param ExtraData additional data attached to key
 * @param KeyAccessor either accessor function of offset
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Template_AddKey(Session *S, Template *T,
                int KeyID, int KeyTypeID,
                const char *Name, const char *Label, const char *Help,
                unsigned int Enabled,
                unsigned int AccessType,
                void *ExtraData,
                IPDR_KeyAccessor KeyAccessor)
{
    int Status, Index;
    Key *K;
    int j;
	
	if( KeyID == 0 || KeyTypeID == 0 || Name == NULL )
	{
		Log_Write1(SESSION_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, 				"Template_AddKey",
					"INVALID KEY DATA <%d>",
					T->TemplateID);
		return IPDR_ERR;
	}	

    /* Check for duplicate within this template. Keys must be unique a
     * template but need not be unique across templates. */

    for (j = 0; j < T->Keys.Length; j++) {
        Key* Kx = (Key *) T->Keys.Data[j];
		if (Kx == NULL || Kx->KeyID == KeyID || 
			0 == strcmp(Kx->Name, Name) /*||
			0 == strcmp(Kx->Label, Label)*/) {
			Log_Write10(SESSION_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "Template_AddKey",
						MSG_EXPORTER_DUPLICATE_KEY,
						T->TemplateID, T->SchemaName,
						KeyID, KeyTypeID, Name, Label==NULL?"":Label,
						Kx->KeyID, Kx->KeyTypeID, Kx->Name, Kx->Label==NULL?"":Kx->Label);
			return IPDR_ERR_KEY_ALREADY_EXISTS;
		}
    }

    Status = Object_Allocate(SESSION_PLATFORM, sizeof(Key), Key_Destructor, (void **) & K);
    if (FAILURE(Status))
        return Status;

    Status = Util_StrDup(SESSION_PLATFORM, Name, & (K->Name));
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, K);
        return Status;
    }

    Status = Util_StrDup(SESSION_PLATFORM, Label, & (K->Label));
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, K);
        return Status;
    }

    Status = Util_StrDup(SESSION_PLATFORM, Help, & (K->Help));
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, K);
        return Status;
    }

    K->KeyID = KeyID;
    K->KeyTypeID = KeyTypeID;
    K->AccessType = AccessType;
    K->ExtraData = ExtraData;
    K->Enabled = Enabled;
    K->KeyAccessor = KeyAccessor;
    K->ValueLength = GetFixedLengthKeysSize(K->KeyTypeID);

    Status = DynamicArray_AppendElement(SESSION_PLATFORM, & (T->Keys), K, & Index);
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, K);
        return Status;
    }

    Log_Write8(SESSION_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG, "Template_AddKey",
               MSG_EXPORTER_CREATE_KEY,
               T->TemplateID, T->SchemaName == NULL ? "" : T->SchemaName,
               KeyID, KeyTypeID, K->Name == NULL ? "" : K->Name, "",
               "", K->Enabled);

    return IPDR_OK;
}

/**
 * Generate a new template.
 *
 * @param S Session
 * @param TemplateID template identifier
 * @param SchemaName template schema name
 * @param TypeName template type name
 * @param NumKeys initial number of keys
 * @param StatusFlag the "T" flag for the template
 * @param T newly generated template (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Template_Create(Session *S, int TemplateID,
                const char *SchemaName, const char* TypeName, int NumKeys,
                IPDR_BOOL StatusFlag, Template **T)
{
    Template *Tmpl;

    int Status = Object_Allocate(SESSION_PLATFORM, sizeof(Template),
                                 Template_Destructor, (void **) & Tmpl);
    if (FAILURE(Status))
        return Status;

    Status = Util_StrDup(SESSION_PLATFORM, SchemaName, & (Tmpl->SchemaName));
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, Tmpl);
        return Status;
    }

    Status = Util_StrDup(SESSION_PLATFORM, TypeName, & (Tmpl->TypeName));
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, Tmpl);
        return Status;
    }

    Status = DynamicArray_Create(SESSION_PLATFORM, & (Tmpl->Keys), NumKeys);
    if (FAILURE(Status)) {
        Object_Release(SESSION_PLATFORM, Tmpl);
        return Status;
    }

    Tmpl->CopyRegions = NULL;
    Tmpl->TemplateID = TemplateID;
    Tmpl->StatusFlag = StatusFlag;

    Log_Write2(SESSION_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG, "Template_Create",
               MSG_EXPORTER_CREATE_TEMPLATE,
               Tmpl->TemplateID, Tmpl->SchemaName?Tmpl->SchemaName:"NULL");

    *T = Tmpl;
    return IPDR_OK;
}

/**
 * Reset the internal template info after keys have been added.
 *
 * @param S Session
 * @param T template
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Template_Reset(Session* S, Template *T)
{
    int Index, i, j, Status, NumRegions, KeyIndex;
    IPDR_BOOL OffsetAccessors = IPDR_FALSE;
    IPDR_BOOL FunctionAccessors = IPDR_FALSE;
    IPDR_BOOL FixedLength = IPDR_TRUE;
    Key *K;
    Region TempRegion;

    Status = DynamicArray_Reset(SESSION_PLATFORM,
                                &(T->VariableLengthKeys), T->Keys.Length);
    if (FAILURE(Status))
        return Status;

    if (T->CopyRegions) {
        Platform_MemoryFree(SESSION_PLATFORM, T->CopyRegions);
        T->CopyRegions = NULL;
    }

    Platform_Check_MEMORYALLOCATEVEC(SESSION_PLATFORM, T->Keys.Length, T->CopyRegions, {});

    NumRegions = 0;

    T->FixedLengthSize = 0;
    T->Enabled = IPDR_FALSE;

    /* determine the template type and the fixed length size */

    for (i = 0; i < T->Keys.Length; i++) {
        K = (Key *) T->Keys.Data[i];
        if (K != NULL && K->Enabled) {
            T->Enabled = IPDR_TRUE;
            if (IPDR_IS_FIXED_LENGTH(K->KeyTypeID)) {
                K->ValueLength = GetFixedLengthKeysSize(K->KeyTypeID);
                T->FixedLengthSize += K->ValueLength;
            } else {
                Object_Duplicate(SESSION_PLATFORM, K);
                DynamicArray_AppendElement(SESSION_PLATFORM,
                                           & (T->VariableLengthKeys), K, & Index);
                FixedLength = IPDR_FALSE;
            }
            if (K->AccessType == IPDR_ACCESS_BY_FUNCTION)
                FunctionAccessors = IPDR_TRUE;
            else {
                OffsetAccessors = IPDR_TRUE;
                if (IPDR_IS_FIXED_LENGTH(K->KeyTypeID)) {
                    T->CopyRegions[NumRegions].Offset = (IPDR_Offset) K->KeyAccessor;
                    T->CopyRegions[NumRegions].Length = K->ValueLength;
                    T->CopyRegions[NumRegions].KeyIndex = i;
                    NumRegions++;
                }
            }
        }
    }

    /* select the record building function */

    T->FixedLengthSize += sizeof(DataMessage);

    if (FixedLength) {
        if (FunctionAccessors && OffsetAccessors)
            T->BuildFunc = Template_BuildMixedFixedLengthRecord;
        else if (FunctionAccessors)
            T->BuildFunc = Template_BuildFuncFixedLengthRecord;
        else
            T->BuildFunc = Template_BuildOffsetFixedLengthRecord;
    } else {
        if (FunctionAccessors && OffsetAccessors)
            T->BuildFunc = Template_BuildMixedVarLengthRecord;
        else if (FunctionAccessors)
            T->BuildFunc = Template_BuildFuncVarLengthRecord;
        else
            T->BuildFunc = Template_BuildOffsetVarLengthRecord;
    }

    /* sort the regions array by offset to optimize keys order */

    /* each region can be copied by a single memcpy(), which makes
     * things a bit more efficient. */

    for (i = 0; i < NumRegions; i++) {
        for (j = i + 1; j < NumRegions; j++) {
            if (T->CopyRegions[i].Offset > T->CopyRegions[j].Offset) {

                K = (Key *) T->Keys.Data[T->CopyRegions[i].KeyIndex];
                T->Keys.Data[T->CopyRegions[i].KeyIndex] =
                    T->Keys.Data[T->CopyRegions[j].KeyIndex];
                T->Keys.Data[T->CopyRegions[j].KeyIndex] = K;

                TempRegion = T->CopyRegions[i];
                T->CopyRegions[i] = T->CopyRegions[j];
                T->CopyRegions[j] = TempRegion;

                KeyIndex = T->CopyRegions[i].KeyIndex;
                T->CopyRegions[i].KeyIndex = T->CopyRegions[j].KeyIndex;
                T->CopyRegions[j].KeyIndex = KeyIndex;
            }
        }
    }

    /* merge regions */

    j = 0;
    for (i = 1; i < NumRegions; i++) {
        if (T->CopyRegions[j].Offset + T->CopyRegions[j].Length ==
            T->CopyRegions[i].Offset)
            T->CopyRegions[j].Length += T->CopyRegions[i].Length;
        else {
            j++;
            T->CopyRegions[j].Length = T->CopyRegions[i].Length;
            T->CopyRegions[j].Offset = T->CopyRegions[i].Offset;
        }
    }
    T->NumRegions = j + 1;

    return IPDR_OK;
}

/**
 * Gets the enabled value of Key block as it comes from protocol
 *
 * @param K Key Block (field descriptor)
 *
 * @return #IPDR_TRUE if enabled, #IPDR_FALSE otherwise
 */

int IsEnabled(const KeyBlock* K)
{
	return IPDR_TRUE;
}
