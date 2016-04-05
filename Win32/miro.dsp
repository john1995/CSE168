# Microsoft Developer Studio Project File - Name="miro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=miro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "miro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "miro.mak" CFG="miro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "miro - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "miro - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "miro - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "miro - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "miro - Win32 Release"
# Name "miro - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Camera.cpp
# End Source File
# Begin Source File

SOURCE=..\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\lexer.lex

!IF  "$(CFG)" == "miro - Win32 Release"

# Begin Custom Build
InputPath=..\lexer.lex

"..\lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -o..\lexer.cpp $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "miro - Win32 Debug"

# Begin Custom Build
InputPath=..\lexer.lex

"..\lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -o..\lexer.cpp $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\main.cpp
# End Source File
# Begin Source File

SOURCE=..\opengl.cpp
# End Source File
# Begin Source File

SOURCE=..\parse.y

!IF  "$(CFG)" == "miro - Win32 Release"

# Begin Custom Build
InputPath=..\parse.y
InputName=parse

"..\parse.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -d -o ..\parse.cpp ..\\$(InputName).y

# End Custom Build

!ELSEIF  "$(CFG)" == "miro - Win32 Debug"

# Begin Custom Build
InputPath=..\parse.y
InputName=parse

"..\parse.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -d -o ..\parse.cpp ..\\$(InputName).y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\rasterize.cpp
# End Source File
# Begin Source File

SOURCE=..\raytrace.cpp
# End Source File
# Begin Source File

SOURCE=..\Scene.cpp
# End Source File
# Begin Source File

SOURCE=..\triangles.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Camera.h
# End Source File
# Begin Source File

SOURCE=..\Image.h
# End Source File
# Begin Source File

SOURCE=..\miro.h
# End Source File
# Begin Source File

SOURCE=..\parse.cpp.h
# End Source File
# Begin Source File

SOURCE=..\Vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\console.cpp
# End Source File
# Begin Source File

SOURCE=..\console.h
# End Source File
# Begin Source File

SOURCE=.\getopt.cpp
# End Source File
# Begin Source File

SOURCE=.\getopt.h
# End Source File
# Begin Source File

SOURCE=..\lexer.cpp
# End Source File
# Begin Source File

SOURCE=..\parse.cpp
# End Source File
# End Group
# Begin Group "Scenes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\example.txt
# End Source File
# End Group
# End Target
# End Project
