package org.ipdr.crane;

import org.ipdr.api.configuration.FieldConfig;

// $Id: ProtocolMsgKeyBlockHandler.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** An abstract class for use handling either basic or extended key
 *  blocks from a template block.
 *
 *  @version $Id: ProtocolMsgKeyBlockHandler.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
 */
public interface ProtocolMsgKeyBlockHandler
{
    /** Get a single extended key block from a raw bunch of bytes.
     *  @param msg The raw byte vector.
     *  @param offset Where the extended key block starts within msg.
     *  @param field The field, which will be updated with the new values
     *               (so, it must already be created by "new FieldConfig()").
     *            <em>This leaves the fieldDesc as null</em> so you must
     *            call setFieldDesc() to finish things off.
     *  @return offset of whatever follows this extended key block.
     *  @see FieldConfig#setFieldDesc
     */
    public int getFromRaw(byte [] msg, int offset, FieldConfig fieldList);
}
