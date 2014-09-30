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
// FILE:          "loop.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LOOP_H
#define LOOP_H

// includes ////////////////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

// defines /////////////////////////////////////////////////////////////////

// type id
#define OTYPE_LOOP                ATYPE_MAKEID('L','o','o','p')

// event ids
#define OLOOP_EVT_EACHITERATION   AEVENT_MAKEID('d', 'o', 'L', 'p')
#define OLOOP_EVT_BAD             AEVENT_MAKEID(' ', ' ', ' ', ' ')

// data
#define DATA_LOOPDATA             0L
#define DATA_NUMDATABLOCKS        1L

// version
#define VERSION_LOOPOBJECT        0x0100

// signals
#define SIGNAL_EACHITERATION      0
#define SIGNAME_EACHITERATION     "Each iteration"
#define SIGNAL_NUMSIGNALS         1

// LoopData structure definition ///////////////////////////////////////////

struct LoopData
  {
  short     sVer;
  long      lStart;
  long      lLength;
  long      lCurrIdx;
  AOBJECTID  oiConnect;
  BOOL      fMultiTask;
  };

// Loop class definition////////////////////////////////////////////////////

class Loop
  {
  private:
    LoopData*       m_data;     // left locked during life of C++ Loop object.
    AEvtInfo        m_event;

  protected:
    AOBJECTID        m_oiSelf;
    BOOL            m_fRunning; // flag indicating that the loop is executing

  public:
                    Loop(AOBJECTID oi);
                   ~Loop();

    // inline function to determine if object creation was successful
    BOOL            Valid()                   {return (m_data!=NULL);}

    // inline access to data
    inline AOBJECTID ObjectID()                {return m_oiSelf;}
    inline AEvtInfo Event()                   {return m_event;}
    inline long     StartValue()              {return m_data->lStart;}
    inline long     Length()                  {return m_data->lLength;}
    inline long     CurrentIndex()            {return m_data->lCurrIdx;}
    inline AOBJECTID ConnectionObject()        {return m_data->oiConnect;}
    inline BOOL     MultiTask()               {return m_data->fMultiTask;}

    // administration function messange handlers
    long            Assign(Loop* pSrcLoop);
    long            BuildSignals(AMEMBLOCKID mi);
    long            Check();
    long            Edit();
    long            Export();
    long            Import();
    long            ValidateIDs(pAObjMessage pMsg);

    // methods to set data
    void            SetEvent(pAEvtInfo pEvent);
    BOOL            SetStartValue(long lStart);
    BOOL            SetLength(long lLength);
    BOOL            SetCurrentIndex(long lIdx);
    BOOL            SetConnectionObject(AOBJECTID oi);
    BOOL            SetMultiTask(BOOL fMultiTask);

    // methods for the actual looping
    inline void     StartLooping()            {m_fRunning = TRUE;}
    inline void     EndLooping()              {m_fRunning = FALSE;}
    inline BOOL     IsLooping()               {return m_fRunning;}
    inline long     NextValue()               {return (Length()>0) ? m_data->lCurrIdx++ : m_data->lCurrIdx--;}
    inline BOOL     DoNext()                  {return (Length()>0) ? (CurrentIndex()<(StartValue()+Length())) : (CurrentIndex()>(StartValue()+Length()));}
  };

////////////////////////////////////////////////////////////////////////////

#endif    // LOOP_H
