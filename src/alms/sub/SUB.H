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
// FILE:          "sub.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SUB_H
#define SUB_H

// defines /////////////////////////////////////////////////////////////////

// type id
#define OTYPE_SUB               ATYPE_MAKEID('S','u','b','r')

// event id
#define OSUB_EVT_BAD            AEVENT_MAKEID(' ',' ',' ',' ')

// data
#define DATA_SUBDATA            0L
#define DATA_NUMDATABLOCKS      1L

// version
#define VERSION_SUBOBJECT       0x0100

// signals
#define SIGNAL_SUBCALLED        0
#define SIGNAME_SUBCALLED       "Called"
#define SIGNAL_NUMSIGNALS       1

// status flags
#define   FLAG_CALLED           0x00000001L

// SubData /////////////////////////////////////////////////////////////////

struct SubData
  {
  short      sVer;
  BOOL       fAutoReturn;
  };

// Subroutine class definition /////////////////////////////////////////////

class Subroutine
  {
  private:
    // object data
    SubData*        m_data;

    // runtime data
    AEvtInfo        m_event;
    AOBJECTID        m_oi1;
    AOBJECTID        m_oi2;
    AOBJECTID        m_oi3;

  protected:
    AOBJECTID        m_oiSelf;
    long            m_fStatus;

  public:
                    Subroutine(AOBJECTID oi);
                   ~Subroutine();

    // inline function to determine if object creation was successful
    inline BOOL     Valid()                   {return (m_data!=NULL);}

    // inline function to get the subroutine object id.
    inline AOBJECTID ObjectID()                {return m_oiSelf;}

    // inline functions to check object status
    inline BOOL     WasCalled()               {return (m_fStatus & FLAG_CALLED) ? TRUE : FALSE;}

    // inline functions to set object status
    inline void     Called()                  {m_fStatus |= FLAG_CALLED;}
    inline void     Returned()                {m_fStatus &= ~FLAG_CALLED;}

    // administration function message handlers
    long            Assign(Subroutine* pSrcSub);
    long            BuildSignals(AMEMBLOCKID mi);
    long            Check();
    long            Edit();
    long            Export();
    long            Import();
    long            ValidateIDs(pAObjMessage pMsg);

    // methods for data access        
    inline BOOL     AutoReturn()              {return m_data->fAutoReturn;}
    inline AEvtInfo Event()                   {return m_event;}
    void            GetParameters(AOBJECTID* oi1, AOBJECTID* oi2, AOBJECTID* oi3);

    // methods for setting data
    BOOL            SetAutoReturn(BOOL fSet);
    BOOL            SetEvent(pAEvtInfo pEvent);
    BOOL            SetParameters(AOBJECTID oi1, AOBJECTID oi2, AOBJECTID oi3);
  };

////////////////////////////////////////////////////////////////////////////

#endif    // #ifdef SUB_H
