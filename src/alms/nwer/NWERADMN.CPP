/*
   File Name:  NWErAdmn.CPP
         Copyright 1994 Novell Inc.
   Description.:
   The Administration Routine for the AppWare Loan object.
*/
#include <windows.h>
#include <a_alm.h>
#include <o_nwerr.h>
#include "nwerror.h"

LONG CreateObject(AOBJECTID oiNWError) ;
LONG CheckObject(AOBJECTID oiNWError) ;

HINSTANCE hInstance ;
extern int _WinAllocFlag ;
/*
   Function:  AdministrationRoutine
   Description:  Creates the and initializes the data structure for
     a Loan Object when the AOBJ_CREATED message is received
*/
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  NetWareErrorAdministration(AOBJECTID oiNWError,
                                                   pAObjMessage theSystem)
{
   switch (theSystem->message1)
      {
      case AOBJ_CREATED:
         return CreateObject(oiNWError) ;

      case AOBJ_EDITOBJECT:
         return EditObject(oiNWError) ;

      case AOBJ_CHECKOBJECT:
         return CheckObject(oiNWError) ;
      }
   return A_NOTHANDLED ;
   }

/*
    Function:  CreateObject
    Description:  Creates a Loan object with default values
*/
LONG CreateObject(AOBJECTID oiNWError)
   {
   ErrorInfo * pErrorInfo ;
   AObjSetDataCount(oiNWError, 1) ;
   AObjResizeData(oiNWError, 0, sizeof(ErrorInfo)) ;
   pErrorInfo = (ErrorInfo *)AObjLockData(oiNWError, 0) ;
   if (pErrorInfo == NULL)
      {
      MessageBox(NULL, "Creation of the Error Reporting object failed", NULL, MB_OK) ;
      return A_ERROR ;
      }
   pErrorInfo->m_nVersion = 0 ;
   pErrorInfo->m_bReportErrors = TRUE ;
   pErrorInfo->m_lErrorCodeID  = 0 ;
   pErrorInfo->m_lContextID = 0 ;
   AObjUnlockData(oiNWError, 0) ;
   return A_OK ;
   }

/*
    CheckObject
    Verifies that only one Error Reporting object exists.
*/
LONG CheckObject(AOBJECTID oiNWError)
   {
   long key = AOBJ_GETFIRSTKEY ;
   int count = 0 ;
    char buffer[80] ;

   while((AObjGetNextObject(OTYPE_NWERROR, &key)) != 0)
      {
        count++ ;
      }
   if (count > 1)
      {
        wsprintf(buffer, "%d error reporting objects were found in this project.  Only one will be used", count) ;
      AObjReportError(oiNWError, AOBJ_CHECKOBJECT, A_WARNING, buffer, 0) ;
      return A_WARNING ;
      }

   return A_OK ;
   }

/*
   Function:  LibMain/*!!PORT!! Replace with DllEntryPoint */
   Description:
   Marks global memory allocated by the DLL with the new operator sharable.
    Initialized the hInstance global variable
    Unlocks the local heap if there is one.
*/
int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */(HINSTANCE hInst, WORD, WORD wHeapSize, LPSTR )
   {
   #ifdef __BORLANDC__
   _WinAllocFlag = GMEM_SHARE ;  // Used by Borland compilers
   #endif

   hInstance = hInst ;
   if ( wHeapSize != 0 )
      UnlockData( 0 ) ;
   return 1;
   }
