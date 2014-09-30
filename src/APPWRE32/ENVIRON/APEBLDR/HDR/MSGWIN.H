#ifndef __MSGWIN_H
#define __MSGWIN_H

#include "memclass.h"
#include "classdef.h"

#define MSGW_MESSAGESEXIST 0x00000001

#define MSGW_EDITCONTROL 1000   //must match the ID of the edit control in
																//message.dlg
#define MSGW_STATICTITLE 1001  	//must match the ID of the static edit
																//control

class MESSAGEWIN:public MEMORY
	{
	private:
		static MESSAGEWIN * thisPtr;

		long status;
		LPSTR messagesPtr;
		int index;
		LPTSTRING titlePtr;
		LPTSTRING captionPtr;
		WNDPROC oldMessageEditProc;

	public:

		MESSAGEWIN();

		~MESSAGEWIN();

		BOOL AddMessage(
			LPSTR newMessagePtr,
			BOOL addIndex);

		BOOL TestMessagesExistFlag()
			{
			return (status & MSGW_MESSAGESEXIST)?TRUE:FALSE;
			};

		void ShowMessages();

		static BOOL CALLBACK MessageDlgProc(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK MessageEditProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);


		void  SetTitle(
			LPSTR newTitle);

		void SetCaption(
			LPSTR newCaption);

		BOOL AppendReturn();

	};

#endif
