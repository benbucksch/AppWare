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
// FILE:          "linklist.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This file contains the methods for the LinkedList class.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

// includes ////////////////////////////////////////////////////////////////

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

#include "xp_macro.h"
#include "linklist.h"

// class variable initialization ///////////////////////////////////////////

LinkedList* LinkedList::sm_pHead = NULL;
LinkedList* LinkedList::sm_pTail = NULL;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       LinkedList::LinkedList()  ---  constructor
//
//  DESCRIPTION:  This will add the *this* pointer to the LinkedList.  It
//                will also set up and modify the sm_pHead and sm_pTail
//                fields as necessary.
//
////////////////////////////////////////////////////////////////////////////

LinkedList::LinkedList
  (
  ) : m_dwProcessID(GetCurrentProcessId()),
      m_pNext(NULL),
      m_pPrev(NULL)

  {
  if(sm_pHead)
    {
    I_ASSERT(sm_pTail);

    // add the current item to the end of the list
    m_pPrev = sm_pTail;
    sm_pTail->m_pNext = this;
    sm_pTail = this;
    }
  else
    {
    I_ASSERT(sm_pTail == NULL);
    sm_pHead = sm_pTail = this;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       LinkedList::~LinkedList()  ---  destructor
//
//  DESCRIPTION:  This will remove the *this* pointer from the list.  It
//                will also modify the sm_pHead and sm_pTail as necessary.
//
////////////////////////////////////////////////////////////////////////////

LinkedList::~LinkedList
  (
  )

  {
  I_ASSERT(sm_pHead && sm_pTail);

  if(m_pNext && m_pPrev)
    {
    /* middle of the list -- fix the pointers to remove this item
     */
    m_pNext->m_pPrev = m_pPrev;
    m_pPrev->m_pNext = m_pNext;
    }
  else if(m_pNext)
    {
    /* start of the list -- fix the head pointer and the new head's
     * previous pointer to remove this item
     */
    I_ASSERT(this == sm_pHead);
    m_pNext->m_pPrev = NULL;
    sm_pHead = m_pNext;
    }
  else if(m_pPrev)
    {
    /* end of the list -- fix the tail pointer and the new tail's
     * next pointer to remove this item
     */
    I_ASSERT(this == sm_pTail);
    m_pPrev->m_pNext = NULL;
    sm_pTail = m_pPrev;
    }
  else
    {
    /* list had a single item -- set the head and tail to NULL
     */
    I_ASSERT((this == sm_pTail)  && (this == sm_pHead));
    sm_pHead = sm_pTail = NULL;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       LinkedList::GetNext(const LinkedList*)
//
//  DESCRIPTION:  This will get the next item in the linked list.  If the
//                parameter is NULL, the first (sm_pHead) item will be
//                returned.  When the last item is passed, NULL is returned.
//
////////////////////////////////////////////////////////////////////////////

LinkedList* LinkedList::GetNext
  (
  const LinkedList* pLL
  )

  {
  DWORD       dwProcessID = pLL ? pLL->m_dwProcessID : GetCurrentProcessId();
  LinkedList* pRet;

  if(pLL)
    pRet = pLL->m_pNext;
  else
    pRet = sm_pHead;

  while(pRet && (pRet->m_dwProcessID != dwProcessID))
    pRet = pRet->m_pNext;

  return pRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       LinkedList::GetPrev(const LinkedList*)
//
//  DESCRIPTION:  This will get the previous item in the linked list.  If
//                the parameter is NULL, the last (sm_pTail) item will be
//                returned.  When the first item is passed, NULL is
//                returned.
//
////////////////////////////////////////////////////////////////////////////

LinkedList* LinkedList::GetPrev
  (
  const LinkedList* pLL
  )

  {
  DWORD       dwProcessID = pLL ? pLL->m_dwProcessID : GetCurrentProcessId();
  LinkedList* pRet;

  if(pLL)
    pRet = pLL->m_pPrev;
  else
    pRet = sm_pTail;

  while(pRet && (pRet->m_dwProcessID != dwProcessID))
    pRet = pRet->m_pPrev;

  return pRet;
  }

////////////////////////////////////////////////////////////////////////////
