////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"evt_term.cpp"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#define max(a,b)    (((a) > (b)) ? (a) : (b))

#include <io.h>
#include "terminal.h"
extern "C"
{
		void ALMAPI Tmnl_StartUpTerminal (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Tmnl_IdleTerminal (pAEvtInfo, pAObjMessage) ;
}
//
//
//
void ALMAPI Tmnl_StartUpTerminal (
	pAEvtInfo /*theEvent*/,
	pAObjMessage /*theSystem*/)
{
	/*AOBJECTID   oiTerm ;
	TERMDATA0* tmnldata ;
	LONG       theGetKey = -1 ;
	AOBJECTID   oiConn ;
  //DATA0*     data0 ;

	I_UNUSED (theEvent) ;
	I_UNUSED (theSystem) ;

	while ((oiTerm = AObjGetNextObject(OTYPE_TERMINAL, &theGetKey))!=NULL)
		{
		tmnldata = (TERMDATA0*) AObjLockData (oiTerm, TMNLDATA) ;
		if (tmnldata)
    	{
			oiConn = tmnldata->oiConn ;
			//data0 = (DATA0*)AObjLockData(oiConn, CONNDATA) ;
			//if (data0&&(!data0->signal.checkBox)&&(!data0->signal.InteText)) //change in Feb 16
      //	{
				AEvtPostStandard (oiTerm, idleTerminal, (long)oiConn, AEVT_ATTAIL) ;
      //	AObjUnlockData (oiConn, CONNDATA) ; 
			//	}
			AObjUnlockData (oiTerm, TMNLDATA) ;
      }
		}*/
}
//
//
//
void ALMAPI Tmnl_IdleTerminal (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	AOBJECTID   oiTerm = (AOBJECTID) theEvent->targetObject ;
	AOBJECTID   oiConn = (AOBJECTID) theEvent->details ;
  TERMINAL   terminal(oiTerm) ;
 
	I_UNUSED (theSystem) ;
	
	if (terminal.GetConnObject()==oiConn)
		AEvtPostStandard (oiTerm, AEVENT_OBJECTCHANGED, 0, AEVT_ATMARK) ;
	
}

