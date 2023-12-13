# Microsoft Developer Studio Project File - Name="roqmux" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=roqmux - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "roqmux.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "roqmux.mak" CFG="roqmux - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "roqmux - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "roqmux - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "roqmux - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "LibSNDFile/Win32/" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D loff_t=long /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386 /out:"../../roqmux.exe"

!ELSEIF  "$(CFG)" == "roqmux - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "LibSNDFile/Win32/" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D loff_t=long /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /out:"../roqmux.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "roqmux - Win32 Release"
# Name "roqmux - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "LibSNDFile"

# PROP Default_Filter ""
# Begin Group "G72x"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g721.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g723_16.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g723_24.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g723_40.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g72x.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g72x.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\G72x\g72x_priv.h
# End Source File
# End Group
# Begin Group "GSM610"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\add.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\code.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\config.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\decode.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm610_priv.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm_create.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm_decode.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm_destroy.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm_encode.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\gsm_option.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\long_term.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\lpc.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\preprocess.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\rpe.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\short_term.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\GSM610\table.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\LibSNDFile\aiff.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\alaw.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\au.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\au.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\au_g72x.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\command.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\common.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\common.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\dither.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\double64.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\dwd.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\dwvw.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\file_io.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\float32.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\float_cast.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\gsm610.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\htk.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\ima_adpcm.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\interleave.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\ircam.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\mat4.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\mat5.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\ms_adpcm.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\nist.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\ogg.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\paf.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\pcm.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\pvf.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\raw.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\rx2.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sd2.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sds.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sfendian.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sfendian.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sndfile.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\strings.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\svx.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\txw.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\ulaw.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\voc.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\vox_adpcm.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\w64.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\wav.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\wav_w64.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\wav_w64.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\wve.c
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\xi.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\adpcm.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\roqmux.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LibSNDFile\Win32\config.h
# End Source File
# Begin Source File

SOURCE=.\mux.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\sndfile.h
# End Source File
# Begin Source File

SOURCE=.\LibSNDFile\Win32\unistd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
