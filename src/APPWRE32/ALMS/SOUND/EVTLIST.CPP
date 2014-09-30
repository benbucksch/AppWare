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
// FILE:          "evtlist.cpp"
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

#include <a_alm.h>
#include <intdebug.h>

#include "evtlist.h"

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::EvtList(int) --- constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

EvtList::EvtList
  (
  int nGrowthSize     // defaults to 10
  ) : m_nGrowthSize(nGrowthSize),
      m_pElements(new pAEvtInfo[m_nGrowthSize]),
      m_nElements(0),
      m_maxElements(m_nGrowthSize)

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::~EvtList() --- destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

EvtList::~EvtList
  (
  )

  {
  delete [] m_pElements;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::GetElement(int) const
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

pAEvtInfo& EvtList::GetElement
  (
  int   i
  ) const

  {
  I_ASSERT(i>=0 || i<m_nElements);

  return m_pElements[i];
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::SetElement(int, const pAEvtInfo&)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

BOOL EvtList::SetElement
  (
  int               i,
  const pAEvtInfo&  element
  )

  {
  BOOL  fRet;

  if(i>0 && i<m_nElements)
    {
    fRet = TRUE;
    m_pElements[i] = element;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::AddElement(const pAEvtInfo&)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int EvtList::AddElement
  (
  const pAEvtInfo&  element
  )

  {
  int nRet;

  if(m_nElements == m_maxElements)
    {
    pAEvtInfo*  pElements = new pAEvtInfo[m_maxElements + m_nGrowthSize];
    if(pElements)
      {
      for(int i=0; i<m_nElements; i++)
        {
        // must not use CopyMemory because it may not assign correctly
        pElements[i] = m_pElements[i];
        }

      delete [] m_pElements;
      m_pElements = pElements;
      m_maxElements += m_nGrowthSize;
      }
    }

  if(m_nElements < m_maxElements)
    {
    m_nElements++;
    m_pElements[m_nElements-1] = element;
    nRet = m_nElements - 1;
    }
  else
    nRet = -1;

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       EvtList::RemoveElement(int)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

BOOL EvtList::RemoveElement
  (
  int   i
  )

  {
  BOOL  fRet;
  int   j;

  if(i>0 && i<m_nElements)
    {
    for(j = i; j<m_nElements; j++)
      m_pElements[j] = m_pElements[j+1];

    m_nElements--;
    fRet = TRUE;
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
