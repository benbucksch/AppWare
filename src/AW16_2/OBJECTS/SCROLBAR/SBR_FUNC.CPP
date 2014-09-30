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
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//	"sbr_func.cpp"
//
// FUNCTIONS for the SCROLL BAR object
//
//	1.	Sbar_Disable
//	2.	Sbar_Enable
//	3.	Sbar_GetIncrement
//	4.	Sbar_GetPosition
//	5.	Sbar_GetRange
//	6.	Sbar_IsEnabled
//	7.	Sbar_SetIncrement
//	8.	Sbar_SetPosition
//	9.	Sbar_SetRange
//
////////////////////////////////////////////////////////////////////////////

#define	ASTRICT
#define STRICT

#include	<windows.h>
#include	<stdlib.h>
#include	<a_alm.h>
#include	<a_almutl.h>
#include	<op_wnd.h>			// window item protocol.
#include	<o_wnd.h>				// window object API.
#include	<o_number.h>
#include	"sbar_hdr.h"
#include	"scroll.h"


// Sbar_Disable ////////////////////////////////////////////////////////////

void far pascal _export Sbar_Disable(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Disable Scroll Bar' function.");
	else {
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);

		if(sbar->Enabled()) {
			sbar->SetEnabled(FALSE);
			sbar->PostObjectChanged(CHANGED_ENABLEDSTATE);
    }
		AFuncReportResult(theSystem, 1L, NULL);
  }
}


// Sbar_Enable /////////////////////////////////////////////////////////////

void far pascal _export Sbar_Enable(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Enable Scroll Bar' function.");
	else {
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		if(!sbar->Enabled()) {
			sbar->SetEnabled(TRUE);
			sbar->PostObjectChanged(CHANGED_ENABLEDSTATE);
    }
		AFuncReportResult(theSystem, 1L, NULL);
  }
}


// Sbar_GetIncrement ///////////////////////////////////////////////////////

void far pascal _export Sbar_GetIncrement(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Bar Increment' function.");
	else {
		OBJECTID	lineID = AObjCreate(OTYPE_NUMBER);
		OBJECTID	pageID = AObjCreate(OTYPE_NUMBER);
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		ONmbrSetInteger(lineID, sbar->LineScroll(), TRUE);
		ONmbrSetInteger(pageID, sbar->PageScroll(), TRUE);
		AFuncSetParameter(1L, lineID);
		AFuncSetParameter(2L, pageID);
		AFuncReportResult(theSystem, 1L, NULL);
  }
}


// Sbar_GetPosition ////////////////////////////////////////////////////////

void far pascal _export Sbar_GetPosition(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Position' function.");
	else {
		OBJECTID	posID = AObjCreate(OTYPE_NUMBER);
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		ONmbrSetInteger(posID, sbar->Position(), TRUE);
		AFuncSetParameter(1L, posID);
		AFuncReportResult(theSystem, 1L, NULL);
  }
}


// Sbar_GetRange ///////////////////////////////////////////////////////////

void far pascal _export Sbar_GetRange(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Range' function.");
	else {
		OBJECTID	minID = AObjCreate(OTYPE_NUMBER);
		OBJECTID	maxID = AObjCreate(OTYPE_NUMBER);
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		ONmbrSetInteger(minID, sbar->Minimum(), TRUE);
		ONmbrSetInteger(maxID, sbar->Maximum(), TRUE);
		AFuncSetParameter(1L, minID);
		AFuncSetParameter(2L, maxID);
		AFuncReportResult(theSystem, 1L, NULL);
  }
}


// Sbar_IsEnabled //////////////////////////////////////////////////////////

void far pascal _export Sbar_IsEnabled(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The input must be a Scroll Bar object for the 'Is Enabled' function.");
	else {
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		if(sbar->Enabled())
			AFuncReportResult(theSystem, 1L, NULL);
		else
			AFuncReportResult(theSystem, 2L, NULL);
	}
}


// Sbar_SetIncrement ///////////////////////////////////////////////////////

void far pascal _export Sbar_SetIncrement(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	OBJECTID	lineID = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID	pageID = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Bar Increment' function.");
	else {
		BOOL			error = FALSE;

		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		if(lineID) {
			long line = labs(ONmbrGetInteger(lineID));
			if(line<=(sbar->Maximum()-sbar->Minimum()))
				sbar->SetLineScroll(line);
			else
				error = TRUE;
    }
		if(pageID) {
			long page = labs(ONmbrGetInteger(pageID));
			if(page<=(sbar->Maximum()-sbar->Minimum()))
				sbar->SetPageScroll(page);
			else
      	error = TRUE;
		}

    if(error)
			AFuncReportResult(theSystem, 2L, NULL);
		else
			AFuncReportResult(theSystem, 1L, NULL);
	}				// else {...
}


// Sbar_SetPosition ////////////////////////////////////////////////////////

void far pascal _export Sbar_SetPosition(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	OBJECTID	posID = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Position' function.");
	else if(!posID)
		AFuncReportResult(theSystem, 0L, "The second input must be a Number object for the 'Set Scroll Position' function.");
	else {
		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);

		long pos = ONmbrGetInteger(posID);
		if((pos<sbar->Minimum()) || (pos>sbar->Maximum()))
			AFuncReportResult(theSystem, 2L, NULL);
		else {
			sbar->SetPosition(pos);
			AFuncReportResult(theSystem, 1L, NULL);
    }
	}				// else {...
}


// Sbar_SetRange ///////////////////////////////////////////////////////////

void far pascal _export Sbar_SetRange(
	pAEvtInfo			/*theEvent*/,
	pAObjMessage	theSystem)
{
	OBJECTID	sbarID = AFuncGetTypedParameter(1L, scrollBarTypeID);
	OBJECTID	minID = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID	maxID = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
	SCROLLBAR*	sbar;

	if(!sbarID)
		AFuncReportResult(theSystem, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Range' function.");
	else {
		long			min, max;

		sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);
		if(minID)
			min = ONmbrGetInteger(minID);
		else
			min = sbar->Minimum();
		if(maxID)
			max = ONmbrGetInteger(maxID);
		else
			max = sbar->Maximum();
		if(min<max) {
			if(min!=sbar->Minimum() || max!=sbar->Maximum()) {
				sbar->SetMinimum(min);
				sbar->SetMaximum(max);
				if (sbar->Position() < min)
					sbar->SetPosition(min);
				if (sbar->Position() > max)
					sbar->SetPosition(max);
				if (sbar->LineScroll() > (max-min))
					sbar->SetLineScroll(max-min);
				if (sbar->PageScroll() > (max-min))
					sbar->SetPageScroll(max-min);
				sbar->PostObjectChanged(CHANGED_SCROLLRANGE);
			}				// if(min!=sbar->Minimum() || max!=sbar->Maximum()) {...
			AFuncReportResult(theSystem, 1L, NULL);
		}				// if(min<max) {...
		else
			AFuncReportResult(theSystem, 2L, NULL);
	}				// if(!sbarID) -- else {...
}

////////////////////////////////////////////////////////////////////////////
