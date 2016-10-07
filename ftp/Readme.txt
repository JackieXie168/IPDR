IPDR FTP Reference Library

The objective of the IPDR Reference libraries is to provide software 
implementers of the IPDR standards with libraries and tools to reduce 
construction cost, increase quality, and increase interoperability of 
their IPDR facilities.  The IPDR C FTP Reference Library provides 
routines that support the implementation of the file transfer protocol 
outlined in the IPDR NDM-U 3.5.

Release 2.0.0 of the IPDR C File Transfer Reference Library is mostly 
unchanged compared to Release 1.0.0, the only differnce being the addition 
of support for the new <BRRecord> element in capability files which was 
introduced in NDM-U 3.5.

The libraries are available in ANSI C and Java and currently support 
NDM-U version 3.5. 

These libraries are governed by the license agreement specified in
the file LICENSE.TXT found at the top of the directory tree.


Release Structure

The structure of this release is as follows

    docs

        Contains installation instructions for windows, installation 
        instruction for unix, integration test cases, Developer Guide, 
        API documentation

    libs

        Contains compiled libraries and shared objects of the reference 
        and third party libraries.

    include

        Contains header files for reference and third party packages

    source

        Contains entire source code

    test

        Contains test scripts to test the libraries



The libraries can be downloaded from www.sourceforge.net/projects/ipdr.

Once downloaded refer to the Installation Steps on Unix.htm or 
Installation Steps on Windows.htm for detailed instructions on the 
build process.


