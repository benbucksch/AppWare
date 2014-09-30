################################################################
#
#                                       Copyright 1993, Novell, Inc.    All rights reserved
#
# THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
# PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
# ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
# A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
# THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
# ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
# WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
# REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
# COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
# WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
# EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
# THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
#
#--------------------------------------------------------------
#
# FILE:                                         MAKEOBJ.MAK
#
# AUTHOR:                                       Scott McCarty
#
# DESCRIPTION:          Master makefile for making objects
#
# CHANGES:
#
################################################################

#.swap
#.noautodepend

all: project

################################################################
#
# External commands
#

CC     = cl.exe
RC     = rc.exe
LINK   = link.exe
IMPLIB = lib.exe

################################################################

#
# Compiler flags macros
#

#	W3		Show warnings at level 3
#	/c		compile only
CCFLAGS_G = /G3 /W3 /D "WIN32" /D "_WINDOWS" /Fo$(OBJDIR)/ /c /nologo /Zp1

#Debug Flags
CCFLAGS_D = /MLd /GX /Z7 /Od /D "ADBG_DEBUG" /D "INTERNAL" /D \
"AW_I_DEBUG" /D "_DEBUG" /Ob0 

#Release Flags
CCFLAGS_R = /ML /GX /O2 /D "NDEBUG" 

################################################################
#
# Linker flags
#

LFLAGS_G = /subsystem:windows /machine:I386 \
 /pdb:NONE /DLL

LFLAGS_D = /debug /incremental:no /DEBUGTYPE:CV

LFLAGS_R = /incremental:no

################################################################
#
# RC flags
#

RCFLAGS_G = /l "0x409" 

RCFLAGS_R = /d "NDEBUG"

RCFLAGS_D = /d "_DEBUG" /d "INTERNAL" /d\
 "ADBG_DEBUG" /d "AW_I_DEBUG" 

RCFLAGS = $(RCFLAGS_G)


!IF "$(OFFICIAL)" != ""
RESFLAGS = -dBUILD_OFFICIAL
!ENDIF

################################################################
#
# Set default compile mode:  default mode is DEBUG
#   INTERNAL => full debug information
#   DEBUG    => ADBG_DEBUG only; full optimizations
#   SHIP     => no debug; full optimizations

# Set up what to build
!IF "$(MAKEDLL)" == "" && "$(MAKELIB)" == ""

#default to making both
MAKEDLL = 1
MAKELIB = 1
!ENDIF

!IF "$(INTERNAL)" == "" && "$(DEBUG)" == "" && "$(SHIP)" == ""
INTERNAL=1
!ENDIF


!IF "$(INTERNAL)" != ""
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_D) -DADBG_DEBUG -DAW_I_DEBUG
LFLAGS    = $(LFLAGS_G)  $(LFLAGS_D)
OBJDIR    = obj
DEBUGFLAG = -DINTERNAL
RCFLAGS = $(RCFLAGS) $(RCFLAGS_D)

!ELSEIF "$(DEBUG)" != ""
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_R) -DADBG_DEBUG
LFLAGS    = $(LFLAGS_G) $(LFLAGS_R)
OBJDIR    = dobj
DEBUGFLAG = -DDEBUGRELEASE
RCFLAGS = $(RCFLAGS) $(RCFLAGS_D)

!ELSE
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_R)
LFLAGS    = $(LFLAGS_G)  $(LFLAGS_R)
OBJDIR    = sobj
DEBUGFLAG = -DSHIPPING
RCFLAGS = $(RCFLAGS) $(RCFLAGS_R)

!ENDIF

!IF "$(OFFICIAL)" != ""
CCFLAGS = $(CCFLAGS) -DBUILD_OFFICIAL
!ENDIF

!IF "$(TIME)" != ""
CCFLAGS = $(CCFLAGS) -DAW_I_TIME
!ENDIF

################################################################
#
# Set up paths for external files
#
!IF !DEFINED(INCLUDE) || !DEFINED(LIB)
!ERROR Verify that INCLUDE and LIB environment variables are non-null
!ENDIF

#.PATH.OBJ = $(OBJDIR)
#.PATH.RES = $(OBJDIR)

STANDARDLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib 

################################################################
#
# Include object-specific information.
#

!include "make\dllinfo.mak"
!include "make\deps"

!IF !DEFINED(DLLNAME) || !DEFINED(DEFNAME) || !DEFINED(OBJECTFILES) || !DEFINED(OBJECTRCFILE)
!error Not all required information supplied in MAKE\OBJINFO.MAK
!ENDIF

#.PATH.CPP = $(CPPDIR)
#.PATH.RC        = $(RCDIR)

################################################################
#
# Implicit rules
#

{..\ALMBLDR\SRC}.CPP{$(OBJDIR)}.OBJ:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

{..\MEMIO\SRC}.CPP{$(OBJDIR)}.OBJ:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

{..\KERNEL\SRC}.CPP{$(OBJDIR)}.OBJ:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

{..\APEBLDR\SRC}.CPP{$(OBJDIR)}.OBJ:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

.cpp{$(OBJDIR)}.obj:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

.c{$(OBJDIR)}.obj:
	$(CC) \
-I$(INCLUDE) \
-I$(LOCALINCLUDE) \
$(CCFLAGS) $<

{.\$(RCDIR)}.rc{$(OBJDIR)}.res:
	$(RC) -r $(DEBUGFLAG) $(RESFLAGS) -fo$@ $<


################################################################
#
# Master target
#

.PRECIOUS: $(OBJDIR)\$(DLLNAME)

!if "$(DLLAPI)" == ""

!ifdef "$(MAKEDLL)" != ""
#only make the .DLL
project: objdir $(OBJDIR)\$(DLLNAME)
!endif

!ELSEIF "$(MAKEDLL)" != "" && "$(MAKELIB)" != ""
#make both the .DLL and .LIB
project: objdir $(OBJDIR)\$(DLLNAME) $(OBJDIR)\$(DLLAPI)
!ELSEIF "$(MAKELIB)" != ""
#only make the .LIB
project: objdir $(OBJDIR)\$(DLLAPI)
!ELSEIF "$(MAKEDLL)" != ""
#make .dll
project: objdir $(OBJDIR)\$(DLLNAME)
!endif




# Create the destination directory if necessary
objdir:
	@IF NOT EXIST $(OBJDIR)\NUL MKDIR $(OBJDIR)

!if "$(MAKELIB)" != ""
!if "$(DLLAPI)" != ""
$(OBJDIR)\$(DLLAPI) : $(OBJECTDEFNAME) $(OBJECTFILES)
	$(IMPLIB) /DEF:$(DEFNAME) /MACHINE:IX86 /OUT:$(OBJDIR)\$(DLLAPI) $(OBJECTFILES) 

!endif
!endif

# Compile and link everything
$(OBJDIR)\$(DLLNAME) : $(OBJECTFILES) $(OBJECTRCFILE) $(DEFNAME) 
	$(LINK) @<<
	$(LFLAGS) 
	/out:"$(OBJDIR)\$(DLLNAME) 
	$(OBJECTFILES) 
	$(STANDARDLIBS) 
	$(LOCALLIBS)
	$(OBJECTRCFILE) 
	/DEF:$(DEFNAME)
	/IMPLIB:$(OBJDIR)\$(DLLAPI)
<<

#!if "$(OBJECTRCFILE)" != ""
#	$(RC) $(RCFLAGS) "$(OBJECTRCFILE)" "$@"
#!else
#	$(RC) "$(RCFLAGS)" "$@"
#!endif


