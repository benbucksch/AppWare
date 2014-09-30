# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Menus - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Menus - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Menus - Win32 Release" && "$(CFG)" != "Menus - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Menus.mak" CFG="Menus - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Menus - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Menus - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Menus - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Menus - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Menus.dll"

CLEAN : 
	-@erase ".\Release\Menus.dll"
	-@erase ".\Release\Menuevt.obj"
	-@erase ".\Release\Itemevt.obj"
	-@erase ".\Release\Menuapi.obj"
	-@erase ".\Release\Barlib.obj"
	-@erase ".\Release\Itemadmn.obj"
	-@erase ".\Release\Testfunc.obj"
	-@erase ".\Release\Menudlg.obj"
	-@erase ".\Release\Bardlg.obj"
	-@erase ".\Release\Barevt.obj"
	-@erase ".\Release\Menuadmn.obj"
	-@erase ".\Release\Baradmn.obj"
	-@erase ".\Release\Main.obj"
	-@erase ".\Release\Menufunc.obj"
	-@erase ".\Release\Menu_vi.obj"
	-@erase ".\Release\Barfunc.obj"
	-@erase ".\Release\Menulib.obj"
	-@erase ".\Release\Itemlib.obj"
	-@erase ".\Release\Itemfunc.obj"
	-@erase ".\Release\Itemdlg.obj"
	-@erase ".\Release\Resource.res"
	-@erase ".\Release\Menus.lib"
	-@erase ".\Release\Menus.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Menus.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Menus.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib aw32wnd.lib aw32wnd.exp /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib aw32wnd.lib aw32wnd.exp /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/Menus.pdb" /machine:I386 /def:".\Alm_menu.def"\
 /out:"$(OUTDIR)/Menus.dll" /implib:"$(OUTDIR)/Menus.lib" 
DEF_FILE= \
	".\Alm_menu.def"
LINK32_OBJS= \
	"$(INTDIR)/Menuevt.obj" \
	"$(INTDIR)/Itemevt.obj" \
	"$(INTDIR)/Menuapi.obj" \
	"$(INTDIR)/Barlib.obj" \
	"$(INTDIR)/Itemadmn.obj" \
	"$(INTDIR)/Testfunc.obj" \
	"$(INTDIR)/Menudlg.obj" \
	"$(INTDIR)/Bardlg.obj" \
	"$(INTDIR)/Barevt.obj" \
	"$(INTDIR)/Menuadmn.obj" \
	"$(INTDIR)/Baradmn.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Menufunc.obj" \
	"$(INTDIR)/Menu_vi.obj" \
	"$(INTDIR)/Barfunc.obj" \
	"$(INTDIR)/Menulib.obj" \
	"$(INTDIR)/Itemlib.obj" \
	"$(INTDIR)/Itemfunc.obj" \
	"$(INTDIR)/Itemdlg.obj" \
	"$(INTDIR)/Resource.res" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32time.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32text.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32run.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32menu.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32datb.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Tips32.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32flag.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32butn.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32tabl.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32wnd.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32date.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32nmbr.lib"

"$(OUTDIR)\Menus.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

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
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Menus.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Menus.dll"
	-@erase ".\Debug\Menudlg.obj"
	-@erase ".\Debug\Itemlib.obj"
	-@erase ".\Debug\Itemevt.obj"
	-@erase ".\Debug\Menuadmn.obj"
	-@erase ".\Debug\Menuapi.obj"
	-@erase ".\Debug\Baradmn.obj"
	-@erase ".\Debug\Barlib.obj"
	-@erase ".\Debug\Itemadmn.obj"
	-@erase ".\Debug\Testfunc.obj"
	-@erase ".\Debug\Menu_vi.obj"
	-@erase ".\Debug\Barfunc.obj"
	-@erase ".\Debug\Itemfunc.obj"
	-@erase ".\Debug\Menulib.obj"
	-@erase ".\Debug\Barevt.obj"
	-@erase ".\Debug\Itemdlg.obj"
	-@erase ".\Debug\Menuevt.obj"
	-@erase ".\Debug\Menufunc.obj"
	-@erase ".\Debug\Bardlg.obj"
	-@erase ".\Debug\Main.obj"
	-@erase ".\Debug\Resource.res"
	-@erase ".\Debug\Menus.ilk"
	-@erase ".\Debug\Menus.lib"
	-@erase ".\Debug\Menus.exp"
	-@erase ".\Debug\Menus.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Menus.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Menus.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib aw32wnd.lib aw32wnd.exp /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib aw32wnd.lib aw32wnd.exp /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/Menus.pdb" /debug /machine:I386\
 /def:".\Alm_menu.def" /out:"$(OUTDIR)/Menus.dll" /implib:"$(OUTDIR)/Menus.lib" 
DEF_FILE= \
	".\Alm_menu.def"
LINK32_OBJS= \
	"$(INTDIR)/Menudlg.obj" \
	"$(INTDIR)/Itemlib.obj" \
	"$(INTDIR)/Itemevt.obj" \
	"$(INTDIR)/Menuadmn.obj" \
	"$(INTDIR)/Menuapi.obj" \
	"$(INTDIR)/Baradmn.obj" \
	"$(INTDIR)/Barlib.obj" \
	"$(INTDIR)/Itemadmn.obj" \
	"$(INTDIR)/Testfunc.obj" \
	"$(INTDIR)/Menu_vi.obj" \
	"$(INTDIR)/Barfunc.obj" \
	"$(INTDIR)/Itemfunc.obj" \
	"$(INTDIR)/Menulib.obj" \
	"$(INTDIR)/Barevt.obj" \
	"$(INTDIR)/Itemdlg.obj" \
	"$(INTDIR)/Menuevt.obj" \
	"$(INTDIR)/Menufunc.obj" \
	"$(INTDIR)/Bardlg.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Resource.res" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32time.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32text.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32run.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32menu.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32datb.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Tips32.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32flag.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32butn.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32tabl.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32wnd.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32date.lib" \
	"..\..\..\..\Appware.w32\Latest\Release\Lib\Aw32nmbr.lib"

"$(OUTDIR)\Menus.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Menus - Win32 Release"
# Name "Menus - Win32 Debug"

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Testfunc.cpp
DEP_CPP_TESTF=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Modinfo.h"\
	

"$(INTDIR)\Testfunc.obj" : $(SOURCE) $(DEP_CPP_TESTF) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Bardlg.cpp
DEP_CPP_BARDL=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\A_almutl.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Barobj.h"\
	".\Barlib.h"\
	".\Hmenumgr.h"\
	".\Menulib.h"\
	".\Modinfo.h"\
	{$(INCLUDE)}"\Helpids.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	".\Menuobj.h"\
	

"$(INTDIR)\Bardlg.obj" : $(SOURCE) $(DEP_CPP_BARDL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Bardlg.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Barevt.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_BAREV=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	".\Barlib.h"\
	".\Barobj.h"\
	

"$(INTDIR)\Barevt.obj" : $(SOURCE) $(DEP_CPP_BAREV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_BAREV=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	".\Barlib.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	".\Barobj.h"\
	

"$(INTDIR)\Barevt.obj" : $(SOURCE) $(DEP_CPP_BAREV) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Barfunc.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_BARFU=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Barobj.h"\
	".\Hmenumgr.h"\
	

"$(INTDIR)\Barfunc.obj" : $(SOURCE) $(DEP_CPP_BARFU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_BARFU=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Barobj.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	

"$(INTDIR)\Barfunc.obj" : $(SOURCE) $(DEP_CPP_BARFU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Barlib.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_BARLI=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Barobj.h"\
	".\Itemobj.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	

"$(INTDIR)\Barlib.obj" : $(SOURCE) $(DEP_CPP_BARLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_BARLI=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Barobj.h"\
	".\Itemobj.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	

"$(INTDIR)\Barlib.obj" : $(SOURCE) $(DEP_CPP_BARLI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Barlib.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Barobj.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Hmenumgr.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemadmn.cpp
DEP_CPP_ITEMA=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Modinfo.h"\
	".\Itemlib.h"\
	".\Itemobj.h"\
	".\Itemdlg.h"\
	".\Hmenumgr.h"\
	
NODEP_CPP_ITEMA=\
	".\mem.h"\
	

"$(INTDIR)\Itemadmn.obj" : $(SOURCE) $(DEP_CPP_ITEMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemdlg.cpp
DEP_CPP_ITEMD=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\A_almutl.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\Helpids.h"\
	".\Modinfo.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	{$(INCLUDE)}"\O_menus.h"\
	
NODEP_CPP_ITEMD=\
	".\mem.h"\
	

"$(INTDIR)\Itemdlg.obj" : $(SOURCE) $(DEP_CPP_ITEMD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemdlg.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemevt.cpp
DEP_CPP_ITEME=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Itemobj.h"\
	

"$(INTDIR)\Itemevt.obj" : $(SOURCE) $(DEP_CPP_ITEME) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemfunc.cpp
DEP_CPP_ITEMF=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Itemobj.h"\
	

"$(INTDIR)\Itemfunc.obj" : $(SOURCE) $(DEP_CPP_ITEMF) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemlib.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_ITEML=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	".\Menudlg.h"\
	".\Menuobj.h"\
	

"$(INTDIR)\Itemlib.obj" : $(SOURCE) $(DEP_CPP_ITEML) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_ITEML=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	".\Menudlg.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	".\Menuobj.h"\
	

"$(INTDIR)\Itemlib.obj" : $(SOURCE) $(DEP_CPP_ITEML) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemlib.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Itemobj.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Main.cpp
DEP_CPP_MAIN_=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	".\Modinfo.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menu_vi.cpp
DEP_CPP_MENU_=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	".\Menu_vi.h"\
	".\Menulib.h"\
	".\Modinfo.h"\
	".\Itemlib.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	".\Menuobj.h"\
	{$(INCLUDE)}"\O_menus.h"\
	

"$(INTDIR)\Menu_vi.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menu_vi.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menuadmn.cpp
DEP_CPP_MENUA=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Menudlg.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Menu_vi.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	
NODEP_CPP_MENUA=\
	".\mem.h"\
	

"$(INTDIR)\Menuadmn.obj" : $(SOURCE) $(DEP_CPP_MENUA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menuapi.cpp
DEP_CPP_MENUAP=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Menuobj.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Barlib.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	".\Barobj.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	

"$(INTDIR)\Menuapi.obj" : $(SOURCE) $(DEP_CPP_MENUAP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menubar.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menudlg.cpp
DEP_CPP_MENUD=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\A_almutl.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_text.h"\
	{$(INCLUDE)}"\O_number.h"\
	".\Modinfo.h"\
	".\Menudlg.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Itemlib.h"\
	{$(INCLUDE)}"\Helpids.h"\
	

"$(INTDIR)\Menudlg.obj" : $(SOURCE) $(DEP_CPP_MENUD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menudlg.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menuevt.cpp
DEP_CPP_MENUE=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Menuobj.h"\
	".\Hmenumgr.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	

"$(INTDIR)\Menuevt.obj" : $(SOURCE) $(DEP_CPP_MENUE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menufunc.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_MENUF=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_number.h"\
	{$(INCLUDE)}"\O_text.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_wndlay.h"\
	".\Menuobj.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	

"$(INTDIR)\Menufunc.obj" : $(SOURCE) $(DEP_CPP_MENUF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_MENUF=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_number.h"\
	{$(INCLUDE)}"\O_text.h"\
	{$(INCLUDE)}"\O_menus.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_wndlay.h"\
	".\Menuobj.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	".\Menulib.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	

"$(INTDIR)\Menufunc.obj" : $(SOURCE) $(DEP_CPP_MENUF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menulib.cpp

!IF  "$(CFG)" == "Menus - Win32 Release"

DEP_CPP_MENUL=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_number.h"\
	{$(INCLUDE)}"\O_text.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	

"$(INTDIR)\Menulib.obj" : $(SOURCE) $(DEP_CPP_MENUL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

DEP_CPP_MENUL=\
	{$(INCLUDE)}"\A_alm.h"\
	".\Menuobj.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Itemobj.h"\
	".\Itemlib.h"\
	{$(INCLUDE)}"\O_wnd.h"\
	{$(INCLUDE)}"\O_number.h"\
	{$(INCLUDE)}"\O_text.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\Op_wnd.h"\
	

"$(INTDIR)\Menulib.obj" : $(SOURCE) $(DEP_CPP_MENUL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menulib.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menuobj.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Modinfo.h

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Resource.rc
DEP_RSC_RESOU=\
	".\Menubar.ico"\
	".\Menu.ico"\
	".\Menuitem.ico"\
	".\Dnarrow.bmp"\
	".\Gdnarrow.bmp"\
	

"$(INTDIR)\Resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Baradmn.cpp
DEP_CPP_BARAD=\
	{$(INCLUDE)}"\A_alm.h"\
	{$(INCLUDE)}"\Intdebug.h"\
	{$(INCLUDE)}"\O_menus.h"\
	".\Modinfo.h"\
	".\Barobj.h"\
	".\Itemobj.h"\
	".\Bardlg.h"\
	".\Barlib.h"\
	".\Menulib.h"\
	".\Hmenumgr.h"\
	".\Menuobj.h"\
	
NODEP_CPP_BARAD=\
	".\mem.h"\
	

"$(INTDIR)\Baradmn.obj" : $(SOURCE) $(DEP_CPP_BARAD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Tips32.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32datb.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32date.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32flag.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32menu.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32nmbr.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32run.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32tabl.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32text.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32time.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32wnd.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32butn.lib

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dnarrow.bmp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gdnarrow.bmp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menu.ico

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menubar.ico

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menuitem.ico

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Menus.mak

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Alm_menu.def

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\TIPS32.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32DATB.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32DATE.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32FLAG.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32MENU.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32NMBR.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32run.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32TABL.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32TEXT.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\Aw32time.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32WND.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Appware.w32\Latest\Release\Lib\AW32BUTN.exp

!IF  "$(CFG)" == "Menus - Win32 Release"

!ELSEIF  "$(CFG)" == "Menus - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
