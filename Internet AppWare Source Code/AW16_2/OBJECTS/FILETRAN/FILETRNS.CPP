////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"Filetrns.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#define CONST
#define SZ_TEXTPROP     "DynDlg_TextSave"

#include <windowsx.h>
#include <commDlg.h>
#include <string.h>
#include "filetrns.h"

static PROTOCOLFRAME protocolFrame[] =
  {
  {"ASCII",  DLG_ASCIIFRAME,  ASCII_FIRSTID,  ASCII_ENDID},
	{"XMODEM", DLG_XMODEMFRAME, XMODEM_FIRSTID, XMODEM_ENDID},
	{"YMODEM", DLG_YMODEMFRAME, YMODEM_FIRSTID, YMODEM_ENDID},
	{"KERMIT", DLG_KERMITFRAME, KERMIT_FIRSTID, KERMIT_ENDID},
	{"ZMODEM", DLG_ZMODEMFRAME, ZMODEM_FIRSTID, ZMODEM_ENDID},
  } ;

//Utility function defination
void ShowProtocolFrame(HWND, int, int,BOOL) ;
static void GetDlgCtlRect(HWND, int, LPRECT) ;
void MoveProtocolFrame(HWND, int, int, int, int) ;
void RemoveProtocolFrameTextProps(HWND,int, int) ;

///////////////////////////////////////////////////////////////////////////
//Function: FILETRANSFER construction
///////////////////////////////////////////////////////////////////////////	
FILETRANSFER::FILETRANSFER (OBJECTID oiObj)
{
	oiFtns = oiObj ;
	FTNSDATA0 oldData0 ;

	if(!oiFtns || AObjGetType(oiFtns)!=OTYPE_FILETRANSFER) return;

	if (AObjGetDataCount(oiFtns)<2L)
		{
		AObjSetDataCount(oiFtns, 2L);
		}

	data0	= (FTNSDATA0*)AObjLockData(oiFtns, FTNSDATA);
	if (!data0)
		{
		if (AObjResizeData(oiFtns, FTNSDATA, sizeof(FTNSDATA0)) &&
				(data0 = (FTNSDATA0*)AObjLockData(oiFtns, FTNSDATA)) != NULL)
			{
			data0->version   	 = 0 ;//CURRENTVERSION ;
			data0->oiConn    	 = NULL ;
			data0->Status    	 = TRUE ;
			data0->fileType  	 = ASCII ; //TEXTFILE ;
			data0->option    	 = 0 ;
			data0->charDelay 	 = 0 ;
			data0->lineDelay 	 = 0 ;
			data0->lineEnd   	 = 0 ;
			data0->wrap      	 = FALSE ;
			data0->wrap_line 	 = 0 ;
			data0->maxTimeout  = 10 ;
			data0->maxTrytimes = 6 ;
			data0->crc_mode    = TRUE ;
			data0->autodownload = TRUE ;
			}
    }
	else if(AObjGetDataSize (oiFtns, FTNSDATA)!=sizeof(FTNSDATA))
	 	{
	 	oldData0 = *data0 ;
		AObjUnlockData (oiFtns, FTNSDATA) ;
		if (AObjResizeData(oiFtns, FTNSDATA, sizeof(FTNSDATA0)) &&
				(data0 = (FTNSDATA0*)AObjLockData(oiFtns, FTNSDATA)) != NULL)
			{
			*data0=oldData0 ;
			data0->crc_mode = TRUE ;
			data0->autodownload = TRUE ;
			}
		}
	
}
///////////////////////////////////////////////////////////////////////////
//Function: ~FILETRANSFER destruction
///////////////////////////////////////////////////////////////////////////
FILETRANSFER::~FILETRANSFER (void)
{
	if (data0)
		AObjUnlockData(oiFtns, 0L);
}
//////////////////////////////////////////////////////////////////////////
//Function:   DoSignalIDs
//Description: Set up the file Transfer signal table.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER::DoSignalIDs(pAObjMessage theSystem)
{
	MEMBLOCKID signalMID ;
	pAObjSignalRecord signal ;

  signalMID = (MEMBLOCKID)theSystem->message2;
	AMemSetSize(signalMID, 7*sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(signalMID);

	signal[0].theID = 0 ;
	lstrcpy(signal[0].theName, "Succeded");
  signal[1].theID = 1 ;
	lstrcpy(signal[1].theName, "Auto-Receiving");
  signal[2].theID = 2;
	lstrcpy(signal[2].theName, "Sending Disabled");
	signal[3].theID = 3 ;
	lstrcpy(signal[3].theName, "Sending Enabled") ;
	signal[4].theID = 4 ;
	lstrcpy(signal[4].theName, "Receiving Disabled");
  signal[5].theID = 5;
	lstrcpy(signal[5].theName, "Receiving Enabled");
	signal[6].theID = 6 ;
	lstrcpy(signal[6].theName, "Failed") ;
	AMemUnlock (signalMID) ;
}
//////////////////////////////////////////////////////////////////////////
//Function:   DoValidateOjectIDs
//Description: Varify the object IDs of the file Transfer.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER::DoValidateObjectIDs (
	pAObjMessage	theSystem)
{
	OBJECTID tmpObj ;

	tmpObj = data0->oiConn;
	if(tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->oiConn = tmpObj;
		}
}
//////////////////////////////////////////////////////////////////////////
//Function:   DoCheckConnObj
//Description: Check if the connection Object is included.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER::DoCheckConnObj  (
	OBJECTID      theObject)
{
	if (data0->oiConn==NULL)
		{
		AObjReportError	(theObject,
										AOBJ_CHECKOBJECT,
										A_ERROR,
										"There isn't a Connection object in the file Transfer Object.",
										0);
		}
}
//////////////////////////////////////////////////////////////////////////
//Function:    FileTransferDlg
//Description: File Transfer call back dialog subroutine.
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK FileTransferDlg (
	HWND hDlg, 
	WORD message,
	WORD wParam, 
	LONG lParam)
{
	FILETRANSFER *ftns ;

	switch (message)
		{
		case WM_INITDIALOG:
			ftns = (FILETRANSFER*)lParam ;
      SetWindowLong (hDlg, DWL_USER, lParam) ;
			ftns->InitDialog(hDlg) ;
			return FALSE ;

		case WM_COMMAND:
    	ftns = (FILETRANSFER*)GetWindowLong(hDlg, DWL_USER) ;
    	switch(wParam)
      	{
				case idConfigure:
        	ftns->DoConfigure (hDlg) ;
          return FALSE ;

				case IDOK:
          ftns->SaveDialog (hDlg) ;
          EndDialog (hDlg, TRUE) ;
	        return FALSE ;

        case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
          return FALSE ;
      	}
    }
 return FALSE ;
}
//////////////////////////////////////////////////////////////////////////
//Function:    InitDialog
//Description: Initialize the dialog parameters of the file Transfer.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER::InitDialog (HWND hDlg)
{
	OBJECTID oiConn ;
  char     objName[256] ;
	long     theGetKey = -1 ;
	WORD     index ;
	char buffer[OBJECTNAMESIZE] ;

	AObjGetName(oiFtns, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hDlg, buffer) ;
	AUtlSetTitleFont(OTYPE_FILETRANSFER, NULL, GetDlgItem(hDlg, IDD_OBJECT));
	AUtlCenterDialog(hDlg, 0);
	
	//Set status
  CheckDlgButton (hDlg, idStatus, !data0->Status) ;
	//Set the Connection Object
	SendDlgItemMessage (hDlg, idConnection, CB_INSERTSTRING, 0,  (long)"(None)") ;
	SendDlgItemMessage (hDlg, idConnection, CB_SETITEMDATA, 0, 0) ;
  SendDlgItemMessage (hDlg, idConnection, CB_SETCURSEL, 0, 0) ;
	while ((oiConn=AObjGetNextObject(OTYPE_CONN, &theGetKey))!=NULL)
    {
		AObjGetName (oiConn, objName, 255) ;
    index = (WORD)SendDlgItemMessage (hDlg, idConnection, CB_INSERTSTRING, -1, (long)objName) ;
    SendDlgItemMessage (hDlg, idConnection, CB_SETITEMDATA, index, (long)oiConn) ;
		if (oiConn==data0->oiConn)
    	{
			SendDlgItemMessage (hDlg, idConnection, CB_SETCURSEL, index, 0) ;
			break ;
      }
		}
}
//////////////////////////////////////////////////////////////////////////
//Function:   DoConfigure
//Description: Call the configure dialog of the file Transfer.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER:: DoConfigure (HWND hDlg)
{
	WORD Index;
	OBJECTID connObj ;

	Index = (WORD)SendDlgItemMessage (hDlg, idConnection, CB_GETCURSEL, 0,0L) ;
	connObj= (OBJECTID)SendDlgItemMessage (hDlg, idConnection, CB_GETITEMDATA, Index, 0) ;
	if (connObj)
  	{
		DialogBoxParam(hInstance, "FileTransfer" , hDlg, (DLGPROC)FileTransferCfg, (LPARAM)this) ;
		}
	else
		{
    MessageBeep(-1) ;
    }
}
//////////////////////////////////////////////////////////////////////////
//Function:   SaveDialog
//Description: Seve the parameters of the file Transfer.
//////////////////////////////////////////////////////////////////////////
void FILETRANSFER::SaveDialog (HWND hDlg)
{
	WORD Index;

	Index = (WORD)SendDlgItemMessage (hDlg, idConnection, CB_GETCURSEL, 0,0L) ;
	data0->oiConn= (OBJECTID)SendDlgItemMessage (hDlg, idConnection, CB_GETITEMDATA, Index, 0) ;
  data0->Status = !(IsDlgButtonChecked (hDlg, idStatus)) ; 
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK FileTransferCfg (
	HWND hDlg, WORD message,
 	WORD wParam, LONG lParam)
{
	FILETRANSFER* ftns ;

	switch (message)
		{
		case WM_INITDIALOG:
			ftns = (FILETRANSFER*)lParam ;
      SetWindowLong (hDlg, DWL_USER, lParam) ;
			ftns->InitConfigure(hDlg) ;
			return TRUE ;

		case WM_COMMAND:
			ftns = (FILETRANSFER*)GetWindowLong (hDlg, DWL_USER) ;
			switch (wParam)
				{
				case idProto:
        	ftns->ChangeProtocol(hDlg) ;
        	return FALSE;
				case IDOK:
					ftns->SaveFileTransferCfg (hDlg) ;
     			EndDialog (hDlg, TRUE) ;
     			return FALSE ;

    		case IDCANCEL:
					ftns->VarifyProtocol(hDlg) ;
					EndDialog (hDlg, FALSE) ;
					return TRUE ;
			 }
    }
  return FALSE ;
}
void FILETRANSFER::InitConfigure(HWND hDlg)
{
	int i ;
	int numProtocol = sizeof(protocolFrame)/sizeof(protocolFrame[0]) ;
										//sizeof(cstProtocol)/sizeof(cstProtocol[0]) ;

	for (i=0;i<numProtocol; i++)
  	{
		WORD index = (WORD)SendDlgItemMessage (
																		hDlg,
																		idProto,
																		CB_INSERTSTRING,
																		-1,
																		(long)protocolFrame[i].lpszName) ; //cstProtocol[i]) ;
		if (data0->fileType==i)
			SendDlgItemMessage (
													hDlg,
													idProto,
												 	CB_SETCURSEL,
													index,
													0) ;
		if (i != data0->fileType)
    	{
      // If this is not the current protocol to be initially shown, then hide it.
      ShowProtocolFrame(
      						hDlg, 
      						protocolFrame[i].nFirstID, 
      						protocolFrame[i].nLastID, 
      						FALSE);
    	}
    // Move the category controls from outside to the inside of the
    // dialog box.
		MoveProtocolFrame(
								hDlg, 
								protocolFrame[i].nFirstID, 
								protocolFrame[i].nLastID,
      					protocolFrame[i].nFrameID, 
      					DLG_FRAME);
		}
	ShowProtocolParam (hDlg, data0->fileType) ;
}
void FILETRANSFER::ShowProtocolParam (HWND hDlg, int protocolID) //, int endloop, BOOL show)
{
	
	if (protocolID==ASCII)
		{
    ShowAsciiParam(hDlg) ;
	  }
	else if(protocolID==XMODEM||protocolID==YMODEM||protocolID==KERMIT)
		{
    ShowBinaryParam(hDlg, protocolID) ;
		}
	else if (protocolID==ZMODEM)
		{
		ShowZmodemParam(hDlg) ;
		//DialogBoxParam(hInstance, "ZMODEM" , hDlg, (DLGPROC)ZMODEMDlg, (LPARAM)this) ;
		//ShowWindow (GetDlgItem(hDlg, 111), show?SW_SHOW:SW_HIDE) ;
		//ShowZmodemParam(hDlg) ;
		}

}
void FILETRANSFER::ShowAsciiParam(HWND hDlg)
{
	int i ;

	SendDlgItemMessage (hDlg,
											idLineEnd,
											CB_RESETCONTENT,
											0,
											0L) ;
	for (i=0;i<3; i++)
 		{
		WORD index = (WORD) SendDlgItemMessage (hDlg,
																						idLineEnd,
																						CB_INSERTSTRING,
																						-1,
																						(long)cstLineEnd[i]) ;
		if (data0->option==i)
			SendDlgItemMessage (hDlg,
													idLineEnd,
												 	CB_SETCURSEL,
													index,
													0) ;
    }
	SetDlgItemInt(hDlg, idCharTime, (UINT)data0->charDelay, TRUE) ;
	SetDlgItemInt(hDlg, idLineTime, (UINT)data0->lineDelay, TRUE) ;
	SetDlgItemInt (hDlg, idLines, data0->wrap_line, TRUE) ;
  CheckDlgButton (hDlg, idWrapLines, data0->wrap) ;
}
void FILETRANSFER::ShowBinaryParam(HWND hDlg, int protocolID)
{
	int i ;
	int firstID = (protocolID==KERMIT)?(KERMIT_FIRSTID):(protocolID==XMODEM?XMODEM_FIRSTID:YMODEM_FIRSTID);
	 
	if (protocolID!=KERMIT)
  	{
		SendDlgItemMessage (hDlg,
												firstID+idOption,
												CB_RESETCONTENT,
												0,
												0L) ;
		for (i=0;i<(protocolID==XMODEM?3:2); i++)
 			{
			WORD index = (WORD)SendDlgItemMessage (hDlg,
                      											firstID+idOption,
																						CB_INSERTSTRING,
																						-1,
																						(long)(protocolID==XMODEM?cstXmodem[i]:cstYmodem[i])) ;
			if (data0->option==i)
				SendDlgItemMessage (hDlg,
														firstID+idOption,
														CB_SETCURSEL,
														index,
														0) ;
			}
    }
	SetDlgItemInt(hDlg, firstID+idTimeout, data0->maxTimeout, TRUE) ;
  SetDlgItemInt(hDlg, firstID+idTryTimes, data0->maxTrytimes, TRUE) ;
 
}
void FILETRANSFER::ShowZmodemParam (HWND hDlg)
{
	int  firstID = ZMODEM_FIRSTID ;
	int  selectID = data0->crc_mode? firstID+2:firstID+1 ;
	
	CheckRadioButton (hDlg, firstID+1, firstID+2, selectID) ;
	CheckDlgButton (hDlg, firstID+3, data0->autodownload) ; 
}

void FILETRANSFER::ChangeProtocol(HWND hDlg)
{
	int  index ;
	int  nCurProtocol = data0->fileType ;
	RECT rc ;

	index = (int)SendDlgItemMessage (
																		hDlg,
																		idProto,
																		CB_GETCURSEL,
	 																	0,
																		0L) ;
	if (index != nCurProtocol && index != LB_ERR)
		{
	  SetWindowRedraw(hDlg, FALSE);
		ShowProtocolFrame(
								hDlg, 
								protocolFrame[nCurProtocol].nFirstID,
		      			protocolFrame[nCurProtocol].nLastID, 
		      			FALSE);
    nCurProtocol = index ;
    ShowProtocolFrame(
    						hDlg, 
    						protocolFrame[nCurProtocol].nFirstID,
		      			protocolFrame[nCurProtocol].nLastID, 
		      			TRUE);
    SetWindowRedraw(hDlg, TRUE);
    GetWindowRect(
    							GetDlgItem(hDlg, DLG_FRAME),
						      &rc);
		MapWindowRect(NULL, hDlg, &rc);
		InvalidateRect(hDlg, &rc, TRUE);
		}
	ShowProtocolParam (hDlg, nCurProtocol) ;
	data0->fileType = nCurProtocol ;
}
void FILETRANSFER::SaveFileTransferCfg(HWND hDlg)
{
	int protocol ;

	protocol = (int)SendDlgItemMessage (hDlg,
																			 idProto,
																			 CB_GETCURSEL,
																			 0,
																			 0L) ;
  data0->fileType = protocol ;
	switch(protocol)
		{
		case ASCII:
    	SaveAsciiParam(hDlg) ;
    	break ;
		case XMODEM:
		case YMODEM:
		case KERMIT:
			SaveXYKParam(hDlg, protocol) ;
    	break ;
		case ZMODEM:
    	SaveZmodemParam(hDlg) ;
    	break ;
    }
	VarifyProtocol(hDlg) ;
}
void FILETRANSFER::SaveAsciiParam(HWND hDlg)
{
	int error ;

	data0->maxTrytimes =GetDlgItemInt(hDlg, idTryTimes, &error, TRUE) ;
	data0->charDelay=GetDlgItemInt(hDlg, idCharTime, &error, TRUE) ;
	data0->lineDelay=GetDlgItemInt(hDlg, idLineTime, &error, TRUE) ;
	if (IsDlgButtonChecked (hDlg, idWrapLines))
			data0->wrap_line= GetDlgItemInt (hDlg, idLines, &error, TRUE) ;
		else
			data0->wrap_line = 0 ;
	data0->lineEnd = (int)SendDlgItemMessage(hDlg,
														 	 						 idLineEnd,
																			     CB_GETCURSEL,
																					 0,
																			     0L) ;
}
void FILETRANSFER::SaveXYKParam(HWND hDlg, int protocolID)
{
	int error ;
	int firstID = (protocolID==KERMIT)?(KERMIT_FIRSTID):(protocolID==XMODEM?XMODEM_FIRSTID:YMODEM_FIRSTID);

	data0->maxTimeout = GetDlgItemInt(hDlg, firstID+idTimeout, &error, TRUE) ;
	data0->maxTrytimes =GetDlgItemInt(hDlg, firstID+idTryTimes, &error, TRUE) ;

	if (protocolID!=KERMIT)
		data0->option = (int)SendDlgItemMessage(hDlg,
																						firstID+idOption,
																						CB_GETCURSEL,
																						0,
																						0L) ;
}
void FILETRANSFER::SaveZmodemParam(HWND hDlg)
{
	int firstID = ZMODEM_FIRSTID ;
	data0->autodownload = IsDlgButtonChecked (hDlg, firstID+3) ;
	data0->crc_mode = IsDlgButtonChecked (hDlg, firstID+2) ;
}
void FILETRANSFER::VarifyProtocol(HWND hDlg)
{
	int i ;
	int numProtocol = sizeof(protocolFrame)/sizeof(protocolFrame[0]) ;

	for(i=0; i<numProtocol; i++)
	  {
		RemoveProtocolFrameTextProps(
																hDlg, 
																protocolFrame[i].nFirstID, 
																protocolFrame[i].nLastID) ;	  
	  }
}

///////////////////////////////////////////////////////////////////////////
//         Utility function of change the protocol frame
//
//Function:    ShowProtocolFrame
//Description: Show or hide the protocol frame
///////////////////////////////////////////////////////////////////////////
void ShowProtocolFrame(
	HWND hwndDlg, 
	int nFirstID, 
	int nLastID, 
	BOOL fShow)
{
  BOOL          fCtlText;
  int           i;
  HWND          hwndCtl;
  int           nLen;
  LPSTR         lpszText;
  HGLOBAL       hText;
  UINT          dwCtlCode;

  for (i = nFirstID; i <= nLastID; i++)
  	{
    hwndCtl = GetDlgItem(hwndDlg, i);

    if (hwndCtl)
    	{
      // Find out if the control is potential danger for mnemonic
      // conflicts. This is especially to get around a bug in Windows
      // which yields focus to hidden controls when they share
      // mnemonics with visible controls on a dialog box. 
			fCtlText = TRUE;
			dwCtlCode = (UINT) SendMessage(hwndCtl, WM_GETDLGCODE, 0, 0);
			if (dwCtlCode & DLGC_WANTCHARS)
				fCtlText = FALSE;
			if ((dwCtlCode & DLGC_STATIC) && (GetWindowStyle(hwndCtl) & SS_NOPREFIX))
				fCtlText = FALSE;

      if (fShow)
      	{	// Restore the control's text?
				if (fCtlText)
					{// Get the text from the property in which it was saved.
				  hText = GetProp(hwndCtl, SZ_TEXTPROP);
				  if (hText)
	  				{ // Restore the text and then free the allocation for the
	    				// text and the property.
						lpszText = (LPSTR) GlobalLock(hText);
				    SetWindowText(hwndCtl, lpszText);
	    			GlobalUnlock(hText);
	    			RemoveProp(hwndCtl, SZ_TEXTPROP);
	    			LocalFree(hText);
	  				}
				}
			ShowWindow(hwndCtl, SW_SHOW);
      }
    else
      {
			ShowWindow(hwndCtl, SW_HIDE);
			if (fCtlText)	// Save and destroy the control's text?
				{
	  		// Get the length of the window text, make an equivalent
	  		// allocation and then assign the handle to a property.
				nLen = GetWindowTextLength(hwndCtl) + 1;
	  		hText = GlobalAlloc(GHND, nLen * sizeof(char));
	  		SetProp(hwndCtl, SZ_TEXTPROP, hText);
				if (hText)
				  {
	    		lpszText = (LPSTR) GlobalLock(hText);
	    		GetWindowText(hwndCtl, lpszText, nLen);
	    		GlobalUnlock(hText);
					// Destroy the control's text.
					SetWindowText(hwndCtl, "");
	  			}
				}
      }
    }
  }
}
///////////////////////////////////////////////////////////////////////////
//Function: MoveProtocolFrame
//Description:
//      Moves controls whose ID fall within a given range from one
//      location to another. The displacement in location is measures
//      by using two controls on the dialog box as relative references.
///////////////////////////////////////////////////////////////////////////
void MoveProtocolFrame( 
	HWND    hwndDlg,
	int     nFirstID,
	int     nLastID,
	int     nSrcID,
	int     nDestID)
{
  int   i;
  RECT  rcSrc;
  RECT  rcDest;
  RECT  rcCtl;
  int   nDeltaX;
  int   nDeltaY;
  HWND  hwndCtl;

  // Get the source and destination rectangle coordinates
	GetDlgCtlRect(hwndDlg, nSrcID, &rcSrc);
  GetDlgCtlRect(hwndDlg, nDestID, &rcDest);

  // Calculate the offset by which to move each control
  nDeltaX = rcDest.left - rcSrc.left;
  nDeltaY = rcDest.top - rcSrc.top;
  for (i = nFirstID; i <= nLastID; i++)
  	{
    hwndCtl = GetDlgItem(hwndDlg, i);
		if (hwndCtl)
    	{
      GetDlgCtlRect(hwndDlg, i, &rcCtl);
			// Displace the control
			SetWindowPos(
				hwndCtl, 
				NULL, 
				rcCtl.left + nDeltaX,
				rcCtl.top + nDeltaY, 
				0, 
				0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    	}
  	}
}
static void GetDlgCtlRect(
	HWND    hwndDlg,
	int     nID,
	LPRECT  lprc)
{
  GetWindowRect(GetDlgItem(hwndDlg, nID), lprc);
  MapWindowRect(NULL, hwndDlg, lprc);
}
///////////////////////////////////////////////////////////////////////////
//Function: MoveProtocolFrame
//Description: Clears the text save property and allocations for the given
//             range of controls. 
///////////////////////////////////////////////////////////////////////////
void RemoveProtocolFrameTextProps(
	HWND hwndDlg, 
	int  nFirstID, 
	int  nLastID)
{
  int           i;
  HWND          hwndCtl;
  HGLOBAL       hlocText;

  for (i = nFirstID; i <= nLastID; i++)
  {
    hwndCtl = GetDlgItem(hwndDlg, i);
    if (hwndCtl)
    {
      hlocText = GetProp(hwndCtl, SZ_TEXTPROP);
      if (hlocText)
      {
			RemoveProp(hwndCtl, SZ_TEXTPROP);
			GlobalFree(hlocText);
      }
    }
  }
}

