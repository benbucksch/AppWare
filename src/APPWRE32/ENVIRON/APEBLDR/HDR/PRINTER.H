#ifndef	__PRINTER_H
#define __PRINTER_H

#include "memclass.h"
#include <commdlg.h>

////////////////////////////////////////////////////////////////////////////

class PRINTER:public MEMORY
	{
  private:
		static HANDLE	hDevMode;
		static HANDLE	hDevNames;
		static BOOL continueOK;
		static HWND	abortDlg;
		static LOGFONT lf;
		static BOOL createFont;
		static BOOL collate;

		PRINTDLG		pd;
		RECT				bounds;
		POINT       pageSize;
		POINT       offset;
		int					fontHeight;
		int					numLines;
		int 				currentLine;
		int					tabPixels;
		HWND 				hWndOwner;
		int					pageNum;
		HFONT 			font;

	public:
		// constructor & destructor
		PRINTER(
			HWND hWnd);

		~PRINTER();

		BOOL PrintDlg();

		BOOL SetUp();

		HDC		GetDC()
			{
			return pd.hDC;
			}

		BOOL	DeleteDC()
			{
			return ::DeleteDC(pd.hDC);
			}

		UINT	NumCopies()
			{
			return pd.nCopies;
			}

		static UINT CALLBACK PrintHook(
			HWND	hWnd,
			UINT	nMsg,
			WPARAM	wParam,
			LPARAM	lParam);

		void CalculatePageSize();

		BOOL StartJob(
			LPCSTR jobName);

		void EndJob();

		void PrintLine(
			int indent,
			LPCSTR string);

		void CenterLine(
			LPCSTR strint);

		void DoubleLine(
			int indent);

		void SingleLine(
			int indent);

		void PrintLabel(
			int indent,
			int label);

		void SkipLine();

		void PrintPageNum();

		int GetNumCopies();

		static BOOL CALLBACK AbortDlgProc(
			HWND hdlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static BOOL CALLBACK AbortProc(
			HDC hdc,
			short code);

		static void PrinterCleanUp();
		};

////////////////////////////////////////////////////////////////////////////

#endif
