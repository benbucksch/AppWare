#include "almsys.h"
#include <windowsx.h>

#include "cfglib.h"
#include "respdlg.h"
#include "newcomp.h"
#include "cfgdesc.h"
#include "rectypes.h"
#include "sereclib.h"
#include "response.h"
#include "evntdesc.h"

BOOL CALLBACK ResponsesDlgProc(
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
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	::objectTypeID = objTypeID;

	result = DialogBoxParam(hAppInstance, "RESPONSES_DLG", hWndParent,
										ResponsesDlgProc, (DWORD)Responses);
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
	HWND  hList				= GetDlgItem(hDlg, LB_RESPONSES);
	int		entryCount;
	int		i;
	long  currentIndex;

	entryCount = ListBox_GetCount(hList);

	for (i=0; i<entryCount; i++)
		{
		currentIndex = ListBox_GetItemData(hList, i);
		if (currentIndex > deletedIndex)
			ListBox_SetItemData(hList, i, currentIndex - 1);
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
	HWND			hEvent		= GetDlgItem(hDlg, CB_EVENT);
	long  eventsCount;
	int   i;

	eventsCount = ComboBox_GetCount(hEvent);

	for (i=0; i<eventsCount; i++)
		{
		if (eventID == ComboBox_GetItemData(hEvent, i))
			{
			break;
			}
		}

	if (i < eventsCount)
		ComboBox_SetCurSel(hEvent, i);
	else
		ComboBox_SetCurSel(hEvent, -1);
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
	HWND			hResponses			= GetDlgItem(hDlg, LB_RESPONSES);
	HWND			hDLL						= GetDlgItem(hDlg, SLE_DLL);
	HWND			hFunc						= GetDlgItem(hDlg, SLE_FUNCTION);
	int			itemIndex;
	int			responseIndex;
	COMPUID eventID;

	itemIndex = ListBox_GetCurSel(hResponses);

	if (itemIndex == LB_ERR)
		{
		Edit_SetText(hDLL, "");
		Edit_SetText(hFunc, "");
		ComboBox_SetCurSel(GetDlgItem(hDlg, CB_EVENT), -1);
		}
	else
		{
		responseIndex = ListBox_GetItemData(hResponses, itemIndex);
		Edit_SetText(hDLL, (LPSTR)(*Responses)[responseIndex].GetDll());
		Edit_SetText(hFunc, (LPSTR)(*Responses)[responseIndex].GetFunction());

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
	HWND					hDLL		= GetDlgItem(hDlg, SLE_DLL);
	HWND					hEvent	= GetDlgItem(hDlg, CB_EVENT);
	HWND					hResp		= GetDlgItem(hDlg, LB_RESPONSES);
	long	 itemID;
	int		 responseCount;
	int		 i;
  long   initialResponseIndex;

	Edit_LimitText(hDLL, MAXFUNCCHARS);

	cfgInitEventList(hEvent);

	//add responses to the response list box

	responseCount = Responses->GetCount();
	for (i=0; i<responseCount; i++)
		{
		itemID = ListBox_AddString(hResp, MakeEventString((*Responses)[i].GetEventID()));

		//item data holds index of response in response array
		//accounts for non-sorted entries in the response array created
		//previous to sorting.

		ListBox_SetItemData(hResp, itemID, i);
		}

	// Set selection to the first response in the listblock

	if (responseCount >= 1)
		{
		if (LB_ERR != ListBox_SetCurSel(hResp, 0))
		  initialResponseIndex = 0;
		else
		  initialResponseIndex = -1;
		SetCurrentResponseData(hDlg, Responses);
		SetFocus(hResp);
		}
	else
    {
    initialResponseIndex = -1;
		SetFocus(hEvent);
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
	HWND						hEvent	= GetDlgItem(hDlg, CB_EVENT);

	Edit_GetText(GetDlgItem(hDlg, SLE_FUNCTION), function, MAXFUNCCHARS+1);

	Edit_GetText(GetDlgItem(hDlg, SLE_DLL), dll, MAXDLLCHARS+1);

	eventID = ComboBox_GetItemData(hEvent, ComboBox_GetCurSel(hEvent));
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
	HWND						hResp		= GetDlgItem(hDlg, LB_RESPONSES);
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
	else if (eventID == (COMPUID) CB_ERR)
		{
		MessageBox(hDlg, "You must select an event to respond to.",
		 "Response Insertion Error", MB_OK);
		MessageBeep(0);
		}
	else
		{
		itemIndex = ListBox_AddString(hResp, MakeEventString(eventID));

		//accounts for non-sorted entries in the response array created
		//previous to sorting.

		//add new response to the end of the response array
		responses->Insert(32767, eventID, dll, function);

		responseIndex = responses->GetCount() - 1;

		ListBox_SetItemData(hResp, itemIndex, responseIndex);

		ListBox_SetCurSel(hResp, itemIndex);

		topIndex = ListBox_GetTopIndex(hResp);

		// make sure the new response is visible
		if ((itemIndex - topIndex > 7) || (itemIndex < topIndex))
			{
			if (itemIndex < 7)
				ListBox_SetTopIndex(hResp, 0);
			else
				ListBox_SetTopIndex(hResp, itemIndex - 4);
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
	HWND						hResp			= GetDlgItem(hDlg, LB_RESPONSES);
	char						function[MAXFUNCCHARS+1];
	char						dll[MAXDLLCHARS+1];
	COMPUID					eventID;
	LPRESPONSE			tempResponse;
	long						responseIndex;
	long						topIndex;

  GetCurrentResponseData(hDlg, eventID, dll, function);

  if (itemIndex == -1)
	  itemIndex = ListBox_GetCurSel(hResp);
	if (itemIndex == LB_ERR)
		MessageBeep(0);
	else
		{
		responseIndex = ListBox_GetItemData(hResp, itemIndex);

		tempResponse = (LPRESPONSE)responses->GetElement(responseIndex);
		tempResponse->SetFunction(function);
		tempResponse->SetDll(dll);

		if (eventID != tempResponse->GetEventID())
			{
			tempResponse->SetEventID(eventID);

			//Delete and reinsert entry in proper order

			ListBox_DeleteString(hResp, itemIndex);
			itemIndex = ListBox_AddString(hResp, MakeEventString(eventID));
			ListBox_SetItemData(hResp, itemIndex, responseIndex);
       
      if (reselectFlag == TRUE)
        { 
				ListBox_SetCurSel(hResp, itemIndex);

				// make sure the updated response is visible
  			topIndex = ListBox_GetTopIndex(hResp);
				if ((itemIndex - topIndex > 7) || (itemIndex < topIndex))
					{
					if (itemIndex < 7)
						ListBox_SetTopIndex(hResp, 0);
					else
						ListBox_SetTopIndex(hResp, itemIndex - 4);
					}
        }
			}
		}

	SetFocus(hResp);
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
	HWND						hResp		= GetDlgItem(hDlg, LB_RESPONSES);
	int							itemIndex;
	int							responseIndex;

	itemIndex = ListBox_GetCurSel(hResp);

	if (itemIndex == LB_ERR)
		{
		MessageBeep(0);
		}
	else
		{
		responseIndex = ListBox_GetItemData(hResp, itemIndex);
		if (Responses->Delete(responseIndex))
		 {
		 ListBox_DeleteString(hResp, itemIndex);
     if (LB_ERR == ListBox_SetCurSel(hResp, itemIndex))
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
BOOL CALLBACK ResponsesDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPRESPONSEARRAY Responses;
	static BOOL						 responseIsDirty;
  static long            responseIndex;
	HWND										hResp			= GetDlgItem(hDlg, LB_RESPONSES);
	BOOL										msgHandled = TRUE;
  NEWCOMPDATA							eventData;


	switch (message)
		{
		case WM_INITDIALOG:
			Responses = (LPRESPONSEARRAY)lParam;
			responseIndex = InitDialog(hDlg, Responses);
			responseIsDirty = FALSE;
			break;

		case WM_COMMAND:
			{
			WORD			wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD			wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			switch (wID)
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
          responseIndex = ListBox_GetCurSel(hResp);
					responseIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (responseIsDirty == TRUE)
						UpdateResponse(hDlg, Responses, ListBox_GetCurSel(hResp));
          responseIndex = ListBox_GetCurSel(hResp);
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
					if (wNotify == LBN_SELCHANGE)
						{
						if ((responseIsDirty == TRUE) && 
						    (responseIndex != -1) &&
						    QuerySaveChanges(hDlg)
						   )
							UpdateResponse(hDlg, Responses, responseIndex, FALSE);
	          responseIndex = ListBox_GetCurSel(hResp);
						SetCurrentResponseData(hDlg, Responses);
						responseIsDirty = FALSE;
						}
					break;

				case CB_EVENT:
					if (wNotify == CBN_SELCHANGE)
						responseIsDirty = TRUE;
					break;

				case SLE_DLL:
					if (wNotify == EN_CHANGE)
						responseIsDirty = TRUE;
					break;

				case SLE_FUNCTION:
					if (wNotify == EN_CHANGE)
						responseIsDirty = TRUE;
					break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;
			}

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}


