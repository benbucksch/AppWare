///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
// Object Group:        Essentials
// Object:                Printer
//
// Filename:    "print_fn.cpp"
//
// Author:      Kevin Smart
//   Start:     June 24, 1992
//   Finish:    July 24, 1992
//
// Description:
//      This file contains the following printer functions:
//        PrintDialog
//        PrintSetup
//        PrintObject
//        PrintObjectNow
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<string.h>
#include	<o_text.h>
#include	<o_number.h>
#include	<op_wnd.h>			// window item protocol.
#include	<o_wnd.h>				// window object API.
#include	<intdebug.h>

#include	"o_print.h"
#include	"prntobj.h"
#include	"print.hpp"

///////////////////////Internal Function Prototypes/////////////////////////

HDC			GetPrinterDC		(LPDEVNAMES,	LPDEVMODE);
BOOL		SendPIDRMessage	(PIDR_BLOCK&,	LPSTR,	 PRINTER*,	OBJECTID);

////////////////////////////Global Container////////////////////////////////

extern	GLOBAL	G;

////////////////////////////////////////////////////////////////////////////
//  PrintDialog         ("JobDialog" in Macintosh)
//
//      inputs:
//        none
//
//      outputs:
//        1.  First page to print
//        2.  Last page to print
//
//      flows:
//        1.  Followed after "OK" button pressed
//        2.  Followed after "Cancel" button pressed
////////////////////////////////////////////////////////////////////////////

void far pascal _export PrintDialog(pAEvtInfo theEvent,
		pAObjMessage theSystem)
{
	I_UNUSED(theEvent);
	OBJECTID	theObject;
	long			theGetKey = AOBJ_GETFIRSTKEY;
	PRINTER*	prn;

	if (AObjGetObjectCount(OTYPE_PRINTER)>0) {
    theObject   = AObjGetNextObject(OTYPE_PRINTER, &theGetKey);
		prn = (PRINTER*) AObjGetRuntimeInfo(theObject);

    // Set the active window for modal dialog box
    prn->SetOwner(GetActiveWindow());
		prn->SetMinPage(1);
		prn->SetMaxPage(9999);

		if(prn->PrintDlg((DWORD)PD_NOSELECTION | (DWORD)PD_USEDEVMODECOPIES |
										prn->PersistentFlags())) { // "OK" button pressed

    	// store the persistent flags (Collate, Print to file, PageNums)
			prn->SetPersistentFlags();

			// Prepare the "from" and "to" numbers for output
      OBJECTID     fromPage	= AObjCreate(OTYPE_NUMBER);
      OBJECTID     toPage	= AObjCreate(OTYPE_NUMBER);

			if(fromPage) {
				ONmbrSetInteger(fromPage, prn->FromPage(), TRUE);
				AFuncSetParameter(1L, fromPage);
			}

      if(toPage) {
				ONmbrSetInteger(toPage, prn->ToPage(), TRUE);
				AFuncSetParameter(2L, toPage);
			}

      // exit with flow 1
      AFuncReportResult(theSystem, 1L, NULL);
      }
    else {  // "Cancel" button pressed

      // The numbers are NOT written out because the numbers would not
      // make any sense.

      // exit with flow 2
      AFuncReportResult(theSystem, 2L, NULL);
      }
    }
  else {
    AFuncReportResult(theSystem, 2L, "You didn't include a Printer object in your application.");
    }

  return;
}


////////////////////////////////////////////////////////////////////////////
//  PrintSetup          ("Page Setup" in Macintosh)
//
//      inputs:
//        none
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after "OK" button pressed
//        2.  Followed after "Cancel" button pressed
////////////////////////////////////////////////////////////////////////////

void far pascal _export         PrintSetup      (pAEvtInfo theEvent,
                                                 pAObjMessage      theSystem)

{
	I_UNUSED(theEvent);
	OBJECTID  theObject;
  long      theGetKey = AOBJ_GETFIRSTKEY;
	PRINTER*	prn;

  if (AObjGetObjectCount(OTYPE_PRINTER)>0) {
    theObject   = AObjGetNextObject(OTYPE_PRINTER, &theGetKey);
		prn = (PRINTER*) AObjGetRuntimeInfo(theObject);

    // Set the active window for modal dialog box
		prn->SetOwner(GetActiveWindow());

		if(prn->PrintDlg((DWORD)PD_PRINTSETUP | (DWORD)PD_NOSELECTION | (DWORD)PD_USEDEVMODECOPIES)) {
      // "OK" button pressed--exit with flow 1
      AFuncReportResult(theSystem, 1, NULL);
      }
    else {
      // "Cancel" button pressed--exit with flow 2
      AFuncReportResult(theSystem, 2, NULL);
      }
    }
  else {
    AFuncReportResult(theSystem, 2, "You didn't include a Printer object in your application.");
    }

  return;
}


////////////////////////////////////////////////////////////////////////////
//  PrintObject
//
//      inputs:
//        1.  Object to be printed
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after the object has been printed
//        2.  Followed if "Cancel" button pressed, an error occurs,
//	      or the print job is cancelled
////////////////////////////////////////////////////////////////////////////

void far pascal _export         PrintObject     (pAEvtInfo theEvent,
                                                 pAObjMessage      theSystem)

{
	I_UNUSED(theEvent);
	OBJECTID      theObject;
  long          theGetKey = AOBJ_GETFIRSTKEY;
  char          errorMessage[256];
	PRINTER*	prn;

  if (AObjGetObjectCount(OTYPE_PRINTER)>0) {
    theObject   = AObjGetNextObject(OTYPE_PRINTER, &theGetKey);
		prn = (PRINTER*) AObjGetRuntimeInfo(theObject);

    // get the input and make sure it is valid
    OBJECTID	inputObject      = AFuncGetParameter(1L);

    // get a valid range of pages for the object--pidr.numPages MUST be TRUE!
    PIDR_BLOCK	pidr;
    memset(&pidr, 0, sizeof(PIDR_BLOCK));
    pidr.numPages = TRUE;
		pidr.hDC	  = GetPrinterDC(prn->GetDevNames(), prn->GetDevMode());
		prn->RelDevNames();
		prn->RelDevMode();

    pidr.rect.left		= 0;
    pidr.rect.top			= 0;
    pidr.rect.right		= GetDeviceCaps(pidr.hDC, HORZRES);
    pidr.rect.bottom	= GetDeviceCaps(pidr.hDC, VERTRES);

    pidr.DPI.x		= GetDeviceCaps(pidr.hDC, LOGPIXELSX);
    pidr.DPI.y		= GetDeviceCaps(pidr.hDC, LOGPIXELSY);

    if(!pidr.hDC) {
      // couldn't get a printer DC--send error and quit
      AFuncReportResult(theSystem, 2, "A print driver error occurred.");
      return;
      }

    HCURSOR	oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		if (!AEvtCallMethod(theObject, inputObject, 0L,
							OPRINT_EVT_PRINTITEMDRAWROUTINE, (long)&pidr)) {
      SetCursor(oldCursor);
      AFuncReportResult(theSystem, 2, "Can't print this object.");
      return;
      }
    SetCursor(oldCursor);

    DeleteDC(pidr.hDC);

    // Set the active window for modal dialog box and update the pd structure
		prn->SetOwner(GetActiveWindow());
		prn->SetFromPage(1);
		prn->SetToPage(pidr.toPage);
		prn->SetMinPage(1);
		prn->SetMaxPage(pidr.toPage);

    // Call the PrintDlg common dialog routine (have it get a printer DC)
		if(prn->PrintDlg((DWORD)PD_ENABLEPRINTHOOK | PD_RETURNDC | PD_NOSELECTION |
										(DWORD)PD_USEDEVMODECOPIES | prn->PersistentFlags()))

			{
			// store the persistent flags (Collate, Print to file, PageNums)
			prn->SetPersistentFlags();

      // Print Dialog "OK" button pressed
			pidr.fromPage = prn->FromPage();
			pidr.toPage   = prn->ToPage();
      pidr.numPages = FALSE;

      // make sure the printer DC is valid
			if ((pidr.hDC=prn->GetDC())==NULL)
				{
				AFuncReportResult(theSystem, 2, "A print driver error occurred.");
				return;
				}

      // call the function that actually does the printing
			if(!SendPIDRMessage(pidr, errorMessage, prn, inputObject))
				{
        // returns FALSE on error
				AFuncReportResult(theSystem, 2, errorMessage);
				return;
				}

      // exit the function 
			if(G.bUserAbort)
				{
				// user aborted the printing--no error, but exit with flow 2
				AFuncReportResult(theSystem, 2, NULL);
        }
			else if(strlen(pidr.error)!=0)
				{
        // an error occurred in AEvtCallMethod--exit with flow 2
				AFuncReportResult(theSystem, 2, pidr.error);
				}
			else
				{  // lstrlen(pidr.error)==0  && G.bUserAbort==FALSE)
        // no errors occurred--exit with flow 1
				AFuncReportResult(theSystem, 1, NULL);
        }
      }
		else
			{  // Print Dialog "Cancel" button pressed
      // exit with flow 2
      AFuncReportResult(theSystem, 2, NULL);
      }
    }
	else
		{  // AObjGetObjectCount(OTYPE_PRINTER)==0
    AFuncReportResult(theSystem, 2, "You didn't include a Printer object in your application.");
    }

  return;
}

////////////////////////////////////////////////////////////////////////////
//  PrintObjectNow
//
//      inputs:
//        1.  Object to be printed
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after the object has been printed
//        2.  Followed if an error occurs
////////////////////////////////////////////////////////////////////////////

void far pascal _export         PrintObjectNow  (pAEvtInfo theEvent,
																								 pAObjMessage      theSystem)

{
	I_UNUSED(theEvent);
	OBJECTID    theObject;
  long        theGetKey = AOBJ_GETFIRSTKEY;
  char        errorMessage[256];
	PRINTER*		prn;

  if (AObjGetObjectCount(OTYPE_PRINTER)>0) {
    theObject   = AObjGetNextObject(OTYPE_PRINTER, &theGetKey);
		prn = (PRINTER*) AObjGetRuntimeInfo(theObject);

    // get the input and make sure it is valid
    OBJECTID  inputObject      = AFuncGetParameter(1);
    TYPEID    inputObjectType  = AObjGetType(inputObject);
		if (!AEvtCheckForMethod(inputObjectType,
									OPRINT_EVT_PRINTITEMDRAWROUTINE)) {
      // no PIDR method--can't print the specified object
      AFuncReportResult(theSystem, 2, "Can't print this object.");
      return;
      }

    // prepare the data to call the method
    PIDR_BLOCK	pidr;
		pidr.fromPage = prn->FromPage();
		pidr.toPage   = prn->ToPage();
    pidr.numPages = FALSE;
		pidr.hDC	  = GetPrinterDC(prn->GetDevNames(), prn->GetDevMode());
		prn->RelDevNames();
    prn->RelDevMode();

    if(!pidr.hDC) {
      // couldn't get a printer DC--send error and quit
      AFuncReportResult(theSystem, 2, "A print driver error occurred.");
      return;
      }

    // call the function that actually does the printing
		if(!SendPIDRMessage(pidr, errorMessage, prn, inputObject)) {
      // SendPIDRMessage returns FALSE on error
      AFuncReportResult(theSystem, 2, errorMessage);
      return;
      }

    // exit the function
		if(G.bUserAbort) {  // strlen(pidr.error)==0
      // user aborted the printing--no error, but exit with flow 2
      AFuncReportResult(theSystem, 2, NULL);
      }
		else if(strlen(pidr.error)!=0) {
      // an error occurred in AEvtCallMethod--exit with flow 2
      AFuncReportResult(theSystem, 2, pidr.error);
      }
		else {  // strlen(pidr.error)==0  && G.bUserAbort==FALSE)
      // no errors occurred--exit with flow 1
      AFuncReportResult(theSystem, 1, NULL);
      }
    }
  else {  // AObjGetObjectCount(OTYPE_PRINTER)<=0
		AFuncReportResult(theSystem, 2, "You didn't include a Printer object in your application.");
    }

  return;
}


////////////////////////////////////////////////////////////////////////////
// GetPrinterDC
//
// Description:
//      This function gets a printer DC when one is not supplied from
//      using a common dialog box.
////////////////////////////////////////////////////////////////////////////

HDC	GetPrinterDC(LPDEVNAMES lpDN, LPDEVMODE lpDM)

{
  char	*szDevice, *szDriver, *szPort;
	char	szPrinter[80];

  if(lpDN==NULL) {
		GetProfileString("windows", "device", ",,,", szPrinter, 80);

    szDevice = strtok(szPrinter,	",");
    szDriver = strtok(NULL,				", ");
    szPort   = strtok(NULL,				", ");
    }
  else {
    // use current printer settings as stored in the object
    szDevice = ((char*)lpDN) + lpDN->wDeviceOffset;
    szDriver = ((char*)lpDN) + lpDN->wDriverOffset;
    szPort   = ((char*)lpDN) + lpDN->wOutputOffset;
    }

	if(szDevice && szDriver && szPort) {
    return CreateDC(szDriver, szDevice, szPort, (char*)lpDM);
    }
  else {
    return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////

BOOL far pascal _export	AbortProc(HDC hDC, short nCode)

{
	I_UNUSED(hDC);
	I_UNUSED(nCode);
	MSG	msg;

  while (!G.bUserAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (!IsDialogMessage(G.hPrintDlg, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
    }

  return !G.bUserAbort;
}


////////////////////////////////////////////////////////////////////////////

BOOL far pascal _export	PrintProc(HWND hDlg, WORD message,
				  WORD wParam, long lParam)

{
	I_UNUSED(wParam);
	I_UNUSED(lParam);
	switch (message) {
    case WM_INITDIALOG :
				EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
				return TRUE;

    case WM_COMMAND :
				G.bUserAbort = TRUE;
				EnableWindow(GetParent(hDlg), TRUE);
				DestroyWindow(hDlg);
				G.hPrintDlg = 0;
				return TRUE;
    }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////

BOOL	SendPIDRMessage(PIDR_BLOCK& pidr,	char* errorMessage,
											PRINTER* prn,	OBJECTID inputObject)

{
	HWND		hWnd;
  char		szMessage[256];
	int			drawPatRect = DRAWPATTERNRECT;

  // initialize the PIDR_BLOCK structure
	pidr.error[0]			= 0;

  pidr.rect.left		= 0;
  pidr.rect.top			= 0;
  pidr.rect.right		= GetDeviceCaps(pidr.hDC, HORZRES);
  pidr.rect.bottom	= GetDeviceCaps(pidr.hDC, VERTRES);

  pidr.DPI.x				= GetDeviceCaps(pidr.hDC, LOGPIXELSX);
	pidr.DPI.y				= GetDeviceCaps(pidr.hDC, LOGPIXELSY);

	pidr.pclPrinter		= Escape(pidr.hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&drawPatRect, NULL);

  // initialize other variables
	hWnd					= GetActiveWindow();
  G.bUserAbort	= FALSE;

  // set up the Printing Dialog box and Abort procedure
  EnableWindow(hWnd, FALSE);
	G.hPrintDlg		= CreateDialog(G.hInst, "PrintDlgBox", hWnd, (DLGPROC)PrintProc);
	pidr.hMsgWnd	= GetDlgItem(G.hPrintDlg, IDD_MESSAGE);

  // write the printer name to the dialog box
	LPDEVNAMES	lpDN = prn->GetDevNames();
  char	*szDevice, *szDriver, *szPort;
	char	szPrinter[80];
	if(lpDN==NULL) {
		GetProfileString("windows", "device", ",,,", szPrinter, 80);

    szDevice = strtok(szPrinter,	",");
    szDriver = strtok(NULL,				", ");
    szPort   = strtok(NULL,				", ");
    }
  else {
    // use current printer settings as stored in the object
    szDevice = ((char*)lpDN) + lpDN->wDeviceOffset;
    szDriver = ((char*)lpDN) + lpDN->wDriverOffset;
    szPort   = ((char*)lpDN) + lpDN->wOutputOffset;
		}
	wsprintf(szMessage, "on the %s printer\n using the %s driver\non %s", szDevice, szDriver, szPort);
  SetDlgItemText(G.hPrintDlg, IDD_PRINTERINFO, szMessage);

	// set up a loop for printing collated copies
	UINT	numCopies = 1;
	if(prn->PersistentFlags() & PD_COLLATE) 
		{
		LPDEVMODE	lpDM = prn->GetDevMode();
    if(lpDM)
			numCopies = lpDM->dmCopies;
		prn->RelDevMode();
		}

	for(int i=0; i<numCopies; i++) {
		// print numCopies times (for collated copies)
		if(numCopies>1) {
			LPDEVMODE	lpDM = prn->GetDevMode();
      if(lpDM) {
				lpDM->dmCopies = 1;
				// get a new DC to only print one copy
				pidr.hDC = GetPrinterDC(prn->GetDevNames(), lpDM);
	    	lpDM->dmCopies = numCopies;
				prn->RelDevNames();
				}
	    prn->RelDevMode();
			}

		Escape(pidr.hDC, SETABORTPROC, 0, (LPSTR)((FARPROC)AbortProc), NULL);

		// load the printing message for the Print Spooler
  	strcpy(szMessage, "APPWARE Print Job.");
		if(Escape(pidr.hDC, STARTDOC, strlen(szMessage), szMessage, NULL)>0) {
			// do the actual printing & report error if one occurs
			AEvtCallMethod(prn->GetObjectID(), inputObject, 0L,
							OPRINT_EVT_PRINTITEMDRAWROUTINE, (long)&pidr);
    	if(strlen(pidr.error)==0) {
      	// end the document--errors would have closed it earlier
      	if(Escape(pidr.hDC, NEWFRAME, 0, NULL, NULL)>0) {
      	  Escape(pidr.hDC, ENDDOC, 0, NULL, NULL);
        	}
      	else {
					// an error occurred while closing the document--clean up and exit
					DeleteDC(pidr.hDC);
        	if(!G.bUserAbort) {
						strcpy(errorMessage, "A printer error occurred while closing the document.");
	  				}
					else {
        	  // no error, but the user aborted
	  				errorMessage[0] = 0;
	  				}
					EnableWindow(hWnd, TRUE);
					DestroyWindow(G.hPrintDlg);
					return FALSE;  // an error occurred
					}
      	}  // end...if(strlen(pidr.error)==0)
			DeleteDC(pidr.hDC);
			if(G.bUserAbort) {
				// Quit if the user aborts
				break;
        }
			}  // end...if(Escape(...
  	else {
			// couldn't open the document--clean up and send the error message
    	DeleteDC(pidr.hDC);
    	strcpy(errorMessage, "Couldn't open document for printing.");
    	EnableWindow(hWnd, TRUE);
    	DestroyWindow(G.hPrintDlg);
			return FALSE;  // an error occurred
			}  // end...if(Escape(...))...else
		}  // end...for(int i=0; i<numCopies; i++)

	// destroy the print dialog box after printing all the copies
	if(!G.bUserAbort) {  // clean up the windows and exit
 	  EnableWindow(hWnd, TRUE);
 	  DestroyWindow(G.hPrintDlg);
 	  }
 	else {
 	  // no error, but the user aborted
 	  errorMessage[0] = 0;
 	  }
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
