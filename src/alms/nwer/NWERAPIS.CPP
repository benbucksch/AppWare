/*
   File Name:  NWErAPIs.CPP
         Copyright 1994 Novell Inc.
   Description.:
   The AppWare NetWare Error Reporting APIs.
*/
#include <windows.h>
#include <a_alm.h>
#include <o_nwerr.h>
#include "nwerror.h"

/*
   FUNCTION:  IsReportingOn
   DESCRIPTION:  Could be called by an ALM developer to determine in NetWare Error
   processing is currently turned on.
   RETURN VALUE: TRUE (non-zero) if reporting is on
            FALSE (zero) if reporting is off
*/
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  IsReportingOn()
   {
   AOBJECTID oiNWError ;
   long key = AOBJ_GETFIRSTKEY ;
   BOOL bReportingOn ;

   if ((oiNWError = AObjGetNextObject(OTYPE_NWERROR, &key)) == 0)
      {
      bReportingOn = TRUE ;
      }

   else
      {
      ErrorInfo* pErrorInfo = (ErrorInfo*)AObjLockData(oiNWError, 0) ;
      bReportingOn = pErrorInfo->m_bReportErrors ;
      AObjUnlockData(oiNWError, 0) ;
      }
   return bReportingOn ;
    }

/*
   FUNCTION:  ReportNWError
   DESCRIPTION:  Reports a NetWare error if Error Reporting is turned on
   RETURN VALUE: TRUE (non-zero) if the error was reported
            FALSE (zero) if the error was not reported
*/
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  ReportNWError(LPSTR lpszText)
   {
   BOOL bReported ;

   if ((bReported = IsReportingOn()) == TRUE)
      bReported = MessageBox(NULL,/*GetActiveWindow()*/lpszText,NULL,
			     MB_OK|MB_TASKMODAL|MB_ICONEXCLAMATION);

   return bReported ;
   }


