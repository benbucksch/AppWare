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
// FILE:          "mci.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is a generic class that can be used for all multi-
//                media objects (CD Player, Laserdisc Player, Animation,
//                Movie, MIDI, Waveform, etc.).
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef MCI_H
#define MCI_H

// includes ////////////////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef MMSYSTEM_H
#include <mmsystem.h>
  #ifndef MMSYSTEM_H
  #define MMSYSTEM_H
  #endif
#endif

#ifndef LINKLIST_H
#include "linklist.h"
#endif

// macros //////////////////////////////////////////////////////////////////

#ifndef AW_I_DEBUG
#define I_MCI_CHECK_RESULT(dwRet)
#else
#define I_MCI_CHECK_RESULT(dwRet)                                 \
  if(dwRet)                                                       \
    {                                                             \
    I_ASSERT(0);                                                  \
    char szMsg[MAXERRORLENGTH];                                   \
    if(mciGetErrorString(dwRet, szMsg, sizeof(szMsg)))            \
      ADbgPrint(ADBG_ERROR, OTYPE_SOUND, szMsg);                  \
    }
#endif

// structures //////////////////////////////////////////////////////////////

/* The following structures are the structures that are defined in "mmsystem.h"
 * except I have added inheritance and constructors.  With the inheritance
 * we are able to make the MCI class more generic because we don't care about the
 * type of message. (We can always use the GenericMCI class.)
 */

struct GenericMCI
  {
  DWORD dwCallback;

        GenericMCI(HWND hwnd = NULL) : dwCallback((DWORD)hwnd)  {}
  };

struct OpenMCI
  : GenericMCI
  {
  UINT    wDeviceID;
  UINT    wReserved0;
  LPCSTR  lpstrDeviceType;
  LPCSTR  lpstrElementName;
  LPCSTR  lpstrAlias;

          OpenMCI(HWND hwnd = NULL) : GenericMCI(hwnd), wDeviceID(0),
                                      wReserved0(0), lpstrDeviceType(0),
                                      lpstrElementName(0), lpstrAlias(0)  {}
  };

struct WaveOpenMCI
  : OpenMCI
  {
  DWORD   dwBufferSeconds;

          WaveOpenMCI(HWND hwnd = NULL) : OpenMCI(hwnd), dwBufferSeconds(0)  {}
  };

struct AnimOpenMCI
  : OpenMCI
  {
  DWORD   dwStyle;
  HWND    hWndParent;
  UINT    wReserved1;

          AnimOpenMCI(HWND hwnd = NULL) : OpenMCI(hwnd), hWndParent(0), wReserved1(0) {}
  };

struct OvlyOpenMCI
  : OpenMCI
  {
  DWORD   dwStyle;
  HWND    hWndParent;
  UINT    wReserved1;

          OvlyOpenMCI(HWND hwnd = NULL) : OpenMCI(hwnd), hWndParent(0), wReserved1(0) {}
  };

struct PlayMCI
  : GenericMCI
  {
  DWORD   dwFrom;
  DWORD   dwTo;

          PlayMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwFrom(0), dwTo(0)  {}
  };

struct VDPlayMCI
  : PlayMCI
  {
  DWORD   dwSpeed;

          VDPlayMCI(HWND hwnd = NULL) : PlayMCI(hwnd), dwSpeed(0) {}
  };

struct AnimPlayMCI
  : PlayMCI
  {
  DWORD   dwSpeed;

          AnimPlayMCI(HWND hwnd = NULL) : PlayMCI(hwnd), dwSpeed(0) {}
  };

struct SeekMCI
  : GenericMCI
  {
  DWORD   dwTo;

          SeekMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwTo(0) {}
  };

struct StatusMCI
  : GenericMCI
  {
  DWORD   dwReturn;
  DWORD   dwItem;
  DWORD   dwTrack;

          StatusMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwReturn(0),
                                        dwItem(0), dwTrack(0) {}
  };

struct InfoMCI
  : GenericMCI
  {
  LPSTR   lpstrReturn;
  DWORD   dwRetSize;

          InfoMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpstrReturn(0), dwRetSize(0)  {}
  };

struct GetDevCapsMCI
  : GenericMCI
  {
  DWORD   dwReturn;
  DWORD   dwItem;

          GetDevCapsMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwReturn(0), dwItem(0)  {}
  };

struct SysInfoMCI
  : GenericMCI
  {
  LPSTR   lpstrReturn;
  DWORD   dwRetSize;
  DWORD   dwNumber;
  UINT    wDeviceType;
  UINT    wReserved0;

          SysInfoMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpstrReturn(0),
                                         dwRetSize(0), dwNumber(0),
                                         wDeviceType(0), wReserved0(0)  {}
  };

struct SetMCI
  : GenericMCI
  {
  DWORD   dwTimeFormat;
  DWORD   dwAudio;

          SetMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwTimeFormat(0), dwAudio(0)  {}
  };

struct WaveSetMCI
  : SetMCI
  {
  UINT    wInput;
  UINT    wReserved0;
  UINT    wOutput;
  UINT    wReserved1;
  UINT    wFormatTag;
  UINT    wReserved2;
  UINT    nChannels;
  UINT    wReserved3;
  DWORD   nSamplesPerSec;
  DWORD   nAvgBytesPerSec;
  UINT    nBlockAlign;
  UINT    wReserved4;
  UINT    wBitsPerSample;
  UINT    wReserved5;

          WaveSetMCI(HWND hwnd = NULL) : SetMCI(hwnd), wInput(0), wReserved0(0),
                                         wOutput(0), wReserved1(0), wFormatTag(0),
                                         wReserved2(0), nChannels(0), wReserved3(0),
                                         nSamplesPerSec(0), nAvgBytesPerSec(0),
                                         nBlockAlign(0), wReserved4(0),
                                         wBitsPerSample(0), wReserved5(0) {}
  };

struct SeqSetMCI
  : SetMCI
  {
  DWORD   dwTempo;
  DWORD   dwPort;
  DWORD   dwSlave;
  DWORD   dwMaster;
  DWORD   dwOffset;

          SeqSetMCI(HWND hwnd = NULL) : SetMCI(hwnd), dwTempo(0), dwPort(0),
                                        dwSlave(0), dwMaster(0), dwOffset(0)  {}
  };

struct BreakMCI
  : GenericMCI
  {
  int     nVirtKey;
  UINT    wReserved0;
  HWND    hwndBreak;
  UINT    wReserved1;

          BreakMCI(HWND hwnd = NULL) : GenericMCI(hwnd), nVirtKey(0), wReserved0(0),
                                       hwndBreak(0), wReserved1(0)  {}
  };

struct SoundMCI
  : GenericMCI
  {
  LPCSTR  lpstrSoundName;

          SoundMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpstrSoundName(0)  {}
  };

struct SaveMCI
  : GenericMCI
  {
  LPCSTR  lpfilename;

          SaveMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpfilename(0) {}
  };

struct OvlySaveMCI
  : SaveMCI
  {
  RECT    rc;

          OvlySaveMCI(HWND hwnd = NULL) : SaveMCI(hwnd) {SetRectEmpty(&rc);}
  };

struct LoadMCI
  : GenericMCI
  {
  LPCSTR  lpfilename;

          LoadMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpfilename(0) {}
  };

struct OvlyLoadMCI
  : LoadMCI
  {
  RECT    rc;

          OvlyLoadMCI(HWND hwnd = NULL) : LoadMCI(hwnd) {SetRectEmpty(&rc);}
  };

struct RecordMCI
  : GenericMCI
  {
  DWORD   dwFrom;
  DWORD   dwTo;

          RecordMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwFrom(0), dwTo(0)  {}
  };

struct VDStepMCI
  : GenericMCI
  {
  DWORD   dwFrames;

          VDStepMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwFrames(0) {}
  };

struct AnimStepMCI
  : GenericMCI
  {
  DWORD   dwFrames;

          AnimStepMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwFrames(0) {}
  };

struct VDEscapeMCI
  : GenericMCI
  {
  LPCSTR  lpstrCommand;

          VDEscapeMCI(HWND hwnd = NULL) : GenericMCI(hwnd), lpstrCommand(0) {}
  };

struct WaveDeleteMCI
  : GenericMCI
  {
  DWORD   dwFrom;
  DWORD   dwTo;

          WaveDeleteMCI(HWND hwnd = NULL) : GenericMCI(hwnd), dwFrom(0), dwTo(0)  {}
  };

struct AnimWindowMCI
  : GenericMCI
  {
  HWND    hWnd;
  UINT    wReserved1;
  UINT    nCmdShow;
  UINT    wReserved2;
  LPCSTR  lpstrText;

          AnimWindowMCI(HWND hwnd = NULL) : GenericMCI(hwnd), hWnd(0), wReserved1(0),
                                            nCmdShow(0), wReserved2(0), lpstrText(0)  {}
  };

struct OvlyWindowMCI
  : GenericMCI
  {
  HWND    hWnd;
  UINT    wReserved1;
  UINT    nCmdShow;
  UINT    wReserved2;
  LPCSTR  lpstrText;

          OvlyWindowMCI(HWND hwnd = NULL) : GenericMCI(hwnd), hWnd(0), wReserved1(0),
                                            nCmdShow(0), wReserved2(0), lpstrText(0)  {}
  };

struct AnimRectMCI
  : GenericMCI
  {
#ifdef MCI_USE_OFFEXT
  POINT   ptOffset;
  POINT   ptExtent;

          AnimRectMCI(HWND hwnd = NULL) : GenericMCI(hwnd), ptOffset({0,0}), ptExtent({0,0})  {}
#else   /* ifdef MCI_USE_OFFEXT */
  RECT    rc;

          AnimRectMCI(HWND hwnd = NULL) : GenericMCI(hwnd)  {SetRectEmpty(&rc);}
#endif  /* ifdef MCI_USE_OFFEXT */
  };

struct OvlyRectMCI
  : GenericMCI
  {
#ifdef MCI_USE_OFFEXT
  POINT   ptOffset;
  POINT   ptExtent;

          OvlyRectMCI(HWND hwnd = NULL) : GenericMCI(hwnd), ptOffset({0,0}), ptExtent({0,0})  {}
#else   /* ifdef MCI_USE_OFFEXT */
  RECT    rc;

          OvlyRectMCI(HWND hwnd = NULL) : GenericMCI(hwnd)  {SetRectEmpty(&rc);}
#endif  /* ifdef MCI_USE_OFFEXT */
  };

struct AnimUpdateMCI
  : GenericMCI
  {
  RECT    rc;
  HDC     hDC;

          AnimUpdateMCI(HWND hwnd = NULL) : GenericMCI(hwnd), hDC(0)  {SetRectEmpty(&rc);}
  };

////////////////////////////////////////////////////////////////////////////

class MediaControlInterface
  : public LinkedList

  {
  public:
                    MediaControlInterface();
    virtual        ~MediaControlInterface();

    // MCI system commands (handled by MCI system)
    DWORD           Break     (DWORD dwFlags, int nVirtKey, HWND hwndBreak);
    DWORD           SysInfo   (DWORD dwFlags, SysInfoMCI* pSysInfo);

    // MCI required commands (handled by all MCI devices)
    virtual DWORD   Close     (BOOL fWait);
    virtual DWORD   GetDevCaps(BOOL fWait, DWORD dwItem, DWORD& dwReturn);
    virtual DWORD   Info      (DWORD dwFlags, LPSTR szProdInfo, DWORD dwSize);
    virtual DWORD   Open      (DWORD dwFlags, OpenMCI* pOpen);
    virtual DWORD   Status    (DWORD dwFlags, DWORD dwItem, DWORD dwTrack, DWORD& dwReturn);

    // MCI basic commands (recognized by all MCI devices)
    virtual DWORD   Load      (DWORD dwFlags, LoadMCI* pLoad);
    virtual DWORD   Pause     (BOOL fWait);
    virtual DWORD   Play      (DWORD dwFlags, PlayMCI* pPlay);
    virtual DWORD   Record    (DWORD dwFlags, DWORD dwFrom, DWORD dwTo);
    virtual DWORD   Resume    (BOOL fWait);
    virtual DWORD   Save      (DWORD dwFlags, SaveMCI* pSave);
    virtual DWORD   Seek      (DWORD dwFlags, DWORD dwTo);
    virtual DWORD   Set       (DWORD dwFlags, SetMCI* pSet);
    virtual DWORD   Step      (DWORD dwFlags, GenericMCI* pStep);
    virtual DWORD   Stop      (BOOL fWait);

    // MCI optional commands
    virtual DWORD   Copy      (BOOL fWait);
    virtual DWORD   Cue       (BOOL fWait);
    virtual DWORD   Cut       (BOOL fWait);
    virtual DWORD   Paste     (BOOL fWait);

  protected:
    /* The following method is called whenevere one of the above MCI commands
     * finish (or has the status change) when the fWait flag is FALSE.  The
     * notification code can be one of the following values (see Windows 3.1
     * Mulitmedia Reference):
     *        MCI_NOTIFY_ABORTED
     *        MCI_NOTIFY_SUCCESSFUL
     *        MCI_NOTIFY_SUPERSEDED
     *        MCI_NOTIFY_FAILURE
     */
    virtual void    Notify    (UINT uNotifyCode);

    /* The following method determines the type of the current MCI object
     * (Sound, Movie, CD, LD, etc.).  This should only be used to iterate
     * through all objects of a particular type.
     */
    virtual TYPEID  Type      () const = 0;

    /* The following method is called when a derived class needs to get
     * the window handle for the callback message (when notify is requested).
     */
    HWND            CallbackWnd() const;

    DWORD           m_dwLastNtfyCmd;  // last command that requests notify
    UINT            m_dwDeviceID;     // device ID for the current MCI object

  private:
    /* This is a generic command handler for the above MCI methods.
     */
    DWORD CallGenericCommand(BOOL fWait, DWORD dwCmd);
    DWORD CallCommand(DWORD dwFlags, DWORD dwCmd, GenericMCI* pGeneric);

    /* Window that recieves MM_MCINOTIFY messages.
     * This message is sent whenever the fWait flag is FALSE.  The
     * Window is notified of any status change.  (See Windows 3.1
     * Multimedia Reference.)
     */
    static HWND     sm_hwndCallback;

    /* The following are methods that manage the callback window and
     * handle the messages and notifications.
     */
    static MediaControlInterface* MapDeviceIDToMCI(UINT dwID);
    static long ALMCALLBACK       CallbackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void            DestroyCallbackWindow();
    void            CreateCallbackWindow();
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef MCI_H
