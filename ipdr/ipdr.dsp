# Microsoft Developer Studio Project File - Name="ipdr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ipdr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ipdr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ipdr.mak" CFG="ipdr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ipdr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipdr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ipdr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPDR_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPDR_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "ipdr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPDR_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPDR_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib expat.lib libcurl.lib msvcrt.lib WSock32.lib /nologo /dll /debug /machine:I386 /out:"../libs/ipdr.dll" /pdbtype:sept /libpath:"../libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Debug\*.lib ..\libs	del ..\libs\*.ilk
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ipdr - Win32 Release"
# Name "ipdr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\xdr\AttributeDescriptorHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\common\descriptor.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\dynamicArray.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\errorHandler.c
# End Source File
# Begin Source File

SOURCE=..\source\common\FNFData.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\FNFDataHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\common\FNFType.c
# End Source File
# Begin Source File

SOURCE=..\source\common\getFile.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\hashtable.c
# End Source File
# Begin Source File

SOURCE=..\source\common\IPDRDocReader.c
# End Source File
# Begin Source File

SOURCE=..\source\common\IPDRDocWriter.c
# End Source File
# Begin Source File

SOURCE=..\source\common\IPDRHeader.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\IPDRMemory.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\IPDRRecord.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\IPDRStreamElementHelper.c
# End Source File
# Begin Source File

SOURCE=.\ipdrWin32.def
# End Source File
# Begin Source File

SOURCE=..\source\xdr\IPDRXDRDocEndHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\IPDRXDRHeaderHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\IPDRXDRRecordHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xml\IPDRXMLDocEndHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xml\IPDRXMLHeaderHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xml\IPDRXMLRecordHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\md5.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\NameSpaceInfoHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\RecordDescriptorHelper.c
# End Source File
# Begin Source File

SOURCE=..\source\common\schema.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\schemaUtil.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\serialize.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\sysdep.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\utils.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\uuid.c
# End Source File
# Begin Source File

SOURCE=..\source\utils\UUIDUtil.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\XDRMarshaller.c
# End Source File
# Begin Source File

SOURCE=..\source\xdr\XDRUnMarshaller.c
# End Source File
# Begin Source File

SOURCE=..\source\xml\XMLMarshaller.c
# End Source File
# Begin Source File

SOURCE=..\source\xml\XMLUnMarshaller.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\xdr\AttributeDescriptorHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\curl\curl.h
# End Source File
# Begin Source File

SOURCE=..\include\common\descriptor.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\dynamicArray.h
# End Source File
# Begin Source File

SOURCE=..\include\curl\easy.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\errorCode.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\errorHandler.h
# End Source File
# Begin Source File

SOURCE=..\include\expat\expat.h
# End Source File
# Begin Source File

SOURCE=..\include\common\FNFData.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\FNFDataHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\common\FNFType.h
# End Source File
# Begin Source File

SOURCE=..\include\common\getFile.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\global.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\hashtable.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\IPDRCommon.h
# End Source File
# Begin Source File

SOURCE=..\include\common\IPDRDocReader.h
# End Source File
# Begin Source File

SOURCE=..\include\common\IPDRDocWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\common\IPDRHeader.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\IPDRMemory.h
# End Source File
# Begin Source File

SOURCE=..\include\test\IPDRReadTool.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\IPDRRecord.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\IPDRStreamElementHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\test\IPDRWriteTool.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\IPDRXDRDocEndHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\IPDRXDRHeaderHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\IPDRXDRRecordHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xml\IPDRXMLDocEndHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xml\IPDRXMLHeaderHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xml\IPDRXMLRecordHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\md5.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\NameSpaceInfoHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\RecordDescriptorHelper.h
# End Source File
# Begin Source File

SOURCE=..\include\common\schema.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\schemaUtil.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\serialize.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\sysdep.h
# End Source File
# Begin Source File

SOURCE=..\include\curl\types.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\utils.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\uuid.h
# End Source File
# Begin Source File

SOURCE=..\include\utils\UUIDUtil.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\XDRMarshaller.h
# End Source File
# Begin Source File

SOURCE=..\include\xdr\XDRUnMarshaller.h
# End Source File
# Begin Source File

SOURCE=..\include\xml\XMLMarshaller.h
# End Source File
# Begin Source File

SOURCE=..\include\xml\XMLUnMarshaller.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
