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
// FILE:          "scroll.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the header file for the Scroll class (ALM Object).
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SCROLL_H
#define SCROLL_H

// required include files //////////////////////////////////////////////////

#ifndef WINDOWS_H
#include <windows.h>
  #ifndef WINDOWS_H
  #define WINDOWS_H
  #endif
#endif

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef ALMOBASE_H
#include "almobase.h"
#endif

// Scroll Bar constants ////////////////////////////////////////////////////

// scroll bar typeid
#define OTYPE_SCROLLBAR                   MAKETYPEID('S', 'b', 'a', 'r')

// scroll bar event ids
#define OSBAR_EVT_MAYBEPOSTSIGNALS        MAKEEVENTID('s', 'b', 'm', 'P')

// signal constants
const long  k_lSignalLineUp             = 0L;
const long  k_lSignalLineDown           = 1L;
const long  k_lSignalPageUp             = 2L;
const long  k_lSignalPageDown           = 3L;
const long  k_lSignalTop                = 4L;
const long  k_lSignalBottom             = 5L;
const long  k_lSignalThumbTrack         = 6L;
const long  k_lSignalThumbPosition      = 7L;
const long  k_lSignalActivated          = 8L;
const long  k_lSignalDeactivated        = 9L;
const long  k_lSignalChanged            = 10L;

// version constants
const short k_sVersion                  = 0x0100;

// data constants
const long  k_lSBarData                 = 0L;
const long  k_lNumDataBlocks            = 1L;

// Old Scroll Bar ALM file structure ///////////////////////////////////////

struct OldSBarData
  {
  short     sVer;
  long      lMin;
  long      lMax;
  long      lLine;
  long      lPage;
  long      lPos;
  short     fEnabled;
  short     fCanActivate;
  short     fVertical;
  OBJECTID  oiPosition;
  short     fDefaultSize;
  };

// Scroll Bar ALM file structure ///////////////////////////////////////////

struct SBarData
  {
  short     sVer;
  long      lMin;
  long      lMax;
  long      lLine;
  long      lPage;
  long      lPos;
  long      fStatus;
  OBJECTID  oiPosition;
  };

// Scroll class definition /////////////////////////////////////////////////

class Scroll :
  public ALMOBase

  {
  public:
                        Scroll(OBJECTID oi, BOOL fTemp);
    virtual            ~Scroll();

    // utility to validate correct construction of the object
    virtual BOOL        Valid() const           {return TRUE;}

    // version checking function to upgrade object
    static long         CheckVersion(OBJECTID oi);

    // initialize the object from Object Data
    virtual void        InitializeFromData();

    // AppWare administration function message handlers
    virtual long        Assign(const ALMOBase* pSrc);
    virtual long        BuildSignals(MEMBLOCKID mi) const;
    virtual long        Check() const;
    virtual long        Edit();
    virtual long        Export() const;
    virtual long        Import();
    virtual long        PostWrite();
    virtual long        PreWrite();
    static long         QueryProtocol(OBJECTID oi, pApQueryInfo pQI);
    virtual long        ValidateIDs(pAObjMessage pMsg);

    // accessor methods
    long                Minimum() const         {return m_lMin;}
    long                Maximum() const         {return m_lMax;}
    long                LineScroll() const      {return m_lLine;}
    long                PageScroll() const      {return m_lPage;}
    long                Position() const        {return m_lCurrPos;}
    OBJECTID            PositionObject() const  {return m_oiPosition;}
    BOOL                MinLTMax() const        {return m_lMin < m_lMax;}
    BOOL                CanActivate() const     {return (m_fStatus & focus)       ? TRUE  : FALSE;}
    BOOL                Enabled() const         {return (m_fStatus & enabled)     ? TRUE  : FALSE;}
    BOOL                DefaultSize() const     {return (m_fStatus & defaultSize) ? TRUE  : FALSE;}
    BOOL                Horizontal() const      {return (m_fStatus & vertical)    ? FALSE : TRUE;}
    BOOL                Vertical() const        {return (m_fStatus & vertical)    ? TRUE  : FALSE;}
    BOOL                TempObject() const      {return (m_fStatus & temporary)   ? TRUE  : FALSE;}

    BOOL                SetMinimum(long);
    BOOL                SetMaximum(long);
    BOOL                SetLineScroll(long);
    BOOL                SetPageScroll(long);
    BOOL                SetPosition(long);
    BOOL                SetPositionObject(OBJECTID);
    BOOL                SetCanActivate(BOOL);
    BOOL                SetEnabled(BOOL);
    BOOL                SetDefaultSize(BOOL);
    BOOL                SetHorizontal();
    BOOL                SetVertical();
    BOOL                SetVertical(BOOL);

  protected:
    enum    // values for the *m_fStatus* field
      {
      enabled     = 0x0001,   // currently enabled
      vertical    = 0x0002,   // vertical/horizontal scroll bar
      focus       = 0x0004,   // does the Scroll Bar get focus?
      defaultSize = 0x0008,   // is a default width/height SBar?
      temporary   = 0x1000    // is this a temporary object?
      };

    long                m_lMin;
    long                m_lMax;
    long                m_lLine;
    long                m_lPage;    
    long                m_fStatus;
    OBJECTID            m_oiPosition;
    long                m_lCurrPos;

  private:
    static BOOL ALMCALLBACK EditDlgProc(HWND, UINT, WPARAM, LPARAM);
    void                InitDlg(HWND);
    void                Command(HWND, int, HWND, UINT);
    BOOL                Update(HWND);
  };

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetMinimum(long) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetMinimum
  (
  long  lMin
  )

  {
  BOOL  fRet = (lMin != m_lMin);

  if (fRet)
    m_lMin = lMin;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetMaximum(long) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetMaximum
  (
  long  lMax
  )

  {
  BOOL  fRet = (lMax != m_lMax);

  if (fRet)
    m_lMax = lMax;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetLineScroll(long) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetLineScroll
  (
  long  lLine
  )

  {
  BOOL  fRet = (lLine != m_lLine);

  if (fRet)
    m_lLine = lLine;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetPageScroll(long) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetPageScroll
  (
  long  lPage
  )

  {
  BOOL  fRet = (lPage != m_lPage);

  if (fRet)
    m_lPage = lPage;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetPositionObject(OBJECTID) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetPositionObject
  (
  OBJECTID  oiPos
  )

  {
  BOOL  fRet = (oiPos != m_oiPosition);

  if (fRet)
    m_oiPosition = oiPos;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetCanActivate(BOOL) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetCanActivate
  (
  BOOL  fCanActivate
  )

  {
  BOOL  fRet = (fCanActivate != CanActivate());

  if (fRet)
    {
    if(fCanActivate)
      m_fStatus |= focus;
    else
      m_fStatus &= ~focus;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetEnabled(BOOL) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetEnabled
  (
  BOOL  fEnabled
  )

  {
  BOOL  fRet = (fEnabled != Enabled());

  if (fRet)
    {
    if(fEnabled)
      m_fStatus |= enabled;
    else
      m_fStatus &= ~enabled;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetDefaultSize(BOOL) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetDefaultSize
  (
  BOOL  fDefaultSize
  )

  {
  BOOL  fRet = (fDefaultSize != DefaultSize());

  if (fRet)
    {
    if(fDefaultSize)
      m_fStatus |= defaultSize;
    else
      m_fStatus &= ~defaultSize;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetHorizontal() -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetHorizontal
  (
  )

  {
  BOOL  fRet = !Horizontal();

  if (fRet)
    m_fStatus &= ~vertical;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetVertical() -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetVertical
  (
  )

  {
  BOOL  fRet = !Vertical();

  if (fRet)
    m_fStatus |= vertical;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetVertical(BOOL) -- inline
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Scroll::SetVertical
  (
  BOOL  fVertical
  )

  {
  BOOL  fRet = (fVertical != Vertical());

  if (fRet)
    {
    if(fVertical)
      m_fStatus |= vertical;
    else
      m_fStatus &= ~vertical;
    }

  return fRet;
  }

// PostSignalsEvt structure ////////////////////////////////////////////////

struct PostSignalsEvt
  {
  Scroll* pScroll;
  long    lPosition;
  long    lidSignal;
  };

// Abs macro ///////////////////////////////////////////////////////////////

template <class T> inline T Abs(const T& t)
  {
  return (t < 0) ? T(-t) : T(t);
  }

////////////////////////////////////////////////////////////////////////////

#endif //#ifndef SCROLL_H
