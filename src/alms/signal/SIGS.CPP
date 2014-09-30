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
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/



#define STRICT
#define ASTRICT
#define	SIGSMODULE

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_wnd.h>
#include <op_wnd.h>
#include "o_pict.h"

#include "signal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <o_text.h>
#include <o_number.h>

#define 	NEG_ERR  -1
extern LPLONG GetRunTimePtr(AOBJECTID theObject, BOOL bIsStartUp );



/*****************************************************************************
* Method: SignalExists  
*         Check to see if the signal already exists
*        
*
*****************************************************************************/
BOOL SIGS::SignalExists(LPSTR sigName)
	{
	I_UNUSED(sigName);
	return FALSE;
	}


/*****************************************************************************
* Method: AdjustUserSignal
*         This method will write a user-defined signal to our datas (if it already does not exist)
*         
*
*****************************************************************************/
int SIGS::AdjustUserSignalIDs()
	{

	int i;
	int j;
	LPSIGSDATA   lpData = (LPSIGSDATA)AObjLockData(m_sigsOID, data0_SIGSDATA);
	LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(m_sigsOID, data1_DEFVALS);
	BOOL 		foundSig;
	LPUPDATESIGNAL lpUpdate = (LPUPDATESIGNAL)AMemLock(m_upListID);
	int			nUpdatedSigs    = AMemGetSize(m_upListID) / sizeof(UPDATESIGNAL);

  // update all the old signal names in the DATAS
	for (i = 0; i < nUpdatedSigs; i++)
		{
		for (j = 0; j < lpData->nUserSignals; j++)
      {
			if (lstrcmp(lpUpdate[i].oldName, lpData->signals[j].name) == 0)
				{
				lstrcpy(lpData->signals[j].name, lpUpdate[i].updName);
				break;
				}
			}
		}
	AMemUnlock(m_upListID);

	for (i = 0; i < m_nUserSignals; i++)
		{
		foundSig = FALSE;

		for (j = 0; j < lpData->nUserSignals; j++)
      {
			if (lstrcmp(m_userSignals[i].name, lpData->signals[j].name) == 0)
				{
				m_userSignals[i].sigID = lpData->signals[j].sigID;
				foundSig = TRUE;
				break;
				}
			}
		if (foundSig == FALSE)
			m_userSignals[i].sigID = lpDefData->maxSignalID++;
		}

	m_maxSignalID = lpDefData->maxSignalID;
	lpData->nUserSignals = m_nUserSignals;
	AObjUnlockData(m_sigsOID, data0_SIGSDATA);
	AObjUnlockData(m_sigsOID, data1_DEFVALS);
	return m_nUserSignals;
	}



/*****************************************************************************
* Method: SaveSiganlsToData
*         This method will build an array of signals based on
*				 	user defined signals and save them to Datas
*
*****************************************************************************/
int SIGS::SaveSignalsToData()
  {


	// at objd time the SIGS structure only consists of the User defined signals
	// Save if the number of new topics has changed or if user-defined signals have been modified.
	if (m_bSave & SIGS_SAVESIGS)
		{
		AdjustUserSignalIDs(); // for old signals use old ID from Datas

		AObjResizeData(m_sigsOID, data0_SIGSDATA, sizeof(SIGSDATA) + (m_nUserSignals) * sizeof(SIGNALINFO));
		LPSIGSDATA   lpData = (LPSIGSDATA)AObjLockData(m_sigsOID, data0_SIGSDATA);

		lpData->nUserSignals = m_nUserSignals;
		CopyMemory((lpData->signals), m_userSignals, (m_nUserSignals) * sizeof(SIGNALINFO));

		AObjUnlockData(m_sigsOID, data0_SIGSDATA);
		}

	
	delete m_userSignals; // delete the userSignals Structure (Allocated during SIGS()
  return TRUE;

	}



/*****************************************************************************
* Method: BuildSignalsFromData
*         This method will build an array of signals from the Datas
*
*
*****************************************************************************/
int SIGS::BuildSignalsFromData()
  {
	LPSIGSDATA    lpData     = (LPSIGSDATA)AObjLockData(m_sigsOID, data0_SIGSDATA);

	if (m_nUserSignals)
    {
		m_userSignals = new SIGNALINFO[m_nUserSignals]; // array of signals
		CopyMemory(m_userSignals, &(lpData->signals), m_nUserSignals * sizeof(SIGNALINFO));
    }
  else
		m_userSignals = NULL;



	AObjUnlockData(m_sigsOID, data0_SIGSDATA);
	return m_nUserSignals;
	}


/************************************************************************
*
*  DESCRIPTION:	Called during AOBJ_CHECKOBJECT and WM_INITDIALOG in SignalDlgProc
*	             	Check to see if the signals are valid
*
*************************************************************************/
BOOL SIGS::CheckSignals()
	{
	int 	i;
	BOOL bError = FALSE;
	SIGNALINFO *newSigs = NULL;
	int nSigs = 0;
	// Remove Signals that are tied to delete signals
	for (i = 0; i < m_nUserSignals; i++)
		{
		nSigs ++;
		SIGNALINFO *tmpSigs = new SIGNALINFO[nSigs]; // increment signal count and reallocate
		CopyMemory(tmpSigs, newSigs, (nSigs -1));
		delete newSigs;  // delete oldSigs
		CopyMemory((LPSTR)&(tmpSigs[nSigs - 1]), (LPSTR)&(m_userSignals[i]), sizeof(SIGNALINFO));
		newSigs  = tmpSigs;
		}

	// if some signals are deleted
	if (nSigs != 	m_nUserSignals)
		{
		m_nUserSignals = nSigs;
		delete m_userSignals;
		m_userSignals = newSigs;
    bError = TRUE;
		}

	return bError;
	}


/***********************************************************
*	SIGS::SIGS(AOBJECTID dObject)
*	SIGS class implementaion
*
************************************************************/
SIGS::SIGS(AOBJECTID dObject)
	{
	LPSIGSDATA    lpData     = (LPSIGSDATA)AObjLockData(dObject, data0_SIGSDATA);
	LPDEFOBJDDATA lpDefData  =  (LPDEFOBJDDATA)AObjLockData(dObject, data1_DEFVALS);

	m_sigsOID       = dObject;
	m_idInst				= ATypeGetRuntimeInfo(OTYPE_SIGS);

	m_bSave         = 0;
	// init from data0 -- SIGSDATA
	m_nUserSignals =  lpData->nUserSignals;
	m_isAlphaSignal = lpData->isAlphaSignal;
	BuildSignalsFromData();
	// memblock to hold list of transaction structures
	m_XMemID = AMemAllocate(0);
	m_maxSignalID   = lpDefData->maxSignalID ; // SKJ Mar. 7, 1994
	m_upListID   = AMemAllocate(0);// Mar. 25, 1995
	AObjUnlockData(dObject, data0_SIGSDATA);
	AObjUnlockData(dObject, data1_DEFVALS);
	}

/*************************************************************************
*	BOOL SIGS::	SetIsAlphaSignal()
*					 		This function will search all signals to check if the ALPHA bit is set
* 						If a single alpha bit is set the m_IsAlphaSignal is set to TRUE
*
*************************************************************************/
BOOL SIGS::SetIsAlphaSignal()
	{
	int i;
	m_isAlphaSignal = 0;  // init to 0
	for (i= 0; i < m_nUserSignals; i++)
		{
		m_isAlphaSignal |= (m_userSignals[i].bisAlphaSignal & SIG_ISALPHA);
		}
	return  m_isAlphaSignal;
	}
/**************************************************************************
* function : 		~SIGS
* Description: 	Destructor for SIGS class.  If the bSave flag is set, the fields
* 							of this instance are written out the SIGS object Datas
* bSave = TRUE --> user has chosen 'OK' @OBJD
*         FALSE -> otherwise
***************************************************************************/
SIGS::~SIGS()
  {
	if (m_bSave & SIGS_SAVEOBJD) // only after OK'd OBJD edit
		{
		LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(m_sigsOID, data1_DEFVALS);

		// Save the signals to Datas
		// Resize Data0 after having done the signal id adjustment
    SaveSignalsToData();

		LPSIGSDATA   lpData = (LPSIGSDATA)AObjLockData(m_sigsOID, data0_SIGSDATA);
		lpData->isAlphaSignal = m_isAlphaSignal;
		lpData->sigsOID = m_sigsOID;

		// save into data0 -- SIGSDATA
		lpData->nUserSignals =  m_nUserSignals;
		// init data 1
    // allocate space to hold all topics and their items
		lpDefData->maxSignalID  = 	m_maxSignalID;  // SKJ Mar. 7, 1994

		AMemFree(m_upListID);

		AObjUnlockData(m_sigsOID, data0_SIGSDATA);
		AObjUnlockData(m_sigsOID, data1_DEFVALS);
		} // eo-bSave

	}


/***********************************************************************
*int SIGS::IsNewSignal(SIGNALINFO defSig)
* 
*
************************************************************************/
int SIGS::IsNewSignal(SIGNALINFO defSig)
  {
	int curSigIndex = SIGS_SIGNALNOTFOUND;

	for (int i = 0; i < m_nUserSignals; i++)
    {

		if (lstrcmp(m_userSignals[i].name, defSig.name) == 0)
      {
      curSigIndex = i;
      break;
      }
    }
  return curSigIndex;
  }

/***********************************************************************
*	BOOL SIGS::RemoveSignal(int index)
*	DESCRIPTION:	Delete a signal from sigs' signal list.  Delete the old list.
*								Recreate a new list.
*
************************************************************************/
BOOL SIGS::RemoveSignal(int index)
	{
  int i;

	if ((index >= m_nUserSignals) || (index < 0))
    return FALSE;

	SIGNALINFO *tmpSignal = new SIGNALINFO[m_nUserSignals - 1];
  for (i = 0; i < index; i++)
		tmpSignal[i] = m_userSignals[i];

	for (i = index + 1; i < m_nUserSignals; i++)
		tmpSignal[i -1] = m_userSignals[i];

	m_nUserSignals --;
	delete m_userSignals;
	m_userSignals = tmpSignal;
  return TRUE;
  }


/***********************************************************************
*	SIGS *GetNextSIGS(long *count)
* 
*
************************************************************************/
SIGS *GetNextSIGS(long *count)
  {
	SIGS *sigS = NULL;
	AOBJECTID theObject = AObjGetNextObject(OTYPE_SIGS, count);
	if (theObject)
    {
		sigS = (SIGS *)GetRunTimePtr(theObject, FALSE);
    }
	return sigS;
	}


/**************************************************************************
* DESC: looks up the value of a number Object (float or whole) and converts it
* to a LONG
**************************************************************************/
static long int GetNumValue(AOBJECTID nObject) {
	return (ONmbrGetInteger(nObject));
	}



/****************************************************************************
 * int SIGS::PostSignal(long theSignal)
 *																																				  *
 *  FUNCTION   : 						                                                *
 *                                                                          *
 *  PURPOSE    : 																													  *
 *																																				  *
 ****************************************************************************/
int SIGS::PostSignal(long theSignal)
	{
	AEvtInfo event;
	int debug = 1;
	// Post Object Changed
	event.sourceObject  = m_sigsOID;          
	event.targetObject  = m_sigsOID;
  event.when          = AAppGetTime();
  event.how           = AEVT_ATONCE;
  event.what          = AEVENT_SIGNAL;
	event.details       = theSignal;

	if (debug)
		{
		return AEvtPostSignalAtMark(m_sigsOID, theSignal);

		}
  else
	  return AEvtPost(&event);

	}


/************************************************************************
* long  SIGS::GetSignal(AOBJECTID expObj)
*
*
************************************************************************/
long  SIGS::GetSignal(AOBJECTID expObj)
	{
	BOOL notFound = TRUE;
	int 	i;
	long	rSignal;
	if (m_isAlphaSignal)
		{
    char *szBuffer = new char[MAXNAME];
		OTextGetToBuffer(expObj, szBuffer, MAXNAME);
		for (i = 0; i < m_nUserSignals; i++)
			{
			if (lstrcmp(m_userSignals[i].name, szBuffer) == 0)
      	{
				rSignal = m_userSignals[i].sigID;
        notFound = FALSE;
				break;
				}
      }
		}
	else {
		long value 		= GetNumValue(expObj); // get the value of the expression
		for (i = 0; i < m_nUserSignals; i++)
			{
			if (atol(m_userSignals[i].name) == value)
      	{
				rSignal = m_userSignals[i].sigID;
        notFound = FALSE;
				break;
				}
      }
		}

	if (notFound)
		rSignal = ISIG_DEFAULT;;
	return rSignal;
	}



