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
// FILE:"conn.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define CONST

#include <windowsx.h>
#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include "com_dlg.h"
#include "commObj.h"

int HexToDec (int) ;
void TextToHex (LPSTR, LPSTR, LPSTR, LONG) ;
FARPROC defOldEditProc ;
void DisplayListString (HWND hwnd, LPSTR string, char mark, int signalNo, int Index) ;
void SetCompareBits (HWND, int) ;
void ClearStringEdit (HWND hwnd) ;
long ChangeHexToDec (char* lpDesc, char* lpSource) ;
LPSTR StrChr (LPSTR, char, int) ;
long StrSpn (LPSTR, LPSTR, int) ;
LPSTR StrCmp (LPSTR, LPSTR, int, int*) ;
//////////////////////////////////////////////////////////////////////////
//Constructor of CONN class
//
//////////////////////////////////////////////////////////////////////////
CONN::CONN (OBJECTID oiObj)
{
	theObject = oiObj ;
	connbusy  = FALSE ;

  if(!theObject || AObjGetType(theObject)!=OTYPE_CONN) return;

	if (AObjGetDataCount(theObject)<NUMDATAS)
		{
		AObjSetDataCount(theObject, NUMDATAS);
		}

	data0	= (DATA0*)AObjLockData(theObject, CONNDATA);
	if (!data0)
		{
		if (AObjResizeData(theObject, CONNDATA, sizeof(DATA0)) &&
				(data0 = (DATA0*)AObjLockData(theObject, CONNDATA)) != NULL)
			{
				data0->version     				  = CURRENTVERSION ;
				data0->checkModem  				  = TRUE ;
        //Port Settings
    		data0->port.BaudRate  			= 3 ;
    		data0->port.DataBits  			= 3 ;
    		data0->port.StopBits  			= ONESTOPBIT ;
    		data0->port.Parity    			= NOPARITY ;
				data0->port.handshake 			= XONXOFF;
    		data0->port.Ports     			= PORT_1;
    		data0->port.ChkParity 			= FALSE ;
				data0->port.CrDetect  			= FALSE ;
				//Modem Settings
    		data0->modem.AnsOrDial      = TRUE ;
    		data0->modem.RingTimes      = 2 ;
    		data0->modem.PhoneNumberLen = 0 ;
    		data0->modem.RedialTimes    = 3 ;
    		data0->modem.Redial         = FALSE ;
				data0->modem.DialMethod     = TONE ;
				data0->modem.seconds        = 10 ;
        //Signal Settings 
				//data0->inteText	= 0L ;
				data0->signal.viewMode      = SGNLASCII;
				data0->signal.checkBox      = EIGHT;
				data0->signal.signalCount   = 0L;
				data0->signal.FoundString   = 0L ;
				data0->signal.FoundIndex    = 0L ;
				data0->signal.InteText      = 0L ;
     	}
		}
}
//////////////////////////////////////////////////////////////////////////
//Destructor of CONN class
//
//////////////////////////////////////////////////////////////////////////
CONN::~CONN (void)
{
	if (data0)
		AObjUnlockData(theObject, 0L);
}
//////////////////////////////////////////////////////////////////////////
//Callback function for the connection parameters setting
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK CntObjProc(HWND hDlg, WORD message,
													WORD wParam, LONG lParam)

{
	CONN* conn ;
	switch (message)
		{
		case WM_INITDIALOG:
			conn = (CONN*)lParam ;
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			conn->InitCntDialog(hDlg) ;
			return FALSE ;

		case WM_COMMAND:
			conn = (CONN*)GetWindowLong(hDlg, DWL_USER) ;
			switch(wParam)
				{
				case idModem:
        	conn->SelectModem (hDlg) ;
        	return TRUE ;
			 	case idModemS:
					conn->ModemSettings (hDlg) ;
					return FALSE ; 			
       	case IDOK:
        	conn->SaveCntConfigure (hDlg) ;  //Save ring times ,redial times and phonenumber
			 	 	EndDialog (hDlg, TRUE) ;
		 		 	return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;

				}
    }
    return FALSE ;
}
//////////////////////////////////////////////////////////////////////////
//Initialize Dialog box 
//////////////////////////////////////////////////////////////////////////
void CONN::InitCntDialog (HWND hDlg)
{
  WORD index ;
	int  i ;
  int  size ;

	//Set Baud Rate
	size = sizeof (cstBaudrate)/sizeof(cstBaudrate[0]) ;
  for (i=0; i<size; i++)
  	{
		index = (WORD)SendDlgItemMessage(hDlg, idBaudRate, CB_INSERTSTRING, -1, (long)cstBaudrate[i]) ;
		if (i==data0->port.BaudRate)
      {
			SendDlgItemMessage (hDlg, idBaudRate, CB_SETCURSEL, index, 0) ;
      }
    }
	//Set Databits
	size = sizeof (cstDatabits)/sizeof(cstDatabits[0]) ;
	for (i=0; i<size; i++)
    {
    index = (WORD)SendDlgItemMessage(hDlg, idDataBits, CB_INSERTSTRING, -1, (long)cstDatabits[i]) ;
    if (i==data0->port.DataBits)
      {
			SendDlgItemMessage (hDlg, idDataBits, CB_SETCURSEL, index, 0) ;
      }
    }
	//Set StopBits
	size = sizeof (cstStopbits)/sizeof(cstStopbits[0]) ;
  for (i=0; i<size; i++)
    {
		index = (WORD)SendDlgItemMessage(hDlg, idStopBits, CB_INSERTSTRING, -1, (long)cstStopbits[i]) ;
    if (i==data0->port.StopBits)
      {
			SendDlgItemMessage (hDlg, idStopBits, CB_SETCURSEL, index, 0) ;
      }
    }
	//Set Parity
	size = sizeof (cstParity)/sizeof(cstParity[0]) ;
  for (i=0; i<size; i++)
    {
    index = (WORD)SendDlgItemMessage(hDlg, idParity, CB_INSERTSTRING, -1, (long)cstParity[i]) ;
    if (i==data0->port.Parity)
  	  {
			SendDlgItemMessage (hDlg, idParity, CB_SETCURSEL, index, 0) ;
      }
    }
	//Set FlowControl
	size = sizeof (csthandshake)/sizeof(csthandshake[0]) ;
  for (i=0; i<size; i++)
    {
		index = (WORD)SendDlgItemMessage(hDlg, idhandshake, CB_INSERTSTRING, -1, (long)csthandshake[i]) ;
		if (i==data0->port.handshake)
      {
			SendDlgItemMessage (hDlg, idhandshake, CB_SETCURSEL, index, 0) ;
  	  }
    }
	// Set Ports
	size = sizeof (cstPorts)/sizeof(cstPorts[0]) ;
	for (i=0; i<size; i++)
    {
    index = (WORD)SendDlgItemMessage(hDlg, idPorts, CB_INSERTSTRING, -1, (long)cstPorts[i]) ;
    if (i==data0->port.Ports)
      {
			SendDlgItemMessage (hDlg, idPorts, CB_SETCURSEL, index, 0) ;
      }
    }

  //Set Parity check
  CheckDlgButton (hDlg, idChkParity, data0->port.ChkParity) ;
	CheckDlgButton (hDlg, idCrDetect, data0->port.CrDetect) ;
	CheckDlgButton (hDlg, idModem, data0->checkModem ) ;
	EnableWindow (GetDlgItem(hDlg, idModemS), data0->checkModem) ;
  //	EnableWindow (GetDlgItem(hDlg,idModemS), data0->checkModem) ;
}
//////////////////////////////////////////////////////////////////////////
//If Modem checkbutton is checked, Enable the Modem button
//////////////////////////////////////////////////////////////////////////
void CONN::SelectModem (HWND hDlg)
{
	BOOL b ;

	b=IsDlgButtonChecked (hDlg, idModem);
	EnableWindow(GetDlgItem(hDlg, idModemS),b) ;
}
//////////////////////////////////////////////////////////////////////////
//Save the connection parameters to the data block0.
//////////////////////////////////////////////////////////////////////////
void CONN::SaveCntConfigure(HWND hDlg)
{
 	data0->port.BaudRate = (int)SendDlgItemMessage (hDlg, idBaudRate, CB_GETCURSEL, 0, 0L) ;
	data0->port.DataBits = (int)SendDlgItemMessage (hDlg, idDataBits, CB_GETCURSEL, 0, 0L) ;
	data0->port.StopBits = (int)SendDlgItemMessage (hDlg, idStopBits, CB_GETCURSEL, 0, 0L) ;
	data0->port.Parity   = (int)SendDlgItemMessage (hDlg, idParity,   CB_GETCURSEL, 0, 0L) ;
	data0->port.handshake= (int)SendDlgItemMessage (hDlg, idhandshake, CB_GETCURSEL, 0, 0L) ;
	data0->port.Ports		 = (int)SendDlgItemMessage (hDlg, idPorts,    CB_GETCURSEL, 0, 0L) ;
	data0->port.ChkParity= IsDlgButtonChecked (hDlg, idChkParity) ;
	data0->port.CrDetect = IsDlgButtonChecked (hDlg, idCrDetect) ;
	data0->checkModem    = IsDlgButtonChecked (hDlg, idModem) ;
}
//////////////////////////////////////////////////////////////////////////
//For modem Settings
//////////////////////////////////////////////////////////////////////////
void CONN::ModemSettings (HWND hDlg)
{
	BOOL ok ;
	MODEM tmpmodem ;

  tmpmodem = data0->modem ;
	ok = DialogBoxParam(hInstance, "Modem", hDlg, (FARPROC)ModemSettingsProc, (long)this) ;
	if (!ok)
		data0->modem = tmpmodem ;
}
//////////////////////////////////////////////////////////////////////////
//CallBack function for the modem settings
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK ModemSettingsProc (HWND hDlg, WORD message,
																 WORD wParam, LONG lParam)
{
	CONN* conn ;
	switch (message)
		{
		case WM_INITDIALOG:
			conn = (CONN*)lParam ;
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			conn->InitModemSettings(hDlg) ;
			return FALSE ;

		case WM_COMMAND:
			conn = (CONN*)GetWindowLong(hDlg, DWL_USER) ;
      switch(wParam)
				{
				case idRing:
				case idDial:
				case idRedial:
					conn->ChangeModemSettings(hDlg) ;
					return FALSE ;

				case IDOK:
        	conn->SaveModemParams (hDlg) ;  
			 	 	EndDialog (hDlg, TRUE) ;
		 		 	return TRUE ;

        case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;
				}
	}
	return FALSE ;
}
//////////////////////////////////////////////////////////////////////////
//Initialize the modem parameters
//////////////////////////////////////////////////////////////////////////
void CONN::InitModemSettings (HWND hDlg)
{
	WORD index ;
  int  i ;
	char *outputText ;
  int  size ; 

	//Set Dial Method
	size =sizeof(cstDialM)/sizeof(cstDialM[0]) ;
  for (i=0; i<size; i++)
    {
		index = (WORD)SendDlgItemMessage(hDlg, idDialMethod, CB_INSERTSTRING, -1, (long)cstDialM[i]) ;
    if (i==data0->modem.DialMethod)
      {
	    SendDlgItemMessage (hDlg, idDialMethod, CB_SETCURSEL, index, 0) ;
      }
		}
	//Set Phone Number
	if (data0->modem.PhoneNumberLen)
    {
    outputText = (char *)AObjLockData(theObject, PHONEN) ;
    if (outputText) SetDlgItemText (hDlg, idDialNumber, outputText) ;
		AObjUnlockData (theObject, PHONEN) ;
		}
	//Set Answer time
	SetDlgItemInt(hDlg, idRingtimes, data0->modem.RingTimes , FALSE) ;
	SetDlgItemInt(hDlg, idSeconds, data0->modem.seconds, FALSE) ;
	if (data0->modem.AnsOrDial)
  	{
    EnableWindow (GetDlgItem(hDlg, idDialNumber), TRUE) ;
		EnableWindow (GetDlgItem(hDlg, idRingtimes), FALSE) ;
    CheckRadioButton (hDlg, idRing, idDial, idDial) ;
		}
	else
  	{
		EnableWindow (GetDlgItem (hDlg, idDialNumber), FALSE) ;
		EnableWindow (GetDlgItem (hDlg, idRingtimes), TRUE) ;
		CheckRadioButton (hDlg, idRing, idDial, idRing) ;
		}
  //Set Redial
  if (data0->modem.Redial)
   	{
   	CheckDlgButton(hDlg, idRedial, TRUE) ;
   	EnableWindow (GetDlgItem(hDlg, idRedialtimes),TRUE) ;
		EnableWindow (GetDlgItem(hDlg, idSeconds), TRUE) ;
   	}
 	else
 	  {
   	EnableWindow (GetDlgItem(hDlg, idRedialtimes),FALSE) ;
		EnableWindow (GetDlgItem(hDlg, idSeconds), FALSE) ;
		}
  //Set Redial time
  SetDlgItemInt(hDlg, idRedialtimes, data0->modem.RedialTimes , FALSE) ;
}
//////////////////////////////////////////////////////////////////////////
//Change the Modem Settings
//////////////////////////////////////////////////////////////////////////
void CONN::ChangeModemSettings (HWND hDlg)
{
	data0->modem.AnsOrDial = IsDlgButtonChecked (hDlg, idDial) ;
	data0->modem.Redial    = IsDlgButtonChecked (hDlg, idRedial) ;
	InitModemSettings (hDlg) ;
}
//////////////////////////////////////////////////////////////////////////
//Save Modem Settings to data block 
//////////////////////////////////////////////////////////////////////////
void CONN::SaveModemParams(HWND hDlg)
{
	char *   outputText ;
  char *   spPhoneNumber ;
  int      length ;
  int      value ;
  BOOL     chkError ;

  //Save Phone number
  outputText = new char[256] ;
  if (outputText)
    {
		length = GetDlgItemText (hDlg, idDialNumber, outputText, 256) ;
		if (AObjResizeData (theObject, PHONEN, (long)length+1))
			{
			spPhoneNumber=(char *)AObjLockData(theObject, PHONEN) ;
			if (spPhoneNumber)
				{	
      	data0->modem.PhoneNumberLen = length+1 ;
				strcpy(spPhoneNumber, outputText) ;
        AObjUnlockData (theObject, PHONEN) ;
      	}
			}
    delete []outputText ;
    }

    //Save ring times and redial times
	value = GetDlgItemInt (hDlg, idRingtimes, &chkError, TRUE) ;
  if (chkError) data0->modem.RingTimes = value ;
  value = GetDlgItemInt (hDlg, idRedialtimes, &chkError, TRUE) ;
  if (chkError) data0->modem.RedialTimes = value ;
	 value = GetDlgItemInt (hDlg, idSeconds, &chkError, TRUE) ;
  if (chkError) data0->modem.seconds = value ;
}
//////////////////////////////////////////////////////////////////////////
//ExportObject Function
//////////////////////////////////////////////////////////////////////////
void CONN::ExportObject(void)
{
	ExportConnDataBlock0 () ;
	//ExportPhoneNumber () ;
	//ExportSignalDataBlock() ;
}
void CONN::ExportConnDataBlock0 (void)
{
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->version, sizeof(int));
	AUpsfExportField("VIEWMODE", 0L, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &data0->signal.viewMode, sizeof(BYTE));
	AUpsfExportField("CHECKBOX", 0L, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &data0->signal.checkBox, sizeof(BYTE));
	AUpsfExportField("SGNLNUMS", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->signal.signalCount, sizeof(int));
	AUpsfExportField("FNDSTR", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->signal.FoundString, sizeof(OBJECTID));
	AUpsfExportField("FNDPOS", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->signal.FoundIndex, sizeof(OBJECTID));
	AUpsfExportField("INTETEXT", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->signal.InteText, sizeof(OBJECTID));
}
/*void CONN::ExportPhoneNumber (void)
{
	LPSTR phonenumber ;

  phonenumber = (LPSTR) AObjLockData (theObject, PHONEN) ;
	AUpsfExportField("PHONEN", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, phonenumber, _fstrlen(phonenumber));
	AObjUnlockData (theObject, PHONEN) ;
}	*/
void CONN::ExportSignalDataBlock(void)
{
	SIGNALINFO *signalInfo ;
	LPSTR       signalBlock ;
  int         i=0 ;

	signalInfo = (SIGNALINFO *) AObjLockData (theObject, SGNLINFO) ;
	signalBlock= (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
	if (signalInfo)
	  {
		for (i=0; i<data0->signal.signalCount; i++, signalInfo++)
		  {
 			AUpsfExportField ("SGNLINX", i, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &signalInfo->signalIndex, sizeof(long)) ;
 			AUpsfExportField ("SGNLLEN", i, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &signalInfo->signalLen, sizeof(long)) ;
 			AUpsfExportField ("CMPBITS", i, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &signalInfo->CompareBits, sizeof(BYTE)) ;
 			AUpsfExportField ("ACTIVATE", i, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &signalInfo->active, sizeof(BOOL));
 			}
		AObjUnlockData (theObject, SGNLINFO) ;
		}
	if (signalBlock)
	  {
 		for (i=0; i<data0->signal.signalCount; i++)
 			{
 		  AUpsfExportField ("SIGNAL", i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, signalBlock,_fstrlen(signalBlock)) ;
 		  signalBlock +=_fstrlen(signalBlock)+1 ; 
    	}
		AObjUnlockData (theObject, SGNLBLOCK) ;
		}
}
void CONN::ImportObject(void)
{
	ImportConnDataBlock0 () ;
	//ImportPhoneNumber () ;
	//ImportSignalDataBlock() ;
}
void CONN::ImportConnDataBlock0(void)
{
	if (AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &data0->version, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"VERSION\" -- defaulting to current version.");
	if(AUpsfImportField("VIEWMODE", 0L, AUPSF_NATV_BYTE, &data0->signal.viewMode, sizeof(BYTE))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"VIEWMODE\" -- defaulting to ViewMode.");
	if(AUpsfImportField("CHECKBOX", 0L, AUPSF_NATV_BYTE, &data0->signal.checkBox, sizeof(BYTE))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"CHECKBOX\" -- defaulting to Checkbox.");
	if(AUpsfImportField("SGNLNUMS", 0L, AUPSF_NATV_INT, &data0->signal.signalCount, sizeof(int))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"SGNLNUMS\" -- defaulting to signalCount.");
	if(AUpsfImportField("FNDSTR", 0L, AUPSF_NATV_OBJECTREF, &data0->signal.FoundString, sizeof(OBJECTID))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"FNDSTR\" -- defaulting to FoundString.");
	if(AUpsfImportField("FNDPOS", 0L, AUPSF_NATV_OBJECTREF, &data0->signal.FoundIndex, sizeof(OBJECTID))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"FNDPOS\" -- defaulting to FoundIndex.");
	if(AUpsfImportField("INTETEXT", 0L, AUPSF_NATV_OBJECTREF, &data0->signal.InteText, sizeof(OBJECTID))!=AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"INTETEXT\" -- defaulting to InteText.");
}
/*void CONN::ImportPhoneNumber (void)
{
	LPSTR phonenumber ;
	long  pnumberLen ;

	AUpsfFieldInfo ("PHONEN",0L, AUPSF_NATV_STRING, NULL, (long*)&pnumberLen) ;
	AObjResizeData (theObject, PHONEN, pnumberLen+1) ;
	phonenumber = (LPSTR) AObjLockData (theObject, PHONEN) ;
	if (phonenumber)
  	{
		*phonenumber = '\0' ; 
		if(AUpsfImportField("PHONEN", 0L, AUPSF_NATV_OBJECTREF, phonenumber, pnumberLen+1)!=AUPSF_NOERROR)
  		{
			AUpsfLogComment("Error importing \"PHONEN\" -- defaulting to phonenumber.");
			}
    AObjUnlockData (theObject, PHONEN) ;
    }
}	*/
void CONN::ImportSignalDataBlock(void)
{
	SIGNALINFO *signalInfo ;
	LPSTR       signalBlock ;
	long        signalCount ;
	long        i ;
	long        blksize1, blksize2=0 ;

	signalCount = AUpsfFieldCount("SIGNAL") ;
	if(AObjResizeData (theObject, SGNLINFO, signalCount*sizeof(SGNLINFO)))
		{
		signalInfo=(SIGNALINFO*)AObjLockData (theObject, SGNLINFO) ;
		if (signalInfo)
			{
			for (i=0; i<signalCount; i++,signalInfo++)
				{
				if(AUpsfImportField("SGNLINX", i, AUPSF_NATV_LONG, &signalInfo->signalIndex, sizeof(long))!=AUPSF_NOERROR)
				  {
					signalInfo->signalIndex = i ;
					AUpsfLogComment("Error importing \"SGNLINX\".");
					}
				if(AUpsfImportField("SGNLLEN", i, AUPSF_NATV_LONG, &signalInfo->signalLen, sizeof(long))!=AUPSF_NOERROR)
				  {
					signalInfo->signalLen = 0 ;
					AUpsfLogComment("Error importing \"SGNLLEN\".");
					}
				if(AUpsfImportField("CMPBITS", i, AUPSF_NATV_BYTE, &signalInfo->CompareBits, sizeof(BYTE))!=AUPSF_NOERROR)
				  {
					signalInfo->CompareBits = EIGHT ;
					AUpsfLogComment("Error importing \"CMPBITS\".");
					}
				if(AUpsfImportField("ACTIVATE", i, AUPSF_NATV_BOOL, &signalInfo->active, sizeof(BOOL))!=AUPSF_NOERROR)
				  {
					signalInfo->active = TRUE ;
					AUpsfLogComment("Error importing \"ACTIVE\".");
					}
				}
			AObjUnlockData (theObject, SGNLINFO) ;
      }
		}
	for (i=0; i<signalCount; i++)
		{
		AUpsfFieldInfo ("SIGNAL", i, AUPSF_NATV_STRING, NULL, &blksize1) ;
    blksize2+=blksize1 ;
    }
	if(AObjResizeData (theObject, SGNLBLOCK, blksize2))
		{
		signalBlock=(LPSTR)AObjLockData (theObject, SGNLBLOCK) ;
		if (signalBlock)
			{
			for (i=0; i<signalCount; i++)
				{
				AUpsfFieldInfo ("SIGNAL", i, AUPSF_NATV_STRING, NULL, (long*)&blksize1) ;
				if(AUpsfImportField("SIGNAL", i, AUPSF_NATV_STRING, signalBlock, blksize1)!=AUPSF_NOERROR)
					AUpsfLogComment("Error importing \"SGNLINX\".");
				else
        	signalBlock+=(WORD)blksize1 ;
				}
      AObjUnlockData (theObject, SGNLBLOCK) ;
      }
		}
}
long CONN::doCheckVersion (pAObjMessage theSystem)
{
	long rVal = A_OK ;;

 	theSystem->message3 = CURRENTVERSION ;
	if (data0->version > CURRENTVERSION)
  	{ 
  	AObjReportError(theObject, theSystem->message1, A_ERROR,
				"Connection\tALM_COMM.DLL", 1);
  	 rVal = A_ERROR;
		}
	return rVal;
}
void CONN::doSignalIDs(pAObjMessage theSystem)
{
	MEMBLOCKID signalMID ;
	LPSTR      signalBlock ;
	pAObjSignalRecord signal ;
  int        i ;

  signalMID = (MEMBLOCKID)theSystem->message2;
	AMemSetSize(signalMID, (data0->signal.signalCount+4) * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(signalMID);

	signal[0].theID = 0 ;
	lstrcpy(signal[0].theName, "Closed");
  signal[1].theID = 1 ;
	lstrcpy(signal[1].theName, "Opened");
  signal[2].theID = 2;
	lstrcpy(signal[2].theName, "Text Intercepted");
	signal[3].theID = 3 ;
	lstrcpy(signal[3].theName, "Listen Canceled") ;

	signalBlock = (LPSTR)AObjLockData (theObject, SGNLBLOCK) ;
	if (signalBlock)
  	{
		for (i=0; i<data0->signal.signalCount; i++)
			{
			signal[i+4].theID = i+4 ;
			if (strlen(signalBlock))
				sprintf (signal[i+4].theName, "%d %c%s%c",i+1,'"', signalBlock, '"') ;
			else
      	sprintf (signal[i+4].theName, "%d", i+1) ;
			signalBlock +=strlen (signalBlock)+1 ;
			}
		AObjUnlockData (theObject, SGNLBLOCK) ;
    }
  AMemUnlock (signalMID) ;
}
void CONN::doValidateObjectIDs(pAObjMessage theSystem)
{
	OBJECTID tmpObj ;

	tmpObj = data0->signal.FoundString;
	if (tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->signal.FoundString = tmpObj;
		}
	tmpObj = data0->signal.FoundIndex;
	if (tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->signal.FoundIndex = tmpObj;
		}
	tmpObj = data0->signal.InteText;
	if (tmpObj)
		{
		AObjValidateID(&tmpObj, theSystem);
		data0->signal.InteText = tmpObj;
		}
}
void CONN::doCheckObjectIDs(void)
{
	if(!AObjCheckType(data0->signal.FoundString, OTYPE_TEXT))
		data0->signal.FoundString = NULL ;
	if(!AObjCheckType(data0->signal.FoundIndex, OTYPE_NUMBER))
		data0->signal.FoundIndex = NULL ;
	if(!AObjCheckType(data0->signal.InteText, OTYPE_TEXT))
		data0->signal.InteText = NULL ;
}
//
//
//
EDIT_SIGNAL::EDIT_SIGNAL(OBJECTID oiObj):CONN(oiObj)
{
	marktable[TRUE]=0xD7 ;
	marktable[FALSE] =0x20 ;
  signal = &data0->signal ;
}
EDIT_SIGNAL::~EDIT_SIGNAL(void)
{
}
//
//
//
void StringToHex (LPSTR lpBuffer, LPSTR lpText, char *Format, int step)
{
	int i= 0 ;
  char c ;
  char tbuf[80] ;

  i=0 ;
	//sprintf (lpBuffer, Format, *lpText++) ;
	*lpBuffer = '\0' ;
  while((c=*lpText++) !='\0')
		{
    i++ ;
    sprintf((char*)tbuf, Format, c);
		strcat(lpBuffer, (char *)tbuf);
		if (i==step)
			{
			i=0 ;
      strcat (lpBuffer, " ") ;
      }
    }
}
void EDIT_SIGNAL::ChangeViewMode (HWND hDlg, LONG wParam)
{
	SIGNALINFO* si, *signalInfo ;
	LPSTR       sb, signalBlock ;
	MEMBLOCKID  BlockID ;
	LPSTR       lpStr ;
	char        Format[6]  ;
	int         step ;
	int         i ;

	si = signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
	sb = signalBlock = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;

	if (si&&sb)
   	{
		switch (wParam)
			{
			case idASCII:
				step= 0 ;
     		currentView = SGNLASCII ;
				break ;

			case idByte:
				strcpy(Format, "%02X") ;
				step = 1 ;
     		currentView = SGNLBYTE ;
				break ;

			case idWord:
				strcpy (Format, "%02X") ;
				step = 2 ;
				currentView = SGNLWORD ;
				break ;

			case idLongword:
				strcpy (Format, "%02X") ;
				step = 4 ;
     		currentView = SGNLLONG ;
				break ;		 		 		 	
			}
		ListBox_ResetContent (GetDlgItem(hDlg, idList)) ; //SendDlgItemMessage (hDlg, idList, LB_RESETCONTENT, 0, 0L) ;
		BlockID = AMemAllocate (si->signalLen) ;
		for (i=0; i<signal->signalCount; i++, si++)
			{
			AMemSetSize (BlockID, 3*si->signalLen) ;
			lpStr = (LPSTR) AMemLock (BlockID) ;
			if (lpStr)
				{
				if (step) StringToHex (lpStr, (LPSTR)sb, Format, step) ;
				if (!step) memcpy(lpStr, (LPSTR)sb, si->signalLen) ;
				sb += si->signalLen ;
				DisplayListString (hDlg, lpStr, marktable[si->active], si->signalIndex+1, -1) ;
				AMemUnlock (BlockID) ;
				}
     }
		AMemFree (BlockID) ;
		}
	ClearStringEdit (hDlg) ;
	if (signalInfo)
		AObjUnlockData (theObject, SGNLINFO) ;
	if (signalBlock)
		AObjUnlockData (theObject, SGNLBLOCK) ;
}
//
//
//
BOOL EDIT_SIGNAL::InitSignalDialog(HWND hwnd)
{
	AUtlSetTitleFont(OTYPE_CONN, NULL, GetDlgItem(hwnd, IDD_OBJECT));

	CheckRadioButton (hwnd, idASCII, idLongword, idASCII) ;
	currentView = SGNLASCII ;

    //Set signal list and current signal
  SetSignalList (hwnd) ;

  //Set check button
  SetComboxContent (hwnd, idFoundStrCon) ;
  SetComboxContent (hwnd, idFoundPosCon) ;
	SetComboxContent (hwnd, idInterceptCon) ;
	SetSignalCheckButton(hwnd) ;

	defOldEditProc = (FARPROC) GetWindowLong(GetDlgItem(hwnd, idInsString), GWL_WNDPROC) ;
	SetWindowLong (GetDlgItem(hwnd, idInsString), GWL_WNDPROC, (long)EditProc) ;

  return TRUE ;
}
void EDIT_SIGNAL::SetComboxContent (HWND hwnd, int idCombox)
{
  TYPEID   theTypeId ;
  long     theGetKey=-1 ;
  OBJECTID oiObject ;
  OBJECTID oiCombox ;
  WORD     index ;
  char    *objName ;

  if (idCombox == idFoundPosCon)
    {
    theTypeId = OTYPE_NUMBER ;
    oiCombox = signal->FoundIndex ;
    }
  if (idCombox == idFoundStrCon)
    {
    theTypeId = OTYPE_TEXT ;
    oiCombox = signal->FoundString ;
    }
  if (idCombox == idInterceptCon)
    {
    theTypeId = OTYPE_TEXT ;
    oiCombox = signal->InteText ;
    }

  objName = new char[255] ;
  lstrcpy(objName, "(None)") ;
  index = (WORD)SendDlgItemMessage (hwnd, idCombox, CB_INSERTSTRING, 0, (long) objName) ;
  SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, 0, 0) ;

	while ((oiObject=AObjGetNextObject(theTypeId, &theGetKey))!=NULL)
    {
    AObjGetName (oiObject, objName, 255) ;
    index = (WORD)SendDlgItemMessage(hwnd, idCombox, CB_INSERTSTRING, -1, (long)objName) ;
    SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, index, (long)oiObject) ;
    if (oiCombox == oiObject)
      {
       SendDlgItemMessage (hwnd, idCombox, CB_SETCURSEL, index, 0) ;
      }
		}

  if (oiCombox ==NULL)
     SendDlgItemMessage (hwnd, idCombox, CB_SETCURSEL, 0, 0) ;

  delete []objName ;
}
void EDIT_SIGNAL::SetSignalCheckButton(HWND hwnd)
{
  SetChkBtnCombox (hwnd, idFoundStr, idFoundStrCon, signal->checkBox&FOUNDSTR) ;
	SetChkBtnCombox (hwnd, idFoundPos, idFoundPosCon, signal->checkBox&FOUNDPOS) ;
	SetChkBtnCombox (hwnd, idIntercept, idInterceptCon, signal->checkBox&INTERCEPT) ;
      
  if (signal->checkBox & INTERCEPT)
    {
    	ShowDlgItem(hwnd, idFoundPos, TRUE) ;
			if (signal->checkBox&FOUNDPOS)
      	ShowDlgItem(hwnd, idFoundPosCon, TRUE) ;
    }
    else
    {
      ShowDlgItem(hwnd, idFoundPos, FALSE) ;
      ShowDlgItem(hwnd, idFoundPosCon, FALSE) ;
    } 
}
void EDIT_SIGNAL::ShowDlgItem(HWND hwnd, int IdItem, BOOL status)
{
    HWND hwndItem = GetDlgItem (hwnd, IdItem) ;

    EnableWindow(hwndItem, status) ;
    if (status)
       ShowWindow(hwndItem, SW_SHOW) ;
    else
       ShowWindow (hwndItem, SW_HIDE) ;
    UpdateWindow (hwndItem) ;

}
void EDIT_SIGNAL::SetChkBtnCombox (HWND hwnd, int idChkBtn, int idCombox, BOOL Status)
{
    //TYPEID   theTypeId ;
		//OBJECTID oiObject ;
    //char     *objName ;
		//int      idItem ;

    CheckDlgButton (hwnd, idChkBtn, Status) ;
    ShowDlgItem (hwnd, idCombox, Status) ; 
    //EnableWindow(GetDlgItem(hwnd, idCombox), Status) ;
/*    if (!Status)return ;

    //List Combox
    if (idCombox == idFoundPosCon)
       theTypeId = numberTypeID ;
    else
       theTypeId = textTypeID ;

    objName = new char[255] ;
    objName = "(None)" ;
    index = SendDlgItemMessage (hwnd, idCombox, CB_INSERTSTRING, 0, (long) objName) ;
    SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, 0, 0) ;

    while ((oiObject=oipGetNextObject(theTypeId, &theGetKey)))
    {
        oipGetObjectName (oiObject, objName, 255) ;
				index = SendDlgItemMessage(hwnd, idCombox, CB_INSERTSTRING, -1, (long)objName) ;
        SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, index, (long)oiObject) ;
    }
*/
}
void EDIT_SIGNAL::SetSignalList (HWND hDlg)
{
	SIGNALINFO *si,*signalInfo ;
  LPSTR       sb, signalBlock ;
	long        len ;
	int         i ;

	if (signal->signalCount>0)
		{
		si = signalInfo  = (SIGNALINFO*)AObjLockData (theObject, SGNLINFO) ;
    sb = signalBlock = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
		if (si&&sb)
			{
			for (i=0; i<signal->signalCount; i++, si++)
				{
				len=si->signalLen ;
				DisplayListString (hDlg, sb, marktable[si->active], si->signalIndex+1, -1) ;
        SetCompareBits (hDlg, si->CompareBits?idLowerseven:idAlleight) ;
				sb +=(WORD)len ;
				}
			}
		if (signalInfo)
			AObjUnlockData (theObject, SGNLINFO) ;
		if (signalBlock)
			AObjUnlockData (theObject, SGNLBLOCK) ;
		}
	else
		SetCompareBits (hDlg, idAlleight) ;
	
}
//
//
//
void EDIT_SIGNAL::AddStringToList(HWND hDlg)
{
	int	        signalCount ;
	int         numChars ;
	char*       strs ;
	HWND        hEdit = GetDlgItem (hDlg, idInsString) ;

  signalCount = signal->signalCount  ;
	numChars = (int)Edit_GetTextLength (hEdit) ; 
	strs = new char [++numChars] ;
	if (strs)
  	{
		Edit_GetText (hEdit, (LPSTR)strs, numChars) ; 
		signalCount++ ;
		DisplayListString (hDlg, strs, 215, signalCount, -1) ;
		signal->signalCount = signalCount ;
		UpdateSignalBlock (hDlg, strs) ;
		delete[] strs ;
  	}
}
void EDIT_SIGNAL::UpdateSignalBlock (HWND hDlg, char* buffer)
{
	LPSTR signalBlock ;
  char *string= NULL ;      
	int	  signalCount=signal->signalCount ;
  WORD  blockSize ; 
	WORD  numChars = strlen(buffer)+1 ;

	blockSize = AObjGetDataSize (theObject, SGNLBLOCK) ;
	string = new char[(numChars+1)/2];
	if (currentView!=SGNLASCII)
		{
		numChars = ChangeHexToDec (string, buffer)+1;
		}
	if (AObjResizeData (theObject, SGNLBLOCK, blockSize+numChars))
		{
		signalBlock = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
		if (signalBlock)
   		{
			signalBlock+=blockSize ;
			if (currentView==SGNLASCII)
				_fmemcpy (signalBlock, (LPSTR)buffer, numChars) ;
			else
      	_fmemcpy (signalBlock, (LPSTR)string, numChars) ;
			AddSignalInfo (hDlg, signalCount, numChars, TRUE) ;
			AObjUnlockData (theObject, SGNLBLOCK) ;
			}
		}
	if (string)
  	delete[]string ; 	
}
void EDIT_SIGNAL::AddSignalInfo (HWND hDlg, long signalNum, long numChars, BOOL active)
{
	SIGNALINFO * signalInfo ;

	if (AObjResizeData (theObject, SGNLINFO, signalNum*sizeof(SIGNALINFO)))
   	{
		signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
		if (signalInfo)
     	{
			signalInfo +=(WORD)signalNum-1 ;
			signalInfo->signalIndex = signalNum-1 ;
			signalInfo->signalLen = numChars ;
			signalInfo->active = active ;
			signalInfo->CompareBits = IsDlgButtonChecked (hDlg, idAlleight)? EIGHT: SEVENT;
      AObjUnlockData (theObject, SGNLINFO) ;
			}
		}
}
//
//
//
void EDIT_SIGNAL::ChangeStringInList(HWND hDlg)
{
	SIGNALINFO *signalInfo ;
	LPSTR str ;
  LPSTR lpTempBlock, signalBlock ;
	int   signalCount ;
	WORD  oldStrSize, newStrSize ; 
	WORD	oldBlockSize, newBlockSize, Size1=0 ;
	int   i ;

	if ((signalCount=signal->signalCount)==0) return ;
	signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
	signalBlock =(LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
	oldBlockSize = AObjGetDataSize (theObject, SGNLBLOCK) ;
	lpTempBlock = (LPSTR) new char [oldBlockSize] ;
  _fmemcpy (lpTempBlock, signalBlock, oldBlockSize) ;
	AObjUnlockData (theObject,SGNLBLOCK) ;

	if (signalInfo)
		{
		oldBlockSize = AObjGetDataSize (theObject, SGNLBLOCK) ;
		int Index = ListBox_GetCurSel(GetDlgItem(hDlg, idList)) ; 
		newStrSize = (int) Edit_GetTextLength (GetDlgItem(hDlg, idInsString)) ;
		str = (LPSTR) farmalloc (++newStrSize) ;
		Edit_GetText (GetDlgItem (hDlg, idInsString), str, newStrSize) ;
		for (i=0;i<Index; i++, signalInfo++)
			Size1 +=signalInfo->signalLen ;
		oldStrSize = signalInfo->signalLen ;
		signalInfo->signalLen = newStrSize ;
		newBlockSize =oldBlockSize + newStrSize - oldStrSize ;
  
		if (AObjResizeData (theObject, SGNLBLOCK, newBlockSize))
 			{
			signalBlock = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
			if (signalBlock&&lpTempBlock)
   			{
				_fmemcpy (signalBlock, lpTempBlock, Size1) ;
				_fmemcpy (signalBlock+Size1, str, newStrSize) ;
   			if (Index != signalCount-1)
					_fmemcpy (signalBlock+Size1+newStrSize, lpTempBlock+Size1+oldStrSize, oldBlockSize-Size1-oldStrSize) ;
				AddSignalInfo (hDlg, signalCount, newStrSize, signalInfo->active) ;

				ListBox_DeleteString (GetDlgItem(hDlg, idList), Index) ;
				DisplayListString (hDlg, str, marktable[signalInfo->active], Index+1, Index) ;
				}
			if (signalBlock)
				AObjUnlockData (theObject, SGNLBLOCK) ;
			if (lpTempBlock)
      	delete[] lpTempBlock ;
			}
		farfree(str) ;
    AObjUnlockData (theObject, SGNLINFO) ;
    }
	
}
//
//
//
void EDIT_SIGNAL::ActivateSignal (HWND hDlg, BOOL active)
{
	SIGNALINFO *signalInfo ;
	LPSTR str ;
	WORD index ;
  WORD numChars ;

	index = (WORD)SendDlgItemMessage (hDlg, idList, LB_GETCURSEL, 0, 0) ;
	if (index==(WORD)LB_ERR) return ;

	signalInfo = (SIGNALINFO*)AObjLockData (theObject, SGNLINFO) ;
	if (signalInfo)
  	{
		signalInfo+=index ;
		if (signalInfo->active != active)
			{
			signalInfo->active = active ;
			numChars = SendDlgItemMessage (hDlg, idList, LB_GETTEXTLEN, index, 0L) ;
			str =(LPSTR) new [numChars] ;
			SendDlgItemMessage (hDlg, idList, LB_GETTEXT, index, (long)str) ; 
			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, index, 0L) ;
    	*str = marktable[active] ;
			SendDlgItemMessage (hDlg, idList, LB_INSERTSTRING, index, (long)str) ;
			delete[] str ;
			}
		AObjUnlockData (theObject, SGNLINFO) ;
		}
}
//
//
//
void EDIT_SIGNAL::SetDisplayText (HWND hDlg, LPSTR lpStr)
{
	WORD len = _fstrlen (lpStr) ;
	int  step = currentView/2 ;

	if (len)
  	{
		MEMBLOCKID BlockID = AMemAllocate (3*len) ;
		LPSTR lpPtr = (LPSTR) AMemLock (BlockID) ;
		if (step) StringToHex (lpPtr, lpStr, "%02X", step) ;
		if (!step) _fmemcpy(lpPtr, lpStr, len+1) ;
		SetDlgItemText (hDlg, idInsString, lpPtr);
		AMemUnlock (BlockID) ;
		AMemFree (BlockID) ;
		}
	else
		SetDlgItemText (hDlg, idInsString, lpStr) ;
}
//
//
//
void EDIT_SIGNAL::DisplayString(HWND hDlg)
{
	SIGNALINFO *signalInfo ;
  LPSTR       signalBlock ;
	int  i ;
	long BlockSize = 0 ;
	long nCounts ;

	nCounts = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT,0, 0L) ;
	if (nCounts!=0)
		EnableWindow (GetDlgItem(hDlg, idRemove), TRUE) ;
	if (nCounts!= 1)
		{
		EnableWindow (GetDlgItem(hDlg, idActivate), FALSE);
		EnableWindow (GetDlgItem(hDlg, idDeactivate),FALSE) ;
		return ;
		}

	signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
  signalBlock = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
	if (signalInfo&&signalBlock)
  	{
		int Index = ListBox_GetCurSel (GetDlgItem(hDlg, idList)) ;//SendDlgItemMessage (hDlg, idList, LB_GETCURSEL, 0, 0) ;
		for (i=0; i<Index; i++,signalInfo++)
   		BlockSize +=signalInfo->signalLen ;
		signalBlock +=(WORD)BlockSize ;
		SetDisplayText (hDlg, signalBlock) ;
		//SetRadioButton (hDlg, idAlleight, idLowerseven, signalInfo->CompareBits) ;
		CheckRadioButton(hDlg, idAlleight, idLowerseven,signalInfo->CompareBits?idLowerseven:idAlleight) ;

		EnableWindow (GetDlgItem(hDlg, idActivate), signalInfo->active^TRUE);
		EnableWindow (GetDlgItem(hDlg, idDeactivate),signalInfo->active) ;
		}
	if (signalInfo)
		AObjUnlockData (theObject, SGNLINFO) ;
	if (signalBlock)
  	AObjUnlockData (theObject, SGNLBLOCK) ;

	return ;
}
//
//
//
void EDIT_SIGNAL::RemoveStringsFromList (HWND hDlg)
{
	SIGNALINFO *si ;
	LPSTR       sb ;
	WORD        nCounts ;
	WORD        BlockSize=0, oldBlockSize ;
	WORD        newSignalCounts= 0, oldSignalCounts ;
  long        index ;
  int        *SelectedIndexes ;
	WORD        i, j;

	nCounts = (WORD)ListBox_GetSelCount (GetDlgItem(hDlg, idList)) ;//SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0, 0L);
	if (nCounts >0)
		{
    //Remove the selected strings from the list
		SelectedIndexes = new int[nCounts] ;
		SendDlgItemMessage(hDlg, idList, LB_GETSELITEMS, nCounts, (long)SelectedIndexes) ;
		while(SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (long) &index) !=0)
			{
			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, (WORD)index, 0) ;
			}
    //Handle the string data block
		si = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
		sb = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
		oldBlockSize = AObjGetDataSize (theObject, SGNLBLOCK) ;
		oldSignalCounts = signal->signalCount ;
		if (si)
			{
			if (sb)
      	{
				for (i=0,j=0; i<oldSignalCounts; i++)
					{
					int Index ;
					if (j==nCounts)
						Index = -1 ;
					else
   					Index = SelectedIndexes [j] ;
					if (i!=Index)
						{
						BlockSize += si->signalLen ;
						sb +=si->signalLen ;
       			si++ ;
       			newSignalCounts++ ;
						}
   				else
						{
						if (Index != oldSignalCounts-1)
							{
							_fmemmove(sb, sb+si->signalLen, oldBlockSize-BlockSize-si->signalLen) ;
							_fmemmove(si,  si+1, (oldSignalCounts-newSignalCounts-1)*sizeof (SIGNALINFO)) ;
							j++ ;
							}
   					}
					}
				AObjUnlockData (theObject, SGNLBLOCK) ;
				AObjResizeData (theObject, SGNLBLOCK, BlockSize) ;
				}
			AObjUnlockData (theObject, SGNLINFO) ;
			AObjResizeData (theObject, SGNLINFO,  newSignalCounts*sizeof(SIGNALINFO)) ;
			signal->signalCount = newSignalCounts ;
			}
		delete []SelectedIndexes ;
		}
	ClearStringEdit (hDlg) ;
	//if (nCounts ==0)
	//	{
		EnableWindow (GetDlgItem (hDlg, idRemove), FALSE) ;
		EnableWindow (GetDlgItem (hDlg, idActivate), FALSE) ;
		EnableWindow (GetDlgItem (hDlg, idDeactivate), FALSE) ;
	//	}
	return ;                                               
}
//
//
//
void EDIT_SIGNAL::SaveCompareBits (HWND hDlg, int idButton)
{
	long nCount ;
	int  Index ;
  SIGNALINFO *signalInfo ;

	signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
  if (signalInfo)
    {
		nCount = ListBox_GetSelCount(GetDlgItem (hDlg, idList)) ;// SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0, 0);
		if (nCount==1)
    	{
			ListBox_GetSelItems(GetDlgItem(hDlg, idList),1, &Index) ; // SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (long)&Index) ;
  		signalInfo +=Index  ; 
			signalInfo->CompareBits = (idButton==idAlleight)?EIGHT:SEVENT ;
			}
    AObjUnlockData (theObject, SGNLINFO) ;
    }
	return ;
}
BOOL EDIT_SIGNAL::ChangeSignalCheckButton (HWND hwnd, int idItem)
{
	BYTE     checkByte ;
	BOOL     check ;

  check = IsDlgButtonChecked (hwnd, idItem) ;
  switch (idItem)
    {
    case idFoundStr:
    	checkByte = FOUNDSTR ;
      break ;
    case idFoundPos:
      checkByte = FOUNDPOS ;
      break ;
    case idIntercept:
      checkByte = INTERCEPT ;
      break ;
    }
  if (check)
    signal->checkBox |=checkByte ;
  else
    signal->checkBox ^=checkByte ;

	SetSignalCheckButton (hwnd) ;

  return TRUE ;
}
void EDIT_SIGNAL::ChangeComboBoxContent (HWND hwnd, int idItem)
{
  WORD     index ;
  OBJECTID oiObj ;

  index = (WORD)SendDlgItemMessage (hwnd, idItem, CB_GETCURSEL, 0, 0) ;
  oiObj = SendDlgItemMessage (hwnd, idItem, CB_GETITEMDATA, index, 0) ;
  switch (idItem)
    {
    case idFoundStrCon:
      signal->FoundString = oiObj ;
      break ;

    case idFoundPosCon:
      signal->FoundIndex = oiObj ;
      break ;
    case idInterceptCon:
      signal->InteText = oiObj ;
      break ;
    } 
}
//
//
//
void EDIT_SIGNAL::EditSignalString (HWND hDlg, LPARAM lParam)
{
	if (HIWORD(lParam)==EN_SETFOCUS)
		SendDlgItemMessage (hDlg, idInsString, EM_SETSEL, TRUE, MAKELONG(0,-1)) ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void EDIT_SIGNAL::ConfigureDialog (HWND hDlg)
{
	BOOL  ok ;
	DATA0 *tmpdata0 ;

	tmpdata0 = new DATA0 ;
	if(tmpdata0)
		*tmpdata0 = *data0 ;
	ok = DialogBoxParam(hInstance, "Connection" , hDlg, (FARPROC)CntObjProc, (long)this) ;
	if(!ok)
		*data0 = *tmpdata0 ;
	if (tmpdata0)
		delete tmpdata0 ;
}

void DisplayListString (HWND hwnd, LPSTR string, char mark, int signalNo, int Index)
{
	char *displaystr ;
	WORD  len = _fstrlen(string);

	displaystr = new char[len+10] ;
  if (len)
		sprintf(displaystr,"%c%d%c%s", mark, signalNo, ':',string) ;
	else
  	sprintf(displaystr,"%c%d", mark, signalNo) ;
	SendDlgItemMessage (hwnd, idList, LB_INSERTSTRING, Index, (long)displaystr) ;
	delete[]displaystr ;
}
void SetCompareBits (HWND hwnd, int idButton)
{
	CheckRadioButton (hwnd, idAlleight, idLowerseven, idButton) ;
}
void ClearStringEdit (HWND hwnd)
{
	Edit_SetSel (GetDlgItem(hwnd,idInsString), 0, -1);
	Edit_ReplaceSel (GetDlgItem(hwnd, idInsString), " ") ;
}
long ChangeHexToDec (char* lpDesc, char* lpSource)
{
	WORD i=0 ;
	int  currChar=0 ;
  int  nextChar=0 ;
	int  Decimal ;
	char c ;

	while(*lpSource!='\0')
    {
		if (*lpSource!=' ')
    	{
			currChar = HexToDec(*lpSource) ;
			while (*++lpSource==' ')
				{
				}
			if (lpSource!='\0')
      	{
				nextChar = HexToDec(*lpSource++) ;
   	 		Decimal = 16*currChar + nextChar ;
      	c= (char) Decimal ;
				lpDesc[i++] = c ;
				}
			else
				break ;
			}
		else
			++lpSource ;
    }
		lpDesc[i] = '\0' ;
  return (strlen (lpDesc)) ;
}
void TextToHex (LPSTR lpBuffer, LPSTR lpText, LPSTR lpFormat, LONG/* len*/)
{
	char c ;
  char tbuf[80] ;

  sprintf (lpBuffer, lpFormat, *lpText++) ;
  while((c=*lpText++) !='\0')
		{
    sprintf((char*)tbuf, lpFormat, c);
    strcat(lpBuffer, (char *)tbuf);
    }
}
//
//
//
int HexToDec (int c)
{
    //int decimal ;

    (char)c>'9'?c=c+9:c ;

		//return(decimal = c&0x0F );
    return (c&0x0F) ;
}
//////////////////////////////////////////////////////////////////////////
//function: StrChr
//Param1:
//Param2:
//Param3:   compareBits means that compare all eight bits (1) or lower sevent
//          bits of a char.
//////////////////////////////////////////////////////////////////////////
LPSTR StrChr (LPSTR str, char c, int compareBits)
{
	while(*str)
		{
		if (compareBits)
    	{
			if (*str==c)
				break ;
			}
		else
			{
			if ((*str|0x7F)==(c|0x7F))
      	break ;
      }
    str++ ;
		}

  return str ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
long StrSpn (LPSTR str1, LPSTR str2, int compareBits)
{
	long length = 0 ;

	while(*str1)
		{
		if (!*str2)
			break ;
		if (compareBits)
    	{
			if(*str1!=*str2)
				break ;
			}
		else
			{
			if((*str1|0x7F)!=(*str2|0x7F))
      	break ;
      }
		str1++ ;
		str2++ ;
		length++ ;	
		}
 return length ;
}
//////////////////////////////////////////////////////////////////////////
//*result = 1 ; str2 is substring of str1, return vaule points the first same
//              of str2 in str1.
//*result = -1; a part of str2 is same as str1, return value points the first
//              same char of str2 in str1.               ;
//*result = 0 ; there is not same part int str1, str2. return NULL pointer.
//////////////////////////////////////////////////////////////////////////
LPSTR StrCmp (LPSTR str1, LPSTR str2, int compareBits, int* result)
{
	LPSTR p ;
	WORD length ;
	 
	*result = 1 ;
	p = str1 ;
	p = StrChr (p,*str2, compareBits) ;

	if (p)
		{ 
		length = StrSpn (p, str2, compareBits) ;
		if (length==_fstrlen(str2))
			{
      *result = 1 ;
			}
		else if(length ==_fstrlen(p))
			{
      *result = -1 ;
			}
		else
    	*result = 0 ;
		}
	else
		{
		p = NULL ;
		*result = 0 ;
    }

  return p ;
}
BOOL ALMCALLBACK ConnectionDialog (
	HWND hDlg,
	WORD message,
	WORD wParam,
	LONG lParam)
{
	EDIT_SIGNAL* es ;

	switch (message)
		{
		case WM_INITDIALOG:
			es = (EDIT_SIGNAL*) lParam ;
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			es->InitSignalDialog(hDlg) ;
			return FALSE ;

    case WM_COMMAND:
			es = (EDIT_SIGNAL *)GetWindowLong (hDlg, DWL_USER) ;
   		switch(wParam)
       	{
        case idASCII:
        case idByte:
        case idWord:
        case idLongword:
					es->ChangeViewMode (hDlg, wParam) ;
					return FALSE ;

        case idAdd:
          es->AddStringToList (hDlg) ;
          return FALSE ;

        case idAdds:
				  es->ChangeStringInList(hDlg) ;
          return FALSE;

				case idRemove:
					es->RemoveStringsFromList (hDlg) ;
					return FALSE ;

				case idActivate:
          es->ActivateSignal (hDlg, TRUE) ;
					return FALSE ;

				case idDeactivate:
				 	es->ActivateSignal (hDlg, FALSE) ;
					return FALSE ;

        case idList:
          switch(HIWORD(lParam))
            {
            case CBN_SELCHANGE:
              es->DisplayString (hDlg) ;
            }
						return FALSE ;

				case idInsString:
        	es->EditSignalString (hDlg, lParam) ;
					if (HIWORD(lParam)==EN_SETFOCUS)
						Edit_SetSel (GetDlgItem(hDlg,idInsString), 0, -1); // SendDlgItemMessage (hDlg, idInsString, EM_SETSEL, TRUE, MAKELONG(0,-1)) ;
					return FALSE ;

				case idAlleight:
				case idLowerseven:
				 	es->SaveCompareBits (hDlg, wParam) ;
					return FALSE ;

        case idFoundStr:
        case idFoundPos:
        case idIntercept:
					es->ChangeSignalCheckButton (hDlg, wParam) ;
          return FALSE ;

        case idFoundStrCon:
        case idFoundPosCon:
        case idInterceptCon:
					es->ChangeComboBoxContent (hDlg, wParam) ;
          return FALSE ;

        case idConfigure:
					es->ConfigureDialog (hDlg) ;
					return FALSE ;

				case IDOK:
					EndDialog (hDlg, TRUE) ;
					return FALSE ;

        case IDCANCEL:
				 	EndDialog (hDlg, FALSE) ;
					return TRUE ;

				}
        return FALSE ;
			}
       return FALSE ;
}
//
// Callback function 
//
long ALMCALLBACK EditProc (HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
{
	BYTE viewMode ;

	switch (message)
		{
		case WM_CHAR:
			viewMode = IsDlgButtonChecked (GetParent (hwnd), idASCII) ;
			if (viewMode) break ;
			if ((wParam==VK_BACK)||(wParam ==VK_INSERT)||(wParam==VK_DELETE)||(wParam<=VK_DOWN&&wParam>=VK_PRIOR)||
				(wParam>=48&&wParam<=57)||(wParam>=65&&wParam<=70)||(wParam>=97&&wParam<=102)) break;
			MessageBeep (-1) ;
			return 0 ;
		}

	return CallWindowProc (defOldEditProc, hwnd, message, wParam, lParam) ;
}
//