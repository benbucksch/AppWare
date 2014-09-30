#ifndef __OBJSHARE_H
#define __OBJSHARE_H

#include "classdef.h"
#include "memclass.h"

class OBJECTSHARE:public MEMORY
	{
	private:
		SUBJECTWINDOW * pSubjWnd;

	public:
		OBJECTSHARE(
			SUBJECTWINDOW * pSubjWnd);

		~OBJECTSHARE();

		void SetupObjectMenu();

		void MakeShareable(
      BOOL isShareable);

		void AliasDialog();

		static BOOL CALLBACK OBJECTSHARE::AliasDlgProc(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void AddSubjects(
			HWND hDlg);

		void AddShareObjects(
			HWND hDlg);

		void MakeAlias(
			HWND hDlg);

		SUBJECTWINDOW * GetSubjWndPtr()
			{
			return pSubjWnd;
			}

	};

#endif
