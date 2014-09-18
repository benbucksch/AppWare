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
// FILE:          "evtlist.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef EVTLIST_H
#define EVTLIST_H

// EvtList class definition ////////////////////////////////////////////////

class EvtList

  {
  public:
    pAEvtInfo&      GetElement(int i) const;
    BOOL            SetElement(int i, const pAEvtInfo& element);
    int             AddElement(const pAEvtInfo& element);
    BOOL            RemoveElement(int i);
    int             CountElements() const           {return m_nElements;}

                    EvtList(int nGrowthSize = 10);
                   ~EvtList();

  private:
    int             m_nGrowthSize;

    pAEvtInfo*      m_pElements;
    int             m_nElements;
    int             m_maxElements;

    // unused methods for protection against incorrect use of this class
                    EvtList(const EvtList&);
    EvtList&        operator=(const EvtList&);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef EVTLIST_H
