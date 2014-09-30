///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Calculation
//
// Filename:  "calc_md.cpp"
//
// Author:  Kevin Smart
//   Start:   July 22, 1992
//   Finish:  July xx, 1992
//
// Description:
//   Event handlers for Calculation object
//   .. Calc_Startup
//   .. Calc_ObjectChanged
//
////////////////////////////////////////////////////////////////////////////

#include  <a_alm.h>

#include  "strclass.h"
#include  "calc_obj.h"
#include  "calc.h"


 

//----------------------------------------------------------------------
//   Calc_Startup
//
// Register each auto-update calculation object as the owner of each
// variable in in it's equation.
//----------------------------------------------------------------------
extern "C"  void far pascal _export Calc_Startup(
  pAEvtInfo  /*theEvent*/,
  pAObjMessage       /*theSystem*/)
  {
  OBJECTID  calcID;
  LONG      calcKey = AOBJ_GETFIRSTKEY;

  while (0 != (calcID = AObjGetNextObject(OTYPE_EXPRESSION, &calcKey)))
    {
    CALCULATION*	calc = (CALCULATION*)AObjGetRuntimeInfo(calcID);
///    CALCULATION  calc(calcID);
    OBJECTID     ownedID;
    LONG         varKey = AOBJ_GETFIRSTKEY;

    while(calc->GetNextVariable(&ownedID, &varKey))
      {
      // Let the OIP know we own this var... no user-data
      AObjRegisterOwner(calcID, ownedID, 0L);
      }
    }
  }
  

//----------------------------------------------------------------------
//   Calc_ObjectChanged
//
// See if the object that has changed is owned by any calculation
// object(s).  If so, see if it is part of an auto-recalc expression
// and recalc if necessary.
//----------------------------------------------------------------------
extern "C"	void far pascal _export Calc_ObjectChanged(
	  pAEvtInfo  theEvent,
	  pAObjMessage       theSystem)
	  {
	  OBJECTID       targetID;
	  OBJECTID       calcID;
	  AObjOwnerInfo  ownerInfo;

	  targetID = theEvent->targetObject;

	  // Get each auto-update calculator that owns this object
	  ownerInfo.key = AOBJ_GETFIRSTKEY;
	  while (AObjGetNextOwner(OTYPE_EXPRESSION, targetID, &ownerInfo))
	    {
	    calcID = ownerInfo.ownerID;
      CALCULATION*	calc = (CALCULATION*)AObjGetRuntimeInfo(calcID);
///	    CALCULATION  calc(calcID, FALSE);

	    // always post the 'Variable Changed' signal
			AEvtPostSignalAtMark(calcID, calcVariableChanged);

			// evaluate if auto-update calculator
			if (calc->AutoUpdate())
				EvalExpression(calcID, theSystem->errorMessage);
			}

	  return;
	  }
