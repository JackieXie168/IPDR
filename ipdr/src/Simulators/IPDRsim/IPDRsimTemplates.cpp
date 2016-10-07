/** @file IPDRsimTemplates.cpp
 *
 *  Implementation of the IPDRsimTemplates
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: IPDRsimTemplates.cpp,v 1.37 2005/05/09 13:30:02 dmitryk Exp $
 *
 */

const char IPDRsimTemplates_rcsid[] = "@(#) $Id: IPDRsimTemplates.cpp,v 1.37 2005/05/09 13:30:02 dmitryk Exp $";

#include "IPDRsimTemplates.h"
#include <time.h>

extern bool fastFillFlag; /// yuck: a global ... defined in IPDRsim.cpp

/** An enumeration of all the template key IDs. Note that there is a
 * unique number of each key, regardless of which template it is used
 * in. The weird number scheme is for historical reasons: some fields
 * were added later. */

enum {
    // common keys
    keyID_transaction_type       =  1,
    keyID_message_type           =  2,
    keyID_invoke_id              =  3,
    keyID_transaction_id         =  4,
    keyID_called_net_adds_len    =  5,
    keyID_called_net_np          =  6,
    keyID_called_net_enc_scheme  =  7,
    keyID_called_net_noa         =  8,
    keyID_called_net_adds        =  9,
    keyID_calling_net_adds_len   = 10,
    keyID_calling_net_np         = 11,
    keyID_calling_net_enc_scheme = 12,
    keyID_calling_net_noa        = 13,
    keyID_calling_net_adds       = 14,
    keyID_timestamp              = 25, // out of order key ID

// end of common keys

// XyzFwd-only keys

    keyID_dest_adds_type         = 15,
    keyID_dest_adds_len          = 16,
    keyID_dest_adds_noa          = 17,
    keyID_dest_adds_np           = 18,
    keyID_dest_adds              = 19,
    keyID_orig_adds_type         = 20,
    keyID_orig_adds_len          = 21,
    keyID_orig_adds_noa          = 22,
    keyID_orig_adds_np           = 23,
    keyID_orig_adds              = 24,

// XyzAck-only keys

    keyID_error                  = 26
};

/** Safe version of strcpy(). It checks the length and only works for
 *  things like "char Xyz[100]"; it won't work for dynamically
 *  allocated (e.g., "char *Xyz = malloc(100)") or function parameters
 *  ... and it won't give an error message that it isn't working
 *  (although the bad results should be immediately obvious)
 *  @param dest The destination of the copy (must <em>not</em> be dynamically allocated or a function pointer).
 *  @param src  The source of the copy.
 */
#define SAFE_STRCPY(dest, src) { strncpy(dest, src, sizeof (dest)); dest[sizeof (dest) - 1] = '\0'; }

/** Fill a C-string with all the same value, to make Purify
 *  happy. Only works for things like "char Xyz[100]"; it wo't work
 *  for dynamically allocated (e.g., "char *Xyz = malloc(100)") or
 *  function parameters ... and it won't give an error message that it
 *  isn't working (although the bad results should be immediately
 *  obvious)
 *  @param dest The destination to be smeared (must <em>not</em> be dynamically allocated or a function pointer).
 *  @param fill The fill character.
 */
#define SMEAR(dest, fill) { memset(dest, fill, sizeof (dest)); }

/** Set a key to disabled
 *  @param ID the key's id.
 *  @param disabled true if the key is to be disabled.
 *  @return true if the key's state (enabled/disabled) was changed; false
 *               if no change or the key wasn't found.
 */
bool Record::setKeyDisabled(unsigned int ID,
                            bool enabled)
{
    for (int i = 0; i < numKeys(); i++) {
        IPDR_Key &ky = keysWritable()[i];
        if (ky.KeyID == ID) {
            unsigned int oldE = ky.Enabled;
            ky.Enabled = enabled ;
            return (oldE != enabled);
        }
    }
    // quietly do nothing if it's not there: should probably throw exception

    return false;
}

/** Helper for printing out key information
 *  @param ostr output stream
 *  @param keys vector of IPDR template keys
 *  @param numKeys number of template keys
 *  @return ostr
 */
static
ostream& printKeysHelper(ostream& ostr, const IPDR_Key keys[], int numKeys)
{
    for (int i = 0; i < numKeys; i++) {
        ostr << keys[i].KeyID << ":\t";
        switch (keys[i].KeyTypeID) {
        case XDR_TYPE_BOOLEAN:	ostr << "bool    "; break;
        case XDR_TYPE_UBYTE:    ostr << "uint8   "; break;
        case XDR_TYPE_BYTE:     ostr << "int8    "; break;
        case XDR_TYPE_USHORT:   ostr << "uint16  "; break;
        case XDR_TYPE_SHORT:    ostr << "int16   "; break;
        case XDR_TYPE_UINT:    	ostr << "uint32  "; break;
        case XDR_TYPE_INT:     	ostr << "int32   "; break;
        case XDR_TYPE_ULONG:    ostr << "uint64  "; break;
        case XDR_TYPE_LONG:     ostr << "int64   "; break;
        case XDR_TYPE_FLOAT:    ostr << "float   "; break;
        case XDR_TYPE_DOUBLE:   ostr << "double  "; break;
        case XDR_TYPE_STRING:   ostr << "string  "; break;
		/*XDR_TYPE_HEXBINARY bug in specs HEXBINARY and BASE64 have same type ID*/
		case XDR_TYPE_BASE64:	ostr << "base64 | hexbinary"; break;
        case XDR_TYPE_IPV4ADDR: ostr << "ipv4Addr"; break;
        case XDR_TYPE_IPV6ADDR: ostr << "ipv6Addr"; break;
        case XDR_TYPE_DATETIME: ostr << "dateTime ";break;
        case XDR_TYPE_TIMEMSEC: ostr << "timeMsec"; break;
        case XDR_TYPE_TIMEUSEC: ostr << "timeUsec"; break;
        default: ostr << "type#" << keys[i].KeyTypeID;  break;
        }
        ostr << " " << keys[i].KeyName << "\t";
        switch (keys[i].AccessType) {
        case IPDR_ACCESS_BY_OFFSET:
        {
            ostr << "offset " << reinterpret_cast<IPDR_Offset>(keys[i].KeyAccessor);
        }
        break;
        case IPDR_ACCESS_BY_FUNCTION:
            ostr << "function " << keys[i].KeyAccessor << "(" << keys[i].ExtraData << ")";
            break;
        default:
            ostr << "unknown access<#" << keys[i].AccessType << ">";
        }
        ostr << endl;
    }
    return ostr << "\t(" << numKeys << " keys)" << endl;
}

void XyzFwdRecord::fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick)
{
    // The values here are just for testing and bear no relationship with reality.
    message_type = recNum;
    transaction_type       = 71; // ASCII "G"
    message_type           = 72; //       "H"
    invoke_id              = 73; //       "I"
    transaction_id         = 16909060; // 0x01020304
    called_net_adds_len    = 75; //       "K"
    called_net_np          = 76; //       "L"
    called_net_enc_scheme  = 77; //       "M"
    called_net_noa         = 78; //       "N"
    calling_net_adds_len   = 0;
    calling_net_np         = 81; //       "Q"
    calling_net_enc_scheme = 82; //       "R"
    calling_net_noa        = 83; //       "S"
    dest_adds_type         = 85; //       "U"
    dest_adds_noa          = 87; //       "W"
    dest_adds_np           = 88; //       "X"
    dest_adds_len          = 0;
	orig_adds_type         = 90; //       "Z"
    orig_adds_len          = 0;
    orig_adds_noa          = 93; //       "]" // skip 92 = backslash
    orig_adds_np           = 97; //       "a"
    timestamp = tick.tv_sec;
    if (tick.tv_nsec >= 500000000) { // round up
        timestamp++;
    }
}

/** Create a time-stamp string to something like ISO standard form
 *  @param buf (updated) Buffer for the string
 *  @param bufSize Size of buf
 *  @param timestamp The time value to turn into a string
 *  @return C-string with the timestamp, a pointer into buf
 */
static char* timestampStr(char* buf, size_t bufSize, IPDR_Time timestamp)
{
    struct tm tmMem;
    struct tm* tmPtr = localtime_r(&timestamp, &tmMem);
    strftime(buf, bufSize, "%Y-%m-%d %H:%M:%S", tmPtr);
    return buf;
}

ostream& XyzFwdRecord::printKeys(ostream& ostr) const {
    return printKeysHelper(ostr, _keys, _numKeys);
}

ostream& XyzFwdRecord::printRec(ostream& ostr) const {
    char timeBuf[50];
    ostr <<
        static_cast<unsigned>(called_net_adds_len) << "," <<
        called_net_enc_scheme << "," <<
        called_net_noa << "," <<
        called_net_np << "," <<
        static_cast<unsigned>(calling_net_adds_len) << "," <<
        static_cast<unsigned>(calling_net_enc_scheme) << "," <<
        calling_net_noa << "," <<
        static_cast<unsigned>(calling_net_np) << "," <<
        static_cast<unsigned>(dest_adds_len) << "," <<
        static_cast<unsigned>(dest_adds_noa) << "," <<
        static_cast<unsigned>(dest_adds_np) << "," <<
        static_cast<unsigned>(dest_adds_type) << "," <<
        static_cast<unsigned>(invoke_id) << "," <<
        static_cast<unsigned>(message_type) << "," <<
        static_cast<unsigned>(orig_adds_len) << "," <<
        static_cast<unsigned>(orig_adds_noa) << "," <<
        static_cast<unsigned>(orig_adds_np) << "," <<
        static_cast<unsigned>(orig_adds_type) << "," <<
        transaction_id << "," <<
        static_cast<unsigned>(transaction_type) << "," <<
        timestampStr(timeBuf, sizeof timeBuf, timestamp);
    return ostr;
}

/** Replacement for C's offsetof macro.  In theory, C's offsetof()
 *  doesn't work with non-POD (non-"Plain Old Data" - that is, a class
 *  or struct with only data and which doesn't override the "&"
 *  operator). So, a good C++ compiler will complain about our use of
 *  offsetof below. The following macro ought to work instead,
 *  although it depends on the length of an ordinary pointer being the
 *  same length as a "long" (it's easy to change:
 *  reinterpret_cast<long>).
 *
 *  You can understand this better by the following stages:
 *  <br>Type *nullTypePtr = 0; // reinterpret_cast<Type*>(0)
 *  <br>int Type::*offset = &Type::field;
 *  <br>long offset = reinterpret_cast<int>(&(nullTypePtr->*field));
 */
#define CPPoffsetof(type, field) (reinterpret_cast<long>(&((reinterpret_cast<type*>(0))->*(&type::field))))


/* The following isn't good style because we update this data with
   setKeyDisabled() but it's global data and should therefore be in
   "read only" storage. But this is a demo program and it would be
   more work to do things the right way. */

/*const*/ IPDR_Key XyzFwdRecord::_keys[] =
{
    { keyID_transaction_type, XDR_TYPE_UBYTE,
       "transaction_type", "transaction_type", "(help for) transaction_type",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, transaction_type) },
    { keyID_message_type, XDR_TYPE_UBYTE,
       "message_type", "message_type", "(help for) message_type",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, message_type) },
    { keyID_invoke_id, XDR_TYPE_UBYTE,
       "invoke_id", "invoke_id", "(help for) invoke_id",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, invoke_id) },
    { keyID_transaction_id, XDR_TYPE_UINT,
       "transaction_id", "transaction_id", "(help for) transaction_id",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, transaction_id) },
    { keyID_called_net_adds_len, XDR_TYPE_UBYTE,
       "called_net_adds_len", "called_net_adds_len", "(help for) called_net_adds_len",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, called_net_adds_len) },
    { keyID_called_net_np, XDR_TYPE_USHORT,
       "called_net_np", "called_net_np", "(help for) called_net_np",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, called_net_np) },
    { keyID_called_net_enc_scheme, XDR_TYPE_SHORT,
      "called_net_enc_scheme", "called_net_enc_scheme", "(help for) called_net_enc_scheme",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, called_net_enc_scheme) },
    { keyID_called_net_noa, XDR_TYPE_UINT,
       "called_net_noa", "called_net_noa", "(help for) called_net_noa",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, called_net_noa) },
    { keyID_calling_net_adds_len, XDR_TYPE_UBYTE,
      "calling_net_adds_len", "calling_net_adds_len", "(help for) calling_net_adds_len",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, calling_net_adds_len) },
    { keyID_calling_net_np, XDR_TYPE_UBYTE,
      "calling_net_np", "calling_net_np", "(help for) calling_net_np",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, calling_net_np) },
    { keyID_calling_net_enc_scheme, XDR_TYPE_UBYTE,
      "calling_net_enc_scheme", "calling_net_enc_scheme", "(help for) calling_net_enc_scheme",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, calling_net_enc_scheme) },
    { keyID_calling_net_noa, XDR_TYPE_INT,
      "calling_net_noa", "calling_net_noa", "(help for) calling_net_noa",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, calling_net_noa) },
    { keyID_timestamp, XDR_TYPE_TIMEUSEC,
      "timestamp", "timestamp", "(help for) timestamp",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, timestamp) },

// End of common keys

    { keyID_dest_adds_type, XDR_TYPE_UBYTE,
      "dest_adds_type", "dest_adds_type", "(help for) dest_adds_type",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, dest_adds_type) },
    { keyID_dest_adds_len, XDR_TYPE_UBYTE,
      "dest_adds_len", "dest_adds_len", "(help for) dest_adds_len",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, dest_adds_len) },
    { keyID_dest_adds_noa, XDR_TYPE_UBYTE,
      "dest_adds_noa", "dest_adds_noa", "(help for) dest_adds_noa",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, dest_adds_noa) },
    { keyID_dest_adds_np, XDR_TYPE_UBYTE,
      "dest_adds_np", "dest_adds_np", "(help for) dest_adds_np",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, dest_adds_np) },
    { keyID_orig_adds_type, XDR_TYPE_UBYTE,
      "orig_adds_type", "orig_adds_type", "(help for) orig_adds_type",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, orig_adds_type) },
    { keyID_orig_adds_len, XDR_TYPE_UBYTE,
      "orig_adds_len", "orig_adds_len", "(help for) orig_adds_len",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, orig_adds_len) },
    { keyID_orig_adds_noa, XDR_TYPE_UBYTE,
      "orig_adds_noa", "orig_adds_noa", "(help for) orig_adds_noa",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, orig_adds_noa) },
    { keyID_orig_adds_np, XDR_TYPE_UBYTE,
      "orig_adds_np", "orig_adds_np", "(help for) orig_adds_np",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzFwdRecord, orig_adds_np) }
};

const int XyzFwdRecord::_numKeys = sizeof _keys / sizeof _keys[0];

void XyzAckRecord::fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick)
{
	// The following are a bunch of testing values

    transaction_type       = 103; // ASCII "g"
    message_type           = 104; //       "h"
    invoke_id              = 105; //       "i"
    transaction_id         = 84281096; // 0x05060708
    called_net_adds_len    = 107; //       "k"
    called_net_np          = 108; //       "l"
    called_net_enc_scheme  = 109; //       "m"
    called_net_noa         = 110; //       "n"
    calling_net_adds_len   = 112; //       "p"
    calling_net_np         = 113; //       "q"
    calling_net_enc_scheme = 114; //       "r"
    calling_net_noa        = 115; //       "s"
	called_net_adds_len    = 0;
	calling_net_adds_len   = 0;
    error                  = 117; //       "u"
    timestamp = tick.tv_sec;
    if (tick.tv_nsec >= 500000000) { // round up
        timestamp++;
    }
}

ostream& XyzAckRecord::printKeys(ostream& ostr) const {
    return printKeysHelper(ostr, _keys, _numKeys);
}

ostream& XyzAckRecord::printRec(ostream& ostr) const {
    char timeBuf[50];
    ostr <<
        static_cast<unsigned>(called_net_adds_len) << "," <<
        called_net_enc_scheme << "," <<
        called_net_noa << "," <<
        called_net_np << "," <<
        static_cast<unsigned>(calling_net_adds_len) << "," <<
        static_cast<unsigned>(calling_net_enc_scheme) << "," <<
        calling_net_noa << "," <<
        static_cast<unsigned>(calling_net_np) << "," <<
        /* dest_adds << */ "," <<
        /* static_cast<unsigned>(dest_adds_len) << */ "," <<
        /* static_cast<unsigned>(dest_adds_noa) << */ "," <<
        /* static_cast<unsigned>(dest_adds_np) << */ "," <<
        /* static_cast<unsigned>(dest_adds_type) << */ "," <<
        static_cast<unsigned>(error) << "," <<
        static_cast<unsigned>(invoke_id) << "," <<
        static_cast<unsigned>(message_type) << "," <<
        /* orig_adds << */ "," <<
        /* static_cast<unsigned>(orig_adds_len) << */ "," <<
        /* static_cast<unsigned>(orig_adds_noa) << */ "," <<
        /* static_cast<unsigned>(orig_adds_np) << */ "," <<
        /* static_cast<unsigned>(orig_adds_type) << */ "," <<
        transaction_id << "," <<
        static_cast<unsigned>(transaction_type) << "," <<
        timestampStr(timeBuf, sizeof timeBuf, timestamp);
    return ostr;
}

/** Function for testing access to fields by function instead of
 *  offset.  The appropriate entry in XyzAckRecord::_keys is set to
 *  #IPDR_ACCESS_BY_FUNCTION instead of #IPDR_ACCESS_BY_OFFSET.
 *
 *  @param data Pointer to the record: XyzAckRecord
 *  @param extraData (unused) Extra context pointer
 *  @param value pointer to (updated) pointer to result
 *  @see XyzAckRecord
 */
 /*
static
void XyzAckRecord_calling_net_adds(const void *data, void *extraData, void const **value)
{
    const XyzAckRecord* rec = static_cast<const XyzAckRecord*>(data);
    *value = rec->calling_net_adds;
}
*/
/* The following isn't good style because we update this data with
   setKeyDisabled() but it's global data and should therefore be in
   "read only" storage. But this is a demo program and it would be
   more work to do things the right way. */

/*const*/ IPDR_Key XyzAckRecord::_keys[] =
{
    // KeyID, KeyTypeID, keyName/Label/Help, keyAttributes, accessType, NULL, Offset
    { keyID_transaction_type, XDR_TYPE_UBYTE,
       "transaction_type", "transaction_type", "(help for) transaction_type",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, transaction_type) },
    { keyID_message_type, XDR_TYPE_UBYTE,
       "message_type", "message_type", "(help for) message_type",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, message_type) },
    { keyID_invoke_id, XDR_TYPE_UBYTE,
       "invoke_id", "invoke_id", "(help for) invoke_id",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, invoke_id) },
    { keyID_transaction_id, XDR_TYPE_UINT,
       "transaction_id", "transaction_id", "(help for) transaction_id",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, transaction_id) },
    { keyID_called_net_adds_len, XDR_TYPE_UBYTE,
       "called_net_adds_len", "called_net_adds_len", "(help for) called_net_adds_len",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, called_net_adds_len) },
    { keyID_called_net_np, XDR_TYPE_USHORT,
       "called_net_np", "called_net_np", "(help for) called_net_np",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, called_net_np) },
    { keyID_called_net_enc_scheme, XDR_TYPE_SHORT,
       "called_net_enc_scheme", "called_net_enc_scheme", "(help for) called_net_enc_scheme",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, called_net_enc_scheme) },
    { keyID_called_net_noa, XDR_TYPE_UINT,
       "called_net_noa", "called_net_noa", "(help for) called_net_noa",
       1, IPDR_ACCESS_BY_OFFSET, NULL,
       (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, called_net_noa) },
    { keyID_calling_net_adds_len, XDR_TYPE_UBYTE,
      "calling_net_adds_len", "calling_net_adds_len", "(help for) calling_net_adds_len",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, calling_net_adds_len) },
    { keyID_calling_net_np, XDR_TYPE_UBYTE,
      "calling_net_np", "calling_net_np", "(help for) calling_net_np",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, calling_net_np) },
    { keyID_calling_net_enc_scheme, XDR_TYPE_UBYTE,
      "calling_net_enc_scheme", "calling_net_enc_scheme", "(help for) calling_net_enc_scheme",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, calling_net_enc_scheme) },
    { keyID_calling_net_noa, XDR_TYPE_INT,
      "calling_net_noa", "calling_net_noa", "(help for) calling_net_noa",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, calling_net_noa) },
    { keyID_timestamp, XDR_TYPE_TIMEUSEC,
      "timestamp", "timestamp", "(help for) timestamp",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, timestamp) },
// End of common keys
    { keyID_error, XDR_TYPE_UBYTE,
      "error", "error", "(help for) error",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(XyzAckRecord, error) }
};

const int XyzAckRecord::_numKeys = sizeof _keys / sizeof _keys[0];


/*const*/ IPDR_Key SimpleRecord::_keys[] = {
    { 143, XDR_TYPE_INT, "xdrINT", "xdrINT", "an integer in the range [-2147483648,2147483647]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrINT) },
    { 145, XDR_TYPE_UINT, "xdrUINT", "xdrUINT", "a nonnegative integer in the range [0,4294967295]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrUINT) },
    { 147, XDR_TYPE_LONG, "xdrLONG", "xdrLONG", "obvious extensions of integer and unsigned integer defined above.",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrLONG) },
    { 149, XDR_TYPE_ULONG, "xdrULONG", "xdrULONG", "obvious extensions of integer and unsigned integer defined above.",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrULONG) },
    { 151, XDR_TYPE_FLOAT, "xdrFlOAT", "xdrFlOAT", "4 bytes single-precision floating-point number",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrFlOAT) },
    { 152, XDR_TYPE_DOUBLE, "xdrDOUBLE", "xdrDOUBLE", "8 bytes single-precision floating-point number",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrDOUBLE) },
    { 153, XDR_TYPE_BASE64, "xdrBASE64", "xdrBASE64", "(help for) xdrBASE64",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, ACCESS_BASE64(xdrBASE64)) },
    { 154, XDR_TYPE_HEXBINARY, "xdrHEXBINARY", "xdrHEXBINARY", "4 byte length indication + hex data",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, ACCESS_HEXBINARY(xdrHEXBINARY)) },
    { 155, XDR_TYPE_BOOLEAN, "xdrBOOLEAN", "xdrBOOLEAN", "1 byte value which contains 0 for FALSE and 1 for TRUE",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrBOOLEAN) },
    { 157, XDR_TYPE_BYTE, "xdrBYTE", "xdrBYTE", "an integer in the range[-128,127]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrBYTE) },
    { 159, XDR_TYPE_UBYTE, "xdrUBYTE", "xdrUBYTE", "a non-negative integer in the range[0,255]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrUBYTE) },
    { 161, XDR_TYPE_SHORT, "xdrSHORT", "xdrSHORT", "an integer in the range [-32783,32782]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrSHORT) },
    { 163, XDR_TYPE_USHORT, "xdrUSHORT", "xdrUSHORT", "a non-negative integer in the range [0,65565]",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrUSHORT) },
    { 165, XDR_TYPE_DATETIME, "xdrDATETIME", "xdrDATETIME", "(help for) xdrDATETIME",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrDATETIME) },
    { 167, XDR_TYPE_TIMEMSEC, "xdrTIMEMSTEC", "xdrTIMEMSTEC", "(help for) xdrTIMEMSTEC",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrTIMEMSTEC) },
    { 169, XDR_TYPE_IPV4ADDR, "xdrIPV4ADDR", "xdrIPV4ADDR", "(help for) xdrIPV4ADDR",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrIPV4ADDR) },
    { 171, XDR_TYPE_IPV6ADDR, "xdrIPV6ADDR", "xdrIPV6ADDR", "(help for) xdrIPV6ADDR",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrIPV6ADDR) },
    { 173, XDR_TYPE_UUID, "xdrUUID", "xdrUUID", "(help for) xdrUUID",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrUUID) },
    { 175, XDR_TYPE_TIMEUSEC, "xdrTIMEUSEC", "xdrTIMEUSEC", "(help for) xdrTIMEUSEC",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrTIMEUSEC) },
    { 177, XDR_TYPE_MACADDR, "xdrMACADDR", "xdrMACADDR", "(help for) xdrMACADDR",
      1, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(SimpleRecord, xdrMACADDR) },
};

const int SimpleRecord::_numKeys = sizeof _keys / sizeof _keys[0];

void SimpleRecord::fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick) {

	xdrINT = 		0x12345678;
	xdrUINT = 		0xFFFFFFFFu;
	xdrLONG = 		0x0102030405060708llu;
	xdrULONG = 		0xFFFFFFFFFFFFFFFFllu;
	xdrFlOAT = 		123.456f;
	xdrDOUBLE = 	123456.123456;
	FILL_BASE64(xdrBASE64, "xdrBASE64")
	FILL_HEXBINARY(xdrHEXBINARY, "xdrHEXBINARY", 12)
	xdrBOOLEAN = 	0x0u;
	xdrBYTE = 		0xEE;
	xdrUBYTE = 		0xFFu;
	xdrSHORT = 		0x1122;
	xdrUSHORT = 	0xFFFFu;
	xdrDATETIME = 	0x21436587u;
	xdrTIMEMSTEC = 	0x9812327643874387llu;
	xdrIPV4ADDR = 	0x01020304u;
	for(unsigned char i = 0 ; i < 16 ; i++)
		xdrIPV6ADDR[i] = i;
	xdrUUID[0] = 0x00u;
	xdrUUID[1] = 0x00u;
	xdrUUID[2] = 0x00u;
	xdrUUID[3] = 0x10u;
	for(unsigned char i = 4 ; i < 20 ; i++)
		xdrUUID[i] = i;
	xdrTIMEUSEC = 	0x9856435677554433llu;
	xdrMACADDR = 	0x0000030405060708llu;
    }

ostream& SimpleRecord::printKeys(ostream& ostr) const {
    return printKeysHelper(ostr, _keys, _numKeys);
}

ostream& SimpleRecord::printRec(ostream& ostr) const {

    ostr << "int(" << ((int)xdrINT) << ")" << "," <<
        "unsigned int(" << xdrUINT << ")" << "," <<
        "long(" << xdrLONG << ")" << "," <<
	"unsigned long(" << xdrULONG << ")" << "," <<
	"float(" << xdrFlOAT << ")" << "," <<
	"double(" << xdrDOUBLE << ")" << "," <<
	"bool(" << xdrBOOLEAN << ")" << "," <<
	"char(" << xdrBYTE << ")" << "," <<
	"unsigned char(" << xdrUBYTE << ")" << "," <<
	"short(" << xdrUBYTE << ")" << "," <<
	"unsigned short(" << xdrUBYTE << ")";
//       << "," << transaction_type << "," << timestamp;

    return ostr;
}


void DOCSISRecord::fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick)
{
	FILL_STRING(	CMTShostName, 			"e.g.cmts01.mso.com")
	/*XDR_IPV4ADDR*/CMTSipAddress = 		0x0a016401u;
	/*XDR_UINT*/	CMTSsysUpTime = 		0x11223344u;
	FILL_STRING(	CMTScatvIfName, 		"CMTScatvIfName");
	/*XDR_UINT*/	CMTScatvIfIndex =		0x54235467u;
	FILL_STRING(	CMTSupIfName, 			"CMTSupIfName");
	/*XDR_INT*/		CMTSupIfType =			0x32124355;
	FILL_STRING(	CMTSdownIfName, 		"CMTSdownIfName");
	/*XDR_MACADDR*/	CMMacAddress =			0x0000998877665544llu;
	/*XDR_IPV4ADDR*/CMipAddress	=			0x0a016422u;
	/*XDR_INT*/		CMdocsisMode = 			0x00000001;
	/*XDR_IPV4ADDR*/CmcpeIpAddress = 		0x0a016411u;
	/*XDR_MACADDR*/	CMcpeMacAddress = 		0x0000223344556677llu;;
	FILL_STRING(	CMcpeFqdn, 				"CMcpeFqdn");
	/*XDR_INT*/ 	RecType = 				0x98675432;
	/*XDR_UINT*/	serviceIdentifier = 	0xFFFFFFFFu;
	FILL_STRING(	serviceClassName, 		"serviceClassName");
	/*XDR_INT*/		serviceDirection = 		0x12345678;
	/*XDR_ULONG*/	serviceOctetsPassed = 	0x9988776655443322llu;
	/*XDR_ULONG*/	servicePktsPassed = 	0x1122334455667788llu;
	/*XDR_UINT*/ 	serviceSlaDropPkts = 	0x99999999u;
	/*XDR_UINT*/ 	serviceSlaDelayPkts = 	0x88888888u;
	/*XDR_UINT*/ 	serviceTimeCreated = 	0x77777777u;
	/*XDR_UINT*/	serviceTimeActive =		0x66666666u;
}

ostream& DOCSISRecord::printKeys(ostream& ostr) const
{
	return printKeysHelper(ostr, _keys, _numKeys);
}

ostream& DOCSISRecord::printRec(ostream& ostr) const
{
	ostr << "CMTShostName(" << CMTShostName << "), "
		 << "CMTSipAddress(" << CMTSipAddress << ") ";
	return ostr;
}

/** DOCSIS Template keys */
IPDR_Key DOCSISRecord::_keys[] =
{   { 101, XDR_TYPE_STRING, "CMTShostName", "L", "CMTS's fully qualified domain name (FQDN), if given or null",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(CMTShostName)) },

    { 102, XDR_TYPE_IPV4ADDR, "CMTSipAddress", "CMTSipAddressL", "CMTS's IPv4 address.Canonical IP address in dotted decimal notation",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMTSipAddress) },

    { 103, XDR_TYPE_UINT, "CMTSsysUpTime", "CMTSsysUpTimeL",
      "32-bit count of hundredths of a second since CMTS system initialization, in decimal notation",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMTSsysUpTime) },

    { 104, XDR_TYPE_STRING, "CMTScatvIfName", "CMTScatvIfNameL",
      "Up to 50 characters length string indicating the IfName of the CATV interface",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(CMTScatvIfName)) },

    { 105, XDR_TYPE_UINT, "CMTScatvIfIndex", "CMTScatvIfIndexL",
      "Value of the iFindex object for the CATV (MAC) interface used by this cable modem.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMTScatvIfIndex) },

    { 106, XDR_TYPE_STRING, "CMTSupIfName", "CMTSupIfNameL",
      "Up to 50 characters length string indicating the IfName of the logical Upstream Channel",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(CMTSupIfName)) },

    { 107, XDR_TYPE_INT, "CMTSupIfType", "CMTSupIfType",
      "The ifType value of the upstream channel associated with the CM. If this corresponds to a DOCSIS upstream channel it reports 129. Ifthis corresponds to a DOCSIS 2.0 upstream logical channels it reports 205",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMTSupIfType) },

    { 108, XDR_TYPE_STRING, "CMTSdownIfName", "CMTSdownIfName",
      "Up to 50 characters length string indicating the IfName of the logical Upstream Channel",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(CMTSdownIfName)) },

    { 109, XDR_TYPE_MACADDR, "CMMacAddress", "CMMacAddress",
      "Subscriber identified by the Cable Modem MAC address in dash delimited hex notation",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMMacAddress) },

    { 110, XDR_TYPE_IPV4ADDR, "CMipAddress", "CMipAddress",
      "CM's current Ipv4 address. Canonical IP address in dotted decimal notation",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMipAddress) },

    { 111, XDR_TYPE_INT, "CMdocsisMode", "CMdocsisMode",
      "The DOCSIS registration mode for this CM. Encoded as 10 => \"DOCSIS 1.0\", 11 => \"DOCSIS 1.1\",",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMdocsisMode) },

    /*TODO: IPDR_CMcpeInfo - Not supported yet*/

    { 113, XDR_TYPE_IPV4ADDR, "CmcpeIpAddress", "CmcpeIpAddress",
      "Current IPv4 address of all CPE using this CM, if any, or zero-length String if none",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CmcpeIpAddress) },

    { 114, XDR_TYPE_MACADDR, "CMcpeMacAddress", "CMcpeMacAddress",
      "CPE MAC address for this CM, if any, or zero-length String if none",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, CMcpeMacAddress) },

    { 115, XDR_TYPE_STRING, "CMcpeFqdn", "CMcpeFqdn",
      "The Fully Qualifies Domain Name (FQDN, RFC-1034) assigned to each CPE using this CM during the reporting interval or zero-length string if none.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(CMcpeFqdn)) },

    { 116, XDR_TYPE_INT, "RecType", "RecType",
      "Interim identifies running Service Flows or 1.0 CM s SIDs (SF/SIDs). Stop identifies terminated SF/SDIs.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, RecType) },

    { 117, XDR_TYPE_UINT, "serviceIdentifier", "serviceIdentifier",
      "Service Flow ID of the SF relative to its RFI MAC layer or For DOCSIS COS CM provisioning, the Service Identifier (SID) relative to its RFI MAC layer for CM upstream traffic. For CM downstream traffic records, this element contains the first CM upstream SID.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceIdentifier) },

    { 118, XDR_TYPE_STRING, "serviceClassName", "serviceClassName",
      "Service Class Name (SCN) of the Service Flow in the CMTS. For DOCSIS 1.0 CM",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, ACCESS_STRING(serviceClassName)) },

    { 119, XDR_TYPE_INT, "serviceDirection", "serviceDirection",
      "Direction of the SF from the CMTS cable interface",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceDirection) },

    { 120, XDR_TYPE_ULONG, "serviceOctetsPassed", "serviceOctetsPassed",
      "64-bit absolute counter value of octets passed by the Service Flow or For DOCSIS COS CM provisioning The current (or final) count of octets passed by this SID or CM Downstream packets, depending on serviceDirection",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceOctetsPassed) },

    { 121, XDR_TYPE_ULONG, "servicePktsPassed", "servicePktsPassed",
      "64-bit absolute counter value of packets passed by the Service flow of a DOCSIS QOS CM provisioning For DOCSIS COS CM provisioning The current (or final) count of packets passed by this SID or CM Downstream packets, depending on serviceDirection.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, servicePktsPassed) },

    { 122, XDR_TYPE_UINT, "serviceSlaDropPkts", "serviceSlaDropPkts",
      "32-bit absolute counter value of packets dropped exceeding SLA by this SF of the DOCSIS QOS CM provisioning. For DOCSIS COS CM provisioning. This counter is optional; if not supported a zero value is reported",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceSlaDropPkts) },

    { 123, XDR_TYPE_UINT, "serviceSlaDelayPkts", "serviceSlaDelayPkts",
      "32-bit absolute counter value of packets delayed exceeding SLA by this SF of the DOCSIS QOS CM provisioning. For DOCSIS COS CM provisioning, this counter is optional; if not supported a zero value is reported",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceSlaDelayPkts) },

    { 124, XDR_TYPE_UINT, "serviceTimeCreated", "serviceTimeCreated",
      "The value of CMTS sysUptime when Service Flow was created for DOCSIS QOS CM provisioning. For the DOCSIS COS CM provisioning is the time the non-temporary SID is created, for downstream CM traffic it indicates the time the CM registers.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceTimeCreated) },

    { 125, XDR_TYPE_UINT, "serviceTimeActive", "serviceTimeActive",
      "The total time that the Service Flow was Active in seconds or the DOCSIS 1.0 CM was registered.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(DOCSISRecord, serviceTimeActive) }
};

/** Number of template keys */
const int DOCSISRecord::_numKeys = sizeof _keys / sizeof _keys[0];

void VoIPRecord ::fill(const char* nowStr, unsigned pad, unsigned long recNum, const timespec tick)
{
	FILL_STRING( subscriberID, "johndoe@10.1.100.17" );
	FILL_STRING( hostName, "Cisco.Gateway" );
	ipAddress = 0x0a016401;
}

ostream& VoIPRecord::printKeys(ostream& ostr) const
{
	return printKeysHelper(ostr, _keys, _numKeys);
}

ostream& VoIPRecord::printRec(ostream& ostr) const
{
	ostr << "subscriberID(" << subscriberID << "), "  << "hostName(" << hostName << ") " << "ipAddress(" << ipAddress << ") " ;
	return ostr;
}

/** VoIP Template keys */
IPDR_Key VoIPRecord::_keys[] =
{
 { 201, XDR_TYPE_STRING, "subscriberID", "", "Unique within a serice provider network",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(VoIPRecord, ACCESS_STRING(subscriberID)) },
	{ 202, XDR_TYPE_STRING, "hostName", "", "Name of host management server",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(VoIPRecord, ACCESS_STRING(hostName)) },
    { 203, XDR_TYPE_IPV4ADDR, "ipAddress", "ipAddress", "IP address",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)CPPoffsetof(VoIPRecord, ipAddress) }
};

/** Number of template keys */
const int VoIPRecord::_numKeys = sizeof _keys / sizeof _keys[0];
