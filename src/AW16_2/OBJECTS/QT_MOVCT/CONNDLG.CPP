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
#define STRICT
#define ASTRICT

#define CONST

#include <windowsx.h>
#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include "com_dlg.h"
#include "commObj.h"

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
			AUtlCenterDialog(hDlg, 0);
			conn = (CONN*)lParam ;
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			conn->InitCntDialog(hDlg) ;
			return FALSE ;

		case WM_COMMAND: //!!PORT!!
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
	ok = DialogBoxParam(hInstance, "Modem", hDlg, (DLGPROC)ModemSettingsProc, (LPARAM)this) ;
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
			AUtlCenterDialog(hDlg, 0);
			conn = (CONN*)lParam ;
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			conn->InitModemSettings(hDlg) ;
			return FALSE ;

		case WM_COMMAND: //!!PORT!!
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
	if (SendDlgItemMessage (hDlg, idDialMethod, CB_GETCOUNT, 0, 0L)==0)
	  {
		size =sizeof(cstDialM)/sizeof(cstDialM[0]) ;
  	for (i=0; i<size; i++)
    	{
			index = (WORD)SendDlgItemMessage(hDlg, idDialMethod, CB_INSERTSTRING, -1, (long)cstDialM[i]) ;
    	if (i==data0->modem.DialMethod)
      	{
	    	SendDlgItemMessage (hDlg, idDialMethod, CB_SETCURSEL, index, 0) ;
      	}
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
				lstrcpy(spPhoneNumber, outputText) ;
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
	//Save Dial Method
	int index = (int)SendDlgItemMessage (hDlg, idDialMethod, CB_GETCURSEL, 0, 0L) ;
	if (index!=CB_ERR)
		data0->modem.DialMethod = index ;
}
//////////////////////////////////////////////////////////////////////////
//ExportObject Function
//////////////////////////////////////////////////////////////////////////
/*void CONN::ExportObject(void)
{
	ExportConnDataBlock0 () ;
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
}	*/
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
	MEMBLOCKID  signalMID ;
	SIGNALINFO *signalInfo ;
	LPSTR       signalBlock ;
	pAObjSignalRecord signal ;
	long        signalCount = data0->signal.signalCount+4 ;
  int         i ;

  signalMID = (MEMBLOCKID)theSystem->message2;
	AMemSetSize(signalMID, signalCount*(long)sizeof(AObjSignalRecord));
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
		signalInfo = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
		if (signalInfo)
		  {
			for (i=0; i<data0->signal.signalCount; i++, signalInfo++)
				{
				signal[i+4].theID = i+4 ;
				if (lstrlen(signalBlock))
					sprintf (signal[i+4].theName, "%d %c%s%c",signalInfo->signalIndex+1,'"', signalBlock, '"') ;
				else
      		sprintf (signal[i+4].theName, "%d", signalInfo->signalIndex+1) ;
				signalBlock +=lstrlen (signalBlock)+1 ;
				}
			AObjUnlockData (theObject, SGNLINFO) ;
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
