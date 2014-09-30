////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "loop.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>
#include <limits.h>

#include <a_alm.h>
#include <intdebug.h>

#include "loop.h"
#include "loop_rc.h"

////////////////////////////////////////////////////////////////////////////

extern HINSTANCE  g_hinst;

////////////////////////////////////////////////////////////////////////////

Loop::Loop
  (
  AOBJECTID  oi
  )

  {
  m_oiSelf   = oi;
  m_fRunning = FALSE;
  memset(&m_event, 0, sizeof(AEvtInfo));
  m_event.what = OLOOP_EVT_BAD;

  // initialize the object data
  if(AObjGetDataCount(m_oiSelf) != DATA_NUMDATABLOCKS)
    AObjSetDataCount(m_oiSelf, DATA_NUMDATABLOCKS);

  // keep loop data locked throughout the life of the object
  if(AObjGetDataSize(m_oiSelf, DATA_LOOPDATA) != sizeof(LoopData))
    {
    AObjResizeData(m_oiSelf, DATA_LOOPDATA, sizeof(LoopData));
    m_data = (LoopData*)AObjLockData(m_oiSelf, DATA_LOOPDATA);
    if(m_data)
      {
      m_data->sVer        = VERSION_LOOPOBJECT;
      m_data->lStart      = 1L;
      m_data->lLength     = 100L;
      m_data->lCurrIdx    = 0L;
      m_data->oiConnect   = NULL;
      m_data->fMultiTask  = FALSE;
      }
    }
  else
    {
    m_data = (LoopData*)AObjLockData(m_oiSelf, DATA_LOOPDATA);

    // Versions through 0x0100 have the same data structure.
    // Make sure the version is updated to the current version.
    if(m_data && (m_data->sVer < VERSION_LOOPOBJECT))
      m_data->sVer = VERSION_LOOPOBJECT;
    }
  }

////////////////////////////////////////////////////////////////////////////

Loop::~Loop
  (
  )

  {
  if(m_data)
    AObjUnlockData(m_oiSelf, DATA_LOOPDATA);
  }

////////////////////////////////////////////////////////////////////////////

long Loop::Assign
  (
  Loop* pSrcLoop
  )

  {
  m_data->lStart     = pSrcLoop->m_data->lStart;
  m_data->lLength    = pSrcLoop->m_data->lLength;
  m_data->oiConnect  = pSrcLoop->m_data->oiConnect;
  m_data->fMultiTask = pSrcLoop->m_data->fMultiTask;

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::BuildSignals
  (
  AMEMBLOCKID  mi
  )

  {
  long  lRet = A_ERROR;

  if(AMemSetSize(mi, SIGNAL_NUMSIGNALS*sizeof(AObjSignalRecord)))
    {
    pAObjSignalRecord pSignals = (pAObjSignalRecord)AMemLock(mi);

    if(pSignals)
      {
      pSignals[0].theID = SIGNAL_EACHITERATION;
      lstrcpy(pSignals[0].theName, SIGNAME_EACHITERATION);
      lRet = A_OK;
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::Check
  (
  )

  {
  long  lRet;

  if(((long double)m_data->lStart + (long double)m_data->lLength) > (long double)LONG_MAX)
    lRet = A_WARNING;
  else if(((long double)m_data->lStart + (long double)m_data->lLength) < (long double)LONG_MIN)
    lRet = A_WARNING;
  else
    lRet = A_OK;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::Edit
  (
  )

  {
  long      lRet;
  BOOL      fValid = AObjCheckType(m_data->oiConnect, 0L);

  if(!fValid)
    m_data->oiConnect = NULL;

  if(DialogBoxParam(g_hinst, MAKEINTRESOURCE(DLG_EDITLOOP), GetActiveWindow(), LoopDlg, (LPARAM)this))
    lRet = A_MODIFIED;
  else
    lRet = A_OK;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::Export
  (
  )

  {
  long  lRet = A_OK;

  AUpsfExportField("VERSION",  0L, AUPSF_NATV_SHORT,     AUPSF_UNIV_INTEGER,   &m_data->sVer,       sizeof(short));
  AUpsfExportField("START",    0L, AUPSF_NATV_LONG,      AUPSF_UNIV_INTEGER,   &m_data->lStart,     sizeof(long));
  AUpsfExportField("LENGTH",   0L, AUPSF_NATV_LONG,      AUPSF_UNIV_INTEGER,   &m_data->lLength,    sizeof(long));
  AUpsfExportField("INDEX",    0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &m_data->oiConnect,  sizeof(AOBJECTID));
  AUpsfExportField("MULTITSK", 0L, AUPSF_NATV_BOOL,      AUPSF_UNIV_INTEGER,   &m_data->fMultiTask, sizeof(BOOL));

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::Import
  (
  )

  {
  long  lRet = A_OK;

  // object is already initialized, so if there is an error the defaults are already set
  AUpsfImportField("START",    0L, AUPSF_NATV_LONG,      &m_data->lStart,     sizeof(long));
  AUpsfImportField("LENGTH",   0L, AUPSF_NATV_LONG,      &m_data->lLength,    sizeof(long));
  AUpsfImportField("INDEX",    0L, AUPSF_NATV_OBJECTREF, &m_data->oiConnect,  sizeof(AOBJECTID));
  AUpsfImportField("MULTITSK", 0L, AUPSF_NATV_BOOL,      &m_data->fMultiTask, sizeof(BOOL));

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Loop::ValidateIDs
  (
  pAObjMessage  pMsg
  )

  {
  if(m_data->oiConnect)
    AObjValidateID(&m_data->oiConnect, pMsg);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////

void Loop::SetEvent
  (
  pAEvtInfo pEvent
  )

  {
  if(pEvent)
    m_event = *pEvent;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Loop::SetStartValue
  (
  long  lStart
  )

  {
  BOOL  fRet;
  
  if(lStart != m_data->lStart)
    {
    m_data->lStart = lStart;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Loop::SetLength
  (
  long  lLength
  )

  {
  BOOL  fRet;
  
  if(lLength != m_data->lLength)
    {
    m_data->lLength = lLength;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Loop::SetCurrentIndex
  (
  long  lIdx
  )

  {
  BOOL  fRet;
  
  if(lIdx != m_data->lCurrIdx)
    {
    m_data->lCurrIdx = lIdx;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Loop::SetConnectionObject
  (
  AOBJECTID  oi
  )

  {
  BOOL  fRet;
  
  if(oi != m_data->oiConnect)
    {
    m_data->oiConnect = oi;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Loop::SetMultiTask
  (
  BOOL  fMultiTask
  )

  {
  BOOL  fRet;
  
  if(fMultiTask != m_data->fMultiTask)
    {
    m_data->fMultiTask = fMultiTask;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
