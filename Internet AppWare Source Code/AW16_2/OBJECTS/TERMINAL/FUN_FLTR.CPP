//////////////////////////////////////////////////////////////
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
// AUTHOR:      Sibai Li
//
// DESCRIPTION: File Transfer Functions:
//							FT_ConfigureFunc
//              FT_CancelFunc
//              FT_PauseFunc
//              FT_ResumeFunc
//              FT_SendFileFunc
//              FT_ReceiveFileFunc
//							FT_GetConfigureFunc
//							FT_SetConfigureFunc
//							FT_SetPathFunc
//
// CHANGES:
//
//////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <commDlg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "filetrns.h"

#define errorFormat "There is an error in the text specifying \
the file transfer Configuration at character position %d."
#define asciiFormat "Ascii Protocol DelayPerChar %ld DelayPerLine %ld Ending %s \
WordWrap %s CharPerLine %d"
#define binaryFormat "%s Protocol Timeout %d Retry %d"

extern "C"
{
	void ALMAPI FT_ConfigureFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_CancelFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_PauseFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_ResumeFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_SendFileFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_ReceiveFileFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI FT_GetConfigFunc (pAObjInfo, pAObjMessage) ;
	void ALMAPI FT_SetConfigFunc (pAObjInfo, pAObjMessage) ;
	void ALMAPI FT_SetPathFunc (pAObjInfo, pAObjMessage) ;
}
extern int DoFileOpenDlg(char *) ;
extern BOOL ALMCALLBACK FileTransferCfg (HWND, WORD,WORD, LONG) ;
extern void PostEvent (OBJECTID, EVENTID, long, long, long, long) ;

void SetupConfigureText (OBJECTID oiFT, LPSTR lpConfigure)
{
	FTNSDATA0* data0 ;

	data0 = (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;
	if (data0)
		{
    int fileType =data0->fileType ;
		switch(fileType)
			{
			case ASCII:
				{
				char wordWrap[10] ;
        char lineEnd[10] ;
				lstrcpy (wordWrap, data0->wrap?"TRUE":"FALSE") ;
				lstrcpy (lineEnd, cstLineEnd[data0->lineEnd]) ;
				sprintf (lpConfigure, asciiFormat, data0->charDelay, data0->lineDelay, lineEnd, wordWrap, data0->wrap_line) ;
				}
				break ;

			case XMODEM:
			case YMODEM:
      case KERMIT:
				{
				char protocol[20] ;
				if (fileType==XMODEM)
					{
					lstrcpy(protocol, cstXmodem[data0->option]) ;
					}
				else if(fileType==YMODEM)
					{
					if (data0->option==0)
						lstrcpy (protocol, "YMODEM standard") ;
					else
          	lstrcpy (protocol, "YMODEM G") ;
					}
				else if (fileType==KERMIT)
					lstrcpy (protocol, "KERMIT") ;

				sprintf (lpConfigure, binaryFormat, protocol, data0->maxTimeout, data0->maxTrytimes) ;
				}
        break ;
      }
		AObjUnlockData (oiFT, FTNSDATA) ;
    }
}
int GetDataString(LPSTR lpConfigure, char* text, void *result)
{
	LPSTR lpstr ;
	char *p ;
  char  errorMsg[256] ;
	BOOL  ok = 1 ;
	WORD  i ;
  int   charP=0 ;

	p = (char*)result ;
  *p = '\0' ;
	lpstr = _fstrstr(lpConfigure, (LPSTR)text) ;
	if (lpstr)
		{
		charP = (int)(lpstr-lpConfigure) ;
    lpstr+=lstrlen(text) ;
		if (*(lpstr++)!=' ')
			{
      charP++ ;
			sprintf (errorMsg, errorFormat, (lpstr-lpConfigure)) ;
			MessageBox (NULL, errorMsg, "Error", MB_OK) ;
			ok = 0;
			}
		else
			{
			for (i=0; i<_fstrlen(lpstr)+1; i++, lpstr++)
				{
        charP++ ;
				if (*lpstr==' '&&lstrlen(p)>0)
					{
					*p='\0' ;
					break ;
          }
				if (*lpstr!=' ')
					*p++=*lpstr ;
				}
			}
		result = (void*)p ;
    }
  return (ok) ;
}
void SaveAsciiConfigure (LPSTR lpConfigure, FTNSDATA0 *data0)
{
	char *endPtr ;
	char str[256] ;
  int  i ;

	if (GetDataString (lpConfigure, "DELAYPERCHAR", &str))
		{
		data0->charDelay = strtoul (str, &endPtr,10) ;
		}
	if(GetDataString (lpConfigure, "DELAYPERLINE", &str))
		{
		data0->lineDelay = strtoul (str, &endPtr,10) ;
    }
	if(GetDataString (lpConfigure, "ENDING", &str))
		{
		for (i=0; i<3; i++)
			{
			if (lstrcmp(str, cstLineEnd[i])==0)
				{
				data0->lineEnd = i ;
        break ;
        }
      }
		}
	if (GetDataString (lpConfigure, "WORDWRAP",&str))
		{
		data0->wrap=lstrcmp(str, "TRUE")?FALSE:TRUE ;
    }
 	if(GetDataString (lpConfigure, "CHARPERLINE", &str))
		{
		data0->wrap_line = (int)strtoul (str, &endPtr,10) ;
   	}
}
void SaveBinaryConfigure (LPSTR lpConfigure, FTNSDATA0 *data0)
{
	char *endPtr ;
	char str[256] ;
  int  i ;

	if (GetDataString (lpConfigure, "TIMEOUT", &str))
		{
		data0->maxTimeout = (int)strtoul (str, &endPtr,10) ;
		}
	if(GetDataString (lpConfigure, "RETRY", &str))
		{
		data0->maxTrytimes = (int)strtoul (str, &endPtr,10) ;
    }
	if(data0->fileType==XMODEM)
		{
		for (i=0; i<3; i++)
			{
			if (GetDataString(lpConfigure, strupr(cstXmodem[i]), &str))
				{
				data0->option = i ;
        break ;
        }
			}
    }                  
	else if(data0->fileType==YMODEM)
		{
		for (i=0; i<2; i++)
			{
			if (GetDataString(lpConfigure, strupr(cstYmodem[i]), &str))
				{
				data0->option = i ;
        break ;
        }
			}
		}
}
void SaveConfigureText (OBJECTID oiFT, LPSTR lpConfigure)
{
	FTNSDATA0 *data0 ;

	_fstrupr(lpConfigure) ;
	data0= (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;
	if (data0)
		{
		if (data0->fileType==ASCII)
			{
      SaveAsciiConfigure (lpConfigure, data0) ;
	 		}
		else
			{
      SaveBinaryConfigure (lpConfigure, data0) ;
      }
		AObjUnlockData (oiFT, FTNSDATA) ;
		}
} 
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL GetFileNameDialog(
	BYTE fileType,
	char* fileName)
{
	OPENFILENAME    ofn;
  //char            fileFilter[256] ;

  ofn.lpstrFilter = NULL ;
	if (fileType==ASCII)
  	ofn.lpstrFilter = "Text file (*.txt)\0*.txt\0" ;
  else
    ofn.lpstrFilter = "All file (*.*)\0*.*\0" ;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetActiveWindow() ;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
  fileName[0] = 0 ;
	ofn.lpstrFile = (LPSTR)fileName;
	ofn.nMaxFile = 128;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Send File";
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrDefExt = NULL;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

	return (GetOpenFileName((LPOPENFILENAME)&ofn));
}
int GetFileName (OBJECTID oiText, BYTE fileType, char *fileName)
{
	int result=1 ;

	if (oiText)
		OTextGetToBuffer (oiText, fileName, 256) ;
	else
	  {
		if (fileType!=YMODEM&&fileType!=ZMODEM)
			result = GetFileNameDialog(fileType, fileName) ;
		else
			result = DoFileOpenDlg(fileName) ;
		}
	if (*fileName=='\0')
		result = 3 ;
	if (result==0)
		result = 2 ;
	return (result);
}
//////////////////////////////////////////////////////////////////////////
//Function:    Configure Function
//Description: Choose and configure a file transfer protocol.
//Input:       1.File Transfer Object
//Flow:        1.Okey
//             2.Cancelled
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_ConfigureFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID      oiFT ;
	BOOL          ok ;
  FILETRANSFER* ftns ;
	HWND          hwnd = GetActiveWindow () ;

	I_UNUSED(theEvent) ;
	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
  ftns = new FILETRANSFER (oiFT) ;
	ok=DialogBoxParam(hInstance, "FileTransfer" , hwnd, (DLGPROC)FileTransferCfg , (LPARAM)ftns) ;
	delete ftns ;
	if (ok)
    AFuncReportResult (theSystem, 1, NULL) ;
	else
		AFuncReportResult (theSystem, 2, NULL) ; //"User cancel the configure dialog.") ;
	
}
//////////////////////////////////////////////////////////////////////////
//Fucntion:    Cancel Function
//Description: Cancel a file transfer currently in progress.
//Input:       1.File Transfer Object
//Flow:        1. Done.
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_CancelFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID   oiFT ;
	FTCLASS   *run_fltr ;

	I_UNUSED(theEvent) ;
	I_UNUSED (theSystem) ;

	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	run_fltr = (FTCLASS*)AObjGetRuntimeInfo(oiFT) ;
	run_fltr->CancelFileTransfer() ;

	KillTimerEvent (oiFT, setPollTimer) ;
	KillTimerEvent (oiFT, setDataTimer) ;
	AFuncReportResult (theSystem, 1, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//Fucntion:    Send File Function
//Description: Initiate a file transfer.
//Input:       1.File Transfer Object
//             2.Full pathname (if pathnameis noy provided, A dialog wiil be displayed)
//Flow:        1.Initialied.
//             2.Dialog cancelled.
//             3.Failed.
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_SendFileFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID   oiFT ;                
  OBJECTID   oiText ;         			 
	FTNSDATA0* data0 ;        //default is ASCII file   			 
	BYTE       fileType =ASCII ;        			 
	HGLOBAL 	 hFileName = GlobalAlloc(GHND, 1024) ;
	char*      fileName = (hFileName)?(char*)GlobalLock (hFileName):NULL;
	int        flow=3 ;   

	I_UNUSED(theEvent) ;

	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	oiText = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	data0 = (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;
	if (data0)
		{
		fileType = data0->fileType ;
		AObjUnlockData (oiFT, FTNSDATA) ;
		}
	if (fileName)
	  {
		flow = GetFileName (oiText, fileType, fileName) ;
		GlobalUnlock (hFileName) ;
		}
	if (flow==1)
	  {
		I_UNUSED (theSystem) ;
		if (fileType==ASCII)
			PostEvent (oiFT, sendTextFile, (long)startEvent, (long)hFileName, AAppGetTime(), AEVT_ATTAIL) ;
		else
			PostEvent (oiFT, sendBinaryFile, (long)startEvent, (long)hFileName, AAppGetTime(), AEVT_ATTAIL) ;
		//PostEvent (oiFT, sendFile, (long)fileType, (long)hFileName, AAppGetTime(), AEVT_ATTAIL) ;
		}
		AFuncReportResult (theSystem, flow, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//Function:    Receive File Function
//Descroption: Begin receiving a file
//Input:       1.File Transfer Object
//Flow:        1.Done.
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_ReceiveFileFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID		oiFT ;
	FTNSDATA0* 	data0 ;
	BYTE        fileType ;

	I_UNUSED(theEvent) ;

	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	///PostEvent (oiFT, receiveFile, 0, 0, AAppGetTime(), AEVT_ATTAIL) ;
	data0 = (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;
	if (data0)
  	{
		fileType = data0->fileType ;
		AObjUnlockData (oiFT, FTNSDATA) ;
		}
	if (fileType==ASCII)
		PostEvent (oiFT, receiveTextFile, (long)startEvent, 0, AAppGetTime(), AEVT_ATMARK) ;
	else
		PostEvent (oiFT, receiveBinaryFile, (long)startEvent, 0, AAppGetTime(), AEVT_ATMARK) ;
	AFuncReportResult (theSystem, 1, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_GetConfigFunc (pAObjInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID oiFT ;
	LPSTR    lpConfigure ;

	I_UNUSED (theEvent) ;
	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	lpConfigure = (LPSTR) new char[200] ;
	if (lpConfigure)
		{
		SetupConfigureText (oiFT, lpConfigure) ;
		OBJECTID oiText = AObjCreate (OTYPE_TEXT) ;
		if (oiText)
			{
			OTextSetValueFromBuffer (oiText, lpConfigure) ;
			AFuncSetParameter (1, oiText) ;
			}
    delete []lpConfigure ;
    }
	AFuncReportResult(theSystem, 1, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI FT_SetConfigFunc (pAObjInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID oiFT ;
	OBJECTID oiConfigure ;
	LPSTR    lpConfigure ;
	WORD     size ;

	I_UNUSED (theEvent) ;

	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	oiConfigure = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	size = OTextGetLength (oiConfigure) ;
	lpConfigure = (LPSTR)new char[size+1] ;
	if (lpConfigure)
		{
    OTextGetToBuffer (oiConfigure, lpConfigure, size+1) ; 
    SaveConfigureText (oiFT, lpConfigure) ;
		delete[]lpConfigure ;
		}
  AFuncReportResult (theSystem, 1, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
/*void ALMAPI FT_SetPathFunc (pAObjInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID oiFT ;
	OBJECTID oiPath ;
	I_UNUSED (theEvent) ;
	WORD     numchars ;
	char*    pathname ;
	FTCLASS* run_ft ;

	oiFT = AFuncGetTypedParameter (1, OTYPE_FILETRANSFER) ;
	oiPath = AFuncGetTypedParameter (1, OTYPE_TEXT) ;
	run_ft = (FTCLASS*) AObjGetRuntimeInfo (oiFT) ;
	if (AObjGetDataCount(oiFT)<PATHNAME+1)
		AObjSetDataCount (oiFT, PATHNAME+1) ;
	numchars  = OTextGetLength (oiPath)+1 ;
	pathname = new char[numchars] ;
	OTextGetToBuffer (oiPath,pathname, numchars) ;
	if (AObjSetData (oiFT, PATHNAME, pathname, numchars))
		run_ft->setReceivePath = TRUE ;

	delete[]pathname ; 

	AFuncReportResult(theSystem, 1, NULL) ;
}*/
