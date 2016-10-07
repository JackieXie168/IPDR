package com.xacct.CRANE.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** An exception that is thrown to indicate a failure to register a field.
 *  @see FieldDescriptorFactory
 */
public class FieldRegistrationException extends Exception
{
    /** $Id: FieldRegistrationException.java,v 1.3 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: FieldRegistrationException.java,v 1.3 2005/01/30 15:13:42 ilyad Exp $"; }

    /** Creates a new FieldRegistrationException with the specified detailed error message.
     *
     * @param message The detailed error message
     */
    public FieldRegistrationException(String message) {
        super(message);
    }
}
