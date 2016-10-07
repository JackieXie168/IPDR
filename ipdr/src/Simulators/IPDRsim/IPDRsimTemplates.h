// -*-C++-*-

/** @file IPDRsimTemplates.h
 *
 * Contains the classes for output records, as implemented in IPDRsimTempalte.cpp
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: IPDRsimTemplates.h,v 1.22 2005/02/14 19:31:18 dmitryk Exp $
 *
 */

/** Base class for output records, containing mainly virtual methods.
 *  It has default destructor, copy constructor, assignment because there are
 *  no pointers.
 */
class Record
{
public:

    XDR_TIMEUSEC	timestamp;
	XDR_UBYTE		transaction_type;

    /** The identifying number for the record (the template ID used by IPDR_CreateTemplate()) */
    virtual int templateId() const = 0;

    /** The identifying name for the record (used by IPDR_CreateTemplate()) */
    virtual const char* templateDescr() const = 0;

    /** The keys for the fields (for template change callback and IPDR_CreateTemplate()). */
    virtual const IPDR_Key* keys() const = 0;

    /** The number of keys for the fields */
    virtual int numKeys() const = 0;

    /** Set enabled/disabled for a key, identified by ID.
     *  @param ID The key ID.
     *  @param enabled The new value for the key's being enabled
     *  @return true if the value was changed; false if no change
     *             or the key ID wasn't found.
     */
    virtual bool setKeyDisabled(unsigned int ID, bool enabled);

    /** The template handle for the record (result from IPDR_CreateTemplate()) */
    IPDR_TemplateHandle templateH;

    /** Constructor: initialize default values */
    Record() { memset(&templateH, 0xff, sizeof templateH); }

    /** Fill the record with suitable artificial values
     *  @param nowStr C-string containing the current time in ISO format
     *  @param pad amount to pad some fields by (in range 0-99)
     *  @param recNum the current record number
     *  @param tick the current time in internal form
     */
    virtual void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick) = 0;

    /** Print the record's contents to a stream
     *  @param ostr The output stream
     *  @return ostr
     */

    virtual ostream& printRec(ostream& ostr) const = 0;

    /** Print the keys for the record in some nice human-readable form
     *  @param ostr The output stream
     *  @return ostr
     */

    virtual ostream& printKeys(ostream& ostr) const = 0;

protected:
    /** The keys for the fields (for template change callback and IPDR_CreateTemplate()). */
    virtual IPDR_Key* keysWritable() = 0;
};


/** XYZ records (abstract)
 *  It has default destructor, copy constructor, assignment because there
 *  are no pointers.
 */
class XyzRecord : public Record
{
public:
    XDR_UBYTE		message_type;
    XDR_UBYTE		invoke_id;
    XDR_UINT		transaction_id; // 32 bit
    XDR_UBYTE		called_net_adds_len;
    XDR_USHORT		called_net_np;
    XDR_SHORT		called_net_enc_scheme;
    XDR_UINT		called_net_noa;
    XDR_UBYTE		calling_net_adds_len;
    XDR_UBYTE		calling_net_np;
    XDR_UBYTE		calling_net_enc_scheme;
    XDR_INT			calling_net_noa;
};


/** Type "1" XYZ records
 *  It has default destructor, copy constructor, assignment because there
 *  are no pointers.
 */
class XyzFwdRecord : public XyzRecord
{
public:
    XDR_UBYTE		dest_adds_type;
    XDR_UBYTE		dest_adds_len;
    XDR_UBYTE		dest_adds_noa;
    XDR_UBYTE		dest_adds_np;
    XDR_UBYTE		orig_adds_type;
    XDR_UBYTE		orig_adds_len;
    XDR_UBYTE		orig_adds_noa;
    XDR_UBYTE		orig_adds_np;

	int templateId() const { return 1; };
    const char* templateDescr() const { return "XyzFwdKeys"; }
    const IPDR_Key* keys() const { return _keys; }
    int numKeys() const { return _numKeys; }

    void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick);

    ostream& printRec(ostream& ostr) const;

    ostream& printKeys(ostream& ostr) const;

protected:
    IPDR_Key* keysWritable() { return _keys; }

    /** Template keys */
    static /*const*/ IPDR_Key _keys[];

    /** Number of template keys */
    static const int _numKeys;
};


/** Type "2" XYZ records
 *  It has default destructor, copy constructor, assignment because there
 *  are no pointers.
 */
class XyzAckRecord : public XyzRecord
{
public:
    XDR_UBYTE		error;


	int templateId() const { return 2; }
    const char* templateDescr() const { return "XyzAckKeys"; }
    const IPDR_Key* keys() const { return _keys; }
    int numKeys() const { return _numKeys; }

    void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick);

    ostream& printRec(ostream& ostr) const;

    ostream& printKeys(ostream& ostr) const;

protected:
    IPDR_Key* keysWritable() { return _keys; }

    /** Template keys */
    static /*const*/ IPDR_Key _keys[];

    /* Number of template keys */
    static const int _numKeys;
};


/** Simple record.
 *  It has default destructor, copy constructor, assignment because there
 *  are no pointers.
 */
class SimpleRecord : public Record
{
public:
    XDR_INT			xdrINT;			/**< an integer in the range [-2147483648,2147483647] */
    XDR_UINT		xdrUINT;		/**< a nonnegative integer in the range [0,4294967295] */
    XDR_LONG		xdrLONG;		/**< obvious extensions of integer and unsigned integer defined above. */
    XDR_ULONG		xdrULONG;		/**< obvious extensions of integer and unsigned integer defined above. */
    XDR_FLOAT		xdrFlOAT;		/**< 4 bytes single-precision floating-point number */
    XDR_DOUBLE		xdrDOUBLE;		/**< 8 bytes double-precision floating-point number */
	DEFINE_BASE64(xdrBASE64, 10)
	DEFINE_HEXBINARY(xdrHEXBINARY, 13)
	XDR_BOOLEAN		xdrBOOLEAN;		/**< 1 byte value which contains 0 for FALSE and 1 for TRUE */
    XDR_BYTE		xdrBYTE;		/**< an integer in the range[-128,127] */
    XDR_UBYTE		xdrUBYTE;		 /**< a non-negative integer in the range[0,255] */
    XDR_SHORT		xdrSHORT;		/**< an integer in the range [-32783,32782] */
    XDR_USHORT		xdrUSHORT;		/**< a non-negative integer in the range [0,65565] */
    XDR_DATETIME	xdrDATETIME;
    XDR_TIMEMSEC	xdrTIMEMSTEC;
    XDR_IPV4ADDR 	xdrIPV4ADDR;
    XDR_IPV6ADDR	xdrIPV6ADDR;
    XDR_UUID		xdrUUID;
    XDR_TIMEUSEC	xdrTIMEUSEC;
    XDR_MACADDR		xdrMACADDR;

    int templateId() const { return 3; }
    const char* templateDescr() const { return "simple"; }
    const IPDR_Key* keys() const { return _keys; }
    int numKeys() const { return _numKeys; }

    void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick);

    ostream& printKeys(ostream& ostr) const;
    ostream& printRec(ostream& ostr) const;

protected:

    IPDR_Key* keysWritable() { return _keys; }

    /** Template keys */
    static /*const*/ IPDR_Key _keys[];

    /** Number of template keys */
    static const int _numKeys;
};

/** DOCSYS record
 * Implements DOCSYS IPDR template, according to
 * DOCSIS 2.0 Service Specification
*/
class DOCSISRecord : public Record
{
private:
	/*static */DEFINE_STRING(CMTShostName, 256)
	/*static */XDR_IPV4ADDR 	CMTSipAddress;
	/*static */XDR_UINT			CMTSsysUpTime;
	/*static */DEFINE_STRING(CMTScatvIfName, 256);
	/*static */XDR_UINT			CMTScatvIfIndex;
	/*static */DEFINE_STRING(CMTSupIfName, 256);
	/*static */XDR_INT			CMTSupIfType;
	/*static */DEFINE_STRING(CMTSdownIfName, 256);
	/*static */XDR_MACADDR		CMMacAddress;
	/*static */XDR_IPV4ADDR		CMipAddress;
	/*static */XDR_INT			CMdocsisMode;
	/*static *//*IPDR_CMcpeInfo	CMcpeInfoList;*/	/*TODO: IPDR_CMcpeInfo not supported yet*/
	/*static */XDR_IPV4ADDR		CmcpeIpAddress;
	/*static */XDR_MACADDR		CMcpeMacAddress;
	/*static */DEFINE_STRING(CMcpeFqdn, 256);
	/*static */XDR_INT 			RecType;
	/*static */XDR_UINT			serviceIdentifier;
	/*static */DEFINE_STRING(serviceClassName, 256);
	/*static */XDR_INT			serviceDirection;
	/*static */XDR_ULONG	 	serviceOctetsPassed;
	/*static */XDR_ULONG	 	servicePktsPassed;
	/*static */XDR_UINT 		serviceSlaDropPkts;
	/*static */XDR_UINT 		serviceSlaDelayPkts;
	/*static */XDR_UINT 		serviceTimeCreated;
	/*static */XDR_UINT			serviceTimeActive;


public:
    int templateId() const { return 10; }
    const char* templateDescr() const { return "DOCSIS 2.0 record"; }
    const IPDR_Key* keys() const { return _keys; }
    int numKeys() const { return _numKeys; }

    void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick);

    ostream& printKeys(ostream& ostr) const;
    ostream& printRec(ostream& ostr) const;

protected:

    IPDR_Key* keysWritable() { return _keys; }

    /** Template keys */
    static /*const*/ IPDR_Key _keys[];

    /** Number of template keys */
    static const int _numKeys;
};

/**  VoIP Record
 * Implements VoIP IPDR Template
 *
*/
class VoIPRecord : public Record
{
private:
	DEFINE_STRING(subscriberID, 256);
	DEFINE_STRING(hostName, 256);
	XDR_IPV4ADDR 	ipAddress;

public:
    int templateId() const { return 11; }
    const char* templateDescr() const { return "VoIP 3.5 record"; }
    const IPDR_Key* keys() const { return _keys; }
    int numKeys() const { return _numKeys; }

    void fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick);

    ostream& printKeys(ostream& ostr) const;
    ostream& printRec(ostream& ostr) const;

protected:

    IPDR_Key* keysWritable() { return _keys; }

    /** Template keys */
    static /*const*/ IPDR_Key _keys[];

    /** Number of template keys */
    static const int _numKeys;
};

