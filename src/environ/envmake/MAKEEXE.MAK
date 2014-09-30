################################################################
#
#         Copyright 1993, Novell, Inc.  All rights reserved
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
# FILE:           MAKEOBJ.MAK
#
# AUTHOR:         Scott McCarty
#
# DESCRIPTION:    Master makefile for making objects
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
/pdb:NONE

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


!if "$(DEMO)" == "1"
RESFLAGS = -dDEMO
!else
RESFLAGS = 
!endif

!IF "$(OFFICIAL)" != ""
RESFLAGS =$(RESFLAGS) -dBUILD_OFFICIAL
!ENDIF


################################################################
#
# Set default compile mode:  default mode is DEBUG
#   INTERNAL => full debug information
#   DEBUG    => ADBG_DEBUG only; full optimizations
#   SHIP     => no debug; full optimizations

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

!include "make\exeinfo.mak"
!include "make\deps"

!IF !DEFINED(EXENAME) || !DEFINED(DEFNAME) || !DEFINED(OBJECTFILES) || !DEFINED(OBJECTRCFILE)
!error Not all required information supplied in MAKE\OBJINFO.MAK
!ENDIF

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
!IF "$(LOCALINCLUDE)" == ""
	$(RC) -r $(DEBUGFLAG) $(RESFLAGS) -fo$@ $<
!ELSE
	$(RC) -r  -I$(LOCALINCLUDE) $(DEBUGFLAG) $(RESFLAGS) -fo$@ $<
!ENDIF



################################################################
#
# Master target
#

.PRECIOUS: $(OBJDIR)\$(EXENAME)

project: objdir $(OBJDIR)\$(EXENAME)

# Create the destination directory if necessary
objdir:
	@IF NOT EXIST $(OBJDIR)\NUL MKDIR $(OBJDIR)


# Compile and link everything
$(OBJDIR)\$(EXENAME) : $(OBJECTFILES) $(OBJECTRCFILE) $(DEFNAME) 
	$(LINK) @<<
	$(LFLAGS) 
	/out:"$(OBJDIR)\$(EXENAME) 
	$(OBJECTFILES) 
	$(STANDARDLIBS) 
	$(LOCALLIBS)
	$(OBJECTRCFILE) 
	/DEF:$(DEFNAME)
<<
