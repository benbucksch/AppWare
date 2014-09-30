#include <memory.h>
#include "almsys.h"

#include "cfglib.h"
#include "respdlg.h"
#include "newcomp.h"
#include "cfgdesc.h"
#include "rectypes.h"
#include "sereclib.h"
#include "response.h"
#include "evntdesc.h"

BOOL I_CALLBACK ResponsesDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXFUNCCHARS	80
#define	MAXUIDCHARS		4

#define BN_INSERT			103
#define BN_REMOVE			104
#define BN_UPDATE			105
#define BN_HELP				114
#define BN_DEFINE			113

#define SLE_DLL	   		108
#define SLE_FUNCTION	109
#define CB_EVENT			107

#define LB_RESPONSES	102


static HINSTANCE	H_APP_INSTANCE;
static HINSTANCE	H_PREV_INSTANCE;

static COMPUID  objectTypeID;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ResponsesDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ResponsesDlgBox(
	HINSTANCE				hAppInstance,
	HINSTANCE				hPrevInstance,
	HWND						hWndParent,
	COMPUID         objTypeID,
	LPRESPONSEARRAY	Responses)
	{
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	::objectTypeID = objTypeID;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)ResponsesDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "RESPONSES_DLG", hWndParent, lpProc,
	 (DWORD)Responses);
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateResponseIndices
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void UpdateResponseIndices(
	HWND  hDlg,
	short deletedIndex)
	{
	short entryCount;
	short i;
	long  currentIndex;
	HWND  hList;

	hList = GetDlgItem(hDlg, LB_RESPONSES);

	entryCount = SendMessage(hList, LB_GETCOUNT, 0, 0);

	for (i=0; i<entryCount; i++)
		{
		currentIndex = SendMessage(hList, LB_GETITEMDATA, i, 0L);
		if (currentIndex > deletedIndex)
			SendMessage(hList, LB_SETITEMDATA, i, currentIndex - 1);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentEvent
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SetCurrentEvent(
	HWND     hDlg,
	COMPUID  eventID)
	{
	long  eventsCount;
	int   i;

	eventsCount = SendDlgItemMessage(hDlg, CB_EVENT, CB_GETCOUNT, 0, 0L);

	for (i=0; i<eventsCount; i++)
		{
		if (eventID == SendDlgItemMessage(hDlg, CB_EVENT, CB_GETITEMDATA,
				i, 0))
			{
			break;
			}
		}

	if (i < eventsCount)
		SendDlgItemMessage(hDlg, CB_EVENT, CB_SETCURSEL, i, 0);
	else
		SendDlgItemMessage(hDlg, CB_EVENT, CB_SETCURSEL, -1, 0);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentResponseData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SetCurrentResponseData(
	HWND						hDlg,
	LPRESPONSEARRAY	Responses)
	{
	int			itemIndex;
	int			responseIndex;
	COMPUID eventID;

	itemIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR)
		{
		SendDlgItemMessage(hDlg, SLE_DLL, WM_SETTEXT, 0, (long)"");
		SendDlgItemMessage(hDlg, SLE_FUNCTION, WM_SETTEXT, 0, (long)"");
		SendDlgItemMessage(hDlg, CB_EVENT, CB_SETCURSEL, -1, 0);
		}
	else
		{
		responseIndex =
		 SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETITEMDATA, itemIndex, 0L);
		SendDlgItemMessage(hDlg, SLE_DLL, WM_SETTEXT, 0,
		 (long)(LPSTR)(*Responses)[responseIndex].GetDll());
		SendDlgItemMessage(hDlg, SLE_FUNCTION, WM_SETTEXT, 0,
		 (long)(LPSTR)(*Responses)[responseIndex].GetFunction());

		eventID = (*Responses)[responseIndex].GetEventID();

		SetCurrentEvent(hDlg, eventID);

		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ResponsesDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void DefineAnEvent(
	HWND   hDlg)
	{
	NEWCOMPDATA  eventData;
	SERUID       eventUID;
	LPCFGDESC    pConfig;

	if (NewCompDlgBox(H_APP_INSTANCE, GetActiveWindow(), "event1", "Event",
			 RT_EVENTDESC, &eventData))
		{
		pConfig = (LPCFGDESC)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		eventUID =
		 pConfig->CreateEventDesc(eventData.compName, eventData.compUID);
		((LPEVENTDESC)GetObject(eventUID))->SetTypeID(::objectTypeID);
		cfgInitEventList(GetDlgItem(hDlg, CB_EVENT));
		SetCurrentEvent(hDlg, eventData.compUID);
    pTheSystem->GetFileObject()->SetDirtyFlag();
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MakeEventString
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static LPSTR	MakeEventString(
	COMPUID eventID)
	{
	static char			buffer[MAXFUNCCHARS+10];  //add room for eventid
	int							charIndex;

	lstrcpy(buffer, cfgGetEventName(eventID));

	lstrcat(buffer, " (");

	charIndex = lstrlen(buffer);

	*((LPCOMPUID)&buffer[charIndex]) = eventID;

	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	buffer[charIndex]^=buffer[charIndex+3]^=buffer[charIndex]^=
	 buffer[charIndex+3];
	buffer[charIndex+1]^=buffer[charIndex+2]^=buffer[charIndex+1]^=
	 buffer[charIndex+2];
	charIndex += sizeof(COMPUID);

	buffer[charIndex] = ')';

	buffer[charIndex + 1] = '\0';

	return buffer;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
/////////////////////////////////////////////////////////////////////////////
static long InitDialog(
	HWND							hDlg,
	LPRESPONSEARRAY		Responses)
	{
	long	 itemID;
	int		 responseCount;
	int		 i;
  long   initialResponseIndex;

	SendDlgItemMessage(hDlg, SLE_DLL, EM_LIMITTEXT, MAXDLLCHARS, 0L);
	SendDlgItemMessage(hDlg, SLE_DLL, EM_LIMITTEXT, MAXFUNCCHARS, 0L);

	cfgInitEventList(GetDlgItem(hDlg, CB_EVENT));

	//add responses to the response list box

	responseCount = Responses->GetCount();
	for (i=0; i<responseCount; i++)
		{
		itemID = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_ADDSTRING, 0,
		 (long)MakeEventString((*Responses)[i].GetEventID()));

		//item data holds index of response in response array
		//accounts for non-sorted entries in the response array created
		//previous to sorting.

		SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETITEMDATA, itemID, i);
		}

	// Set selection to the first response in the listblock

	if (responseCount >= 1)
		{
		if (LB_ERR != SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETCURSEL, 0, 0L))
		  initialResponseIndex = 0;
		else
		  initialResponseIndex = -1;
		SetCurrentResponseData(hDlg, Responses);
		SetFocus(GetDlgItem(hDlg, LB_RESPONSES));
		}
	else
    {
    initialResponseIndex = -1;
		SetFocus(GetDlgItem(hDlg, CB_EVENT));
    }
    
  return initialResponseIndex;  

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: HaveResponseAlready
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL HaveResponseAlready(
  COMPUID         eventID,
	LPRESPONSEARRAY	Responses)
  {
  short responseCount;
  short i;

	responseCount = Responses->GetCount();

	for (i=0; i<responseCount; i++)
		{
		if ( eventID == (*Responses)[i].GetEventID() )
			return TRUE;
		}

	return FALSE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetCurrentResponseData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void GetCurrentResponseData(
	HWND						hDlg,
	COMPUID         &eventID,
	LPSTR						dll,
	LPSTR           function)
	{

	SendDlgItemMessage(hDlg, SLE_FUNCTION, WM_GETTEXT, MAXFUNCCHARS+1,
	 (long)function);

	SendDlgItemMessage(hDlg, SLE_DLL, WM_GETTEXT, MAXDLLCHARS+1,
	 (long)dll);

	eventID = SendDlgItemMessage(hDlg, CB_EVENT, CB_GETITEMDATA,
	 SendDlgItemMessage(hDlg, CB_EVENT, CB_GETCURSEL, 0L, 0L), 0);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InsertResponse
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertResponse(
	HWND						hDlg,
	LPRESPONSEARRAY	responses)
	{
	char						function[MAXFUNCCHARS+1];
	char						dll[MAXDLLCHARS+1];
	COMPUID					eventID;
	long						responseIndex;
	long						topIndex;
  long            itemIndex;

  GetCurrentResponseData(hDlg, eventID, dll, function);

	if (HaveResponseAlready(eventID, responses))
		{
		MessageBox(hDlg, "A response for this event has already been defined.",
		 "Response Error", MB_OK);
		MessageBeep(0);
		}
	else if (eventID == CB_ERR)
		{
		MessageBox(hDlg, "You must select an event to respond to.",
		 "Response Insertion Error", MB_OK);
		MessageBeep(0);
		}
	else
		{
		itemIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_ADDSTRING,
		 0, (long)MakeEventString(eventID));

		//accounts for non-sorted entries in the response array created
		//previous to sorting.

		//add new response to the end of the response array
		responses->Insert(32767, eventID, dll, function);

		responseIndex = responses->GetCount() - 1;

		SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETITEMDATA, itemIndex,
		 responseIndex);

		SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETCURSEL, itemIndex, 0);

		topIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETTOPINDEX, 0, 0L);

		// make sure the new response is visible
		if ((itemIndex-topIndex>7) || (itemIndex<topIndex))
			{
			if (itemIndex < 7)
				SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETTOPINDEX, 0, 0L);
			else
				SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETTOPINDEX,
				 itemIndex-4, 0L);
			}

		SetFocus(GetDlgItem(hDlg, CB_EVENT));
    
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateResponse
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateResponse(
	HWND						hDlg,
	LPRESPONSEARRAY	responses,
	long            itemIndex,
	BOOL            reselectFlag = TRUE)
	{
	char						function[MAXFUNCCHARS+1];
	char						dll[MAXDLLCHARS+1];
	COMPUID					eventID;
	LPRESPONSE			tempResponse;
	long						responseIndex;
	long						topIndex;

  GetCurrentResponseData(hDlg, eventID, dll, function);

  if (itemIndex == -1)
	  itemIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);
	if (itemIndex == LB_ERR)
		MessageBeep(0);
	else
		{
		responseIndex =
		 SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETITEMDATA, itemIndex, 0);

		tempResponse = (LPRESPONSE)responses->GetElement(responseIndex);
		tempResponse->SetFunction(function);
		tempResponse->SetDll(dll);

		if (eventID != tempResponse->GetEventID())
			{
			tempResponse->SetEventID(eventID);

			//Delete and reinsert entry in proper order

			SendDlgItemMessage(hDlg, LB_RESPONSES, LB_DELETESTRING, itemIndex, 0L);
			itemIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_ADDSTRING, 0,
			 (long)MakeEventString(eventID));
			SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETITEMDATA, itemIndex,
			 responseIndex);
       
      if (reselectFlag == TRUE)
        { 
				SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETCURSEL, itemIndex, 0L);

				// make sure the updated response is visible
  			topIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETTOPINDEX, 0, 0L);
				if ((itemIndex-topIndex>7) || (itemIndex<topIndex))
					{
					if (itemIndex < 7)
						SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETTOPINDEX, 0, 0L);
					else
						SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETTOPINDEX,
						 itemIndex-4, 0L);
					}
        }
			}
		}

	SetFocus(GetDlgItem(hDlg, LB_RESPONSES));

  	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: QuerySaveChanges
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL QuerySaveChanges(
	HWND	hDlg)
	{
	int 	response;
	BOOL	result;

	response = MessageBox(hDlg, "Save Changes to current Response?",
	 "Save Changes", MB_YESNO|MB_ICONQUESTION);

	if (response == IDYES)
		result = TRUE;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteCurrentResponse
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static LONG DeleteCurrentResponse(
	HWND						hDlg,
	LPRESPONSEARRAY	Responses)
	{
	short  itemIndex;
	short  responseIndex;

	itemIndex = SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR)
		{
		MessageBeep(0);
		}
	else
		{
		responseIndex =
		 SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETITEMDATA, itemIndex, 0L);
		if (Responses->Delete(responseIndex))
		 {
		 SendDlgItemMessage(hDlg, LB_RESPONSES, LB_DELETESTRING, itemIndex, 0L);
     if (LB_ERR == 
         SendDlgItemMessage(hDlg, LB_RESPONSES, LB_SETCURSEL, itemIndex, 0L))
       itemIndex = -1;
		 UpdateResponseIndices(hDlg, responseIndex);
		 SetCurrentResponseData(hDlg, Responses);
		 }
		}

  return itemIndex;
  
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ResponsesDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL I_CALLBACK ResponsesDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPRESPONSEARRAY Responses;
	static BOOL						 responseIsDirty;
  static long            responseIndex;
	BOOL									 msgHandled = TRUE;
  NEWCOMPDATA            eventData;


	switch (message)
		{
		case WM_INITDIALOG:
			Responses = (LPRESPONSEARRAY)lParam;
			responseIndex = InitDialog(hDlg, Responses);
			responseIsDirty = FALSE;
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if ((responseIsDirty == TRUE) &&
						  (responseIndex != -1) &&
					    QuerySaveChanges(hDlg))
						UpdateResponse(hDlg, Responses, responseIndex, FALSE);
					EndDialog(hDlg, TRUE);
					break;

				case BN_INSERT:
					InsertResponse(hDlg, Responses);
          responseIndex = 
           SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);
					responseIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (responseIsDirty == TRUE)
						UpdateResponse(hDlg, Responses,
						 SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L));
          responseIndex = 
           SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);
					responseIsDirty = FALSE;
					break;

				case BN_REMOVE:
					responseIndex = DeleteCurrentResponse(hDlg, Responses);
					responseIsDirty = FALSE;
					break;

				case BN_DEFINE:
					DefineAnEvent(hDlg);
					break;

				case LB_RESPONSES:
					if (HIWORD(lParam) == LBN_SELCHANGE)
						{
						if ((responseIsDirty == TRUE) && 
						    (responseIndex != -1) &&
						    QuerySaveChanges(hDlg)
						   )
							UpdateResponse(hDlg, Responses, responseIndex, FALSE);
	          responseIndex = 
	           SendDlgItemMessage(hDlg, LB_RESPONSES, LB_GETCURSEL, 0, 0L);
						SetCurrentResponseData(hDlg, Responses);
						responseIsDirty = FALSE;
						}
					break;

				case CB_EVENT:
					if (HIWORD(lParam) == CBN_SELCHANGE)
						responseIsDirty = TRUE;
					break;

				case SLE_DLL:
					if (HIWORD(lParam) == EN_CHANGE)
						responseIsDirty = TRUE;
					break;

				case SLE_FUNCTION:
					if (HIWORD(lParam) == EN_CHANGE)
						responseIsDirty = TRUE;
					break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}


