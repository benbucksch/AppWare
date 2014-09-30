///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////
//#include <windows.h>
//#include <stdio.h>

#include "a_alm.h"
#include "o_wnd.h"
#include "o_print.h"
#include "pg_glob.h"
#include "pg_class.h"                                
#include "pg_macro.h"
#include "pg_dialg.h"

static int  GetVersion(AOBJECTID  theObject)
 {
  int         version;
  LPDATA0     paged0;

  paged0 = LockPageData0(theObject);
  version = paged0->version;
  UnlockPageData0(theObject);

  return version;

  }


extern "C" LONG ALMCALLBACK PageAdminProc(AOBJECTID oiPage, AObjMessage* theSystem)
{

  PAGE* page;
  LONG  rVal;

    switch(theSystem->message1)
      {
      case AOBJ_CREATED :
        page = new PAGE(oiPage);
        AObjSetRuntimeInfo(oiPage, (long)page);
        rVal  = A_OK;
        break;

      case AOBJ_AWAKENED :
        page = new PAGE(oiPage);
        AObjSetRuntimeInfo(oiPage, (long)page);
        rVal = A_CONTINUE;
        break;

      case AOBJ_DESTROYED :
      case AOBJ_ASLEEP :
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        delete page;
        rVal  = A_CONTINUE;
        break;

      case AOBJ_IMPORTOBJECT :
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        page->ImportObject();
        rVal  = A_OK;
        break;
  
      case AOBJ_EXPORTOBJECT :
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        page->ExportObject();
        rVal  = A_OK;
        break;

      case AOBJ_READ:

        if(GetVersion(oiPage)!=VERSION_OBJECT)
          {
          rVal = A_ERROR;
          AObjReportError(oiPage, AOBJ_READ, A_ERROR,
                          "Page\alm_page.dll", VERSION_OBJECT);
          }
        else
          rVal = A_OK;  
        break;

      case AOBJ_EDITOBJECT :
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        page->CheckObjectID();
        if (DialogBoxParam(g_hInstance, MAKEINTRESOURCE(PAGEDIALOG), GetActiveWindow(),
          (DLGPROC)PAGE::PageDialog,(LONG)oiPage))
          rVal = A_MODIFIED;
         else
          rVal = A_OK;
        break;

      case AOBJ_VALIDATEOBJECTIDS:
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        page->Validate(theSystem);
        rVal = A_OK;
        break;

      case AOBJ_BUILDSIGNALS:
        {
        AMEMBLOCKID signalBlock = (AMEMBLOCKID)theSystem->message2;
        AMemSetSize(signalBlock, NUM_PAGE_SIGNALS * sizeof(AObjSignalRecord));
        pAObjSignalRecord signalPtr = (pAObjSignalRecord)AMemLock(signalBlock);

        signalPtr[0].theID = SIGNAL_PAGEFINISHED ;
        lstrcpy(signalPtr[0].theName, "Page finished");
        }
        break;

      case AOBJ_CHECKOBJECT : 
        page = (PAGE*)AObjGetRuntimeInfo(oiPage);
        page->ObjectHealthy((AObjInfo*)theSystem->message2);
        rVal = A_OK;
        break;

      default:
        rVal = A_NOTHANDLED;
        break;
      }

    return rVal;
}

