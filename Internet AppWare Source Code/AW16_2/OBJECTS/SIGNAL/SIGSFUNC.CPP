/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:				SIGSFUNC.cpp
//
// AUTHOR:      Shaili Jain	
//
// DESCRIPTION:	This module contains appbldr-level functions
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_number.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


#include "signal.h"




/***************************************************************************
* Switch function performs a multi way decision that tests whether an
* expression (as passed in the 2nd parameter) matches one of the constant
* integer values (or string values) and sends a branch flow accordingly.
* For each signal object we can have user-defined number of case signals. Each
* object also has a 'default' and 'any' signal.
***************************************************************************/
extern "C" void FAR PASCAL _export GenerateSignals(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID signalObj, expObj;
	long		 signal;
	signalObj = AFuncGetParameter(1); // the signal object
	SIGS *sigs = (SIGS *)new SIGS(signalObj);

	if (sigs->m_isAlphaSignal)
		expObj    = AFuncGetTypedParameter(2, OTYPE_TEXT);
	else
		expObj    = AFuncGetTypedParameter(2, OTYPE_NUMBER);

	if (expObj)
  	{
		signal    = sigs->GetSignal(expObj);
		sigs->PostSignal(signal);
    if (signal != ISIG_DEFAULT)
			sigs->PostSignal(ISIG_ANY);
		system->message1 = 1;
		}
	else
		{
		lstrcpy(system->errorMessage, "Error in Expression.");
		system->message1 = 2;
		}
	I_UNUSED(theEvent);
	
	}


