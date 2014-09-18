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
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
// Object Group:  Essentials
// Object:          Printer
//
// Filename:  "print_ar.cpp"
//
// Author:  Kevin Smart
//   Start:   June 24, 1992
//   Finish:  July 24, 1992
//
// Description:
//  Printer Administration Routine and DLL entry point
//
////////////////////////////////////////////////////////////////////////////

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <string.h>
#include  <intdebug.h>

#include  "o_print.h"
#include  "print.hpp"
#include  "prntobj.h"


////////////////////////////Global Container////////////////////////////////

GLOBAL      G;
#ifdef __BORLANDC__
extern int  _WinAllocFlag;
#endif


////////////////////////////////////////////////////////////////////////////

extern "C" int far pascal LibMain (
  HINSTANCE hInstance,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine)

{
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

#ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE; // line to make "new" work in borland C++.
#endif
  G.hInst = hInstance;
  if(wHeapSize>0)
    UnlockData(0);
  return 1;
}


////////////////////////////////////////////////////////////////////////////

LONG far pascal _export PrintAdmn(
  OBJECTID      theObject,
  pAObjMessage  theSystem)
  {
  long  rVal = A_OK;
  PRINTER*  print;

  switch(theSystem->message1)
    {
    case AOBJ_AWAKENED :
      AObjSetRuntimeInfo(theObject, (long) new PRINTER(theObject));
      rVal = A_CONTINUE;
      break;
  
    case AOBJ_CREATED :
      AObjSetRuntimeInfo(theObject, (long) new PRINTER(theObject));
      break;

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      print = (PRINTER*)AObjGetRuntimeInfo(theObject);
      delete print;
      AObjSetRuntimeInfo(theObject, (long)NULL);
      rVal = A_CONTINUE;
      break;

    case AOBJ_READ :
      {
      PRINTER print(theObject);
      theSystem->message3 = (long)print.Version();    // version
      if(print.Version()!=VERSION_OBJECT)
        {
        theSystem->message4 = A_ERROR;                // statuts
        AObjReportError(theObject, AOBJ_READ, theSystem->message4,
                "Printer\talm_prnt.dll", theSystem->message3);
        rVal = A_ERROR;
        }
      }
      break;

    case AOBJ_EDITOBJECT :
      rVal = A_NOTHANDLED;
      break;

    case AOBJ_CHECKOBJECT:
      break;

    default :
      rVal = A_NOTHANDLED;
      break;
    }
  return rVal;
  }


////////////////////////////////////////////////////////////////////////////

long far pascal _export GetSerVersion(
  TYPEID theType)
{
  if(theType==OTYPE_PRINTER)
    return VERSION_DLL;
  else
    return 0L;
}

////////////////////////////////////////////////////////////////////////////
