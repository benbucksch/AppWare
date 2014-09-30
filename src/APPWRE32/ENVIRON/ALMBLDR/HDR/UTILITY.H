/* ****************************************************************
 *
 * Copyright (c) 1995 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef UTILITY_H
#define UTILITY_H

#include <commdlg.h>
class SFilePath;

typedef BOOL (*CheckProc)(HANDLE, LPCSTR);

void	InitOpenFileName(
	LPOPENFILENAME	pOFN,
	HWND						hOwner,			// in: owner of dialog
	LPCSTR					pszFilter,	// in:
	SFilePath&			filePath,		// in/out:  filled in
	LPCSTR					pszDlgTitle,
	DWORD						flags,
	LPCSTR					pszExt,			// in:  default extension
	BOOL						fWrite			// in:  file will be opened for writing
	);

void	SetupHook(
	LPOPENFILENAME	pOFN,
	CheckProc				pfnCheck
	);

////////////////////////////////////////////////////////////////
//
// CLASS:		SFormattedMsg
//
// DESCR:		Supports long formatted messages (via wsprintf).  The memory
//					for the resulting formatted string is automatically allocated.
//					The major use for this is in forming messages with long file
//					names.
//
////////////////////////////////////////////////////////////////

class SFormattedMsg
	{
	public:
							SFormattedMsg(void) : m_pszMsg(NULL),
																		m_nBytes(0),
																		m_nLen(0)				{ }
						 ~SFormattedMsg(void)										{delete m_pszMsg;}

							operator const char*()	const					{return m_pszMsg;}
		int				Length(void)						const					{return m_nLen;}

		BOOL			Format(char*, ...);
		BOOL			AppendFmt(char*, ...);

	private:
		char*			m_pszMsg;						// buffer to write to
		int				m_nBytes;						// size of m_pszMsg
		int				m_nLen;							// lstrlen of msg
	};

#endif
