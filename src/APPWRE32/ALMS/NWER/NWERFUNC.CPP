/*
   File Name:  NWErFunc.CPP
         Copyright 1994 Novell Inc.
   Description.:
   The AppWare NetWare Error Reporting Functions.
*/
#include <windows.h>
#include <a_alm.h>
#include <o_nwerr.h>
#include "nwerror.h"

/*
   FUNCTION:  TurnReportingOnFunc
   DESCRIPTION:  Turns automatic NetWare Error Reporting ON"
   INPUTS: None
   OUTPUTS: None
   FLOW 1: After Turning Error Reporting On
   FLOW 2: No Error Object Available
*/
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  TurnReportingOnFunc(pAEvtInfo, pAObjMessage theSystem)
   {
   AOBJECTID oiNWError ;
   long key = AOBJ_GETFIRSTKEY ;

   if ((oiNWError = AObjGetNextObject(OTYPE_NWERROR, &key)) == 0)
      {
      theSystem->message1 = 2 ; // No error reporting objects found
      return  ;
      }

   ErrorInfo* pErrorInfo = (ErrorInfo*)AObjLockData(oiNWError, 0) ;
    pErrorInfo->m_bReportErrors = TRUE ;
    AObjUnlockData(oiNWError, 0) ;
   theSystem->message1 = 1 ;
   }

/*
   FUNCTION:  TurnReportingOffFunc
   DESCRIPTION:  Turns automatic NetWare Error Reporting OFF"
   INPUTS: None
   OUTPUTS: None
   FLOW 1: After Turning Error Reporting Off
   FLOW 2: No Error Object Available
*/
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  TurnReportingOffFunc(pAEvtInfo, pAObjMessage theSystem)
   {
   AOBJECTID oiNWError ;
   long key = AOBJ_GETFIRSTKEY ;

   if ((oiNWError = AObjGetNextObject(OTYPE_NWERROR, &key)) == 0)
      {
      theSystem->message1 = 2 ; // No error reporting objects found
      return  ;
      }

   ErrorInfo* pErrorInfo = (ErrorInfo*)AObjLockData(oiNWError, 0) ;
    pErrorInfo->m_bReportErrors = FALSE ;
    AObjUnlockData(oiNWError, 0) ;
   theSystem->message1 = 1 ;
   }

/*
   FUNCTION:  IsReportingOnFunc
   DESCRIPTION:  Get the current state of NetWare Error Reporting "
   INPUTS: None
   OUTPUTS: None
   FLOW 1: Error Reporting On
   FLOW 2: Error Reporting Off
*/
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  IsReportingOnFunc(pAEvtInfo, pAObjMessage theSystem)
   {
   AOBJECTID oiNWError ;
   long key = AOBJ_GETFIRSTKEY ;

   if ((oiNWError = AObjGetNextObject(OTYPE_NWERROR, &key)) == 0)
      {
      theSystem->message1 = 1 ; // No error reporting objects found: Reporting on by default
      return  ;
      }

   ErrorInfo* pErrorInfo = (ErrorInfo*)AObjLockData(oiNWError, 0) ;
   if (pErrorInfo->m_bReportErrors)
      theSystem->message1 = 1 ;    // Error Reporting is ON
   else
      theSystem->message1 = 2 ;    // Error Reporting is OFF
    AObjUnlockData(oiNWError, 0) ;
   }

