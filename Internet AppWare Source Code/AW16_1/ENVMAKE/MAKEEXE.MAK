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

.swap
.noautodepend

all: project

################################################################
#
# External commands
#

CC   = bcc
RC   = rc
LINK = optlinks

################################################################
#
# Compiler flags macros
#

# Compile flags that are common to all compile modes
# -2   = 286 code
# -c   = compile only
# -ml  = large, DS == SS
# -Vf  = far virtual tables
# -V   = one instance of virtual table or non-expanded inline function
# -w   = all warnings on
# -WS  = Windows EXE, smart callbacks
CCFLAGS_G = -2 -c -Vf -V -w -WS


# -k   = standard stack frame
# -N   = check stack overflow
# -Od  = disable all optimizations
# -v   = debug
# -vi- = don't expand inline
CCFLAGS_D = -k -N -Od -v -vi-


# -k-   = no standard stack frame
# -O... = optimizations
# -v-   = no debug
# -vi   = expand inline
# -X    = suppress auto-dependency output
# -Z    = redundant register load optimization
# Cannot inline intrinsic functions without causing size problems;
# Cannot use -Om to move invariant code out of loops without causing
#  problems.
#CCFLAGS_R = -k- -O -Obegilmpvs -v- -vi  -X -Z
CCFLAGS_R  = -k- -O -Obeglpvs   -v- -vi- -X -Z

################################################################
#
# Linker flags
#

# -c   = case-sensitive link
# -C   = EXPORTS & IMPORTS are case-sensitive
# -P-  = don't pack code segments
# -x   = no map file
# -Twe = Windows' EXE
LFLAGS_G = -c -C -P- -x -Twe
LFLAGS_O = /ONERROR:NOEXE /TLINK 

# -v   = debug info
LFLAGS_D = -v

LFLAGS_R =

################################################################
#
# RC flags
#

RCFLAGS_G = -31 -t

RCFLAGS = $(RCFLAGS_G)

################################################################
#
# Set default compile mode:  default mode is DEBUG
#   INTERNAL => full debug information
#   DEBUG    => ADBG_DEBUG only; full optimizations
#   SHIP     => no debug; full optimizations

!if !$d(INTERNAL) && !$d(DEBUG) && !$d(SHIP)
INTERNAL=1
!endif


!ifdef INTERNAL
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_D) -DADBG_DEBUG -DAW_I_DEBUG
LFLAGS    = $(LFLAGS_G)  $(LFLAGS_D)
OBJDIR    = obj
DEBUGFLAG = -DINTERNAL

!elif  $d(DEBUG)
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_R) -DADBG_DEBUG
LFLAGS    = $(LFLAGS_G)  $(LFLAGS_R)
OBJDIR    = dobj
DEBUGFLAG = -DDEBUGRELEASE

!else
CCFLAGS   = $(CCFLAGS_G) $(CCFLAGS_R)
LFLAGS    = $(LFLAGS_G)  $(LFLAGS_R)
OBJDIR    = sobj
DEBUGFLAG = -DSHIPPING

!endif

################################################################
#
# Set up paths for external files
#

!if !$d(INCLUDE) || !$d(LIB)
!error Verify that INCLUDE and LIB environment variables are non-null
!endif

.PATH.OBJ = $(OBJDIR)
.PATH.RES = $(OBJDIR)

STARTUPOBJ   = c0wl.obj
STANDARDLIBS = mathwl.lib import.lib cwl.lib

################################################################
#
# Include object-specific information.
#

!include "make\exeinfo.mak"
!include "make\deps"

!if !$d(EXENAME) || !$d(DEFNAME) || !$d(OBJECTFILES) || !$d(OBJECTRCFILE)
!error Not all required information supplied in MAKE\OBJINFO.MAK
!endif

.PATH.CPP = $(CPPDIR)
.PATH.RC  = $(RCDIR)

################################################################
#
# Implicit rules
#

{..\CFGUPDAT\SRC}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

{..\DEV\SRC}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

{..\MEMIO\SRC}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

{..\OIP\SRC}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

{..\PRO\SRC}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

{..\UPSF}.CPP{$(OBJDIR)}.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

.CPP.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
-I$(INCLUDE)
-I$(LOCALINCLUDE)
$(CCFLAGS) $(LOCALCCFLAGS)
! $<

.C.OBJ:
  $(CC) @&&!
-n$(OBJDIR)
$(CCFLAGS) $(LOCALCCFLAGS)
-I$(LOCALINCLUDE)
! $<

.RC.RES:
  $(RC) -r $(DEBUGFLAG) -fo$@ $<


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
  $(LINK) $(LFLAGS_O) $(LFLAGS) -L$(LIB) @&&!
$(STARTUPOBJ) $(OBJECTFILES)
$(OBJDIR)\$(EXENAME)
NUL
$(STANDARDLIBS) $(LOCALLIBS)
$(DEFNAME)
!
!if "$(OBJECTRCFILE)" != ""
  $(RC) $(RCFLAGS) $(OBJECTRCFILE) $@
!else
  $(RC) $(RCFLAGS) $@
!endif
