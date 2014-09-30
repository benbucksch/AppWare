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
// FILE:"fun_tmnl.cpp"
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

#include <stdio.h>
#include <dos.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "terminal.h"
#define format1 "Terminal Mode %s Font Face Name %s Column Width %d Cursor %s Line Wrap %s LocalEcho%s \
InBound %s OutBound %s BufferSize %d Scroll bar %s Translate language %s"
#define errorFormat "There is an error in the text specifying \
the connection Configuration at character position %d."

extern BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ TerminalConfigureDialog (HWND, WORD, WORD, LONG) ;
extern "C"
{
	void ALMAPI Tmnl_ConfigureFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_ClearFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_ReceiveTextFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_ResetFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_CaptureTextFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_EndCaptureTextFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Tmnl_GetConfigFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Tmnl_SetConfigFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Tmnl_GetTerminalTextFunc (pAEvtInfo, pAObjMessage) ;
}

void PostTerminalEvent (
	pAEvtInfo theEvent,
	AOBJECTID oiTerminal,
	long details,
	long modifiers)
{
		theEvent->targetObject = oiTerminal ;
		theEvent->what         = AEVENT_OBJECTCHANGED ;
		theEvent->details      = details ;
    theEvent->modifiers    = modifiers ;
		theEvent->how          = AEVT_ATMARK ;

    AEvtPost (theEvent) ;
}
BOOL OpenCaptureFile (HANDLE *fh)
{
  char            lpFileName[255] ;
	OPENFILENAME    ofn;
	BOOL            ok ;

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetActiveWindow() ;
	ofn.lpstrFilter       = NULL;
	ofn.lpstrFilter       = "Text file (*.txt)\0*.txt\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
  lpFileName[0]         = '\0' ;
  ofn.lpstrFile         = (LPSTR)lpFileName;
	ofn.nMaxFile          = 128;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = "Capture Text File";
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrDefExt       = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_HIDEREADONLY;

  ok = GetSaveFileName(&ofn);
	if (lpFileName)
		{
		*fh = CreateFile (
										lpFileName, 
										GENERIC_WRITE, 
										0, 
										NULL, 
										CREATE_NEW, 
										FILE_ATTRIBUTE_NORMAL, 
										NULL) ;
		if (*fh==INVALID_HANDLE_VALUE)
			ok = FALSE ;
			//SetFilePointer(*fh,0,0) ;
    }
	return (ok) ;
}
void SetupConfigureText (AOBJECTID oiTerminal, LPSTR lpConfigure)
{
	TERMDATA0* data0 ;
	char       fontface[40] ;
	char      *terminalMode ;
	int        ColumnWidth ;
	char       Cursor[20] ;
	char       LineWrap[20] ;
	char       LocalEcho[20] ;
	char       inbound[20] ;
	char       outbound[20] ;
	int        buffersize ;
	char      *language ;
  char       scrollbar[20] ;

	data0 = (TERMDATA0*)AObjLockData (oiTerminal, TMNLDATA) ;
	if (data0)
		{
		terminalMode = cstEmulation[data0->Emulation] ;
		lstrcpy (fontface, (char*)data0->logFont.lfFaceName) ;
		language = cstTranslations[data0->translate] ;
		ColumnWidth = data0->columns?80:132 ;
		lstrcpy (Cursor, data0->curShape?"Block":"UnderLine") ;
		lstrcpy (LineWrap, data0->Mode&LINEWRAP?"TRUE":"FALSE") ;
		lstrcpy (LocalEcho, data0->Mode&LOCALECHO?"TRUE":"FALSE") ;
		lstrcpy (inbound, data0->CRorLF&INBOUND?"TRUE":"FALSE") ;
		lstrcpy (outbound, data0->CRorLF&OUTBOUND?"TRUE":"FALSE") ;
		buffersize = data0->bufferSize ;
		lstrcpy (scrollbar, data0->scrollBar&VSCROLL?"TRUE":"FALSE") ;
		wsprintf (lpConfigure,
						format1,
						terminalMode,
						fontface,
						ColumnWidth,
						Cursor,
						LineWrap,
						LocalEcho,
						inbound,
            outbound, 
						buffersize,
            scrollbar,
						language) ;
		AObjUnlockData (oiTerminal, TMNLDATA) ;
    }
}
int GetDataString(LPSTR lpConfigure, char* text, char *result, char* cstResult[], int size)
{
	LPSTR lpstr ;
	char *p ;
  char  errorMsg[256] ;
	int   ok = 0 ;
	WORD  i ;
  int   charP=0 ;

  p = result ;
  *result ='\0' ; 
	lpstr = _fstrstr(lpConfigure, (LPSTR)text) ;
	if (lpstr)
		{
		charP = (int)(lpstr-lpConfigure) ;
    lpstr+=lstrlen(text) ;
		if (*(lpstr++)!=' ')
			{
      charP++ ;
			wsprintf (errorMsg, errorFormat, (lpstr-lpConfigure)) ;
			MessageBox (NULL, errorMsg, "Error", MB_OK) ;
			ok = -1 ;
			}
		else
			{
      while (*lpstr!='\0')
				{
        charP++ ;
				if (*lpstr==' '&&lstrlen(result)>0)
					{
					*result='\0' ;
					break ;
          }
				if (*lpstr!=' ')
					*result++=*lpstr ;
				lpstr++ ;
        }
			}
		if (*lpstr=='\0')
			*result='\0' ;
		result = p ;
		if (ok!=-1)
			{
			for (i=0; i<size; i++)
				{
      	ok = -1 ;
				if (!lstrcmp(strupr(cstResult[i]), strupr(result)))
					{
        	ok = i ;
					break;
					}
				}
			if (ok==-1)
    		{
				wsprintf (errorMsg, errorFormat, (charP-lstrlen(result))) ;
				MessageBox (NULL, errorMsg, "Error", MB_OK) ;
      	}
			}
    }
  return (ok) ;
}
void SaveConfigureText (AOBJECTID oiTerminal, LPSTR lpConfigure)
{
	TERMDATA0 *data0 ;
	int  result ;
	char str[256] ;
	char *endPtr ;
	int tmp ;

	_fstrupr(lpConfigure) ;
	data0= (TERMDATA0*)AObjLockData (oiTerminal, TMNLDATA) ;
	if (data0)
		{
		result = GetDataString (lpConfigure, "TERMINAL MODE", str, cstEmulation, 3) ;
		if (result>0) 
			data0->Emulation = result;
		result = GetDataString (lpConfigure, "LANGUAGE", str, cstTranslations, 11) ;
		if (result>0) 
			data0->translate = result;
		result = GetDataString (lpConfigure, "COLUMN WIDTH", str, NULL, 0) ;
		if (result>0)
			{
      tmp = (int)strtoul(str, &endPtr, 10) ;
			data0->columns = tmp<=80?Columns80:Columns132 ;
			}
		result = GetDataString (lpConfigure, "CURSOR", str, NULL, 0) ;
		if (result>0)
			{
			if (!lstrcmp(str,"Block"))
				data0->curShape = BLOCK ;
			else if (!lstrcmp(str, "UnderLine"))
				data0->curShape = UNDERLINE ;
			else
      	MessageBox (NULL, "Can not set Cursor shape, Use the default settings", "Error", MB_OK) ;
			}
		result = GetDataString (lpConfigure, "LINEWRAP", str, NULL, 0) ;
		if (result>0)
			{
			if (!lstrcmp(str,"TRUE"))
				data0->Mode |= LINEWRAP ;
      }
		result = GetDataString (lpConfigure, "LOCAL ECHO", str, NULL, 0) ;
		if (result>0)
    	{
			if(!lstrcmp(str, "TRUE"))
				data0->Mode |= LOCALECHO;
			}
		result = GetDataString (lpConfigure, "BUFFER SIZE", str, NULL, 0) ;
		if (result>0)
			{
			tmp = (int)strtoul(str, &endPtr, 10) ; 
			data0->bufferSize = tmp;
			}
		result = GetDataString (lpConfigure, "INBOUND", str, NULL, 0) ;
		if (result>0)
			{
      if (!lstrcmp(str, "TRUE"))
				data0->CRorLF |= INBOUND ;
      }
		result = GetDataString (lpConfigure, "OUTBOUND", str, NULL, 0) ;
		if (result>0)
			{ 
			if (!lstrcmp(str, "TRUE"))
				data0->CRorLF |= OUTBOUND;
      }
		result = GetDataString (lpConfigure, "SCROLL BAR", str, NULL, 0) ;
		if (result>0)
			{
      if (!lstrcmp(str, "TRUE"))
				data0->scrollBar |= VSCROLL;
      }
		AObjUnlockData (oiTerminal, TMNLDATA) ;
		}
}
//
//
//
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_ConfigureFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
		AOBJECTID  oiTerminal ;
		TERMINAL* tmnl ;
		BOOL     ok ;

		oiTerminal = AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
		tmnl = new TERMINAL (oiTerminal) ;
		if (tmnl)
			{
			ok=DialogBoxParam(hInstance, "Terminal" , GetActiveWindow(), (DLGPROC)TerminalConfigureDialog, (LPARAM)tmnl) ;
			if (ok)
				{
				AFuncReportResult (theSystem, 1, NULL) ;
				PostTerminalEvent (theEvent, oiTerminal, (long)changeConfigure, 0L) ;
				}
			else
				AFuncReportResult (theSystem, 2, NULL) ;
			delete tmnl ;
			}
}
//
//
//
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_ClearFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
		AOBJECTID oiTerminal ;

		oiTerminal = AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
		PostTerminalEvent (theEvent, oiTerminal, (long) clearTerminal, 0L) ;
    AFuncReportResult (theSystem, 1, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_ReceiveTextFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
		AOBJECTID oiTerminal ;
    AOBJECTID oiText ;

		oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
		oiText = AFuncGetTypedParameter (2, OTYPE_TEXT) ;

		PostTerminalEvent (theEvent, oiTerminal, (long)receiveText, (long)oiText);
    AFuncReportResult (theSystem, 1, NULL) ;

}
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_ResetFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;
	//BOOL     cancel ;

	oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	PostTerminalEvent (theEvent, oiTerminal, (long)resetTerminal, 0L) ;
	AFuncReportResult (theSystem, 1, NULL) ;
}
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_CaptureTextFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;
  HANDLE   hf ;
	BOOL     ok ;

	oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	ok = OpenCaptureFile (&hf) ;
	PostTerminalEvent (theEvent, oiTerminal, (long)startCapture, (long)hf) ;
	if (ok)
		AFuncReportResult (theSystem, 1, NULL) ;
	else
		AFuncReportResult (theSystem, 2, NULL) ;
}
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tmnl_EndCaptureTextFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;

	oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	AFuncReportResult(theSystem, 1, NULL) ;
	PostTerminalEvent (theEvent, oiTerminal, (long)endCapture, 0L) ;
}
void ALMAPI Tmnl_GetConfigFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;
	LPSTR    lpConfigure ;

	I_UNUSED (theEvent) ;
	oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	lpConfigure = (LPSTR) new char[500] ;
	if (lpConfigure)
		{
		SetupConfigureText (oiTerminal, lpConfigure) ;
		AOBJECTID oiText = AObjCreate (OTYPE_TEXT) ;
		if (oiText)
			{
			OTextSetValueFromBuffer (oiText, lpConfigure) ;
			AFuncSetParameter (1, oiText) ;
			}
    delete []lpConfigure ;
    }
	AFuncReportResult(theSystem, 1, NULL) ;
}
void ALMAPI Tmnl_SetConfigFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;
	AOBJECTID oiConfigure ;
	LPSTR    lpConfigure ;
	WORD     size ;

	I_UNUSED (theEvent) ;

	oiTerminal = AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	oiConfigure = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	size = OTextGetLength (oiConfigure) ;
	lpConfigure = (LPSTR) new char[size+1] ;

	if (lpConfigure)
		{
		OTextGetToBuffer (oiConfigure, lpConfigure, size+1) ;
 		SaveConfigureText (oiTerminal, lpConfigure) ;
    delete[]lpConfigure ;
    }
	AFuncReportResult (theSystem, 1, NULL) ;
}
void ALMAPI Tmnl_GetTerminalTextFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	AOBJECTID oiTerminal ;

	I_UNUSED(theEvent) ;

	oiTerminal =AFuncGetTypedParameter (1, OTYPE_TERMINAL) ;
	if (oiTerminal)
		{
		AOBJECTID oiText = AObjCreate (OTYPE_TEXT) ;
		RUNTMNL *run_tmnl = (RUNTMNL*)AObjGetRuntimeInfo(oiTerminal) ;
		if (run_tmnl)
    	{
			LPSTR lpText ;
			WORD  maxChars = run_tmnl->bMaxCol*run_tmnl->bMaxRow ;
			lpText = (LPSTR)new char [maxChars+1] ;
			if (lpText)
  			{
				CopyMemory (lpText, run_tmnl->termBuffer, maxChars) ;
				lpText[maxChars] = '\0' ;
				OTextSetValueFromBuffer (oiText, lpText) ;
				AFuncSetParameter (1, oiText) ;
    		delete[] lpText ;
    		}
			}
    }
	AFuncReportResult(theSystem, 1, NULL) ;
}
