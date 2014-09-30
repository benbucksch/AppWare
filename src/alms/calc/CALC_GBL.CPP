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
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc_gbl.h"
//
// Owner (plagerizer):	Russ Graves
//
// Description:
//	Methods for the Global Task Class -- manage the information that MUST be global
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<string.h>

#include	"calc_obj.h"

// Constructor /////////////////////////////////////////////////////////////

GLOBAL::GLOBAL()

{
}

// Destructor //////////////////////////////////////////////////////////////

GLOBAL::~GLOBAL()

{
}

// GetGlobalData ///////////////////////////////////////////////////////////

TASKGLOBALDATA*	GLOBAL::GetGlobalData()

{
	TASKGLOBALDATA*	gData = (TASKGLOBALDATA*)ATypeGetRuntimeInfo(OTYPE_EXPRESSION);

	return gData;
}

// AddGlobalData ///////////////////////////////////////////////////////////

TASKGLOBALDATA*	GLOBAL::AddGlobalData()

{
	TASKGLOBALDATA* gData = new TASKGLOBALDATA;

  ATypeSetRuntimeInfo(OTYPE_EXPRESSION, (long)gData);

	return gData;
}

// DeleteGlobalData ////////////////////////////////////////////////////////

void GLOBAL::DeleteGlobalData()

{
	if(AObjGetObjectCount(OTYPE_EXPRESSION)<=0)
  	{
		TASKGLOBALDATA*	gData = (TASKGLOBALDATA*)ATypeGetRuntimeInfo(OTYPE_EXPRESSION);

	  delete gData;
		ATypeSetRuntimeInfo(OTYPE_EXPRESSION, 0L);
		}
}


// TaskGlobalData //////////////////////////////////////////////////////////////

TASKGLOBALDATA*	GLOBAL::GlobalData()

{
	TASKGLOBALDATA*	gData;

	gData = GetGlobalData();
	if(!gData)
		gData = AddGlobalData();

	return gData;
}

// OldProc() ////////////////////////////////////////////////////////////

FARPROC	GLOBAL::OldProc()

{
	TASKGLOBALDATA*	gData = GlobalData();
	FARPROC			lpfnRet;

	if(gData)
		lpfnRet = gData->lpfnOldProc;
  else
		lpfnRet = NULL;

  return lpfnRet;
}

// SetOldProc(BOOL) /////////////////////////////////////////////////////

void	GLOBAL::SetOldProc(FARPROC oldProc)

{
	TASKGLOBALDATA*	gData = GlobalData();
	if(gData)
		gData->lpfnOldProc = oldProc;
}

// ParseError() ////////////////////////////////////////////////////////////

BOOL	GLOBAL::ParseError()

{
	TASKGLOBALDATA*	gData = GlobalData();
	BOOL				fRet;

	if(gData)
		fRet = gData->parseError;
  else
		fRet = FALSE;

  return fRet;
}

// SetParseError(BOOL) /////////////////////////////////////////////////////

void	GLOBAL::SetParseError(BOOL error)

{
	TASKGLOBALDATA*	gData = GlobalData();
	if(gData)
		gData->parseError = error;
}

// Error() /////////////////////////////////////////////////////////////////

char*	GLOBAL::Error()

{
	TASKGLOBALDATA*	gData = GlobalData();
	char*				szRet;

	if(gData)
		szRet = gData->error;
  else
		szRet = NULL;

  return szRet;
}

// SetError(char*) /////////////////////////////////////////////////////////

void	GLOBAL::SetError(char* error)

{
	TASKGLOBALDATA*	gData = GlobalData();
	if(gData)
		lstrcpyn(gData->error, error, 255);
}

// FPEerror() //////////////////////////////////////////////////////////////

BOOL	GLOBAL::FPEerror()

{
	TASKGLOBALDATA*	gData = GlobalData();
	BOOL				fRet;

	if(gData)
		fRet = gData->FPEerror;
  else
		fRet = FALSE;

	return fRet;
}

// SetFPEerror(BOOL) ///////////////////////////////////////////////////////

void	GLOBAL::SetFPEerror(BOOL error)

{
	TASKGLOBALDATA*	gData = GlobalData();
	if(gData)
		gData->FPEerror = error;
}

// MATHerror() /////////////////////////////////////////////////////////////

BOOL	GLOBAL::MATHerror()

{
	TASKGLOBALDATA*	gData = GlobalData();
	BOOL				fRet;

	if(gData)
		fRet = gData->MATHerror;
  else
		fRet = FALSE;

  return fRet;
}

// SetMATHerror(BOOL) //////////////////////////////////////////////////////

void	GLOBAL::SetMATHerror(BOOL error)

{
	TASKGLOBALDATA*	gData = GlobalData();
	if(gData)
		gData->MATHerror = error;
}

////////////////////////////////////////////////////////////////////////////
