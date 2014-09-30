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
// FILE:          "sound.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Sound ALM constants (typeid, eventids, signals, etc.).
//                This file also contains the class definition for the Sound
//                class, which is the C++ interface to the Sound ALM.  The
//                Sound class is a MediaControlInterface (MCI) object, that
//                overrides the methods of interest.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_H
#define SOUND_H

// includes ////////////////////////////////////////////////////////////////

#ifndef WINDOWS_H
#include <windows.h>
  #ifndef WINDOWS_H
  #define WINDOWS_H
  #endif
#endif

#ifndef MCI_H
#include "mci.h"
#endif

#ifndef EVTLIST_H
#include "evtlist.h"
#endif

#ifndef STRING_H
#include <string.h>
  #ifndef STRING_H
  #define STRING_H
  #endif
#endif

#ifndef DIR_H
#include <dir.h>
  #ifndef DIR_H
  #define DIR_H
  #endif
#endif

// defines /////////////////////////////////////////////////////////////////

// type id
#define OTYPE_SOUND                   MAKETYPEID('S','o','n','d')

// event ids
#define OSOUND_EVT_REPEAT             MAKEEVENTID('p','l','s','t')
#define OSOUND_EVT_BAD                MAKEEVENTID(' ',' ',' ',' ')

// data
#define DATA_SOUNDINFO                0L
#define DATA_SOUNDRESOURCE            1L
#define DATA_NUMDATABLOCKS            2L

// version
#define VERSION_SOUNDOBJECT           0x0101

// signals
const long  k_lSignalSoundDone      = 0L;
const long  k_lSignalPlaybackFailed = 1L;

// SoundInfo structure definition //////////////////////////////////////////

// Sampling rates
const long  k_lSR_11025_Hz          = 11025L;
const long  k_lSR_22050_Hz          = 22050L;
const long  k_lSR_44100_Hz          = 44100L;

// Bits per sample
const short k_sBPS_4                = 4;
const short k_sBPS_8                = 8;
const short k_sBPS_16               = 16;

struct SoundInfo      /* Object data:  DATA_SOUNDINFO */
  {
  enum    // values for the *flags* field
    {
    resource  = 0x0001, // specifies if the sound is object data or a file
    stereo    = 0x0002  // specifies if the sound will be recorded in stereo or mono
    };

  short sVer;           /* current sound object version */
  long  lSampleRate;    /* default sampling rate value for recording */
  short sBitsPerSample; /* default number of bits for each sample */
  short flags;          /* combination of above enum flags */
  char  szPath[];       /* path to the sound file */
  };

// SoundResource structure definition //////////////////////////////////////

/* The SoundResource object data is something that will not be in memory
 * most of the time.  It is structure that is used mainly for saving the
 * sound.  When the Sound C++ object is created, the SoundResource data will
 * be copied into a temporary file, so the MCI commands will work (because
 * they need files).
 */

struct SoundResource  /* Object data:  DATA_SOUNDRESOURCE */
  {
  enum
    {
    waveform  = 1,  // the resource is a .WAV file
    midi      = 2   // the resource is a .MID/.RMI file
    };

  short sVer;         /* current sound resource version */
  short sType;        /* sound resource type -- see above enum */
  long  lLength;      /* length of the file */
  BYTE  data[];       /* copy of the data file (file stored in memory block) */
  };

// Sound class definition //////////////////////////////////////////////////

class Sound
  : public MediaControlInterface

  {
  public:
                    Sound(OBJECTID oi, BOOL fTemp); // used if associated AppWare object
                    Sound();                        // used if only C++ object
    virtual        ~Sound();
                    Sound(const Sound&, BOOL = TRUE);
    Sound&          operator=(const Sound&);

    // general initialization routine to initialize from object data
    void            InitializeFromData();

    // miscellaneous methods to view data
    OBJECTID        ObjectID() const        {return m_oiSelf;}
    BOOL            TempObject() const      {return (m_fFlags & temporary)  ? TRUE  : FALSE;}
    BOOL            Repeating() const       {return (m_fFlags & repeat)     ? TRUE  : FALSE;}
    BOOL            Playing() const         {return (m_fFlags & playing)    ? TRUE  : FALSE;}
    BOOL            PlayingFromFile() const {return (m_fFlags & play_file)  ? TRUE  : FALSE;}
    BOOL            Internal() const        {return (m_fFlags & resource)   ? TRUE  : FALSE;}
    BOOL            External() const        {return (m_fFlags & resource)   ? FALSE : TRUE;}
    BOOL            RecordInStereo() const  {return (m_fFlags & stereo)     ? TRUE  : FALSE;}
    BOOL            RecordInMono() const    {return (m_fFlags & stereo)     ? FALSE : TRUE;}
    long            SamplingRate() const    {return m_lSampleRate;}
    short           BitsPerSample() const   {return m_sBitsPerSample;}
    LPCSTR          GetFilename() const     {return m_szFile;}

    // methods to browse through all sound objects
    static Sound*   GetNext(const Sound* pSound);
    static Sound*   GetPrev(const Sound* pSound);

    // utility to validate correct construction of the object
    BOOL            Valid() const;

    // version checking function to upgrade object
    static long     CheckVersion(OBJECTID oiSound);

    // method to get the startup directory for the Open/SaveFile dialogs
    LPSTR           GetInitDir()            {return m_szInitDir[0] ? m_szInitDir : NULL;}

    // AppWare administration function message handlers
    long            Assign(const Sound* pSrcSound);
    long            BuildSignals(MEMBLOCKID mi) const;
    long            Check() const;
    long            Edit();
    long            Export() const;
    long            Import();
    long            PostWrite();
    long            PreWrite();
    static long     QueryProtocol(OBJECTID oiSound, pApQueryInfo pQI);
    static long     ReportRequiredFiles(pAObjMessage pMsg);
    long            ValidateIDs(pAObjMessage pMsg);

    // AppWare sound object functions
    static long     SoundDriverPresent();             /* Reports if a sound card/driver is present */
    static long     StopSounds();                     /* Stops all playing sounds */
    static long     WaitUntilDone(OBJECTID oiSound, pAEvtInfo pEvent);

    long            Load(LPCSTR szFilename);
    long            Play();
    long            PlayFromFile(LPCSTR szFilename);
    long            PlayRepeatedly();
    long            Record();
    long            RecordToFile(LPCSTR szFilename) const;
    long            SoundEmpty() const;
    long            Stop();
    long            Clear();

    // methods to set data -- inline methods (see below)
    BOOL            SetSamplingRate(long lSR);
    BOOL            SetBitsPerSample(short sBPS);
    BOOL            SetInternal(BOOL fSet);
    BOOL            SetRecordInStereo(BOOL fSet);
    BOOL            SetFilename(LPCSTR szFile);

    // methods for temporary files
    static void     GetTmpSoundFileName(short sFlags, LPSTR szFile);
    void            DeleteTmpFile();

    // overridden MCI methods
    virtual DWORD   Close     (BOOL fWait);
    virtual DWORD   GetDevCaps(BOOL fWait, DWORD dwItem, DWORD& dwReturn);
    virtual DWORD   Info      (DWORD dwFlags, LPSTR szProdInfo, DWORD dwSize);
    virtual DWORD   Open      (DWORD dwFlags, OpenMCI* pOpen);
    virtual DWORD   Status    (DWORD dwFlags, DWORD dwItem, DWORD dwTrack, DWORD& dwReturn);

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

  protected:
    // Method to tell the object an action has completed (or something).
    virtual void    Notify    (UINT uNotifyCode);

    // method to copy the sound file
    void            SetSoundFromFile(LPCSTR szFile);

    // methods to copy objects
    void            CopyObject(const Sound& src, BOOL fCopyFile);

  private:
    // methods to access data
    BOOL            WaitUntilThisDone() const {return (m_fFlags & wait)     ? TRUE : FALSE;}
    BOOL            WaitUntilAllDone() const  {return (m_fFlags & wait_all) ? TRUE : FALSE;}

    // methods to set data -- inline methods (see below)
    BOOL            SetRepeating(BOOL fRepeating);
    BOOL            SetPlaying(BOOL fPlaying);
    BOOL            SetPlayingFromFile(BOOL fPlaying);
    BOOL            SetWaitUntilThisDone(BOOL fWait);
    BOOL            SetWaitUntilAllDone(BOOL fWait);

    // Method to give the type of this MCI object
    virtual TYPEID  Type      () const    {return OTYPE_SOUND;}

    // misc private methods
    void            UpdateInitDir();

    /* These fields are used when this C++ object has a matching AppWare
     * object.  (When this C++ object has NO matching AppWare object,
     * these fields are 0 and not used.)
     */
    OBJECTID        m_oiSelf;             // ALM object id

  protected:
    BOOL            m_fOpen;              // flag specifing the device is open
    char            m_szFile[MAXPATH];    // filename of the current object
    char            m_szInitDir[MAXPATH]; // initial directory for dialogs

  private:
    long            m_lSampleRate;        // sampling rate for this object
    short           m_sBitsPerSample;     // number of bits per sample

    /* This flags field keeps track of wheter or not the sound is playing,
     * being repeatedly played, a temporary (run-time) AppWare object, etc.
     */
    enum
      {
      temporary   = 0x0001,   // this is a temporary AppWare object
      repeat      = 0x0002,   // the sound will start playing after it stops
      playing     = 0x0004,   // the sound is currently playing
      play_file   = 0x0008,   // the sound is playing from a file
      wait        = 0x0010,   // we are waiting for this one to finish playing
      wait_all    = 0x0020,   // we are waiting for all sounds to finish playing
      resource    = 0x0100,   // the sound is a resource (not a file)
      stereo      = 0x0200    // record the sound in stereo (not mono)
      };
    short           m_fFlags; // status flags -- combination of above enum.
    EvtList*        m_pEvt;   // list of stored events
  };

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetRepeating --- private
//
//  DESCRIPTION:  This method will set/clear the flag that says that the
//                sound is playing repeatedly.  If data changes, TRUE is
//                returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetRepeating
  (
  BOOL  fRepeating
  )

  {
  BOOL  fRet = ((fRepeating && !Repeating()) || (!fRepeating && Repeating()));

  if(fRet)
    {
    if(fRepeating)
      m_fFlags |= repeat;
    else
      m_fFlags &= ~repeat;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetPlaying --- private
//
//  DESCRIPTION:  This method will set/clear the flag that says that the
//                sound is playing.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetPlaying
  (
  BOOL  fPlaying
  )

  {
  BOOL  fRet = ((fPlaying && !Playing()) || (!fPlaying && Playing()));

  if(fRet)
    {
    if(fPlaying)
      m_fFlags |= playing;
    else
      m_fFlags &= ~playing;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetPlayingFromFile --- private
//
//  DESCRIPTION:  This method will set/clear the flag that says that the
//                sound is playing.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetPlayingFromFile
  (
  BOOL  fPlaying
  )

  {
  BOOL  fRet = ((fPlaying && !PlayingFromFile()) || (!fPlaying && PlayingFromFile()));

  if(fRet)
    {
    if(fPlaying)
      m_fFlags |= play_file;
    else
      m_fFlags &= ~play_file;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetWaitUntilThisDone --- private
//
//  DESCRIPTION:  This method will set/clear the flag that says that the
//                system needs to be notified when this sound is finished
//                playing.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetWaitUntilThisDone
  (
  BOOL  fWait
  )

  {
  BOOL  fRet = ((fWait && !WaitUntilThisDone()) || (!fWait && WaitUntilThisDone()));

  if(fRet)
    {
    if(fWait)
      m_fFlags |= wait;
    else
      m_fFlags &= ~wait;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetWaitUntilAllDone --- private
//
//  DESCRIPTION:  This method will set/clear the flag that says that the
//                system needs to be notified when all sounds are finished
//                playing.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetWaitUntilAllDone
  (
  BOOL  fWait
  )

  {
  BOOL  fRet = ((fWait && !WaitUntilAllDone()) || (!fWait && WaitUntilAllDone()));

  if(fRet)
    {
    if(fWait)
      m_fFlags |= wait_all;
    else
      m_fFlags &= ~wait_all;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetSamplingRate --- protected
//
//  DESCRIPTION:  This method will set the default sampling rate for
//                recording.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetSamplingRate
  (
  long  lSR
  )

  {
  BOOL fRet = (lSR != m_lSampleRate);

  if(fRet)
    {
    if((lSR != k_lSR_11025_Hz)
          &&
       (lSR != k_lSR_22050_Hz) 
          && 
       (lSR != k_lSR_44100_Hz))
      {
      lSR = k_lSR_11025_Hz;
      }

    m_lSampleRate = lSR;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetBitsPerSample --- protected
//
//  DESCRIPTION:  This method will set the default bits per sample for
//                recording.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetBitsPerSample
  (
  short sBPS
  )

  {
  BOOL fRet = (sBPS != m_sBitsPerSample);

  if(fRet)
    {
    if((sBPS != k_sBPS_4)
          &&
       (sBPS != k_sBPS_8)
          &&
       (sBPS != k_sBPS_16))
      {
      sBPS = k_sBPS_8;
      }

    m_sBitsPerSample = sBPS;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetResource --- protected
//
//  DESCRIPTION:  This will change the object to actually own a sound,
//                rather than have a filename.  If data changes, TRUE is
//                returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetInternal
  (
  BOOL  fSet
  )

  {
  BOOL  fRet = ((fSet && !Internal()) || (!fSet && Internal()));

  if(fRet)
    {
    if(fSet)
      {
      m_fFlags |= resource;
      m_szFile[0] = '\0';
      }
    else
      {
      DeleteTmpFile();
      m_fFlags &= ~resource;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetRecordInStereo --- protected
//
//  DESCRIPTION:  This method will set the flag which will cause recording
//                to record in stereo.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetRecordInStereo
  (
  BOOL  fSet
  )

  {
  BOOL  fRet = ((fSet && !RecordInStereo()) || (!fSet && RecordInStereo()));

  if(fRet)
    {
    if(fSet)
      m_fFlags |= stereo;
    else
      m_fFlags &= ~stereo;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SetFilename --- protected
//
//  DESCRIPTION:  This method will set the filename of the current sound
//                object.  If data changes, TRUE is returned.
//
////////////////////////////////////////////////////////////////////////////

inline BOOL Sound::SetFilename
  (
  LPCSTR  szFile
  )

  {
  BOOL  fRet = External() && lstrcmpi(szFile, m_szFile);

  if(fRet)
    {
    if(m_fOpen)
      Close(TRUE);

    lstrcpy(m_szFile, szFile);
    UpdateInitDir();
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::DeleteTmpFile --- private
//
//  DESCRIPTION:  This method should be called to delete an temporary file.
//
////////////////////////////////////////////////////////////////////////////

inline void Sound::DeleteTmpFile
  (
  )

  {
  if(Internal() && m_szFile[0])
    {
    if(Playing())
      Stop();

    if(m_fOpen)
      Close(TRUE);

    OFSTRUCT  of;
    #ifdef AW_I_DEBUG
    HFILE     hfile =
    #endif

    OpenFile(m_szFile, &of, OF_DELETE);
    m_szFile[0] = '\0';

    I_ASSERT(hfile != HFILE_ERROR);
    }
  }

////////////////////////////////////////////////////////////////////////////

#endif  //#ifndef SOUND_H
