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
// FILE:          "linklist.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is a generic doubly linked list class.  Since this
//                has static members, this can only be the base for a single
//                type of object.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LINKLIST_H
#define LINKLIST_H

// class LinkedList definition /////////////////////////////////////////////

class LinkedList

  {
  public:
                        LinkedList();
    virtual            ~LinkedList();

    /* The following methods will return a pointer to the next (previous)
     * item in the list, or if pLL is NULL, then it will point to the first
     * or last item.  When the item is last (GetNext) or first (GetPrev),
     * NULL will be returned.  This will only loop through items of a single
     * process.
     */
    static LinkedList*  GetNext(const LinkedList* pLL);
    static LinkedList*  GetPrev(const LinkedList* pLL);

  protected:
    /* Provide a way to get the head and tail pointers.  This will return the
     * head/tail item for the current process.
     */
    static LinkedList*  GetHead()     {return GetNext(NULL);}
    static LinkedList*  GetTail()     {return GetPrev(NULL);}

    /* The static members of this class are managed by the constructor
     * and destructor.  When the first LinkedList item is created, the
     * sm_pHead and sm_pTail are initialized to that item.  When items
     * are added or deleted, these pointer change as necessary.
     */
    static LinkedList*  sm_pHead;
    static LinkedList*  sm_pTail;

    /* These are pointers to the previous and next objects in the linked
     * list.
     */
    LinkedList*         m_pNext;
    LinkedList*         m_pPrev;

  private:
    /* Save the process id so we only loop through items in a specific
     * process.
     */
    DWORD               m_dwProcessID;
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LINKLIST_H
