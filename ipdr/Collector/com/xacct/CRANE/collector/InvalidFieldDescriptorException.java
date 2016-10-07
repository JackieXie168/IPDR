package com.xacct.CRANE.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** An exception that is thrown when an invalid FieldDescriptor is
 *  used (e.g., if it's in a pool but has been deallocated).
 *  @see FieldDescriptor
 */
public class InvalidFieldDescriptorException extends Exception
{
    /** $Id: InvalidFieldDescriptorException.java,v 1.3 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: InvalidFieldDescriptorException.java,v 1.3 2005/01/30 15:13:43 ilyad Exp $"; }

    /**
     * Creates a new InvalidFieldDescriptorException without detail message.
     */
    public InvalidFieldDescriptorException() {
        super();
    }

    /**
     * Creates a new InvalidFieldDescriptorException with the specified detail message.
     *
     * @param msg the detail message.
     */
    public InvalidFieldDescriptorException(String message) {
        super(message);
    }
}
