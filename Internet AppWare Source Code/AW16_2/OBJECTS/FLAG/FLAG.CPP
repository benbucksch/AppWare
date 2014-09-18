////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag.cpp"
//
// The FLAG class member function module 
//
////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<a_alm.h>
#include	"flag_hdr.h"
#include	"flag.h"
#include	"flag_rsc.h"


// FLAG::FLAG -- full object build /////////////////////////////////////////

FLAG::FLAG(
	OBJECTID	objID)
{
	if(objID!=NULL)
	{							// load and/or initialize the object
		theObject = objID;
	  if(AObjGetDataCount(theObject)!=NUM_FLAGDATA)
  	  AObjSetDataCount(theObject, NUM_FLAGDATA);			
		fdata = (FLAGDATA*)AObjLockData(theObject, DATA_FLAGDATA);
		setText = (char*)AObjLockData(theObject, DATA_SETTEXT);
		clearText = (char*)AObjLockData(theObject, DATA_CLEARTEXT);
		if(!fdata)
		{
			if(AObjResizeData(theObject, DATA_FLAGDATA, (long)sizeof(FLAGDATA)))
      {
				fdata = (FLAGDATA*)AObjLockData(theObject, DATA_FLAGDATA);
				ADBG_ASSERT_E(fdata != NULL, return, "invalid data ptr in flag constructor");
				fdata->version = VERSION_OBJECT;
				fdata->isSet = FALSE;
				fdata->interruptChains = FALSE;
      }
		}			// if(!fdata)...
		if(!setText)
		{
			if (AObjResizeData(theObject, DATA_SETTEXT, (long)lstrlen(DEFAULT_SETTEXT)+1L))
			{
				setText = (char*)AObjLockData(theObject, DATA_SETTEXT);
				ADBG_ASSERT_E(setText != NULL, return, "invalid set text ptr in flag constructor");
				lstrcpy(setText, DEFAULT_SETTEXT);
      }
		}			// if(!setText)...
		if(!clearText)
		{
			if(AObjResizeData(theObject, DATA_CLEARTEXT, (long)lstrlen(DEFAULT_CLEARTEXT)+1L))
			{
				clearText = (char*)AObjLockData(theObject, DATA_CLEARTEXT);
				ADBG_ASSERT_E(clearText != NULL, return, "invalid clear text ptr in flag constructor");
				lstrcpy(clearText, DEFAULT_CLEARTEXT);
      }
		}			// if(!clearText)...
		AObjUnlockData(theObject, DATA_FLAGDATA);
		AObjUnlockData(theObject, DATA_SETTEXT);
		AObjUnlockData(theObject, DATA_CLEARTEXT);
	}			// if(objID!=NULL)...
}			// FLAG::FLAG(...


// FLAG::FLAG -- partial object build //////////////////////////////////////

FLAG::FLAG(
	OBJECTID	objID,
	BOOL			/*partialBuild*/)

	{
	theObject = objID;
	setText = NULL;
	clearText = NULL;

	fdata = (FLAGDATA*)AObjLockData(theObject, DATA_FLAGDATA);
	if(!fdata)
		{
		// initialize the object
    AObjSetDataCount(theObject, NUM_FLAGDATA);
		if (!(AObjResizeData(theObject, DATA_FLAGDATA, (long)sizeof(FLAGDATA))))
			return;
		fdata = (FLAGDATA*)AObjLockData(theObject, DATA_FLAGDATA);
		fdata->version = VERSION_OBJECT;
		fdata->isSet = FALSE;
		fdata->interruptChains = FALSE;
		}
	AObjUnlockData(theObject, DATA_FLAGDATA);
	}

// FLAG::Export ////////////////////////////////////////////////////////////

void FLAG::Export()

	{
	AUpsfExportField("VERSION ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &fdata->version, (long)sizeof(int));
	AUpsfExportField("VALUE   ", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &fdata->isSet, (long)sizeof(BOOL));
	AUpsfExportField("INTCHAIN", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &fdata->interruptChains, (long)sizeof(BOOL));
	AUpsfExportField("ONVALUE ", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, setText, (long)lstrlen(setText));
	AUpsfExportField("OFFVALUE", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, clearText, (long)lstrlen(clearText));
	}
  
// FLAG::Import ////////////////////////////////////////////////////////////

void FLAG::Import()

	{
  int	version;
	AUpsfImportField("VERSION ", 0L, AUPSF_NATV_INT, &version, (long)sizeof(int));

	AUpsfImportField("VALUE   ", 0L, AUPSF_NATV_BOOL, &fdata->isSet, (long)sizeof(BOOL));
	AUpsfImportField("INTCHAIN", 0L, AUPSF_NATV_BOOL, &fdata->interruptChains, (long)sizeof(BOOL));

  long	size;
	if(AUpsfFieldInfo("ONVALUE ", 0L, AUPSF_NATV_STRING, NULL, &size) == AUPSF_NOERROR)
	{
		AObjUnlockData(theObject, DATA_SETTEXT);
		if (AObjResizeData(theObject, DATA_SETTEXT, size))
    {
			setText = (char*)AObjLockData(theObject, DATA_SETTEXT);
			AUpsfImportField("ONVALUE ", 0L, AUPSF_NATV_STRING, setText, size);
			AObjUnlockData(theObject, DATA_SETTEXT);
		}
	}
	else
		AUpsfLogComment("Error importing the text for the flag's set state.");

	if(AUpsfFieldInfo("OFFVALUE", 0L, AUPSF_NATV_STRING, NULL, &size) == AUPSF_NOERROR)
	{
		AObjUnlockData(theObject, DATA_CLEARTEXT);
		if (AObjResizeData(theObject, DATA_CLEARTEXT, size))
    {
			clearText = (char*)AObjLockData(theObject, DATA_CLEARTEXT);
			AUpsfImportField("OFFVALUE", 0L, AUPSF_NATV_STRING, clearText, size);
			AObjUnlockData(theObject, DATA_CLEARTEXT);
		}
	}
	else
		AUpsfLogComment("Error importing the text for the flag's clear state.");
	}

// FLAG::InitDlgControls ///////////////////////////////////////////////////

void FLAG::InitDlgControls(
	HWND	hDlg)

	{
  // set the initial value radio buttons
	CheckDlgButton(hDlg, IDD_SET, fdata->isSet);
	CheckDlgButton(hDlg, IDD_CLEAR, !fdata->isSet);

  // set the signal priority radio buttons
	CheckDlgButton(hDlg, IDD_AFTERCHAIN, !fdata->interruptChains);
	CheckDlgButton(hDlg, IDD_INTERRUPT, fdata->interruptChains);

  // set the text equivalent edit fields
	SetDlgItemText(hDlg, IDD_SETTEXT, setText);
	SetDlgItemText(hDlg, IDD_CLEARTEXT, clearText);
	}

// FLAG::UpdateObject //////////////////////////////////////////////////////

void FLAG::UpdateObject(
	HWND	hDlg)
{
	fdata->isSet = IsDlgButtonChecked(hDlg, IDD_SET);
	fdata->interruptChains = IsDlgButtonChecked(hDlg, IDD_INTERRUPT);

	WORD	length = (WORD)SendDlgItemMessage(hDlg, IDD_SETTEXT, WM_GETTEXTLENGTH, 0, 0L);

	AObjUnlockData(theObject, DATA_SETTEXT);
	if (AObjResizeData(theObject, DATA_SETTEXT, (long)length+1))
	{
		setText = (char*)AObjLockData(theObject, DATA_SETTEXT);
		GetDlgItemText(hDlg, IDD_SETTEXT, setText, length+1);
		length = (WORD)SendDlgItemMessage(hDlg, IDD_CLEARTEXT, WM_GETTEXTLENGTH, 0, 0L);
		AObjUnlockData(theObject, DATA_SETTEXT);
		if (AObjResizeData(theObject, DATA_CLEARTEXT, (long)length+1))
		{
			clearText = (char*)AObjLockData(theObject, DATA_CLEARTEXT);
			GetDlgItemText(hDlg, IDD_CLEARTEXT, clearText, length+1);
			AObjUnlockData(theObject, DATA_CLEARTEXT);
		}
		AObjUnlockData(theObject, DATA_SETTEXT);
	}
}

// FLAG::Clear /////////////////////////////////////////////////////////////

void FLAG::Clear()

	{
	if(fdata->isSet)
  	Toggle();
	}

// FLAG::Set ///////////////////////////////////////////////////////////////

void FLAG::Set()

	{
	if(!fdata->isSet)
		Toggle();
	}

// FLAG::Toggle ////////////////////////////////////////////////////////////

void FLAG::Toggle()

	{
	if(fdata->isSet)
		fdata->isSet = FALSE;
	else
		fdata->isSet = TRUE;

	if(fdata->interruptChains)
		// send the signals AEVT_ATMARK
		AEvtPostSignalAtMark(theObject, fdata->isSet ? SIGNAL_SET : SIGNAL_CLEARED);
	else
		// send the signals AEVT_ATTAIL | AEVT_KILLDUPLICATES
		AEvtPostStandard(theObject, AEVENT_SIGNAL, fdata->isSet ? SIGNAL_SET : SIGNAL_CLEARED, AEVT_ATTAIL | AEVT_KILLDUPLICATES);

	// post the AEVENT_OBJECTCHANGED for other objects.....
	AEvtPostStandard(theObject, AEVENT_OBJECTCHANGED, 0L, AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL));
	}

////////////////////////////////////////////////////////////////////////////
