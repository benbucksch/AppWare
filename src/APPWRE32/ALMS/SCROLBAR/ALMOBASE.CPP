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
// FILE:          "almobase.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is a base class for ALM objects.  This will handle
//                the basic Administration function messages.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

#include "almobase.h"

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::ALMOBase(AOBJECTID, BOOL)
//
//  DESCRIPTION:  This is the ALM object base constructor.
//
////////////////////////////////////////////////////////////////////////////

ALMOBase::ALMOBase
  (
  AOBJECTID  oi,
  BOOL      fTemp
  ) : m_oiSelf(oi)

  {
  I_UNUSED(fTemp);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::~ALMOBase()
//
//  DESCRIPTION:  This is the ALM object destructor.
//
////////////////////////////////////////////////////////////////////////////

ALMOBase::~ALMOBase
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::Assign(const Sound*)
//
//  DESCRIPTION:  This will assign one object to another.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::Assign
  (
  const ALMOBase* pSrc
  )

  {
  long  lRet = A_NOTHANDLED;

  // protect against self assignment
  if(pSrc != this)
    {
    // do the assignment
    } // if(pSrc != this)

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::BuildSignals(AMEMBLOCKID) const
//
//  DESCRIPTION:  This will build the signal structure for the given object.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::BuildSignals
  (
  AMEMBLOCKID  mi
  ) const

  {
  I_UNUSED(mi);

  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::Check() const
//
//  DESCRIPTION:  This will verify that the object is correctly configured.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::Check
  (
  ) const

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::Edit()
//
//  DESCRIPTION:  This will cause the given object to launch the editing
//                dialog and start editing.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::Edit
  (
  )

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::Export() const
//
//  DESCRIPTION:  This will cause the object to UPSF export itself.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::Export
  (
  ) const

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::Import()
//
//  DESCRIPTION:  This will cause the object to UPSF import itself.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::Import
  (
  )

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::PostWrite()
//
//  DESCRIPTION:  This message tells the object to fix its object data
//                after being saved.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::PostWrite
  (
  )

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::PreWrite()
//
//  DESCRIPTION:  This method converts runtime information to "file" data.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::PreWrite
  (
  )

  {
  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::QueryProtocol(AOBJECTID, pApQueryInfo) --- static
//
//  DESCRIPTION:  This method is called when AppWare needs to know if a
//                given object or type supports a particular protocol.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::QueryProtocol
  (
  AOBJECTID      oi,
  pApQueryInfo  pQI
  )

  {
  long  lRet;

  I_UNUSED(pQI);

  if(oi) // object protocol support
    lRet = AP_NOTSUPPORTED;
  else        // type protocol support
    lRet = AP_NOTSUPPORTED;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::ReportRequiredFiles(pAObjMessage) --- static
//
//  DESCRIPTION:  This method is called when the AppWare bus needs to know
//                what files a particular type needs to run (be shipped
//                with).
//
//  LIMITATIONS:  This routine will only work for DOS (8.3 filenames).
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::ReportRequiredFiles
  (
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pMsg);

  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ALMOBase::ValidateIDs(pAObjMessage)
//
//  DESCRIPTION:  This method is called to notify an object that OBJECTIDs
//                may have changed.  The object needs to update any IDs
//                that it owns.
//
////////////////////////////////////////////////////////////////////////////

long ALMOBase::ValidateIDs
  (
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pMsg);

  return A_NOTHANDLED;
  }

////////////////////////////////////////////////////////////////////////////
