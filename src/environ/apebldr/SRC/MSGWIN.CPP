#include "almsys.h"
#include <windowsx.h>

#include "msgwin.h"
#include "progsys.h"
#include "oiputils.h"


#define MSGW_BORDER 8
#define MSGW_TERMINATORSIZE 1

MESSAGEWIN * MESSAGEWIN::thisPtr=NULL;

//////////////////////////////////////////////////////////////////
MESSAGEWIN::MESSAGEWIN()
	{
	status = 0L;
	index = 0;
	titlePtr = NULL;
	captionPtr = NULL;
	messagesPtr = (LPSTR)GlobalAlloc(0, MSGW_TERMINATORSIZE);
	if(messagesPtr)
	//initialize with a null terminator so string appending will work
	//properly.
		messagesPtr[0] = '\0';
	else
		messagesPtr = NULL;

	}

//////////////////////////////////////////////////////////////////
MESSAGEWIN::~MESSAGEWIN()
	{
	if(captionPtr)
		delete captionPtr;

	if(titlePtr)
		delete titlePtr;

	if(messagesPtr)
		{
		GlobalFree((HGLOBAL)messagesPtr);
		}
	}


//////////////////////////////////////////////////////////////////
void MESSAGEWIN::ShowMessages()
	{
	//If there is already an instance in existance then return to prevent
	//errors
	if(thisPtr)
		return;

	//If the message block wasn't allocated then return
	if(!messagesPtr)
		return;

	thisPtr = this;

	DialogBoxParam(PROGSYSTEM::hInst, (LPSTR)"MESSAGEWINDLG",
		GetActiveWindow(), (DLGPROC)MESSAGEWIN::MessageDlgProc, (long)this);

	thisPtr = NULL;
	}

//////////////////////////////////////////////////////////////////
//return TRUE if the message was added else return FALSE
//If the user wants carriage returns to be added to the end of the message
//It's the users responsibility to add the returns
BOOL MESSAGEWIN::AddMessage(
	LPSTR newMessagePtr,
	BOOL addIndex)
	{
	char indexBuf[16];
	int oldLength;

	status|=MSGW_MESSAGESEXIST;
	BOOL result = FALSE;

	if(addIndex)
		{
		//increment index and put it in indexBuf
		index++;
		wsprintf((LPSTR)indexBuf, "%i) ", index);
		}
	else //make index buffer empty
		indexBuf[0] = '\0';

	//resize the messages block

	//First get the messages block length
	if(messagesPtr)
		oldLength = lstrlen(messagesPtr);
	else
		oldLength = 0;


	//add an extra 1 to hold the null terminator
	messagesPtr = (LPSTR)GlobalReAlloc((HGLOBAL)messagesPtr, oldLength + lstrlen(newMessagePtr) +
		lstrlen((LPSTR)indexBuf) + MSGW_TERMINATORSIZE, GMEM_MOVEABLE);

	if(messagesPtr)
		{
		//Append an index if necessary
		if(addIndex)
			lstrcat(messagesPtr, (LPSTR)indexBuf);

		lstrcat(messagesPtr, newMessagePtr);
		result = TRUE;
		}

	return result;
	}

//////////////////////////////////////////////////////////////////
//appends a return onto the end of the messages so the next message will
//appear on the following next line.
//Returns TRUE for success or FALSE for failure
BOOL MESSAGEWIN::AppendReturn()
	{
	BOOL result = FALSE;
	int oldLength;

	if(messagesPtr)
		oldLength = lstrlen(messagesPtr);
	else
		oldLength = 0;

	//resize the block to hold the return characters
	messagesPtr = (LPSTR)GlobalReAlloc(messagesPtr, oldLength + 2 + MSGW_TERMINATORSIZE,
		GMEM_MOVEABLE);

	if(messagesPtr)
		{
		lstrcat(messagesPtr, (LPSTR)"\r\n");
		result = TRUE;
		}

	return result;
	}

//////////////////////////////////////////////////////////////////
//Sets the static text which appears above the messages
void  MESSAGEWIN::SetTitle(
	LPSTR newTitle)
	{
	if(!titlePtr)
		titlePtr = new TSTRING();

	*titlePtr = newTitle;
	}

//////////////////////////////////////////////////////////////////
//Sets the caption of the Dialog Box
void MESSAGEWIN::SetCaption(
	LPSTR newCaption)
	{
	if(!captionPtr)
		captionPtr = new TSTRING();

	*captionPtr = newCaption;
	}


//////////////////////////////////////////////////////////////////
BOOL CALLBACK MESSAGEWIN::MessageDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM /*lParam*/)
	{
  UINT command;
	HWND	hEdit;
  
	switch(message)
		{
		case WM_INITDIALOG:

				//end the dialog if the thisPtr is NULL
				if(!MESSAGEWIN::thisPtr)
					EndDialog(hDlg, -1);

				CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);

				MESSAGEWIN::thisPtr->oldMessageEditProc = (WNDPROC)SetWindowLong(
				GetDlgItem(hDlg, MSGW_EDITCONTROL), GWL_WNDPROC,
					(LONG)MESSAGEWIN::MessageEditProc);

			hEdit = GetDlgItem(hDlg, MSGW_EDITCONTROL);

			if(thisPtr->messagesPtr)
				Edit_SetText(hEdit, thisPtr->messagesPtr);

			//Unselect Everything
			Edit_SetSel(hEdit, 1, 0);

			//set the title and caption if necessary
			if(MESSAGEWIN::thisPtr->titlePtr)
				SetWindowText(GetDlgItem(hDlg, MSGW_STATICTITLE),
					(LPSTR) MESSAGEWIN::thisPtr->titlePtr);

			if(MESSAGEWIN::thisPtr->captionPtr)
				SetWindowText(hDlg, (LPSTR)MESSAGEWIN::thisPtr->captionPtr);

			return FALSE;


		case WM_SYSCOMMAND:
    	command = (UINT)wParam;
      //fall through
		case WM_COMMAND:
			if(message == WM_COMMAND)
				command = GET_WM_COMMAND_ID(wParam, lParam);
      
			switch(command)
				{
				case IDOK:
				case SC_CLOSE:
					//reset the edit control original proc and place the subclass proc
					//in oldEditMessageProc so it can be freed.

					MESSAGEWIN::thisPtr->oldMessageEditProc =
						(WNDPROC)SetWindowLong(GetDlgItem(hDlg, MSGW_EDITCONTROL),
						GWL_WNDPROC, (LONG)MESSAGEWIN::thisPtr->oldMessageEditProc);
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;


		case WM_SIZE:
			RECT clientRect;
			RECT editRect;
			RECT buttonRect;
			RECT titleRect;
			int i;

			GetClientRect(hDlg, &clientRect);
			GetClientRect(GetDlgItem(hDlg, MSGW_EDITCONTROL), (LPRECT)&editRect);
			GetClientRect(GetDlgItem(hDlg, MSGW_STATICTITLE), (LPRECT)&titleRect);
			GetClientRect(GetDlgItem(hDlg, IDOK), (LPRECT)&buttonRect);

			i = titleRect.bottom - titleRect.top;
			titleRect.left = MSGW_BORDER;
			titleRect.top = MSGW_BORDER;
			titleRect.bottom =titleRect.top + i;
			titleRect.right = clientRect.right - MSGW_BORDER;

			i = buttonRect.right - buttonRect.left;
			buttonRect.left = (clientRect.right - i)/2;
			buttonRect.right = buttonRect.left + i;
			i = buttonRect.bottom - buttonRect.top;
			buttonRect.bottom = clientRect.bottom - MSGW_BORDER;
			buttonRect.top = buttonRect.bottom - i;

			editRect.top = titleRect.bottom + MSGW_BORDER;
			editRect.bottom = buttonRect.top - MSGW_BORDER;
			editRect.left = titleRect.left;
			editRect.right = titleRect.right;


			MoveWindow(GetDlgItem(hDlg, MSGW_STATICTITLE),
				titleRect.left,
				titleRect.top,
				titleRect.right - titleRect.left,
				titleRect.bottom - titleRect.top,
					TRUE);

			MoveWindow(GetDlgItem(hDlg, MSGW_EDITCONTROL),
				editRect.left,
				editRect.top,
				editRect.right - editRect.left,
				editRect.bottom - editRect.top,
				TRUE);

			MoveWindow(GetDlgItem(hDlg, IDOK),
				buttonRect.left,
				buttonRect.top,
				buttonRect.right - buttonRect.left,
				buttonRect.bottom - buttonRect.top,
				TRUE);

			return 0;

		}
	return FALSE;
	}

//////////////////////////////////////////////////////////////////
LRESULT CALLBACK MESSAGEWIN::MessageEditProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{

	HideCaret(hwnd);
	switch(message)
		{
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:

//		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MOUSEMOVE:
			return 0;

		case WM_SETCURSOR:
			return 0;

		}

	return CallWindowProc((FARPROC)MESSAGEWIN::thisPtr->oldMessageEditProc,
		hwnd, message, wParam, lParam);

	}
