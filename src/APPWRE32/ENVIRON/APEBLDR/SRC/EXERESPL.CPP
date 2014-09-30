//======================================================================
// exerespl.cpp -- runtime response list
//
//  An EXERESPONSELIST is a list of EXERESPONSEs, sorted by typeID.
//  Note that each EXEEVENT on the EXEEVENTLIST points to an
//  EXERESPONSELIST.  Each EXERESPONSE on an EXEEVENT's EXERESPONSELIST
//  contains a typeID (unique for the list) of a serius object type
//  that has registered to listen for the "owning" event, and also,
//  the address of that object type's handler for that event.
//
// jon 12-Jun-92 created
// jon 28-Sep-92 moved dtor out of .h to lose "can't inline for stmts"
//======================================================================

	#include "almsys.h"
	#include "intalm.h"

	#include "sereclib.h"
  #include "exerespl.h"

EXERESPONSELIST::~EXERESPONSELIST()
  {
  LPEXERESPONSE  exeResponse;
  long           i;

  for (i = 0 ; 0 != (exeResponse = (LPEXERESPONSE)this->IdxGetNode(i)) ; i++)
    {
    delete exeResponse;
    }
  }

// -- end exerespl.cpp
