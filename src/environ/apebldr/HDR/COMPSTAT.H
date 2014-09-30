#ifndef __COMPSTAT_H
#define __COMPSTAT_H

#include "tstring.h"
#include "memclass.h"

#define COMPSTAT_UPSF				 		0x00000001
#define COMPSTAT_COMPILE 				0x00000002

class COMPILESTATUS:public MEMORY
	{
	private:
		static COMPILESTATUS * thisPtr;
		static HBITMAP upsfPict;

		long currentPoints;
		long totalPoints;
		HWND hDlgCompStat;
		TSTRING operation;
		TSTRING fileName;
		TSTRING title;
		int oldEdge;
		long status;

	void DrawStatusBar(
		LPDRAWITEMSTRUCT di);

	public:
		COMPILESTATUS(
			HWND owner,
			LPCSTR operation,
			LPCSTR fileName,
			long maxPoints,
			long initStatus,
			LPCSTR title,
			long typeStatus);

		~COMPILESTATUS();

		void UpdateStatus(
			long points);

		void FillToEnd();

		void ChangeName(
			LPCSTR newOperation,
			LPCSTR newFileName);

		void ShowFinish();

		void ChangeSize(
			long newPoints);

		static BOOL CALLBACK CompStatProc(
			HWND hDlg,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);
	};

#endif
