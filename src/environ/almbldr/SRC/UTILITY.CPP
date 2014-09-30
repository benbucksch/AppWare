/* ****************************************************************
 *
 * Copyright (c) 1995 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#include "almsys.h"
#include <stdarg.h>
#include "intdebug.h"
#include "utility.h"
#include "fnameobj.h"
#include "a_almutl.h"

////////////////

static UINT		s_nOkString;

typedef struct
	{
	SFilePath*		pPath;
	CheckProc			checker;
	BOOL					fWrite;			// wants to open for write
	}		HookData;

static UINT	CALLBACK OpenHookProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////////////////////////////////////////////
//
// FUNCT:		InitOpenFileName
//
// DESCR:		Initializes an OFN structure based on the passed in parameters.
//					This should be called before GetSaveFileName or GetOpenFileName
//					if file existence/writability or filecontent checking is
//					desired.
//
/////////////////////////////////////////////////////////////////////////////

void	InitOpenFileName(
	LPOPENFILENAME	pOFN,
	HWND						hOwner,			// in: owner of dialog
	LPCSTR					pszFilter,	// in:
	SFilePath&			filePath,		// in/out:  filled in
	LPCSTR					pszDlgTitle,
	DWORD						flags,
	LPCSTR					pszExt,			// in:  default extension
	BOOL						fWrite			// in:  TRUE => file will be opened for write
	)
	{
	HookData*				pHook		= new HookData;

	pOFN->lStructSize       = sizeof(OPENFILENAME);
	pOFN->hwndOwner         = hOwner;
	pOFN->lpstrFilter       = pszFilter;
	pOFN->lpstrCustomFilter = NULL;
	pOFN->lpstrFile         = filePath.BeginModify();
	pOFN->nMaxFile          = filePath.MaxLength();
	pOFN->nFilterIndex      = 1;
	pOFN->lpstrFileTitle    = NULL;
	pOFN->lpstrInitialDir   = NULL;		// if not null, then can't have trailing '\'
	pOFN->lpstrTitle        = pszDlgTitle;
	pOFN->Flags             = flags | OFN_ENABLEHOOK;
	pOFN->lpfnHook          = OpenHookProc;
	pOFN->lpstrDefExt       = pszExt;
	pOFN->lCustData         = (DWORD) pHook;

	pHook->checker = NULL;
	pHook->fWrite  = fWrite;
	pHook->pPath   = &filePath;

	if (s_nOkString == 0)
		s_nOkString = RegisterWindowMessage(FILEOKSTRING);
	}

/////////////////////////////////////////////////////////////////////////////
//
// FUNCT:		SetupHook
//
// DESCR:		Sets up a callback to verify the contents of the file before
//					the dialog closes.  The callback should return TRUE if the
//					contents are okay and the file should be opened.
//
/////////////////////////////////////////////////////////////////////////////

void	SetupHook(
	LPOPENFILENAME	pOFN,
	CheckProc				pfnCheck
	)
	{
	((HookData*) pOFN->lCustData)->checker = pfnCheck;
	}

/////////////////////////////////////////////////////////////////////////////
//
// FUNCT:		OpenHookProc
//
// DESCR:		Hookproc that handles the opening of the file and the calling
//					of the callback proc to verify the contents of the file before
//					really opening the file.
//
/////////////////////////////////////////////////////////////////////////////

static UINT	CALLBACK OpenHookProc(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	UINT		nRet	= 0;

	I_UNUSED(wParam);

	if (nMsg == WM_INITDIALOG)
		{
		AUtlCenterDialog(hDlg, 0);
		nRet = 1;
		}
	else if (nMsg == s_nOkString)
		{
		LPOPENFILENAME	pOFN		= (LPOPENFILENAME) lParam;
		HANDLE					hFile;
		HookData*				pHook;
		DWORD						bOpenAccess;
		DWORD						bCreateOpt;

		pHook = (HookData*) pOFN->lCustData;
		I_ASSERT(pHook != NULL);

		if (pHook->fWrite)
			{
			bOpenAccess = GENERIC_WRITE;
			bCreateOpt  = CREATE_ALWAYS;
			}
		else
			{
			bOpenAccess = GENERIC_READ;
			bCreateOpt  = OPEN_EXISTING;
			}

		hFile = CreateFile(pOFN->lpstrFile, bOpenAccess, 0, NULL,
											bCreateOpt, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile != INVALID_HANDLE_VALUE)
			{
			if (pHook->checker != NULL)
				nRet = !(*pHook->checker)(hFile, pOFN->lpstrFile);
			else
				nRet = FALSE;

			CloseHandle(hFile);

			if (nRet == FALSE)			// the dialog will close
				{
				pHook->pPath->EndModify();
				delete pHook;
				pOFN->lCustData = 0;
				}
			}
		}

	return nRet;
	}

/////////////////////////////////////////////////////////////////////////////
//
// METHOD:	SFormattedMsg::Format
//
// DESCR:		Allocates a buffer big enough to write the formatted string to.
//					Currently this is just a large buffer, but it should eventually
//					scan through the format string and allocate the memory based on
//					that.
//
/////////////////////////////////////////////////////////////////////////////

BOOL	SFormattedMsg::Format(
	char*		pszFormat,
	...
	)
	{
	BOOL					fAlloced;
	va_list				vaList;

	//!! This should be expanded to figure size out and use m_nBytes...
	if (m_pszMsg == NULL)
		m_pszMsg = new char[1024];

	fAlloced = (m_pszMsg != NULL);

	if (fAlloced)
		{
		va_start(vaList, pszFormat);

		m_nLen = wvsprintf(m_pszMsg, pszFormat, vaList);

		va_end(vaList);
		}

	return fAlloced;
	}

/////////////////////////////////////////////////////////////////////////////
//
// METHOD:	SFormattedMsg::AppendFmt
//
// DESCR:		Appends to the currently stored message.  This has the
//					same capabilities as the Format() method.
//
/////////////////////////////////////////////////////////////////////////////

BOOL	SFormattedMsg::AppendFmt(
	char*		pszFormat,
	...
	)
	{
	BOOL					fAlloced;
	va_list				vaList;

	//!! This should be expanded to figure size out and use m_nBytes...
	if (m_pszMsg == NULL)
		m_pszMsg = new char[1024];

	fAlloced = (m_pszMsg != NULL);

	if (fAlloced)
		{
		va_start(vaList, pszFormat);

		m_nLen += wvsprintf(m_pszMsg + m_nLen, pszFormat, vaList);

		va_end(vaList);
		}

	return fAlloced;
	}
