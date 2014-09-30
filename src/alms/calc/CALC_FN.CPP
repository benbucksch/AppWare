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
// Filename:	"calc_fn.cpp"
//
// Author:	Kevin Smart
//   Start:		July 22, 1992
//   Finish:	July xx, 1992
//
// Description:
//	Calculation Object Functions
//	  1.	Calculate
//	  2.	SetExpression
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<intdebug.h>
#include	<limits.h>
#include	<o_text.h>
#include	<o_number.h>

#include	"strclass.h"
#include	"calc_obj.h"
#include	"calc.h"

////////////////////////////////////////////////////////////////////////////
//  AutoUpdateOff
//
//      inputs:
//        1.  Calculation Object
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after setting autoupdate OFF
////////////////////////////////////////////////////////////////////////////

extern "C" 
	void ALMCALLBACK         AutoUpdateOff	(pAEvtInfo theEvent,
																							 pAObjMessage      theSystem)

	{
		AOBJECTID		theObject	= AFuncGetTypedParameter(1L, OTYPE_EXPRESSION);

		I_UNUSED(theEvent);

  	if(theObject==NULL) {
    	AFuncReportResult(theSystem, 1L, "The input to the \"Auto Update Off\" function must be a Calculation Object");
	    return;
			}

		CALCHEADER*	calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
		if(calcHeader) {
			calcHeader->autoUpdate	= FALSE;
	    AFuncReportResult(theSystem, 1L, NULL);
  	  }
		else
			AFuncReportResult(theSystem, 1L, "Unable to change the Auto Update flag due to a memory error.");

		AObjUnlockData(theObject, 3L);
	}


////////////////////////////////////////////////////////////////////////////
// 	AutoUpdateOn
//
//      inputs:
//        1.  Calculation Object
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after setting AutoUpdate ON
////////////////////////////////////////////////////////////////////////////

extern "C" 
	void ALMCALLBACK         AutoUpdateOn	(pAEvtInfo theEvent,
																							 pAObjMessage      theSystem)

	{
		AOBJECTID		theObject	= AFuncGetTypedParameter(1L, OTYPE_EXPRESSION);

		I_UNUSED(theEvent);

  	if(theObject==NULL) {
			AFuncReportResult(theSystem, 1L, "The input to the \"Auto Update On\" function must be a Calculation Object");
	    return;
  	  }

		CALCHEADER*	calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
		if(calcHeader) {
			AOBJECTID* oid = (AOBJECTID*)AObjLockData(theObject, 0L);
			BOOL			changeOK = TRUE;
  	  int				i = 0;
			while(i<calcHeader->count) {
				if(oid[i]==calcHeader->resultID) {
					changeOK = FALSE;
					AFuncReportResult(theSystem, 1L, "Calculation:  Auto Update On\r\rUnable to start auto updating because the result object is in the variable list.");
					break;
    	    }
      	i++;
				}

			if(changeOK) {
				calcHeader->autoUpdate = TRUE;
				AFuncReportResult(theSystem, 1L, NULL);
		    }

      if(oid)
    	  AObjUnlockData(theObject, 0L);
			AObjUnlockData(theObject, 3L);
			}
		else
			AFuncReportResult(theSystem, 1L, "Unable to change the Auto Update flag due to a memory error.");
	}


////////////////////////////////////////////////////////////////////////////
//  Calculate
//
//      inputs:
//        1.  Calculation Object
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after calculating
////////////////////////////////////////////////////////////////////////////

extern "C" 
	void ALMCALLBACK         Calculate	(pAEvtInfo theEvent,
                                           pAObjMessage      theSystem)

	{
  	AOBJECTID	inputObject	= AFuncGetTypedParameter(1L, OTYPE_EXPRESSION);

		I_UNUSED(theEvent);
    
    I_ASSERT(inputObject);

///  	if(inputObject==NULL) {
///	    AFuncReportResult(theSystem, 1L, "The input to the \"Calculate\" function must be a Calculation Object");
///	    return;
///	    }
///		else {
	    EvalExpression(inputObject, theSystem->errorMessage);
	    AFuncReportResult(theSystem, 1L, NULL);
	    return;
///	    }
	}

////////////////////////////////////////////////////////////////////////////
//  SetExpression
//
//      inputs:
//        1.  Calculation Object
//			  2.  Expression (Text Object)
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after setting the expression
////////////////////////////////////////////////////////////////////////////

extern "C" 
	void ALMCALLBACK         SetExpression	(pAEvtInfo theEvent,
	                                               pAObjMessage      theSystem)

	{
	  AOBJECTID	calcObject	= AFuncGetTypedParameter(1L, OTYPE_EXPRESSION);
	  AOBJECTID	exprObject	= AFuncGetTypedParameter(2L, OTYPE_TEXT);

		I_UNUSED(theEvent);

	  if(calcObject==NULL) {
	    AFuncReportResult(theSystem, 1L, "The first input to the \"Set Expression\" function must be a Calculation Object");
	    return;
	    }

	  if(exprObject==NULL) {
	    AFuncReportResult(theSystem, 1L, "The second input to the \"Set Expression\" function must be a Text Object");
	    return;
	    }

	  AMEMBLOCKID	theBlock = AMemAllocate(1L);
	  if(!AObjConvertToText(exprObject, theBlock)) {
	    AFuncReportResult(theSystem, 1L, "Unable to get the text information from the \"Expression\" parameter.");
	    AMemFree(theBlock);
	    return;
	    }

	  long		exprLen = AMemGetSize(theBlock);
		char*		expr	= (char*)AMemLock(theBlock);
	  CALCULATION*	calc = (CALCULATION*)AObjGetRuntimeInfo(calcObject);
	//  if(!AObjSetData(calcObject, 2L, expr, exprLen) && exprLen) {
		if (!calc->SetExpression(expr, exprLen) && exprLen) {
	    AFuncReportResult(theSystem, 1L, "Error updating the expression.");
	    AMemFree(theBlock);
	    return;
	    }
	  else {
	    AEvtPostStandard(calcObject, AEVENT_OBJECTCHANGED, 0L, AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
	    AEvtPostSignalAtMark(calcObject, calcVariableChanged);
	    AFuncReportResult(theSystem, 1L, NULL);
	    AMemFree(theBlock);
	    return;
	    }
	}


////////////////////////////////////////////////////////////////////////////
// EvalExpression
//
//   Returns TRUE when the "Result" number object is filled with a vaild
//   number.
////////////////////////////////////////////////////////////////////////////

BOOL	EvalExpression(AOBJECTID theObject, char* errorMessage)

{
  if(theObject==NULL) {
    lstrcpy(errorMessage, "Invalid calculation object.");
    return FALSE;
    }

  CALCULATION*	calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
///	CALCULATION		calc(theObject, TRUE);
  long double		result;

  if(!evaluate(calc->Expression(), result, errorMessage, theObject)) {
    // the expression evaluated correctly
		AOBJECTID		ResultID = calc->Result();

    errorMessage[0] = '\0';  // no error

    if(ResultID==NULL)
			{
			// no error message necessary--taken care of at compile time.
			AEvtPostSignalAtMark(theObject, calcObjectUpdated);
			return FALSE;
      }

///		switch (ONmbrGetType(ResultID))
///			{
///			case ONMBR_INTEGER:
				ONmbrSetReal(ResultID, result, FALSE);
///				break;

///			case ONMBR_REAL:
///				ONmbrSetReal(ResultID, result, FALSE);
///				break;

///			default:
///				ADBG_PRINT_W("Invalid number type in CALC::EvalExpression");
///				break;
///			}

    AEvtPostSignalAtMark(theObject, calcObjectUpdated);
///		AEvtPostStandard(ResultID, AEVENT_OBJECTCHANGED, 0L, AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
    return TRUE;
    }
  else {
    // there was an error in the expression
		AEvtPostSignalAtMark(theObject, calcExpressionError);

 		return FALSE;
    }  
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Expression--Number Object function
//
//      inputs:
//        1.  X--Number Object (optional)
//			  2.  Y--Number Object (optional)
//			  3.  Expression (Text Object)
//
//      outputs:
//        1.  Result--Number Object
//
//      flows:
//        1.  Followed after calculating
//			  2.  Followed if an error occurred in the calculation
////////////////////////////////////////////////////////////////////////////

extern "C" {
	void ALMCALLBACK Expression	(pAEvtInfo	theEvent,
																		 pAObjMessage	theSystem)

	{
	  AOBJECTID	inOb1	= AFuncGetTypedParameter(1L, OTYPE_NUMBER);
    I_ASSERT(inOb1);
    
	  AOBJECTID	inOb2	= AFuncGetTypedParameter(2L, OTYPE_NUMBER);
    I_ASSERT(inOb2);
    
	  AOBJECTID	expID	= AFuncGetTypedParameter(3L, OTYPE_TEXT);
    I_ASSERT(expID);
    
	  long double	theValues[2];
	
		I_UNUSED(theEvent);

///	  if(!expID) {
///  	  AFuncReportResult(theSystem, 2L, "The third input to the \"Expression\" function must be a Text Object.");
///    	return;
///	    }

  	if (!inOb1)
    	theValues[0] = 0.0;
	  else
			ONmbrGetReal(inOb1, &theValues[0]);

	  if(!inOb2)
	    theValues[1] = 0.0;
	  else
			ONmbrGetReal(inOb2, &theValues[1]);

  	AMEMBLOCKID	theBlock = AMemAllocate(1L);
    I_ASSERT(theBlock);
///	  if(!theBlock) {
///	    AFuncReportResult(theSystem, 2L, "Unable to allocate memory for the \"Expression\" function.");
///	    return;
///      }

	  if(!AObjConvertToText(expID, theBlock)) {
	    AFuncReportResult(theSystem, 2L, "Unable to get the text from the \"Expression\" parameter of the \"Expression\" function.");
	    AMemFree(theBlock);
	    return;
	    }

	  char*		expr = (char*)AMemLock(theBlock);
    I_ASSERT(expr);
    
	  if(!expr) {
	    AFuncReportResult(theSystem, 2L, "Unable to get the text from the \"Expression\" parameter of the \"Expression\" function.");
	    AMemFree(theBlock);
	    return;
	    }

	  long double result;
	  if(!evaluate(expr, result, theSystem->errorMessage, NULL, theValues)) {
	    // expression evaluated correctly--no error
	    theSystem->errorMessage[0] = 0;
	    AMemFree(theBlock);

	    AOBJECTID	theResult = AObjCreate(OTYPE_NUMBER);
      I_ASSERT(theResult);
      
///      if(!theResult) {
///	      AFuncReportResult(theSystem, 2L, "Unable to create the result for the \"Expression\" function");
///      return;
///      }

			ONmbrSetReal(theResult, result, TRUE);

  	  // Write the number out
			AFuncSetParameter(1L, theResult);
	    AFuncReportResult(theSystem, 1L, NULL);
///			AObjUnlockData(theResult, 0L);
	    return;
	    }
	  else {
	    // expression error:  theSystem->errorMessage is already updated
	    AFuncReportResult(theSystem, 2L, NULL);
	    AMemFree(theBlock);
	    return;
	    }
	}
}

////////////////////////////////////////////////////////////////////////////
