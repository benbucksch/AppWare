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
// FILE:          "edit_snd.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_SND_H
#define EDIT_SND_H

// includes ////////////////////////////////////////////////////////////////

#ifndef SOUND_H
#include "sound.h"
#endif

// EditSound class definition //////////////////////////////////////////////

class EditSound
  : public Sound

  {
  public:
                    EditSound(const Sound* pSound);
    virtual        ~EditSound();

    static BOOL ALMCALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL            InitDialog(HWND hwnd);
    void            Command(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
    void            DlgStateChanged();
    BOOL            Update();
    BOOL            RecordDlg();
    BOOL            LoadDlg();
    BOOL            BBPlay();
    BOOL            BBPause();
    BOOL            BBStop();

  protected:
    virtual void    Notify(UINT uNotifyCode);
    
  private:
    BOOL            m_fChanged;
    BOOL            m_fPaused;
    HWND            m_hwnd;
  };

////////////////////////////////////////////////////////////////////////////

#endif // #ifndef EDIT_SND_H
