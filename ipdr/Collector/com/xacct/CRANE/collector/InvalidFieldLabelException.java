package com.xacct.CRANE.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** An exception that is thrown to indicate an invalid field label.
 *  @see FieldDescriptorFactory#validateFieldLabel
 */
public class InvalidFieldLabelException extends Exception
{
    /** $Id: InvalidFieldLabelException.java,v 1.3 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: InvalidFieldLabelException.java,v 1.3 2005/01/30 15:13:39 ilyad Exp $"; }

    /** Creates a new InvalidFieldLabelException with the specified detailed error message.
     *
     * @param detailMessage The detailed error message
     */
    public InvalidFieldLabelException(String detailMessage) {
        super(detailMessage);
    }
}
