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
// FILE:          "sub.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Subroutine class methods
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>
#include <string.h>

#include <a_alm.h>
#include <intdebug.h>

#include "sub.h"
#include "sub_rc.h"

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE  g_hinst;

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Constructor
//
//  Description:  This will lock and initialize any object data and runtime
//                data.
//
////////////////////////////////////////////////////////////////////////////

Subroutine::Subroutine
  (
  OBJECTID  oi
  )
  
  {
  // initialize runtime
  m_oiSelf = oi;
  memset(&m_event, 0, sizeof(AEvtInfo));
  m_event.what = OSUB_EVT_BAD;
  m_oi1 = NULL;
  m_oi2 = NULL;
  m_oi3 = NULL;
  m_fStatus = 0L;

  // make sure the correct number of data blocks are allocated
  if(AObjGetDataCount(m_oiSelf) != DATA_NUMDATABLOCKS)
    AObjSetDataCount(m_oiSelf, DATA_NUMDATABLOCKS);

  // get a pointer to the data
  m_data = (SubData*)AObjLockData(m_oiSelf, DATA_SUBDATA);

  if(!m_data)
    {
    // initialize the object data
    if(AObjResizeData(m_oiSelf, 0L, sizeof(SubData)))
      {
      m_data = (SubData*)AObjLockData(m_oiSelf, DATA_SUBDATA);

      if(m_data)
        {
        m_data->sVer        = VERSION_SUBOBJECT;
        m_data->fAutoReturn = TRUE;
        }
      }
    }
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  Method:       Destructor
//
//  Description:  This will unlock any locked data.
//
////////////////////////////////////////////////////////////////////////////

Subroutine::~Subroutine
  (
  )
  
  {
  if(m_data)
    AObjUnlockData(m_oiSelf, DATA_SUBDATA);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::Assign
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Subroutine::Assign
  (
  Subroutine* pSrcSub
  )

  {
  long  lRet;

  if(pSrcSub)
    {
    m_data->fAutoReturn = pSrcSub->m_data->fAutoReturn;
    lRet = A_OK;
    }
  else
    lRet = A_ERROR;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::BuildSignals
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Subroutine::BuildSignals
  (
  MEMBLOCKID  mi
  )

  {
  long  lRet = A_ERROR;

  if(AMemSetSize(mi, SIGNAL_NUMSIGNALS*sizeof(AObjSignalRecord)))
    {
    pAObjSignalRecord pSignals = (pAObjSignalRecord)AMemLock(mi);

    if(pSignals)
      {
      pSignals[0].theID = SIGNAL_SUBCALLED;
      lstrcpy(pSignals[0].theName, SIGNAME_SUBCALLED);
      lRet = A_OK;
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::Check
//
//  Description:  Subroutines will never have any errors, so always return
//                A_OK.
//
////////////////////////////////////////////////////////////////////////////

long Subroutine::Check
  (
  )

  {
  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::Edit
//
//  Description:  This calls the subroutine editing dialog.
//
////////////////////////////////////////////////////////////////////////////

long Subroutine::Edit
  (
  )

  {
  long      lRet;

  if(DialogBoxParam(g_hinst, MAKEINTRESOURCE(DLG_EDITSUB), GetActiveWindow(), (DLGPROC)SubDlg, (LPARAM)this))
    lRet = A_MODIFIED;
  else
    lRet = A_OK;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::Export
//
//  Description:  This does an UPSF export on the subroutine.
//
////////////////////////////////////////////////////////////////////////////

long  Subroutine::Export
  (
  )

  {
  AUpsfExportField("VERSION", 0L, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER, &m_data->sVer, sizeof(short));
  AUpsfExportField("MODAL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &m_data->fAutoReturn, sizeof(BOOL));

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::Import
//
//  Description:  This does an UPSF import on the subroutine.
//
////////////////////////////////////////////////////////////////////////////

long  Subroutine::Import
  (
  )

  {
  AUpsfImportField("MODAL", 0L, AUPSF_NATV_BOOL, &m_data->fAutoReturn, sizeof(BOOL));

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::ValidateIDs
//
//  Description:  Subroutine doesn't hold onto any object ids, so always
//                return A_OK.
//
////////////////////////////////////////////////////////////////////////////

long Subroutine::ValidateIDs
  (
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pMsg);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::GetParameters
//
//  Description:  This function will retrieve the input parameters.
//
////////////////////////////////////////////////////////////////////////////

void  Subroutine::GetParameters
  (
  OBJECTID* oi1,
  OBJECTID* oi2,
  OBJECTID* oi3
  )

  {
  *oi1 = m_oi1;
  *oi2 = m_oi2;
  *oi3 = m_oi3;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::SetAutoReturn
//
//  Description:  This function will set auto return flag and return if it
//                changed.
//
////////////////////////////////////////////////////////////////////////////

BOOL Subroutine::SetAutoReturn
  (
  BOOL  fSet
  )

  {
  BOOL  fRet = (fSet!=m_data->fAutoReturn);

  if(fRet)
    m_data->fAutoReturn = fSet;

  return fRet;  
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::SetEvent
//
//  Description:  This function will set the call function event.
//
////////////////////////////////////////////////////////////////////////////

BOOL Subroutine::SetEvent
  (
  pAEvtInfo pEvent
  )

  {
  BOOL  fRet = FALSE;

  if(pEvent)
    {
    fRet = (memcmp(pEvent, &m_event, sizeof(AEvtInfo)) == 0) ? FALSE : TRUE;
    m_event = *pEvent;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Subroutine::SetParameters
//
//  Description:  This function will set the input parameters to the
//                specified objects.
//
////////////////////////////////////////////////////////////////////////////

BOOL  Subroutine::SetParameters
  (
  OBJECTID  oi1,
  OBJECTID  oi2,
  OBJECTID  oi3
  )

  {
  BOOL  fRet = (oi1!=m_oi1) || (oi2!=m_oi2) || (oi3!=m_oi3);

  if(fRet)
    {
    m_oi1 = oi1;
    m_oi2 = oi2;
    m_oi3 = oi3;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
