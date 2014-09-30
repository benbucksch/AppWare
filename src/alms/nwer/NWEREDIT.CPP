/*
   File Name:  LoanObjd.CPP
         Copyright 1994 Novell Inc.
   Description.:
   The AppWare Loan object's Object Definition Routine used to edit the
      Loan object's data.
*/
#include <windows.h>
#include <bwcc.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_nwerr.h>
#include "nwerror.h"

BOOL  InitDialog(HWND, AOBJECTID oiNWError) ;
BOOL  UpdateObject(HWND) ;
void  CenterDialogOnCRT(HWND hDlg);
void  CenterDialogOnParent(HWND hDlg);

LONG EditObject( AOBJECTID oiNWError )
   {
   BOOL result = DialogBoxParam( hInstance, "NWError", GetActiveWindow(),
         (DLGPROC)EditObjectDialogProc, (LONG)oiNWError) ;
   if (result == FALSE)
      return A_OK ;
   return A_MODIFIED ;
   }

/*
    EditObjectDialogProc
    Used to edit the data members of the Loan object at design time
*/
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditObjectDialogProc (HWND hDlg, UINT message,
                         UINT wParam, LONG lParam)
   {
   static int  ccode=0;
   switch (message)
      {
      case WM_INITDIALOG:
         CenterDialogOnCRT(hDlg);
         return InitDialog(hDlg, (AOBJECTID)lParam) ;

      case WM_COMMAND: //!!PORT!!
         switch (wParam)
            {
            case IDOK:
               if (ccode)
                  WinHelp(hDlg,"ALM_NWER.hlp",HELP_QUIT,0x00001000);
               return UpdateObject(hDlg) ;

            case IDHELP:
               ccode = WinHelp(hDlg,"ALM_NWER.hlp",HELP_CONTEXT,0x00001000);
               break;

            case IDCANCEL:
               if (ccode)
                  WinHelp(hDlg,"ALM_NWER.hlp",HELP_QUIT,0x00001000);
               EndDialog (hDlg, FALSE) ;
               return TRUE ;
            }
      }
    return FALSE ;
    }

/*
   FUNCTION:  InitDialog
   DESCRIPTION:  Initialized the Loan Object's edit dialog box.
*/
BOOL InitDialog ( HWND hDlg, AOBJECTID oiNWError)
   {
   ErrorInfo * pErrorInfo ;
   char buf[255];

   /*------ set the object name into the caption ------*/
   AObjGetName(oiNWError,buf,sizeof(buf));
   SetWindowText(hDlg,buf);

   SetWindowLong(hDlg, DWL_USER, (LONG)oiNWError) ; // save the object ID in the window extra bytes
   HWND hText = GetDlgItem(hDlg, IDC_OBJECT) ;
   AUtlSetTitleFont(OTYPE_NWERROR, NULL, hText);

   pErrorInfo = (ErrorInfo *)AObjLockData(oiNWError, 0) ;
   if (pErrorInfo == NULL)
      return TRUE ;

   SendDlgItemMessage(hDlg, IDC_ShowNWErrors, BM_SETCHECK, pErrorInfo->m_bReportErrors, 0) ;

   AObjUnlockData(oiNWError, 0) ;
   return TRUE ;
   }

/*
   FUNCTION:  UpdateObject
   DESCRIPTION:  Updates the Loan object's data.
*/
BOOL  UpdateObject(HWND hDlg)
   {
   AOBJECTID oiNWError ;
   ErrorInfo * pErrorInfo ;
   //char buffer[40] ;

   oiNWError = (AOBJECTID)GetWindowLong (hDlg, DWL_USER) ;
   pErrorInfo = (ErrorInfo *) AObjLockData(oiNWError, 0) ;
   if (pErrorInfo == NULL)
      {
      EndDialog (hDlg, FALSE) ;
      return TRUE ;
      }

   pErrorInfo->m_bReportErrors = (BOOL)SendDlgItemMessage(hDlg, IDC_ShowNWErrors, BM_GETCHECK, 0, 0) ;

   AObjUnlockData(oiNWError, 0) ;
   EndDialog (hDlg, TRUE) ;
   return TRUE ;
   }


/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CenterDialogOnCRT() )

 NAME        : void  CenterDialogOnCRT(HWND hDlg)

 DESCRIPTION : Centers the dialog box on the video screen

 SYNOPSIS    : 1) Get the size of the dialog
               2) Now position the dialog in the center of the video screen

 PARAMETERS  : -> input       <- output
               -> hDlg

 RETURN      : None

 END_MANUAL_ENTRY
*****************************************************************************/
void  CenterDialogOnCRT(HWND hDlg)
{

   RECT  rc;

   GetWindowRect(hDlg,&rc);
   SetWindowPos(hDlg,
                NULL,
                ((GetSystemMetrics(SM_CXSCREEN)-(rc.right-rc.left))/2),
                ((GetSystemMetrics(SM_CYSCREEN)-(rc.bottom-rc.top))/2),
                0,
                0,
                SWP_NOSIZE|SWP_NOACTIVATE);

}  /* CenterDialogOnCRT */


/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CenterDialogOnParent() )

 NAME        : void  CenterDialogOnParent(HWND hDlg)

 DESCRIPTION : Centers the dialog box on the parent window

 SYNOPSIS    : 1) Get the size of the parent's window
               2) Now set the dialog in the center of the parent

 PARAMETERS  : -> input       <- output
               -> hDlg

 RETURN      : None

 END_MANUAL_ENTRY
*****************************************************************************/
void  CenterDialogOnParent(HWND hDlg)
{

   RECT  rc1,
         rc2;
   HWND  hPar;

   hPar = GetParent(hDlg);
   GetClientRect(hPar,&rc1);
   ClientToScreen(hPar,(LPPOINT)&rc1.left);
   ClientToScreen(hPar,(LPPOINT)&rc1.right);
   GetWindowRect(hDlg,&rc2);
   SetWindowPos(hDlg,
                NULL,
                rc1.left+(((rc1.right-rc1.left)-(rc2.right-rc2.left))/2),
                rc1.top+(((rc1.bottom-rc1.top)-(rc2.bottom-rc2.top))/2),
                0,
                0,
                SWP_NOSIZE|SWP_NOACTIVATE);

}  /* CenterDialogOnParent */
