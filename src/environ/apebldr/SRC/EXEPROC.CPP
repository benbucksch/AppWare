//======================================================================
// exeproc.cpp
//
// An EXEPROC is a descriptor for runtime proc entry points in a DLL.
// Each EXELIB on the EXELIBLIST points to a EXEPROCLIST containing
// these EXEPROC's.
//
// See also EXEPROCLIST and EXELIB.
//
// jon  6-Jul-92 created
//======================================================================

  #include "almsys.h"

	#include "sereclib.h"
  #include "exehead.h"
  #include "exeproc.h"
  #include "exebeadl.h"

//----------------------------------------------------------------------
//   getTheBeadList (local)
//
// Fetches the singular global exe list of binding beads.
//----------------------------------------------------------------------
inline static LPEXEBEADLIST getTheBeadList()
  {
  return ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
          -> GetExeBeadList();
  }

//----------------------------------------------------------------------
//   EXEPROC::DoBindBead (private)
//
// Creates and binds a EXEBEAD connector with this proc descriptor's
// strand of beads.  The parameter, theUID, is either an EXEFUNCINST,
// an EXEOBJINST or NULLSERUID.  Returns TRUE if the bead was bound.
//----------------------------------------------------------------------
BOOL EXEPROC::DoBindBead(
  SERUID   theUID,
  ATYPEID   theType,
  AEVENTID  theEvent)
  {
  LPEXEBEADLIST  beadList = getTheBeadList();
  LPEXEBEAD      newBead = (LPEXEBEAD) beadList->GetNewNodeAddr();

  if (!newBead)
    return FALSE;

  newBead->SetUID(theUID);
  newBead->SetTypeID(theType);
  newBead->SetEventID(theEvent);
  newBead->SetNextIdx(this->GetFirstBeadIdx());

 // It went into the last slot of the bead list, and that is
 // now the first bead on this proc's strand
  this->SetFirstBeadIdx(beadList->GetNumNodes() - 1);
  return TRUE;
  }

//----------------------------------------------------------------------
//   EXEPROC::GetNextBead
//
// Get the next binding bead from this proc's bead strand, based on the
// key (*theKey) that is passed in.  Specify a key of -1L to start the
// enumeration.
//----------------------------------------------------------------------
BOOL EXEPROC::GetNextBead(
  LPEXEBEAD  * theNextBead,
  LPLONG          theKey)
  {
  LPEXEBEADLIST  beadList = getTheBeadList();

  // be pessimistic
  *theNextBead = 0;

  if (*theKey < 0)
    {
    // Signal to start a new enumeration
    *theKey = this->GetFirstBeadIdx();
    }
  else if (*theKey >= beadList->GetNumNodes())
    {
    // We're done with this strand
    return FALSE;
    }
  else
    {
    // Get the next bead's index
    *theKey = beadList->IdxGetNode(*theKey)->GetNextIdx();
    }

  if (*theKey < 0 || *theKey >= beadList->GetNumNodes())
    {
    // out of bounds ... we're done
    *theKey = beadList->GetNumNodes();
    return FALSE;
    }

  // Being here means we found the next bead on the strand...
  *theNextBead = beadList->IdxGetNode(*theKey);
  return TRUE;
  }

//======================================================================
// end -- exeproc.cpp
//======================================================================
