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
// FILE:          "snd_main.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This file contains LibMain/*!!PORT!! Replace with DllEntryPoint */, WEP/*!!PORT!! Replace with DllEntryPoint */, AppWare Administration
//                function, and other AppWare entry points for the Sound ALM.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <dir.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_text.h>

#if !defined(__FLAT__)
#endif

#include "sound.h"
#include "opendlg.h"
#include "resstrng.h"

// Global variables ////////////////////////////////////////////////////////

HINSTANCE   g_hinstDLL;

#ifdef __BORLANDC__
extern int  _WinAllocFlag;
#endif

#if !defined(__FLAT__)
// ****************************************************************
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ****************************************************************

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  if (p)
    MemFree(p);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     LibMain/*!!PORT!! Replace with DllEntryPoint */
//
//  DESCRIPTION:  This is the module that gets called when the DLL is first
//                loaded into memory.  All global initialization should be
//                done here.
//
////////////////////////////////////////////////////////////////////////////

extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */
  (
  HINSTANCE hinstDLL,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinstDLL = hinstDLL;

  #ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
  #endif

  if(wHeapSize>0)
    UnlockData(0);

  return 1;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     WEP/*!!PORT!! Replace with DllEntryPoint */
//
//  DESCRIPTION:  This is the Windows Exit Procedure (WEP/*!!PORT!! Replace with DllEntryPoint */).  This is called
//                when the DLL is being removed from memory.  All handles
//                and resources should be released after this is called.
//
////////////////////////////////////////////////////////////////////////////

int CALLBACK /*!!PORT!! was _far_ _pascal_*/ WEP/*!!PORT!! Replace with DllEntryPoint */
  (
  int nParameter
  )

  {
  I_UNUSED(nParameter);

  return 1;
  }

#else

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     DllEntryPoint
//
//  DESCRIPTION:  This is the module that gets called when the DLL is first
//                loaded into memory or freed from memory.  All global
//                initialization should be de-allocation should be done here.
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllEntryPoint
  (
  HINSTANCE hinstDLL,
  DWORD     fdwReason,
  LPVOID    pReserved
  )

  {
  I_UNUSED(pReserved);

  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
      g_hinstDLL = hinstDLL;
      break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_DETACH:
      break;

    default:
      I_ASSERT(0);
      break;
    }

  return TRUE;
  }

#endif
////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     SoundAdmn
//
//  DESCRIPTION:  This is the administration function for the Sound object.
//                This is called with various messages and requests.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMCALLBACK SoundAdmn
  (
  AOBJECTID      oiSound,
  pAObjMessage  pMsg
  )

  {
  Sound*    pSound;
  long      lRet;

  switch(pMsg->message1)
    {
    case AOBJ_AWAKENED:   // object receives this message after being read from disk
      pMsg->message4 = 0;
      // fall through
    case AOBJ_CREATED:    // sent when the AppWare object is first created
      pSound = new Sound(oiSound, (pMsg->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE);
      if(pSound && pSound->Valid())
        {
        AObjSetRuntimeInfo(oiSound, (long)pSound);
        lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
        }
      else
        {
        delete pSound;
        AObjSetRuntimeInfo(oiSound, (long)NULL);
        lRet = A_ERROR;
        }
      break;

    case AOBJ_DESTROYED:  // sent when object is destroyed or delete
    case AOBJ_ASLEEP:     // sent when worksheet is closed or application terminates
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      delete pSound;
      AObjSetRuntimeInfo(oiSound, (long)NULL);
      lRet = A_CONTINUE;
      break;

    case AOBJ_ASSIGNOBJECT:   // set an object to the same state as another object.
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      if(Sound::CheckVersion((AOBJECTID)pMsg->message3) != A_ERROR)
        {
        if(AObjGetDataCount((AOBJECTID)pMsg->message3) != 0)
          pSound->InitializeFromData();

        lRet = pSound->Assign((Sound*)AObjGetRuntimeInfo((AOBJECTID)pMsg->message3));
        }
      else
        lRet = A_ERROR;
      break;

    case AOBJ_BUILDSIGNALS:   // system is asking for signal information
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->BuildSignals((AMEMBLOCKID)pMsg->message2);
      break;

    case AOBJ_CHECKOBJECT:    // object being checked for errors
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->Check();
      break;

    case AOBJ_EDITOBJECT:     // object is being requested to edit
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->Edit();
      break;

    case AOBJ_EXPORTOBJECT:   // export object to UPSF file
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->Export();
      break;

    case AOBJ_IMPORTOBJECT:   // import the object from UPSF file
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->Import();
      break;

    case AOBJ_POSTWRITE:      // the object has finished writing to file
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->PostWrite();
      break;

    case AOBJ_PREWRITE:       // prepare object to write to file
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->PreWrite();
      break;

    case AOBJ_QUERYPROTOCOL:  // does the object/type support a given protocol
      lRet = Sound::QueryProtocol(oiSound, (pApQueryInfo)pMsg->message4);
      break;

    case AOBJ_READ:           /* object data read from disk -- check version
                               * and upgrade as necessary.  This is done
                               * before AOBJ_AWAKENED message is sent.
                               */
      lRet = Sound::CheckVersion(oiSound);
      if(lRet == A_ERROR)
        AObjReportError(oiSound, AOBJ_READ, A_ERROR, "Sound\talm_soun.dll", VERSION_SOUNDOBJECT);
      break;

    case AOBJ_REPORTREQUIREDFILES:  // Report any files this ALM needs
      lRet = Sound::ReportRequiredFiles(pMsg);
      break;

    case AOBJ_VALIDATEOBJECTIDS:    // validate all Object IDs this object holds
      pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
      I_ASSERT(pSound);
      lRet = pSound->ValidateIDs(pMsg);
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     ClearSound --- ALM Function
//
//  Description:  This function will clear the given sound.  This will be
//                called to free up resources.
//
//  ALM Function Info:
//    inputs:
//      Sound
//
//    outputs:
//      none
//
//    flows:
//      1.  Done
//      2.  Error
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK ClearSound
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*    pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound ? pSound->Clear() : 2L, NULL);
    }
  else
    AFuncReportResult(pMsg, 2L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     DevicePresent --- ALM Function
//
//  Description:  This will determine if the sound device driver is present.
//
//  ALM Function Info:
//    inputs:
//      none
//
//    outputs:
//      none
//
//    flows:
//      1.  MIDI and Waveform supported
//      2.  Waveform only
//      3.  MIDI only
//      4.  none
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK DevicePresent
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AFuncReportResult(pMsg, Sound::SoundDriverPresent(), NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     IsSoundPlaying --- ALM Function
//
//  Description:  This will allow the user to determine if the specified
//                sound is playing.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed if the sound is playing.
//      2.  Followed if the sound is not playing.
//      3.  Followed if another error occurred.
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK IsSoundPlaying
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*    pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    AFuncReportResult(pMsg, pSound->Playing() ? 1L : 2L, NULL);
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     LoadSound --- ALM Function
//
//  Description:  This will load a sound file into the specified sound object.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//      2.  Pathname (optional text object --
//                      if not present will display open file dialog)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed if the sound resource is loaded.
//      2.  Followed if the dialog was cancelled.
//      3.  Followed if an error occurred while loading.
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK LoadSound
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);
  AOBJECTID  oiPath  = AFuncGetTypedParameter(2L, OTYPE_TEXT);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    if(oiPath)
      {
      char  szFile[MAX_PATH];

      OTextGetToBuffer(oiPath, szFile, sizeof(szFile));
      AFuncReportResult(pMsg, pSound->Load(szFile), NULL);
      }
    else
      {
      OpenDialog  od(GetActiveWindow());
      ResString   rsLoadSound(STR_LOADSOUND);

      od.SetTitle(rsLoadSound.String());
      od.SetInitDir(pSound->GetInitDir());
      if(od.FileDlg())
        AFuncReportResult(pMsg, pSound->Load(od.FullPath()), NULL);
      else
        AFuncReportResult(pMsg, 2L, NULL);
      }
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     Play --- ALM Function
//
//  Description:  This will play the sound referenced by the object.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after starting to play the object.
//      2.  Unexpected Error.
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Play
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound->Play(), NULL);
    }
  else
    AFuncReportResult(pMsg, 2L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     PlayFromFile --- ALM Function
//
//  Description:  This will play a sound from a file
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//      2.  Pathname (optional text object --
//                      if not present will display open file dialog)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after starting to play the object.
//      2.  Followed if the dialog is cancelled.
//      3.  Followed if an error (invalid file name/type).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK PlayFromFile
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);
  AOBJECTID  oiPath  = AFuncGetTypedParameter(2L, OTYPE_TEXT);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    if(oiPath)
      {
      char  szFile[MAX_PATH];

      OTextGetToBuffer(oiPath, szFile, sizeof(szFile));
      AFuncReportResult(pMsg, pSound->PlayFromFile(szFile), NULL);
      }
    else
      {
      OpenDialog  od(GetActiveWindow());
      ResString   rsLoadSound(STR_PLAYSOUND);

      od.SetTitle(rsLoadSound.String());
      od.SetInitDir(pSound->GetInitDir());
      if(od.FileDlg())
        AFuncReportResult(pMsg, pSound->PlayFromFile(od.FullPath()), NULL);
      else
        AFuncReportResult(pMsg, 2L, NULL);
      }
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     PlayRepeatedly --- ALM Function
//
//  Description:  This will play a sound object continuously.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after starting to play the object.
//      2.  Followed if an error (invalid object).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK PlayRepeatedly
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound->PlayRepeatedly(), NULL);
    }
  else
    AFuncReportResult(pMsg, 2L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     Record --- ALM Function
//
//  Description:  This will record to the specified object
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after finishing the recording session.
//      2.  Followed if the record dialog is cancelled.
//      3.  Followed if an error (invalid object).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Record
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound->Record(), NULL);
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     RecordToFile --- ALM Function
//
//  Description:  This will record to a file, using the settings from the
//                specified object.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//      2.  Pathname (optional text object --
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after finishing the recording session.
//      2.  Followed if the record dialog is cancelled.
//      3.  Followed if there is a general error (invalid pathname).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK RecordToFile
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);
  AOBJECTID  oiPath  = AFuncGetTypedParameter(1L, OTYPE_TEXT);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);

    if(oiPath)
      {
      char  szFile[MAX_PATH];

      OTextGetToBuffer(oiPath, szFile, sizeof(szFile));
      AFuncReportResult(pMsg, pSound->RecordToFile(szFile), NULL);
      }
    else
      {
      OpenDialog  od(GetActiveWindow());
      ResString   rsLoadSound(STR_RECORDSOUND);

      od.SetTitle(rsLoadSound.String());
      od.SetInitDir(pSound->GetInitDir());
      if(od.FileDlg())
        AFuncReportResult(pMsg, pSound->RecordToFile(od.FullPath()), NULL);
      else
        AFuncReportResult(pMsg, 2L, NULL);
      }
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     SoundEmpty --- ALM Function
//
//  Description:  This will allow the user to determine if the specified
//                sound object is empty.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed if the sound is empty.
//      2.  Followed if the sound is not empty.
//      3.  Followed if an error (invalid object).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK SoundEmpty
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound->SoundEmpty(), NULL);
    }
  else
    AFuncReportResult(pMsg, 3L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     StopSounds --- ALM Function
//
//  Description:  This will stop all playing sounds or a specific sound.
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object) -- optional
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after stopping <all> sound<s>.
//      2.  Followed if an error (invalid object).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK StopSounds
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

  if(oiSound)
    {
    Sound*  pSound = (Sound*)AObjGetRuntimeInfo(oiSound);
    AFuncReportResult(pMsg, pSound->Stop(), NULL);
    }
  else
    AFuncReportResult(pMsg, Sound::StopSounds(), NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     WaitUntilDone --- ALM Function
//
//  Description:  This will wait until all sounds have finished playing
//                (except for continuously playing sounds).
//
//  ALM Function Info:
//    inputs:
//      1.  Sound (sound object) -- optional
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after all sounds have finished playing
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK WaitUntilDone
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  if(pEvent->targetObject == (AOBJECTID)OTYPE_SOUND)
    AFuncReportResult(pMsg, 1L, NULL);  // flow is continuing
  else
    {
    AOBJECTID  oiSound = AFuncGetTypedParameter(1L, OTYPE_SOUND);

    AFuncReportResult(pMsg, Sound::WaitUntilDone(oiSound, pEvent), NULL);
    }
  }

////////////////////////////////////////////////////////////////////////////
