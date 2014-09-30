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
// FILE:"terminal.cpp"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#define CONST

#include <commDlg.h>
#include <string.h>
#include "com_dlg.h"
#include "terminal.h"

typedef UINT (far pascal *HOOKFUNC)(HWND, UINT, WPARAM, LPARAM);
UINT far pascal _export terminalEditFontHook(HWND, UINT message, WPARAM wParam, LPARAM lParam) ;
void DrawSampleText(HWND hWnd, LPDRAWITEMSTRUCT di) ;

TERMINAL::TERMINAL (OBJECTID oiObj)
{
	oiTerm = oiObj ;

	if(!oiTerm || AObjGetType(oiTerm)!=OTYPE_TERMINAL) return;

	if (AObjGetDataCount(oiTerm)<2L)
		{
		AObjSetDataCount(oiTerm, 2L);
		}

	data0	= (TERMDATA0*)AObjLockData(oiTerm, TMNLDATA);
	if (!data0)
		{
		if (AObjResizeData(oiTerm, TMNLDATA, sizeof(TERMDATA0)) &&
				(data0 = (TERMDATA0*)AObjLockData(oiTerm, TMNLDATA)) != NULL)
			{
			data0->version        = 0 ;
			data0->oiConn         = 0 ;
			data0->cacheText      = 0 ;
      data0->cacheTextObj   = NULL ; 
	    data0->Emulation      = VT100 ;
			data0->Mode          |= LINEWRAP; //|SOUND ;
    	data0->curShape       = BLOCK ;
    	data0->CRorLF         = INBOUND&OUTBOUND ;
    	data0->columns        = Columns80 ;
			data0->bufferSize     = 100 ;
			data0->scrollBar      = VSCROLL ;
			data0->translate      = 0 ;
			data0->activate       = TRUE ;
			HFONT hFont = (HFONT)GetStockObject (SYSTEM_FIXED_FONT) ;
      GetObject (hFont, sizeof(LOGFONT), &data0->logFont) ;
			}
		}
}
TERMINAL::~TERMINAL (void)
{
	if (data0)
		AObjUnlockData(oiTerm, 0L);
}
long TERMINAL::CheckVersion (pAObjMessage theSystem)
{
	long rVal = A_OK ;

 	theSystem->message3 = CURRENTVERSION ;
	if (data0->version > CURRENTVERSION)
	{ AObjReportError(oiTerm, theSystem->message1, A_ERROR,
				"Terminal\tALM_COMM.DLL", 1);
    rVal = A_ERROR;
	}

	return rVal;
}
void TERMINAL::ExportObject(void)
{
 	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->version, sizeof(int));
	AUpsfExportField("CONNOBJ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->oiConn, sizeof(OBJECTID));
	AUpsfExportField("CACHE",  0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->cacheText, sizeof (BOOL)) ;
	AUpsfExportField("CACHEOBJ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->cacheTextObj, sizeof(OBJECTID)) ;
	/*AUpsfExportField("EMULATION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->Emulation, sizeof(int));
	AUpsfExportField("MODE", 0L, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &data0->Mode, sizeof(BYTE));
	AUpsfExportField("CURSHAPE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->curShape, sizeof(int));
	AUpsfExportField("COLUMNS", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->columns, sizeof(BOOL));
	AUpsfExportField("BUFSIZE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->bufferSize, sizeof(int));
	AUpsfExportField("SRLBAR", 0L, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &data0->scrollBar, sizeof(BYTE));
	AUpsfExportField("ACTIVE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->activate, sizeof(BOOL));
	AUpsfExportField("TRANS", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->translate, sizeof(int));*/
}
void TERMINAL::ImportObject(void)
{
	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &data0->version, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"VERSION\" -- defaulting to current version.");

	if (AUpsfImportField("CONNOBJ", 0L, AUPSF_NATV_OBJECTREF, &data0->oiConn, sizeof(int)) != AUPSF_NOERROR)
		{
    data0->oiConn = NULL ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if (AUpsfImportField("CACHE", 0L, AUPSF_NATV_BOOL, &data0->cacheText, sizeof(BOOL)) != AUPSF_NOERROR)
		{
    data0->cacheText = FALSE ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if (AUpsfImportField("CACHEOBJ", 0L, AUPSF_NATV_OBJECTREF, &data0->cacheTextObj, sizeof(OBJECTID)) != AUPSF_NOERROR)
		{
    data0->cacheTextObj = NULL ;
		AUpsfLogComment("Error importing -- using default.");
		}
	/*if(AUpsfImportField("EMULATION",0L, AUPSF_NATV_INT, &data0->Emulation, sizeof(int))!=AUPSF_NOERROR)
		{
		data0->Emulation = VT100 ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("MODE", 0L, AUPSF_NATV_BYTE, &data0->Mode, sizeof(BYTE))!=AUPSF_NOERROR)
		{
		data0->Mode |= LINEWRAP;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("CURSHAPE", 0L, AUPSF_NATV_BOOL, &data0->curShape, sizeof(BOOL))!=AUPSF_NOERROR)
		{
		data0->curShape = BLOCK ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("COLUMNS", 0L, AUPSF_NATV_INT, &data0->columns, sizeof(int))!=AUPSF_NOERROR)
		{
		data0->columns = Columns80 ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("BUFSIZE", 0L, AUPSF_NATV_INT, &data0->bufferSize, sizeof(int))!=AUPSF_NOERROR)
		{
		data0->bufferSize = 100 ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("SRLBAR", 0L, AUPSF_NATV_BYTE, &data0->scrollBar, sizeof(BYTE))!=AUPSF_NOERROR)
		{
		data0->scrollBar |= VSCROLL ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("ACTIVE", 0L, AUPSF_NATV_BOOL, &data0->activate, sizeof(BOOL))!=AUPSF_NOERROR)
		{
		data0->activate = TRUE ;
		AUpsfLogComment("Error importing -- using default.");
		}
	if(AUpsfImportField("TRANS", 0L, AUPSF_NATV_INT,  &data0->translate, sizeof(int))!=AUPSF_NOERROR)
		{
		data0->translate = 0 ;
		AUpsfLogComment("Error importing -- using default.");
		}*/
}

void TERMINAL::DoValidateObjectIDs (
	pAObjMessage	theSystem)
{
	OBJECTID tmpObj ;

	tmpObj = data0->oiConn;
	if(tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->oiConn = tmpObj;
		}
	tmpObj = data0->cacheTextObj ;
	if(tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->cacheTextObj = tmpObj;
		}      
}
void TERMINAL::DoCheckConnObj  (
	OBJECTID      theObject)
{
	if (data0->oiConn==NULL)
		{
		AObjReportError	(theObject,
										AOBJ_CHECKOBJECT,
										A_ERROR,
										"There isn't a Connection object in the terminal Object.",
										0);
		}
}
BOOL ALMCALLBACK TerminalDialog (HWND hDlg, WORD message,
																WORD wParam, LONG lParam)
{
	TERMINAL* tmnl ;

	switch (message)
		{
		case WM_INITDIALOG:
			tmnl = (TERMINAL *) lParam ;
      SetWindowLong (hDlg, DWL_USER, lParam) ;
			tmnl->InitTerminalDialog(hDlg) ;
			return FALSE ;

		case WM_COMMAND:
    	tmnl = (TERMINAL *)GetWindowLong(hDlg, DWL_USER) ;
    	switch(wParam)
				{
				case idCacheText:
					tmnl->CacheText(hDlg) ;
					return FALSE ;

				case idConfigure:
					tmnl->TerminalConfigure (hDlg) ;
					return FALSE ;
							
        case IDOK:
       		tmnl->SaveTerminal (hDlg) ;
          EndDialog (hDlg, TRUE) ;
          return FALSE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
          return FALSE ;
        }
   }

	return FALSE ;
}
void TERMINAL::InitTerminalDialog (HWND hDlg)
{
	char buffer[OBJECTNAMESIZE] ;

	AUtlSetTitleFont(OTYPE_TERMINAL, NULL, GetDlgItem(hDlg, IDD_OBJECT));
	AObjGetName(oiTerm, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hDlg, buffer) ;
	AUtlCenterDialog(hDlg, 0);
	
	//Set the Connection Object
	SetConnectionObj (hDlg) ;
	SetCacheTextObj (hDlg) ;
}
void TERMINAL::SetConnectionObj (HWND hDlg)
{
	OBJECTID oiConn ;
	long     theGetKey = -1 ;
	WORD     index ;
	char     objName[256] ;

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
void TERMINAL::SetCacheTextObj (HWND hDlg)
{
	long     theGetKey = -1 ;
	WORD     index ;
	char     objName[256] ;
	OBJECTID oiText ;

	CheckDlgButton (hDlg, idCacheText, data0->cacheText) ;
	EnableWindow (GetDlgItem(hDlg, idCacheTextObj), data0->cacheText) ;
	SendDlgItemMessage (hDlg, idCacheTextObj, CB_INSERTSTRING, 0,  (long)"(None)") ;
  SendDlgItemMessage (hDlg, idCacheTextObj, CB_SETITEMDATA, 0, 0) ;
  SendDlgItemMessage (hDlg, idCacheTextObj, CB_SETCURSEL, 0, 0) ;
	while ((oiText=AObjGetNextObject(OTYPE_TEXT, &theGetKey))!=NULL)
    {
		AObjGetName (oiText, objName, 255) ;
    index = (WORD)SendDlgItemMessage (hDlg, idCacheTextObj, CB_INSERTSTRING, -1, (long)objName) ;
    SendDlgItemMessage (hDlg, idCacheTextObj, CB_SETITEMDATA, index, (long)oiText) ;
		if (oiText==data0->cacheTextObj)
    	{
			SendDlgItemMessage (hDlg, idCacheTextObj, CB_SETCURSEL, index, 0) ;
			break ;
      }
		}
}
void TERMINAL::CacheText (HWND hDlg)
{
	BOOL checked ;

	checked = IsDlgButtonChecked (hDlg, idCacheText) ;
	EnableWindow (GetDlgItem(hDlg, idCacheTextObj), checked) ;
}
void TERMINAL::TerminalConfigure (HWND hDlg)
{
	BOOL ok ;
	TERMDATA0 *tmpdata0 ;
  WORD      Index ;
	OBJECTID  connObj ;

	Index = (WORD)SendDlgItemMessage (hDlg, idConnection, CB_GETCURSEL, 0,0L) ;
	connObj = (OBJECTID)SendDlgItemMessage (hDlg, idConnection, CB_GETITEMDATA, Index, 0) ;
	if (connObj)
  	{
		tmpdata0 = new TERMDATA0;
		if (tmpdata0)
			 *tmpdata0 = *data0 ;
		ok = DialogBoxParam(hInstance, "Terminal" , hDlg, (DLGPROC)TerminalConfigureDialog, (LPARAM)this) ;
		if (!ok)
			{
			*data0 = *tmpdata0 ;
			}
		if (tmpdata0)
			delete tmpdata0 ;
		}
	else
  	MessageBeep(-1) ;

}
void TERMINAL::SaveTerminal (HWND hDlg)
{
	WORD Index ;

	Index = (WORD)SendDlgItemMessage (hDlg, idConnection, CB_GETCURSEL, 0,0L) ;
	data0->oiConn= (OBJECTID)SendDlgItemMessage (hDlg, idConnection, CB_GETITEMDATA, Index, 0) ;
	data0->cacheText = IsDlgButtonChecked (hDlg, idCacheText) ;
	Index = (WORD)SendDlgItemMessage (hDlg, idCacheTextObj, CB_GETCURSEL, 0,0L) ;
	data0->cacheTextObj = (OBJECTID)SendDlgItemMessage (hDlg, idCacheTextObj, CB_GETITEMDATA, Index, 0) ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK TerminalConfigureDialog (HWND hDlg, WORD message,
                                        WORD wParam, LONG lParam)
{
	TERMINAL* tmnl ;

	switch (message)
		{
		case WM_INITDIALOG:
				tmnl = (TERMINAL*)lParam ;
        SetWindowLong(hDlg, DWL_USER, lParam) ; 
    		tmnl->InitTerminalConfiguration(hDlg) ;
			 	return FALSE ;

		case WM_COMMAND:
    		tmnl = (TERMINAL*)GetWindowLong(hDlg, DWL_USER) ;
        switch (wParam)
				{
					case idFont:
							tmnl->EditTerminalFont (hDlg) ;
							return FALSE ;

					case IDOK:
							tmnl->SaveTerminalConfiguration(hDlg) ;
          		EndDialog (hDlg, TRUE) ;
              return FALSE ;

          case IDCANCEL:
              EndDialog (hDlg, FALSE) ;
              return TRUE ;
       }
	}
	return FALSE ;
}
void TERMINAL::InitTerminalConfiguration(HWND hDlg)
{
	AUtlCenterDialog(hDlg, 0);
	SetEmulationType (hDlg, data0->Emulation) ;
	SetTerminalMode  (hDlg, data0->Mode) ;
	SetCursorStatus  (hDlg, data0->curShape, FALSE) ;
	SetCRorLF        (hDlg, data0->CRorLF) ;
	SetColumns       (hDlg, data0->columns) ;
	SetBufferSize    (hDlg, data0->bufferSize) ;
	SetScrollBar     (hDlg, data0->scrollBar) ;
  SetTranslation   (hDlg, data0->translate) ;
}
void TERMINAL::SaveTerminalConfiguration (HWND hDlg)
{
	data0->Emulation  = SaveEmulationType (hDlg) ;
	data0->Mode       = SaveMode (hDlg) ;
	data0->curShape   = SaveCursorShape (hDlg) ;
	data0->CRorLF     = SaveCRorLF (hDlg) ;
	data0->columns    = SaveColumns (hDlg) ;
	data0->bufferSize = SaveBufferSize (hDlg) ;
	data0->scrollBar  = SaveScrollBar (hDlg) ;
  data0->translate  = SaveTranslation (hDlg) ;
}
int FAR PASCAL GetFontSize (LPLOGFONT lplf, LPTEXTMETRIC lptm, short nFontType, LPTEXTMETRIC lpResult)
{
	I_UNUSED(lplf) ;
	I_UNUSED(nFontType) ;

	*lpResult = *lptm ;
   return 0 ;
}
void TERMINAL::EditTerminalFont (HWND hDlg)
{
	CHOOSEFONT    cfTemp ;
	LOGFONT				lfTemp = data0->logFont ;

	memset(&cfTemp, 0, sizeof(CHOOSEFONT));
	cfTemp.lStructSize 		= sizeof( CHOOSEFONT );
	cfTemp.hwndOwner   		= hDlg;
	cfTemp.hDC         		= 0;
	cfTemp.lpLogFont   		= &lfTemp;	// Store the result here
	cfTemp.Flags       		= CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK | CF_LIMITSIZE | CF_SCREENFONTS |CF_ENABLETEMPLATE | CF_FIXEDPITCHONLY ;
	cfTemp.rgbColors   		= 0 ;	      // Color and font dialogs use the same color
	cfTemp.lCustData   		= 0L;
	cfTemp.lpfnHook    		= (HOOKFUNC)terminalEditFontHook ; //0 ;
	cfTemp.lpTemplateName = MAKEINTRESOURCE(FONT_DIALOG) ;
	cfTemp.hInstance   		= hInstance ;
	cfTemp.lpszStyle   		= NULL;
	cfTemp.nFontType   		= SCREEN_FONTTYPE;
	cfTemp.nSizeMin    		= 6;
	cfTemp.nSizeMax    		= 18 ;

	if (ChooseFont( &cfTemp ) == TRUE )
		data0->logFont = lfTemp ;
}
///////////////////////////////////////////////////////////

UINT far pascal _export terminalEditFontHook(
  HWND              hWnd,
  UINT              message,
  WPARAM            wParam,
  LPARAM            lParam)
	{
	I_UNUSED(wParam) ;
	LPDRAWITEMSTRUCT di ;
  switch(message)
    {
		case WM_INITDIALOG:
			AUtlCenterDialog(hWnd, 0);
			break;

    case WM_DRAWITEM:
      di = (LPDRAWITEMSTRUCT)lParam;
      switch(di->CtlID)
        {
        case IDD_SAMPLETEXT:
          DrawSampleText(hWnd, di);
          break;
        default:
          return FALSE;
        }
      return TRUE;
		case WM_COMMAND:
    	if (HIWORD(lParam) == BN_CLICKED || HIWORD(lParam) == CBN_SELCHANGE || HIWORD(lParam) == CBN_KILLFOCUS)
      	{
      	InvalidateRect( GetDlgItem(hWnd, IDD_SAMPLETEXT), NULL, TRUE);
       	}
      return FALSE;

    default:
      break;
    }

  return 0;
  }

// DrawSampleText //////////////////////////////////////////////////////////

void DrawSampleText(
  HWND              hWnd,
  LPDRAWITEMSTRUCT  di)

  {
  int               oldMode;
  LOGFONT           lf;
  HFONT             hFont, hFontOld;
  char              sampleText[51];

  SendMessage(hWnd, WM_CHOOSEFONT_GETLOGFONT, 0, (LONG)&lf);
  hFont = CreateFontIndirect(&lf);

	oldMode = SetBkMode (di->hDC, TRANSPARENT) ;
  hFontOld = (HFONT)SelectObject(di->hDC, hFont);
  GetDlgItemText(hWnd, IDD_SAMPLETEXT, sampleText, 50);
  DrawText(di->hDC, sampleText, lstrlen(sampleText), &di->rcItem, DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	SelectObject (di->hDC, hFontOld) ;
	SetBkMode (di->hDC, oldMode) ;
  DeleteObject(hFont);
  }

void SetEmulationType (HWND hDlg, int EmulationName)
{
	int i ;
  WORD index ;

	for (i=0; i<3; i++)
    {
    index = (WORD)SendDlgItemMessage(hDlg, idEmulation, CB_INSERTSTRING, -1, (long)cstEmulation[i]) ;
    if (i==EmulationName)
      {
			SendDlgItemMessage (hDlg, idEmulation, CB_SETCURSEL, index, 0) ;
      }
		}
}
void SetTerminalMode (HWND hDlg, BYTE Mode)
{
	if (Mode&0x01)
   	CheckDlgButton (hDlg, idLineWrap, TRUE) ;
	if (Mode&0x02)
  	CheckDlgButton (hDlg, idLocalEcho, TRUE) ;
	//if (Mode&0x04)
  // 	CheckDlgButton (hDlg, idSound, TRUE) ;
}
void SetCursorStatus (HWND hDlg, int curShape, BOOL Blink)
{
	CheckRadioButton (hDlg, idBlock, idUnderline, idUnderline-curShape) ;
	if (Blink)
  	CheckDlgButton (hDlg, idBlink, TRUE) ;
}
void SetCRorLF (HWND hDlg, BYTE CRorLF)
{
	if (CRorLF&0x01)
		CheckDlgButton (hDlg, idInbound, TRUE) ;
	if (CRorLF&0x02)
  	CheckDlgButton (hDlg, idOutbound, TRUE) ;
}
void SetColumns (HWND hDlg, int columns)
{
		CheckRadioButton (hDlg, idColumn80, idColumn132, idColumn132-columns) ;
}
void SetBufferSize (HWND hDlg, int bufferSize)
{
	SetDlgItemInt (hDlg, idBufferSize, (UINT)bufferSize, FALSE) ;
}
void SetScrollBar (HWND hDlg, BYTE scrollbar)
{
	if (scrollbar&VSCROLL)
		CheckDlgButton (hDlg, idVertBar, TRUE) ;
	if (scrollbar&HSCROLL)
 		CheckDlgButton (hDlg, idHorizBar, TRUE) ;
}
void SetTranslation (HWND hDlg, int translate)
{
	int i ;
  WORD index ;

	for (i=0; i<11; i++)
    {
		index = (WORD)SendDlgItemMessage(hDlg, idTranslate, CB_INSERTSTRING, -1, (long)cstTranslations[i]) ;
    if (i==translate)
      {
			SendDlgItemMessage (hDlg, idTranslate, CB_SETCURSEL, index, 0) ;
      }
		}
}
//
//
//
int SaveEmulationType (HWND hDlg)
{
	int index ;

	index = (int)SendDlgItemMessage (hDlg, idEmulation, CB_GETCURSEL, 0, 0) ;
	if (index !=CB_ERR)
		return (index) ;
	else
		return 1 ;
}
BYTE  SaveMode (HWND hDlg)
{
	BYTE Mode=0x00 ;
  BYTE loop ;
	int  id ;

  for (id=idLineWrap,loop =0x01; id<=idSound; id++,loop *=2) 
		if (IsDlgButtonChecked(hDlg, id)) Mode |=loop ;

  return (Mode) ;
}
BOOL SaveCursorShape (HWND hDlg)
{
	return(IsDlgButtonChecked (hDlg, idBlock)) ;
}
BOOL SaveCursorBlink (HWND hDlg)
{
	return (IsDlgButtonChecked (hDlg, idBlink)) ;
}
BYTE SaveCRorLF (HWND hDlg)
{
	BYTE CRorLF=0 ;

	if (IsDlgButtonChecked (hDlg, idInbound))
		CRorLF|=0x01 ;
	if (IsDlgButtonChecked (hDlg, idOutbound))
		CRorLF|=0x02 ;

  return (CRorLF) ;
}
BOOL SaveColumns (HWND hDlg)
{
	return (IsDlgButtonChecked (hDlg, idColumn80)) ;
}
int SaveBufferSize (HWND hDlg)
{
	int bufferSize ;
	BOOL error ;

	bufferSize = GetDlgItemInt(hDlg, idBufferSize, &error, FALSE) ;
	if (error !=CB_ERR)
		return bufferSize ;
	else
 		return 100 ;
}
BYTE SaveScrollBar (HWND hDlg)
{
	BYTE scrollBar = 0x00 ;

	if (IsDlgButtonChecked (hDlg, idVertBar))
		scrollBar |=0x01 ;
	//if (IsDlgButtonChecked (hDlg, idHorizBar))
	//	scrollBar |=0x02 ;
	return (scrollBar) ;
}
int SaveTranslation (HWND hDlg)
{
	int language ;
		
	language = (int)SendDlgItemMessage (hDlg, idTranslate, CB_GETCURSEL, 0, 0) ;
	if (language !=CB_ERR)
		return (language) ;
	else
		return 0 ;
}
