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
# FILE:					 MAKEOBJ.MAK
#
# AUTHOR:				 Scott McCarty with additions by Troy Wright
#
# DESCRIPTION:	 Master makefile for making objects
#
# CHANGES:
#
################################################################

#.swap
#.noautodepend

all : project

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
CCFLAGS_D = /MLd /GX /Od /D "ADBG_DEBUG" /D "INTERNAL" /D \
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

RCFLAGS_G = /l 0x409

RCFLAGS_R = /d "NDEBUG"

RCFLAGS_D = /d "_DEBUG" /d "INTERNAL" /d\
 "ADBG_DEBUG" /d "AW_I_DEBUG" 

RCFLAGS = $(RCFLAGS_G)

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

################################################################
#
# Include object-specific information.
#

!include "make\objinfo.mak"
!include "make\deps"

!IF !DEFINED(OBJECTDLLNAME) || !DEFINED(OBJECTDEFNAME) || !DEFINED(OBJECTFILES) || !DEFINED(OBJECTRCFILE)
!error Not all required information supplied in MAKE\OBJINFO.MAK
!ENDIF

################################################################
#
# Set up paths for external files
#

!IF !DEFINED(INCLUDE) || !DEFINED(LIB)
!ERROR Verify that INCLUDE and LIB environment variables are non-null
!ENDIF

!IF "$(RCDIR)" == ""
RCDIR     = .
!ENDIF

#.PATH.OBJ = $(OBJDIR)
#.PATH.RES = $(OBJDIR)
#.PATH.CPP = .
#.PATH.C   = .
#.PATH.RC  = $(RCDIR)

STANDARDLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib aw32run.lib

!IF "$(LOCALINCLUDE)" != ""
NEW_INCLUDE = $(INCLUDE) -I$(LOCALINCLUDE)
!ELSE
NEW_INCLUDE = $(INCLUDE)
!ENDIF

################################################################
#
# Implicit rules
#

.cpp{$(OBJDIR)}.obj:
	$(CC) \
  -I$(NEW_INCLUDE) \
	$(CCFLAGS) $(LOCALCCFLAGS) $<  


.c{$(OBJDIR)}.obj:
	$(CC) \ 
  -I$(NEW_INCLUDE) \
	$(CCFLAGS) $(LOCALCCFLAGS) $<  

.rc{$(OBJDIR)}.res:
!IF "$(LOCALINCLUDE)" == ""
	$(RC) /r $(DEBUGFLAG) -fo$@ $<
!else
	$(RC) -I$(LOCALINCLUDE) -r $(DEBUGFLAG) -fo$@ $<
!endif

################################################################
#
# Master target
#

!if "$(OBJECTAPI)" == ""

!if "$(MAKEDLL)" != ""
#only make the .DLL
project: objdir $(OBJDIR)\$(OBJECTDLLNAME)
!endif

!ELSEIF "$(MAKEDLL)" != "" && "$(MAKELIB)" != ""
#make both the .DLL and .LIB
project: objdir $(OBJDIR)\$(OBJECTDLLNAME) $(OBJDIR)\$(OBJECTAPI)
!ELSEIF "$(MAKELIB)" != ""
#only make the .LIB
project: objdir $(OBJDIR)\$(OBJECTAPI)
!ELSEIF "$(MAKEDLL)" != ""
project: objdir $(OBJDIR)\$(OBJECTDLLNAME)
!endif


# Create the destination directory if necessary
objdir:
	@IF NOT EXIST $(OBJDIR)\NUL MKDIR $(OBJDIR)


IMPLIB_FLAGS =

!IF "$(MAKELIB)" != ""
!if "$(OBJECTAPI)" != ""
IMPLIB_FLAGS = /IMPLIB:$(OBJDIR)\$(OBJECTAPI)

$(OBJDIR)\$(OBJECTAPI) : $(OBJECTDEFNAME) $(OBJECTFILES)
	$(IMPLIB) /DEF:$(OBJECTDEFNAME) /MACHINE:IX86 /OUT:$(OBJDIR)\$(OBJECTAPI) $(OBJECTFILES)

!endif
!endif

!IF "$(MAKEDLL)" != ""

# Compile and link everything
$(OBJDIR)\$(OBJECTDLLNAME) : $(OBJECTFILES) $(OBJECTRCFILE) $(OBJECTDEFNAME) 
	$(LINK) @<<
	$(LFLAGS) 
	/out:"$(OBJDIR)\$(OBJECTDLLNAME)
        /DEF:$(OBJECTDEFNAME)
        $(IMPLIB_FLAGS)
	$(OBJECTFILES) 
	$(STANDARDLIBS) 
	$(LOCALLIBS) 
	$(OBJECTRCFILE) 
<< 


!endif 

