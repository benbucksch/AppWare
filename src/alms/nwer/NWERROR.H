/*
   File Name:  Loan.h
         Copyright 1994 Novell Inc.
   Description.:
   Header file for the AppWare NetWare Error Reporting ALM
*/

// IDs for the definition dialog box
#define IDC_ShowNWErrors   100
#define IDC_OBJECT 103

// Exported functions
extern "C" {
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ NetWareErrorAdministration(AOBJECTID oiLoan, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TurnReportingOnFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TurnReportingOffFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ IsReportingOnFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ EditObjectDialogProc (HWND, UINT, UINT, LONG) ;
}

LONG EditObject( AOBJECTID oiNWError ) ;

typedef struct
   {
   int m_nVersion ;
   BOOL m_bReportErrors ;
   long m_lErrorCodeID ;
    long m_lContextID ;
   } ErrorInfo ;

extern HINSTANCE hInstance ;
