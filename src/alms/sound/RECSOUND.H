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
// FILE:          "recsound.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef RECSOUND_H
#define RECSOUND_H

// required includes ///////////////////////////////////////////////////////

#ifndef SOUND_H
#include "sound.h"
#endif

#define WAVE_FORMAT_11K       (WAVE_FORMAT_1M08 | WAVE_FORMAT_1M16 | \
                               WAVE_FORMAT_1S08 | WAVE_FORMAT_1S16)

#define WAVE_FORMAT_11K_08    (WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08)

#define WAVE_FORMAT_11K_16    (WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16)

#define WAVE_FORMAT_11K_M     (WAVE_FORMAT_1M08 | WAVE_FORMAT_1M16)

#define WAVE_FORMAT_11K_S     (WAVE_FORMAT_1S08 | WAVE_FORMAT_1S16)

#define WAVE_FORMAT_22K       (WAVE_FORMAT_2M08 | WAVE_FORMAT_2M16 | \
                               WAVE_FORMAT_2S08 | WAVE_FORMAT_2S16)

#define WAVE_FORMAT_22K_08    (WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08)

#define WAVE_FORMAT_22K_16    (WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16)

#define WAVE_FORMAT_22K_M     (WAVE_FORMAT_2M08 | WAVE_FORMAT_2M16)

#define WAVE_FORMAT_22K_S     (WAVE_FORMAT_2S08 | WAVE_FORMAT_2S16)

#define WAVE_FORMAT_44K       (WAVE_FORMAT_4M08 | WAVE_FORMAT_4M16 | \
                               WAVE_FORMAT_4S08 | WAVE_FORMAT_4S16)

#define WAVE_FORMAT_44K_08    (WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08)

#define WAVE_FORMAT_44K_16    (WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16)

#define WAVE_FORMAT_44K_M     (WAVE_FORMAT_4M08 | WAVE_FORMAT_4M16)

#define WAVE_FORMAT_44K_S     (WAVE_FORMAT_4S08 | WAVE_FORMAT_4S16)

// RecordSound class definition ////////////////////////////////////////////

class RecordSound
  : public Sound

  {
  public:
                            RecordSound(const Sound& srcSound);
                           ~RecordSound();

    BOOL                    SetFilename(LPCSTR szFile);

    static BOOL ALMCALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    void                    Command(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
    void                    GetRecordInfo(long& lSR, short& sBPS, BOOL& fStereo);
    void                    ValidateRecordState(long& lSR, short& sBPS, BOOL& fStereo);
    BOOL                    Check11K(long& lSR, short& sBPS, BOOL& fStereo);
    BOOL                    Check22K(long& lSR, short& sBPS, BOOL& fStereo);
    BOOL                    Check44K(long& lSR, short& sBPS, BOOL& fStereo);
    BOOL                    InitDialog(HWND hwnd);
    BOOL                    Update();
    BOOL                    BBRecord();
    BOOL                    BBPlay();
    BOOL                    BBPause();
    BOOL                    BBStop();

  protected:
    virtual void            Notify(UINT uNotifyCode);

  private:
    static WAVEINCAPS       sm_wic;
    static BOOL             sm_fWICInitialized;

    BOOL                    m_fPaused;
    HWND                    m_hwnd;
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef RECSOUND_H
