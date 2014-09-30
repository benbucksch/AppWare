#ifndef __SRCHMGR_H
#define __SRCHMGR_H

#include "memclass.h"
#include "classdef.h"
#include "sereclib.h"
#include "fileobj.h"

#define SRCH_NUMTYPES	6
#define SRCH_MAXSTRING		128

struct SEARCHINDEX:public MEMORY
	{
	int index;
	long indexType;
	int paramIndex;
	RUNTIMEUID srchBlock;
	};

class SEARCHMANAGER:public MEMORY
	{
	private:
		static BOOL								sm_registered;
		static char								sm_className[];
		static char  							sm_endMessageTitle[];
		static char								sm_notFoundMessage[];
		static char  							sm_endMessage[];
		static char								sm_cantFindMessage[];
		static POINT							sm_foundListOrigin;
		static int								sm_widthFoundList;
		static int								sm_heightFoundList;

		long 					m_searchStatus;
		char 					m_searchString[SRCH_MAXSTRING];
		LPPROJECT		 	m_srchProject;
		SERUID				m_srchSubject;
		int						m_subjectIndex;
		int						m_indexIndex;
		SEARCHINDEX		m_indexArray[SRCH_NUMTYPES];
		int						m_indexSize;
		HWND					m_hwndDisplayList;
		HWND					m_hwndOwnerList;
		LPSUBJECT			m_currentSubject;
		long					m_currentType;
		LPBITREE			m_displaySortTree;
		LPFILEOBJECT  m_searchContext;

		static BOOL CALLBACK FindDlgProc(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		BOOL PatternMatch(
			LPSTR string,
			LPCSTR pattern,
			BOOL wholeWordMatch);

		void InitFindDialog(
			HWND hDlg);

		BOOL SetSearchParameters();

		BOOL GetSearchStateInfo(
			HWND hDlg,
			BOOL reset = FALSE);

		BOOL ResetIndexArray();

		BOOL CreateFindWindow();

		void Register();

		LRESULT SlaveWndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK MasterWndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void ReportSubject(
			LPSUBJECT subjectPtr);

		void ReportType(
			long type);

		void ReportFoundItem(
			void * itemPtr);

		void DisplaySortTree();

		long AddDisplayLine(
 			LPCSTR text,
 			long data);

		void FormatDisplayLine(
			LPSTR line,
 			LPCSTR name,
			WORD formatType);
		
		void GetCurrentTypeName(
			LPSTR buffer);

		BOOL DoSearch();

		BOOL IsFindAllCode(
			LPCSTR string);

		 void HighlightItem(
			long type,
			void * itemPtr,
			LPSUBJECT subjectPtr);

		BOOL Search();

		BOOL HighlightValidListItem(
			HWND hListBox,
			int item);

		void ReadProfileInfo();

		void WriteProfileInfo();

		void * GetNextSearchItem(
			LPSTR itemName);

 		void * GetNextFunction	(
			LPSTR itemName);

		void * GetNextObject(
			LPSTR itemName);

		void * GetNextGroup(
			LPSTR itemName);

 		void * GetNextParameter(
			LPSTR itemName);

		void * GetNextComment(
			LPSTR itemName);

		void InitSearchTypes(
			HWND hDlg);

		void SEARCHMANAGER::InitSearchOptions(
			HWND hDlg);

 		void InitSearchRange(
			HWND hDlg);

		void RebuildIndexArray();

		BOOL GetSearchOptions(
			HWND hDlg);

		BOOL GetSearchType(
			HWND hDlg,
			BOOL * searchOK);

		BOOL GetSearchRange(
			HWND hDlg,
			BOOL * searchOK);

		BOOL SetSearchForNewContext(
 			LPFILEOBJECT newContext);

		BOOL SetSearchForSameContext();

		void SetDefaultFoundListLocation();

		void HandleOwnerWindowSizeChange(
			WPARAM changeCode,
			int width,
			int height);

	public:
		SEARCHMANAGER();

		~SEARCHMANAGER();

		BOOL ShowFindDialog();

		BOOL OkToSearchAgain();

		BOOL SearchAgain(
			LPFILEOBJECT currentContext);

		void ReportProjectClosed(
			LPFILEOBJECT projectContext);

	};


#endif
