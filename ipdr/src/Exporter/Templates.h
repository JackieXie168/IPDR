/** @file Exporter/Templates.h
 *
 * Structures and functions for handling IPDR templates, as defined
 * in Exporter/Templates.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Templates.h,v 1.13 2005/01/30 15:41:26 guyg Exp $
 * 
 * The file contains structures and function prototypes for managing
 * templates and generating records according to specific templates.
 */
#ifndef _IPDR_TEMPLATES_H_
#define _IPDR_TEMPLATES_H_

#include <IPDR/Config.h>
#define TEMPLATES_H_RCSID "@(#) $Id: Templates.h,v 1.13 2005/01/30 15:41:26 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define Key_Destructor          IPDR__Key_Destructor
#define Template_AddKey         IPDR__Template_AddKey
#define Template_Create         IPDR__Template_Create
#define Template_Destructor     IPDR__Template_Destructor
#define Template_IsModified     IPDR__Template_IsModified
#define Template_ModifyTemplate IPDR__Template_ModifyTemplate
#define Template_Reset          IPDR__Template_Reset
#define Templates_Initialize    IPDR__Templates_Initialize
#define Templates_h_rcsid       IPDR__Templates_h_rcsid
#define Templates_rcsid         IPDR__Templates_rcsid
#endif

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/DynamicArray.h>
#include <Common/ChunksManager.h>
#include <Exporter/Exporter.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

struct IPDR_tag_Template;
struct IPDR_tag_Exporter;

/** pointer to a function used to generate a record. */

typedef IPDR_Status (*BuildRecordFunc)(ChunksManager *CMgr, struct IPDR_tag_Session *S,
                                        struct IPDR_tag_Template *T, const void *Data,
                                        BufferHandle *BH,
                                        unsigned long *RecordSize);

/**
 * Internal key representation
 *  (identical to IPDR_Key, but with additional field).
 */
typedef struct IPDR_tag_Key
{
    unsigned int KeyID;
    unsigned int KeyTypeID;
    char *Name;
    char *Label;
    char *Help;

    unsigned int Enabled;
    unsigned int AccessType;

    void *ExtraData;
    IPDR_KeyAccessor KeyAccessor;

    unsigned int ValueLength; /**< length of value, for fixed length types */
}
Key;

/**
 * A region of memory which needs to be copied (used
 * for optimizing the number of copies).
 */
typedef struct IPDR_tag_Region
{
    IPDR_Offset Offset;
    unsigned long Length;
    int KeyIndex; /*< index of key which originated the region
                   * originates from */
}
Region;

/**
 * Internal template representation.
 */
typedef struct IPDR_tag_Template
{
    unsigned short TemplateID;
    char *SchemaName;
    char *TypeName;

    DynamicArray Keys;               /**< keys belonging to the template */
    DynamicArray VariableLengthKeys; /**< list of variable length keys */

    Region *CopyRegions;             /**< list of memory regions to be copied */
    int NumRegions;                  /**<  when a record is generated */

    unsigned int FixedLengthSize;    /**< total size of fixed length keys */

    BuildRecordFunc BuildFunc; 		/**< function used to generate a record */
    IPDR_BOOL Enabled;        		/**< true if there is an enabled key */
    IPDR_BOOL StatusFlag;     		/**< true if this for sending status data */
}
Template;

IPDR_Status Templates_Initialize(void);
IPDR_Status Template_AddKey(struct IPDR_tag_Session *S,
                             Template *T,
                             int KeyID, int KeyTypeID,
                             const char *Name, const char *Label, const char *Help,
                             unsigned int Enabled,
                             unsigned int AccessType,
                             void *ExtraData,
                             IPDR_KeyAccessor KeyAccessor);
IPDR_Status Template_Create(struct IPDR_tag_Session *S,
                             int TemplateID,
                             const char *SchemaName,
							 const char  *TypeName,
                             int NumKeys,
                             IPDR_BOOL StatusFlag,
                             Template **T);
IPDR_Status Template_Reset(struct IPDR_tag_Session *S, Template *T);
IPDR_Status Template_IsModified(Template *Tmpl, int NumKeys,
                                 const KeyBlock *KB,
                                 IPDR_BOOL *IsModified);
IPDR_Status Template_ModifyTemplate(struct IPDR_tag_Session *S,
                                     Template *Tmpl,
                                     int NumKeys,
                                     const KeyBlock *KB,
                                     Template **NewTmpl);

void Key_Destructor(IPDR_PlatformServices *PS, void *Obj);
void Template_Destructor(IPDR_PlatformServices *PS, void *Obj);

int IsEnabled(const KeyBlock* K);

#ifdef __cplusplus
}
#endif

#endif
