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
// FILE:          "sound.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_upsf.h>

#include "sound.h"
#include "resstrng.h"
#include "soundres.h"
#include "edit_snd.h"
#include "recsound.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Sound(OBJECTID, BOOL)
//
//  DESCRIPTION:  This is the ALM Sound object constructor.
//
////////////////////////////////////////////////////////////////////////////

Sound::Sound
  (
  OBJECTID  oi,
  BOOL      fTemp
  ) : MediaControlInterface(),
      m_oiSelf(oi),
      m_lSampleRate(k_lSR_22050_Hz),
      m_sBitsPerSample(k_sBPS_8),
      m_fFlags(fTemp ? temporary : 0),
      m_pEvt(NULL),
      m_fOpen(FALSE)

  {
  long  nData = AObjGetDataCount(m_oiSelf);

  // initialize the initial directory
  m_szInitDir[0]  = '\0';

  if(DATA_NUMDATABLOCKS == nData)
    InitializeFromData();
  else
    {
    // initialize the other fields
    m_szFile[0]     = '\0';

    // eliminate object data to free memory
    AObjSetDataCount(m_oiSelf, 0L);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Sound()
//
//  DESCRIPTION:  This is the C++ only sound object constructor. (NO
//                CORRESPONDING APPWARE OBJECT.)
//
////////////////////////////////////////////////////////////////////////////

Sound::Sound
  (
  ) : MediaControlInterface(),
      m_oiSelf(NULL),
      m_lSampleRate(k_lSR_22050_Hz),
      m_sBitsPerSample(k_sBPS_8),
      m_fFlags(0),
      m_pEvt(NULL),
      m_fOpen(FALSE)

  {
  m_szFile[0]     = '\0';
  m_szInitDir[0]  = '\0';
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Sound(const Sound&, BOOL) -- copy constructor
//
//  DESCRIPTION:  This is the copy constructor.
//
////////////////////////////////////////////////////////////////////////////

Sound::Sound
  (
  const Sound&  src,
  BOOL          fCopyFile     // = TRUE
  ) : MediaControlInterface(),
      m_oiSelf(src.m_oiSelf),
      m_lSampleRate(src.m_lSampleRate),
      m_sBitsPerSample(src.m_sBitsPerSample),
      m_fFlags(0),
      m_pEvt(NULL),
      m_fOpen(FALSE)

  {
  m_szInitDir[0]  = '\0';
  m_szFile[0]     = '\0';
  CopyObject(src, fCopyFile);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::~Sound()
//
//  DESCRIPTION:  This is the Sound object destructor.  This will unlock
//                and free memory as necessary.
//
////////////////////////////////////////////////////////////////////////////

Sound::~Sound
  (
  )

  {
  if(Playing())
    Stop();

  if(m_fOpen)
    Close(TRUE);

  // release any temporary files
  DeleteTmpFile();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::operator=(const Sound&)
//
//  DESCRIPTION:  This is the assignment operator.
//
////////////////////////////////////////////////////////////////////////////

Sound& Sound::operator=
  (
  const Sound&  src
  )

  {
  if(this != &src)
    CopyObject(src, TRUE);

  return *this;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::InitializeFromData()
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////


void Sound::InitializeFromData
  (
  )

  {
  I_ASSERT(AObjGetDataCount(m_oiSelf) == DATA_NUMDATABLOCKS);

  // set up the C++ object from the object data
  SoundResource*  pSR = (SoundResource*)AObjLockData(m_oiSelf, DATA_SOUNDRESOURCE);
  SoundInfo*      pSI = (SoundInfo*)AObjLockData(m_oiSelf, DATA_SOUNDINFO);

  // set the flags and other fields from the object data
  if(pSI)
    {
    if(pSI->flags & SoundInfo::resource)
      {
      SetInternal(TRUE);
      if(pSR && (pSR->lLength>0))
        {
        HFILE     hfile;
        OFSTRUCT  of;

        GetTmpSoundFileName(pSR->sType, m_szFile);
        hfile = OpenFile(m_szFile, &of, OF_CREATE | OF_WRITE);
        if(hfile != HFILE_ERROR)
          {
          if(pSR->lLength != _hwrite(hfile, pSR->data, pSR->lLength))
            {
            // fail gracefully..... ????
            _lclose(hfile);
            DeleteTmpFile();
            }
          else
            _lclose(hfile);
          }
        }
      }
    else
      SetInternal(FALSE);

    SetRecordInStereo((pSI->flags & SoundInfo::stereo) ? TRUE : FALSE);
    SetSamplingRate(pSI->lSampleRate);
    SetBitsPerSample(pSI->sBitsPerSample);

    if(External())
      {
      lstrcpy(m_szFile, pSI->szPath);
      UpdateInitDir();
      }
    }

  // unlock the object data
  if(pSI)
    AObjUnlockData(m_oiSelf, DATA_SOUNDINFO);
  if(pSR)
    AObjUnlockData(m_oiSelf, DATA_SOUNDRESOURCE);

  // eliminate object data to free memory
  AObjSetDataCount(m_oiSelf, 0L);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::GetNext(const Sound*) --- static
//
//  DESCRIPTION:  This will get the next sound object.  If there are not
//                any more sound objects, NULL is returned.
//
////////////////////////////////////////////////////////////////////////////

Sound* Sound::GetNext
  (
  const Sound*  pSound
  )

  {
  Sound*  pRet = (Sound*)LinkedList::GetNext(pSound);

  while(pRet && (pRet->Type() != OTYPE_SOUND))
    pRet = (Sound*)LinkedList::GetNext(pRet);

  return pRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::GetNext(const Sound*) --- static
//
//  DESCRIPTION:  This will get the previous sound object.  If there are not
//                any more sound objects, NULL is returned.
//
////////////////////////////////////////////////////////////////////////////

Sound* Sound::GetPrev
  (
  const Sound*  pSound
  )

  {
  Sound*  pRet = (Sound*)LinkedList::GetPrev(pSound);

  while(pRet && (pRet->Type() != OTYPE_SOUND))
    pRet = (Sound*)LinkedList::GetPrev(pRet);

  return pRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Valid() const
//
//  DESCRIPTION:  This method is called to make sure that the sound object
//                was correctly constructed.
//
////////////////////////////////////////////////////////////////////////////

BOOL Sound::Valid
  (
  ) const

  {
  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::CheckVersion(OBJECTID) --- static
//
//  DESCRIPTION:  This will check the sound object version.  If the version
//                is incorrect, it will upgrade the object or return A_ERROR.
//
////////////////////////////////////////////////////////////////////////////

long Sound::CheckVersion
  (
  OBJECTID  oiSound
  )

  {
  long  lRet    = A_OK;
  long  lCount  = AObjGetDataCount(oiSound);

  if(lCount < DATA_NUMDATABLOCKS)
    { // this is an ECO object -- strip it totally .......
    AObjSetDataCount(oiSound, 0L);
    lRet = A_WARNING;
    }
  else if(lCount > DATA_NUMDATABLOCKS)
    lRet = A_ERROR;
  else
    { // check the version fields to make sure it is the right version!
    short*  pVer;

    pVer = (short*)AObjLockData(oiSound, DATA_SOUNDINFO);
    if(!pVer || (*pVer != VERSION_SOUNDOBJECT))
      lRet = A_ERROR;
    AObjUnlockData(oiSound, DATA_SOUNDINFO);

    if(lRet != A_ERROR)
      {
      pVer = (short*)AObjLockData(oiSound, DATA_SOUNDRESOURCE);
      if(!pVer || (*pVer != VERSION_SOUNDOBJECT))
        lRet = A_ERROR;
      AObjUnlockData(oiSound, DATA_SOUNDRESOURCE);
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Assign(const Sound*)
//
//  DESCRIPTION:  This will assign one sound object to another.
//
////////////////////////////////////////////////////////////////////////////

long Sound::Assign
  (
  const Sound*  pSrcSound
  )

  {
  long  lRet = A_OK;

  // protect against self assignment
  if(pSrcSound != this)
    {
    OFSTRUCT  of;

    DeleteTmpFile();    // only deletes if internal
    SetSamplingRate(pSrcSound->SamplingRate());
    SetBitsPerSample(pSrcSound->BitsPerSample());
    SetInternal(pSrcSound->Internal());
    SetRecordInStereo(pSrcSound->RecordInStereo());
    lstrcpy(m_szInitDir, pSrcSound->m_szInitDir);

    if(Internal())
      {
      const char* pTmp      = strrchr(pSrcSound->m_szFile, '.');
      short       sType     = (!pTmp || (tolower(pTmp[1]) == 'w')) ? SoundResource::waveform : SoundResource::midi;
      HFILE       hfileDst;
      HFILE       hfileSrc  = OpenFile(pSrcSound->m_szFile, &of, OF_READ);

      if(hfileSrc != HFILE_ERROR)
        {
        GetTmpSoundFileName(sType, m_szFile);
        hfileDst = OpenFile(m_szFile, &of, OF_CREATE | OF_WRITE);
        if(hfileDst != HFILE_ERROR)
          {
          // copy the source file to the destination file
          HGLOBAL hgbl  = GlobalAlloc(GMEM_FIXED, 0x00007FFFL);
          void*   p     = GlobalLock(hgbl);
          long    lLen;

          if(p)
            {
            do
              {
              lLen = _hread(hfileSrc, p, 0x00007FFFL);

              if(lLen != _hwrite(hfileDst, p, lLen))
                {
                // fail gracefully..... ????
                _lclose(hfileDst);
                DeleteTmpFile();
                hfileDst = HFILE_ERROR;
                break;
                }
              }
              while(lLen == 0x00007FFFL);

            GlobalUnlock(hgbl);
            }

          GlobalFree(hgbl);

          if(hfileDst != HFILE_ERROR)
            _lclose(hfileDst);
          } // if(hfileDst != HFILE_ERROR)

        _lclose(hfileSrc);
        } // if(hfileSrc != HFILE_ERROR)
      } // if(Internal())
    else
      {
      lstrcpy(m_szFile, pSrcSound->m_szFile);
      UpdateInitDir();
      }
    } // if(pSrcSound != this)

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::BuildSignals(MEMBLOCKID) const
//
//  DESCRIPTION:  This will build the signal structure for the given
//                sound object.
//
////////////////////////////////////////////////////////////////////////////

long Sound::BuildSignals
  (
  MEMBLOCKID  mi
  ) const

  {
  /* Structure for storing the static signal table.
   */
  struct SignalInfo
    {
    /* This structure is used as a convenient way to store signal
     * names and ids, so they can all be looked at in one location.
     */
    long    lIdx;     /* signal id    */
    LPCSTR  szName;   /* signal name  */
    };

  // signal names
  static const ResString  k_rsSignalSoundDone(STR_SIGNAL_SOUNDDONE);
  static const ResString  k_rsSignalPlaybackFailed(STR_SIGNAL_PLAYBACKFAILED);

  static const SignalInfo signals[] =
    {
    /* This is a list of all the sound object signals.
     */
      {k_lSignalSoundDone,      k_rsSignalSoundDone.String()},
      {k_lSignalPlaybackFailed, k_rsSignalPlaybackFailed.String()}
    };

  // number of signals for the sound object
  static int        nNumSignals = sizeof(signals)/sizeof(signals[0]);

  // all sound objects have the same signals
  pAObjSignalRecord pSignals;
  long              lRet;

  if(AMemSetSize(mi, nNumSignals*sizeof(AObjSignalRecord)))
    {
    pSignals = (pAObjSignalRecord)AMemLock(mi);
    if(pSignals)
      {
      // copy the signals from the constant signal array (in "sound.h")
      for(int i=0; i<nNumSignals; i++)
        {
        pSignals[i].theID = signals[i].lIdx;
        lstrcpy(pSignals[i].theName, signals[i].szName);
        }

      lRet = A_OK;
      }
    else
      lRet = A_ERROR;
    }
  else
    lRet = A_ERROR;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Check() const
//
//  DESCRIPTION:  This will verify that the sound object is correctly
//                configured.
//
////////////////////////////////////////////////////////////////////////////

long Sound::Check
  (
  ) const

  {
  // the sound cannot be configured incorrectly
  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Edit()
//
//  DESCRIPTION:  This will cause the given sound object to launch the
//                editing dialog and start editing.
//
////////////////////////////////////////////////////////////////////////////

long Sound::Edit
  (
  )

  {
  long        lRet;
  EditSound*  pEdit = new EditSound(this);

  if(pEdit && DialogBoxParam(g_hinstDLL, ID(DLG_EDITSOUND), GetActiveWindow(), EditSound::DlgProc, (long)pEdit))
    {
    *this = *((Sound*)pEdit);
    lRet = A_MODIFIED;
    }
  else
    lRet = A_OK;

  delete pEdit;
  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Export() const
//
//  DESCRIPTION:  This will cause the sound to UPSF export itself.
//
////////////////////////////////////////////////////////////////////////////

long Sound::Export
  (
  ) const

  {
  ExportShort("VERSION", 0L, VERSION_SOUNDOBJECT);
  ExportBool("INTERNAL", 0L, Internal());
  ExportBool("STEREO",   0L, RecordInStereo());
  ExportShort("BPS",     0L, BitsPerSample());
  ExportLong("SMPLRATE", 0L, SamplingRate());

  if(Internal() && m_szFile[0])
    {
    long      lLen = 0;
    OFSTRUCT  of;
    HFILE     hfile = OpenFile(m_szFile, &of, OF_READ);

    if(hfile != HFILE_ERROR)
      {
      HGLOBAL hgblFile;

      // get the file length
      lLen = _llseek(hfile, 0, SEEK_END);
      _llseek(hfile, 0, SEEK_SET);

      hgblFile = GlobalAlloc(GMEM_FIXED, lLen);
      if(hgblFile)
        {
        void* pFile = GlobalLock(hgblFile);

        if(pFile)
          {
          const char* szTmp = strrchr(m_szFile, '.');

          // export the type of sound
          ExportBool("WAVEFORM", 0L, (!szTmp || tolower(szTmp[1]) == 'w'));

          // store the file data
          _hread(hfile, pFile, lLen);
          AUpsfExportField("SOUND", 0L, MAKEID('b', 'l', 'o', 'b'),
                            MAKEID('b', 'l', 'o', 'b'), pFile, lLen);

          GlobalUnlock(hgblFile);
          }

        GlobalFree(hgblFile);
        }

      _lclose(hfile);
      }
    }
  else
    {
    AUpsfExportField("FILENAME", 0L, AUPSF_NATV_STRING, AUPSF_NATV_STRING,
                     (void*)GetFilename(), lstrlen(m_szFile));
    }

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Import()
//
//  DESCRIPTION:  This will cause the sound to UPSF import itself.
//
////////////////////////////////////////////////////////////////////////////

long Sound::Import
  (
  )

  {
  short     s;
  BOOL      f;
  long      l;
  OFSTRUCT  of;

  DeleteTmpFile();

  if(ImportBool("INTERNAL", 0L, &f) == AUPSF_NOERROR)
    SetInternal(f);
  if(ImportBool("STEREO",   0L, &f) == AUPSF_NOERROR)
    SetRecordInStereo(f);
  if(ImportShort("BPS",     0L, &s) == AUPSF_NOERROR)
    SetBitsPerSample(s);
  if(ImportLong("SMPLRATE", 0L, &l) == AUPSF_NOERROR)
    SetSamplingRate(l);

  if(Internal())
    {
    l = 0L;
    AUpsfFieldInfo("SOUND", 0L, MAKEID('b', 'l', 'o', 'b'), NULL, &l);

    if(l > 0L)
      {
      HGLOBAL hgblFile = GlobalAlloc(GMEM_FIXED, l);

      if(hgblFile)
        {
        void* pFile = GlobalLock(hgblFile);
        if(pFile)
          {
          HFILE hfile;

          // get a temporary filename
          f = TRUE;
          ImportBool("WAVEFORM", 0L, &f);
          GetTmpSoundFileName(f ? SoundResource::waveform : SoundResource::midi, m_szFile);

          hfile = OpenFile(m_szFile, &of, OF_CREATE | OF_WRITE);

          if(hfile != HFILE_ERROR)
            {
            if(AUpsfImportField("SOUND", 0L, MAKEID('b', 'l', 'o', 'b'), pFile, l)
                        ==
               AUPSF_NOERROR)
              {
              if(l != _hwrite(hfile, pFile, l))
                {
                // fail gracefully..... ????
                _lclose(hfile);
                DeleteTmpFile();
                }
              else
                _lclose(hfile);
              }
            else
              _lclose(hfile);
            }

          GlobalUnlock(hgblFile);
          }

        GlobalFree(hgblFile);
        }
      }
    }
  else
    {
    AUpsfImportField("FILENAME", 0L, AUPSF_NATV_STRING, m_szFile, sizeof(m_szFile));
    UpdateInitDir();
    }

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::PostWrite()
//
//  DESCRIPTION:  This message tells the sound object to fix its object
//                data after being saved.
//
////////////////////////////////////////////////////////////////////////////

long Sound::PostWrite
  (
  )

  {
  // remove object data from memory
  AObjSetDataCount(m_oiSelf, 0L);

  return A_CONTINUE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::PreWrite()
//
//  DESCRIPTION:  This method converts runtime information to "file" data.
//                The SoundResource data will be created from the temporary
//                file.  This will avoid keeping the data in memory while
//                it is also saved as a file.
//
////////////////////////////////////////////////////////////////////////////

long Sound::PreWrite
  (
  )

  {
  SoundInfo*      pSI;
  SoundResource*  pSR;
  long            lLen;
  HFILE           hfile;
  OFSTRUCT        of;

  AObjSetDataCount(m_oiSelf, DATA_NUMDATABLOCKS);

  // set up the SoundInfo data block
  lLen = sizeof(SoundInfo) + (External() ? lstrlen(m_szFile) + 1 : 1);
  if(AObjResizeData(m_oiSelf, DATA_SOUNDINFO, lLen))
    {
    pSI = (SoundInfo*)AObjLockData(m_oiSelf, DATA_SOUNDINFO);
    
    if(pSI)
      {
      pSI->sVer           = VERSION_SOUNDOBJECT;
      pSI->lSampleRate    = m_lSampleRate;
      pSI->sBitsPerSample = m_sBitsPerSample;
      pSI->flags          = (Internal() ? SoundInfo::resource : 0)
                            | (RecordInStereo() ? SoundInfo::stereo : 0);

      if(External())
        lstrcpy(pSI->szPath, m_szFile);
      else
        pSI->szPath[0]    = '\0';        

      AObjUnlockData(m_oiSelf, DATA_SOUNDINFO);
      }
    }

  // determine the length of the resource
  lLen = 0;
  if(Internal())
    {
    hfile = OpenFile(m_szFile, &of, OF_READ);
    if(hfile != HFILE_ERROR)
      {
      lLen = _llseek(hfile, 0, SEEK_END);
      _llseek(hfile, 0, SEEK_SET);
      }
    }

  // set up the SoundResource data block
  if(AObjResizeData(m_oiSelf, DATA_SOUNDRESOURCE, sizeof(SoundResource) + lLen))
    {
    pSR = (SoundResource*)AObjLockData(m_oiSelf, DATA_SOUNDRESOURCE);
    
    if(pSR)
      {
      char* szTmp = strrchr(m_szFile, '.');

      pSR->sVer         = VERSION_SOUNDOBJECT;
      pSR->sType        = (!szTmp || tolower(szTmp[1]) == 'w') ? SoundResource::waveform : SoundResource::midi;
      pSR->lLength      = lLen;

      if(lLen > 0)
        _hread(hfile, pSR->data, lLen);
      }

    AObjUnlockData(m_oiSelf, DATA_SOUNDRESOURCE);
    }

  // close the resource file if necessary
  if(Internal() && (hfile != HFILE_ERROR))
    _lclose(hfile);

  return A_CONTINUE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::QueryProtocol(OBJECTID, pApQueryInfo) --- static
//
//  DESCRIPTION:  This method is called when AppWare needs to know if a
//                given object or type supports a particular protocol.
//
////////////////////////////////////////////////////////////////////////////

long Sound::QueryProtocol
  (
  OBJECTID      oiSound,
  pApQueryInfo  pQI
  )

  {
  long  lRet;

  I_UNUSED(pQI);

  if(oiSound) // object protocol support
    lRet = AP_NOTSUPPORTED;
  else        // type protocol support
    lRet = AP_NOTSUPPORTED;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::ReportRequiredFiles(pAObjMessage) --- static
//
//  DESCRIPTION:  This method is called when the AppWare bus needs to know
//                what files a particular type needs to run (be shipped
//                with).  The sound object may need to report a sound file.
//                This method will loop through all sound objects and report
//                files as necessary.
//
//  LIMITATIONS:  This routine will only work for DOS (8.3 filenames).
//
////////////////////////////////////////////////////////////////////////////

long Sound::ReportRequiredFiles
  (
  pAObjMessage  pMsg
  )

  {
  OBJECTID  oiSound;
  Sound*    pSound;
  long      lKey    = AOBJ_GETFIRSTKEY;

  while(NULL != (oiSound = AObjGetNextObject(OTYPE_SOUND, &lKey)))
    {
    pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    if(pSound && pSound->External())
      {
      // report a sound file
      LPCSTR  pFullPath = pSound->m_szFile;
      LPCSTR  pFile     = strrchr(pFullPath, '\\');

      // only give the filename (don't give the path)
      if(pFile)
        AObjAddRequiredFile(pMsg, (LPSTR)++pFile);
      else  // no '\' existed
        AObjAddRequiredFile(pMsg, (LPSTR)pFullPath);
      }
    }

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::ValidateIDs(pAObjMessage)
//
//  DESCRIPTION:  This method is called to notify an object that OBJECTIDs
//                may have changed.  The object needs to update any IDs
//                that it owns.
//
////////////////////////////////////////////////////////////////////////////

long Sound::ValidateIDs
  (
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pMsg);

  // The sound object holds no other object IDs.
  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SoundDriverPresent() --- static
//
//  DESCRIPTION:  Return 1: Waveform only
//                Return 2: No audio support
//                Return 3: Both MIDI and .WAV
//                Return 4: MIDI only
//
////////////////////////////////////////////////////////////////////////////

long Sound::SoundDriverPresent
  (
  )

  {
  UINT  nWav  = waveOutGetNumDevs();  // get number of waveform output devices
  UINT  nMidi = midiOutGetNumDevs();  // get number of midi output devices
  long  lRet;

  if((nWav > 0) && (nMidi > 0))
    lRet = 3L;
  else if(nWav > 0)
    lRet = 1L;
  else if(nMidi > 0)
    lRet = 4L;
  else
    lRet = 2L;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::StopSounds() --- static
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::StopSounds
  (
  )

  {
  Sound*  pSound = NULL;

  while( NULL != (pSound = GetNext(pSound)))
    {
    if(pSound->Playing())
      {
      if(pSound->Repeating())
        pSound->SetRepeating(FALSE);

      pSound->SetPlaying(FALSE);
      pSound->SetPlayingFromFile(FALSE);
      pSound->Stop(TRUE);
      pSound->Close(TRUE);
      AEvtPostSignalAtMark(pSound->m_oiSelf, k_lSignalSoundDone);
      }
    }

  return 1L;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::WaitUntilDone(OBJECTID, pAEvtInfo) --- static
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::WaitUntilDone
  (
  OBJECTID  oiSound,
  pAEvtInfo pEvent
  )

  {
  Sound*  pSound = NULL;
  long    lRet = 1L;

  if(oiSound)
    {
    pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    if(pSound && pSound->Playing() && !pSound->Repeating())
      {
      pSound->SetWaitUntilThisDone(TRUE);
      lRet = 0L;
      }

    if(0L == lRet)
      {
      pAEvtInfo pEvt = new AEvtInfo;

      if(pEvt)
        {
        *pEvt = *pEvent;
        pEvt->targetObject = (OBJECTID)OTYPE_SOUND;
        pEvt->when         = 0;
        pEvt->how          = AEVT_ATMARK;      

        if(!pSound->m_pEvt)
          pSound->m_pEvt = new EvtList;

        if(pSound->m_pEvt)
          pSound->m_pEvt->AddElement(pEvt);
        else
          delete pEvt;
        }
      }
    }
  else
    {
    while((pSound = GetNext(pSound)) != NULL)
      {
      if(pSound->Playing() && !pSound->Repeating())
        {
        pSound->SetWaitUntilAllDone(TRUE);
        lRet = 0L;
        }
      }

    if(0 == lRet)
      {
      pAEvtInfo pEvt      = new AEvtInfo;
      EvtList*  pEvtType  = (EvtList*)ATypeGetRuntimeInfo(OTYPE_SOUND);

      if(pEvt)
        {
        *pEvt = *pEvent;
        pEvt->targetObject = (OBJECTID)OTYPE_SOUND;
        pEvt->when         = 0;
        pEvt->how          = AEVT_ATMARK;      

        if(!pEvtType)
          {
          pEvtType = new EvtList;
          ATypeSetRuntimeInfo(OTYPE_SOUND, (long)pEvtType);
          }

        if(pEvtType)
          pEvtType->AddElement(pEvt);
        else
          delete pEvt;
        }
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Load(LPCSTR)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::Load
  (
  LPCSTR  szFilename
  )

  {
  long    lRet;
  OpenMCI open;
  open.lpstrElementName = szFilename;

  if(0 == Open(MCI_WAIT | MCI_OPEN_ELEMENT, &open))
    {
    Close(TRUE);
    SetInternal(FALSE);
    SetFilename(szFilename);
    lRet = 1L;
    }
  else
    lRet = 3L;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Play()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::Play
  (
  )

  {
  long  lRet = 2L;

  if(!Playing())
    {
    OpenMCI open;
    open.lpstrElementName = m_szFile;

    if(0 == Open(MCI_WAIT | MCI_OPEN_ELEMENT, &open))
      {
      if(0 == Seek(MCI_WAIT | MCI_SEEK_TO_START, 0))
        {
        PlayMCI play;

        SetPlaying(TRUE);
        if(0 == Play(MCI_NOTIFY, &play))
          lRet = 1;
        else  
          {
          SetPlaying(FALSE);
          Close(TRUE);
          }
        }
      else
        Close(TRUE);
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::PlayFromFile(LPCSTR)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::PlayFromFile
  (
  LPCSTR  szFilename
  )

  {
  long  lRet = 3L;

  if(!Playing())
    {
    OpenMCI open;
    open.lpstrElementName = szFilename;

    if(0 == Open(MCI_WAIT | MCI_OPEN_ELEMENT, &open))
      {
      PlayMCI play;

      if(0 == Seek(MCI_WAIT | MCI_SEEK_TO_START, 0))
        {
        SetPlaying(TRUE);
        SetPlayingFromFile(TRUE);
        if(0 == Play(MCI_NOTIFY, &play))
          lRet = 1;
        else
          {
          SetPlaying(FALSE);
          SetPlayingFromFile(FALSE);
          Close(TRUE);
          }
        }
      else
        Close(TRUE);
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::PlayRepeatedly()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::PlayRepeatedly
  (
  )

  {
  if(!Playing())
    {
    if(Play()==1) // if it successfully starts playing
      SetRepeating(TRUE);
    }
  else
    SetRepeating(TRUE);

  return 1L;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Record()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::Record
  (
  )

  {
  long        lRet;
  RecordSound rs(*this);

  SetInternal(TRUE);

  if(DialogBoxParam(g_hinstDLL, ID(DLG_RECORDSOUND), GetActiveWindow(), RecordSound::DlgProc, (LPARAM)&rs))
    {
    CopyObject(rs, TRUE);
    lRet = 1L;
    }
  else
    lRet = 2L;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::RecordToFile(LPCSTR) const
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::RecordToFile
  (
  LPCSTR  szFilename
  ) const

  {
  long        lRet;
  RecordSound rs(*this);

  rs.SetFilename(szFilename);

  if(DialogBoxParam(g_hinstDLL, ID(DLG_RECORDSOUND), GetActiveWindow(), RecordSound::DlgProc, (LPARAM)&rs))
    lRet = 1L;
  else
    lRet = 2L;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::SoundEmpty() const
//
//  DESCRIPTION:  return 1L:  Empty
//                return 2L:  Not empty
//
////////////////////////////////////////////////////////////////////////////

long Sound::SoundEmpty
  (
  ) const

  {
  return m_szFile[0] ? 1L : 2L;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Stop()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Sound::Stop
  (
  )

  {
  if(Repeating())
    SetRepeating(FALSE);

  if(Playing())
    {
    SetPlaying(FALSE);
    SetPlayingFromFile(FALSE);
    Stop(TRUE);
    Close(TRUE);
    AEvtPostSignalAtMark(m_oiSelf, k_lSignalSoundDone);
    }

  if(m_fOpen)
    Close(TRUE);

  return 1L;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Clear()
//
//  DESCRIPTION:  This will make the sound object empty (or cleared).
//
////////////////////////////////////////////////////////////////////////////

long Sound::Clear
  (
  )

  {
  if(External())
    SetFilename("");
  else
    SetSoundFromFile(NULL);

  return 1L;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::GetTmpSoundFileName(short, LPSTR)
//                                            --- protected, static
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Sound::GetTmpSoundFileName
  (
  short sFlags,
  LPSTR szFile
  )

  {
  char      szBuffer[MAXPATH];
  char*     szTmp;
  BOOL      fGoodName = FALSE;
  OFSTRUCT  of;

  while(!fGoodName)
    {
    GetTempFileName(0, "snd", 0, szBuffer);
    OpenFile(szBuffer, &of, OF_DELETE);

    szTmp = strrchr(szBuffer, '.');
    if(szTmp)
      lstrcpy(szTmp, (sFlags & SoundResource::waveform) ? ".wav" : ".mid");
    else
      lstrcat(szBuffer, (sFlags & SoundResource::waveform) ? ".wav" : ".mid");

    fGoodName = (OpenFile(szBuffer, &of, OF_EXIST) == HFILE_ERROR);
    }

  lstrcpy(szFile, szBuffer);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Close(BOOL) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Close
  (
  BOOL  fWait
  )

  {
  I_ASSERT(m_fOpen);

  DWORD dwRet = MediaControlInterface::Close(fWait);
  I_MCI_CHECK_RESULT(dwRet);
  if(0 == dwRet)
    m_fOpen = FALSE;

  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::GetDevCaps(BOOL, DWORD, DWORD&) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::GetDevCaps
  (
  BOOL    fWait,
  DWORD   dwItem,
  DWORD&  dwReturn
  )

  {
  DWORD dwRet = MediaControlInterface::GetDevCaps(fWait, dwItem, dwReturn);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Info(DWORD, LPSTR, DWORD) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Info
  (
  DWORD dwFlags,
  LPSTR szProdInfo,
  DWORD dwSize
  )

  {
  DWORD dwRet = MediaControlInterface::Info(dwFlags, szProdInfo, dwSize);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Open(DWORD, OpenMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Open
  (
  DWORD     dwFlags,
  OpenMCI*  pOpen
  )

  {
  DWORD dwRet;
  I_ASSERT(!m_fOpen);

  if(m_fOpen)
    Close(TRUE);
      
  dwRet = MediaControlInterface::Open(dwFlags, pOpen);
  I_MCI_CHECK_RESULT(dwRet);

  if(dwRet == 0)
    m_fOpen = TRUE;

  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Status(DWORD, DWORD, DWORD, DWORD&) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Status
  (
  DWORD   dwFlags,
  DWORD   dwItem,
  DWORD   dwTrack,
  DWORD&  dwReturn
  )

  {
  DWORD dwRet = MediaControlInterface::Status(dwFlags, dwItem, dwTrack, dwReturn);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Load(DWORD, LoadMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Load
  (
  DWORD     dwFlags,
  LoadMCI*  pLoad
  )

  {
  DWORD dwRet = MediaControlInterface::Load(dwFlags, pLoad);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Pause(BOOL) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Pause
  (
  BOOL  fWait
  )

  {
  DWORD dwRet = MediaControlInterface::Pause(fWait);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Play(DWORD, PlayMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Play
  (
  DWORD     dwFlags,
  PlayMCI*  pPlay
  )

  {
  DWORD dwRet = MediaControlInterface::Play(dwFlags, pPlay);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Record(DWORD, DWORD, DWORD) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Record
  (
  DWORD dwFlags,
  DWORD dwFrom,
  DWORD dwTo
  )

  {
  DWORD dwRet = MediaControlInterface::Record(dwFlags, dwFrom, dwTo);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Resume(BOOL) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Resume
  (
  BOOL  fWait
  )

  {
  DWORD dwRet = MediaControlInterface::Resume(fWait);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Save(DWORD, SaveMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Save
  (
  DWORD     dwFlags,
  SaveMCI*  pSave
  )

  {
  DWORD dwRet = MediaControlInterface::Save(dwFlags, pSave);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Seek(dwFlags, dwTo) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Seek
  (
  DWORD dwFlags,
  DWORD dwTo
  )

  {
  DWORD dwRet = MediaControlInterface::Seek(dwFlags, dwTo);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Set(DWORD, SetMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Set
  (
  DWORD   dwFlags,
  SetMCI* pSet
  )

  {
  DWORD dwRet = MediaControlInterface::Set(dwFlags, pSet);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Step(DWORD, GenericMCI*) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Step
  (
  DWORD       dwFlags,
  GenericMCI* pStep
  )

  {
  DWORD dwRet = MediaControlInterface::Step(dwFlags, pStep);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Stop(BOOL) --- protected
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD Sound::Stop
  (
  BOOL  fWait
  )

  {
  DWORD dwRet = MediaControlInterface::Stop(fWait);
  I_MCI_CHECK_RESULT(dwRet);
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::Notify(UINT) --- private
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Sound::Notify
  (
  UINT  uNotifyCode
  )

  {
  int i;

  I_ASSERT(uNotifyCode != MCI_NOTIFY_FAILURE);

  if(WaitUntilThisDone())
    {
     // the function call events are cached with the object
    SetWaitUntilThisDone(FALSE);
    for(i=0; i<m_pEvt->CountElements(); i++)
      {
      pAEvtInfo pEvt = m_pEvt->GetElement(i);
      AEvtPost(pEvt);
      delete pEvt;
      }

    delete m_pEvt;
    m_pEvt = NULL;
    }

  if(WaitUntilAllDone())
    {
    BOOL    fWait = FALSE;
    Sound*  pSound = NULL;
    SetWaitUntilAllDone(FALSE);

    while(!fWait && ((pSound = GetNext(pSound)) != NULL))
      fWait = pSound->WaitUntilAllDone();

    if(!fWait)
      {
      // the function call events are cached with the type
      EvtList*  pEvtList = (EvtList*)ATypeGetRuntimeInfo(OTYPE_SOUND);

      if(pEvtList)
        {
        for(i=0; i<pEvtList->CountElements(); i++)
          {
          pAEvtInfo pEvt = pEvtList->GetElement(i);
          AEvtPost(pEvt);
          delete pEvt;
          }

        delete pEvtList;
        ATypeSetRuntimeInfo(OTYPE_SOUND, (long)NULL);
        }
      }
    }

  if(Playing() && Repeating())
    {
    PlayMCI play;

    if(PlayingFromFile())
      {
      SetPlayingFromFile(FALSE);
      Close(TRUE);
      AEvtPostSignalAtMark(m_oiSelf, k_lSignalSoundDone);
      if(1 != Play())
        SetRepeating(FALSE);
      }
    else if(uNotifyCode != MCI_NOTIFY_FAILURE)
      {        
      Seek(MCI_WAIT | MCI_SEEK_TO_START, 0);
      Play(MCI_NOTIFY, &play);
      }
    else
      {
      SetPlaying(FALSE);
      SetPlayingFromFile(FALSE);
      Close(TRUE);
      AEvtPostSignalAtMark(m_oiSelf, k_lSignalPlaybackFailed);
      }
    }
  else if(Playing())
    {
    SetPlaying(FALSE);
    SetPlayingFromFile(FALSE);
    Close(TRUE);
    AEvtPostSignalAtMark(m_oiSelf, k_lSignalSoundDone);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::CopyObject(const Sound&, BOOL)  ---  private
//
//  DESCRIPTION:  This method is used to copy one sound into another sound.
//                The copy constructor and assingment operator calls this
//                method.
//
////////////////////////////////////////////////////////////////////////////


void Sound::CopyObject
  (
  const Sound&  src,
  BOOL          fCopyFile
  )

  {
  SetSamplingRate(src.SamplingRate());
  SetBitsPerSample(src.BitsPerSample());
  SetInternal(src.Internal());
  SetRecordInStereo(src.RecordInStereo());

  if(fCopyFile)
    {
    if(!Internal())
      SetFilename(src.GetFilename());
    else
      {
      SetSoundFromFile(src.GetFilename());
      lstrcpy(m_szInitDir, src.m_szInitDir);
      }
    }
  else
    SetFilename("");
  }

////////////////////////////////////////////////////////////////////////////

void Sound::SetSoundFromFile
  (
  LPCSTR  szFile
  )

  {
  OFSTRUCT    of;

  if(Internal() && szFile)
    { // copy the resource file to a new filename
    long        lLen;
    HFILE       hfileSrc;
    HFILE       hfileDst;
    const char* pType     = strrchr(szFile, '.');
    short       sType     = (!pType || (tolower(pType[1]) == 'w')) ? SoundResource::waveform : SoundResource::midi;

    hfileSrc = OpenFile(szFile, &of, OF_READ);

    if(hfileSrc != HFILE_ERROR)
      {
      // remove the old resource
      DeleteTmpFile();

      // setup the initial directory
      SetInternal(FALSE);
      lstrcpy(m_szFile, szFile);
      UpdateInitDir();
      SetInternal(TRUE);

      // add the new resource
      GetTmpSoundFileName(sType, m_szFile);
      hfileDst = OpenFile(m_szFile, &of, OF_CREATE | OF_WRITE);

      if(hfileDst != HFILE_ERROR)
        {
        // get the filelength
        lLen = _llseek(hfileSrc, 0, SEEK_END);
        _llseek(hfileSrc, 0, SEEK_SET);

        // copy the file
        if(lLen > 0)
          {
          HGLOBAL hgbl  = GlobalAlloc(GMEM_FIXED, 0x00007FFFL);
          void*   pFile = GlobalLock(hgbl);
          long    lSize = 0x00007FFFL;

          while(lLen > 0)
            {
            if(lSize > lLen)
              lSize = lLen;

            _lread(hfileSrc, pFile, (UINT)lSize);
            _lwrite(hfileDst, pFile, (UINT)lSize);

            lLen -= lSize;
            }
          }

        _lclose(hfileDst);
        } // if(hfileDst != HFILE_ERROR)
      else
        m_szFile[0] = '\0';

      _lclose(hfileSrc);
      } // if(hfileSrc != HFILE_ERROR)
    }
  else
    DeleteTmpFile();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Sound::UpdateInitDir --- private
//
//  DESCRIPTION:  This method will set the initial directory from the external
//                file.
//
////////////////////////////////////////////////////////////////////////////

void Sound::UpdateInitDir
  (
  )

  {
  if(External())
    {
    lstrcpy(m_szInitDir, m_szFile);
    if(m_szInitDir[0])
      {
      char* p;

      p = strrchr(m_szInitDir, '\\');
      if(p)
        {
        if((p != m_szInitDir) && (*(p-1) != ':'))
          *p = '\0';
        else
          *(p+1) = '\0';
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
