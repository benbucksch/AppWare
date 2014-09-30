/*//////////////////////////////////////////////////////////////
//
//					Copyright 1995, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					FNAMEOBJ.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains implementation of SFilePath (the filepath
//								abstraction).
//
// CHANGES:
//
//!! KNOWN PROBLEMS:
//		the correct use and maintence of the fNameHasPeriod
//
//////////////////////////////////////////////////////////////*/

#include "almsys.h"
#include "intdebug.h"				// internal debug help
#include "fnameobj.h"

//////////////// Defines
#define K_DIRSEPCHAR				'\\'
#define K_DIRSEPCHAR2				'/'
#define	K_DIRSEPSIZE				sizeof(K_DIRSEPCHAR)
#define K_EXTCHAR						'.'
#define K_EXTCHARSIZE				sizeof(K_EXTCHAR)
#define K_DRIVESEPCHAR			':'
#define K_NULLCHAR					'\0'

////////////////////////////////////////////////////////////////
//
// FUNCT:		IsDirSeparator
//
// DESCR:		Returns TRUE if the character is a directory separator.
//
////////////////////////////////////////////////////////////////

inline BOOL		IsDirSeparator(
	char						c
	)
	{
	return (c == K_DIRSEPCHAR || c == K_DIRSEPCHAR2);
	}

////////////////////////////////////////////////////////////////
//
// FUNCT:		IsExtSeparator
//
// DESCR:		Returns TRUE if the character is an extension separator.
//
////////////////////////////////////////////////////////////////

inline BOOL		IsExtSeparator(
	char						c
	)
	{
	return (c == K_EXTCHAR);
	}

////////////////////////////////////////////////////////////////
//
// FUNCT:		CopyUpTo
//
// DESCR:		Copies characters from source to dest, upto but not including,
//					the stopping character.
//
////////////////////////////////////////////////////////////////

void	CopyUpTo(
	char*					pDst,
	int						nDstSize,
	const char*		pSrc,
	const char*		pEnd					// stopping point
	)
	{
	while ((pSrc < pEnd) && (nDstSize > 0))
		{
		*pDst = *pSrc;
		pDst++;
		pSrc++;
		nDstSize--;
		}

	*pDst = K_NULLCHAR;
	}

////////////////////////////////////////////////////////////////
//
// FUNCT:		RemoveDirSeparators
//
// DESCR:		Removes trailing directory separators by overwriting them
//					with nulls.  A special case is <drive>:\ (removing that '\'
//					will mean current directory).
//
////////////////////////////////////////////////////////////////

void	RemoveDirSeparators(
	char*		pszPath
	)
	{
	char*		pszScan		= pszPath + lstrlen(pszPath) - 1;

	while (pszScan >= pszPath)
		{
		if (IsDirSeparator(*pszScan))
			{
			int		nLen	= lstrlen(pszPath);

			// don't remove trailing "\" on c:\ (length is 3), but do allow
			// removing it on a\ (or something like that)
			if ((nLen > 3) || ((nLen > 1) && (pszPath[1] != K_DRIVESEPCHAR)))
				*pszScan = K_NULLCHAR;
			}
		else
			break;

		pszScan--;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::MaxLength
//
// DESCR:		Returns the maximum length of a file path.  This is MAX_PATH
//					in general, although on NT3.5, it can be longer by using the
//					syntax of "\\?\c:\ <a long path>".
//
////////////////////////////////////////////////////////////////

int	SFilePath::MaxLength(
	void
	)
	{
	return MAX_PATH;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SFilePath
//
// DESCR:		Constructs a default filepath
//
////////////////////////////////////////////////////////////////

SFilePath::SFilePath(
	void
	) :
		m_pszPath(&m_cDummy),
		m_pszFullName(NULL),
		m_pszExtension(NULL),
		m_fNameHasPeriod(FALSE),
		m_nPathSize(0),
		m_cDummy(K_NULLCHAR)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SFilePath
//
// DESCR:		Constructs a filepath from a char* description of it
//
////////////////////////////////////////////////////////////////

SFilePath::SFilePath(
	const char*		pszFilePath
	) :
		m_pszPath(&m_cDummy),
		m_pszFullName(NULL),
		m_pszExtension(NULL),
		m_fNameHasPeriod(FALSE),
		m_nPathSize(0),
		m_cDummy(K_NULLCHAR)
	{
	I_ASSERT(pszFilePath != NULL);

	(*this) = pszFilePath;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::~SFilePath
//
// DESCR:		Destroys a file path object.
//
////////////////////////////////////////////////////////////////

SFilePath::~SFilePath(
	void
	)
	{
	if (m_pszPath != &m_cDummy)
		GlobalFree((HANDLE)m_pszPath);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::Normalize
//
// DESCR:		Converts path to canonical form:  all characters are converted
//					to lowercase, directory separators are made '\'.
//
////////////////////////////////////////////////////////////////

void	SFilePath::Normalize(
	void
	)
	{
	char*			pszScan		= m_pszPath;

	CharLowerBuff(m_pszPath, lstrlen(m_pszPath));
	for (;*pszScan != K_NULLCHAR; pszScan++)
		{
		if (IsDirSeparator(*pszScan))
			*pszScan = K_DIRSEPCHAR;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::MakeRoom
//
// DESCR:		Allocates enough memory to hold a path of the given length.
//					FALSE is returned if not enough memory can be allocated, or
//					if the length is more than the maximum allowed.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::MakeRoom(
	int			nSize								// includes room for K_NULLCHAR
	)
	{
	BOOL		fRoom		= FALSE;

	if ((nSize > 0) && (nSize <= MAX_PATH))
		{
		// round up
		nSize = ((nSize / 10) + 1) * 10;

		if (nSize > m_nPathSize)
			{
			char*		pszNewPath		= (LPSTR)GlobalAlloc(GMEM_FIXED|GMEM_SHARE, nSize);

			if (pszNewPath != NULL)
				{
				if (m_pszFullName != NULL)
					m_pszFullName = pszNewPath + (m_pszFullName - m_pszPath);

				if (m_pszExtension != NULL)
					m_pszExtension = pszNewPath + (m_pszExtension - m_pszPath);

				if (m_pszPath != &m_cDummy)
					{
					lstrcpy(pszNewPath, m_pszPath);
					GlobalFree((HANDLE)m_pszPath);
					}
				else
					*pszNewPath = K_NULLCHAR;

				m_pszPath   = pszNewPath;
				m_nPathSize = nSize;

				fRoom = TRUE;
				}
			}

		else
			fRoom = TRUE;
		}

	return fRoom;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SetupNameExt
//
// DESCR:		Adjusts the internal pointers to point at the fullname
//					and the extension.  If there is no extension, then m_pszExtension
//					is NULL.  m_fNameHasPeriod is set if there is a '.' but no
//					extension.
//
////////////////////////////////////////////////////////////////

void	SFilePath::SetupNameExt(
	void
	)
	{
	char*		pszScan;

	m_pszFullName  = NULL;
	m_pszExtension = NULL;
	m_pszFullName  = m_pszPath;		// full thing if no separators

	pszScan = m_pszPath + lstrlen(m_pszPath) - 1;

	while (pszScan >= m_pszPath)
		{
		if (IsExtSeparator(*pszScan))
			{
			if (m_pszExtension == NULL)
				{
				m_pszExtension = pszScan + 1;

				if (*m_pszExtension == K_NULLCHAR)
					{
					m_fNameHasPeriod = TRUE;
					m_pszExtension   = NULL;
					}
				}
			}
		else if (IsDirSeparator(*pszScan))
			{
			m_pszFullName = pszScan + 1;

			// shouldn't have trailing \ or /
			I_ASSERT(!IsDirSeparator(*m_pszFullName));
			break;
			}

		pszScan--;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::CleanName
//
// DESCR:		Cleans the name by removing trailing directory separators.
//
////////////////////////////////////////////////////////////////

void	SFilePath::CleanName(
	void
	)
	{
	RemoveDirSeparators(m_pszPath);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::operator =
//
// DESCR:		Sets the full path from the argument.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::operator =(
	const char*		pszPath
	)
	{
	BOOL					fAssigned		= FALSE;
	int						nLen;

	if (pszPath != NULL)
		nLen = lstrlen(pszPath);
	else
		nLen = 0;

	if (MakeRoom(nLen + 1))
		{
		if (pszPath != NULL)
			lstrcpy(m_pszPath, pszPath);
		else
			*m_pszPath = K_NULLCHAR;

		CleanName();
		Normalize();
		SetupNameExt();

		fAssigned = TRUE;
		}

	return fAssigned;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::operator =
//
// DESCR:		Sets the full path from the argument.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::operator =(
	const SFilePath&		rhs
	)
	{
	BOOL						fAssigned		= FALSE;
	const char*			pszRhs			= (const char*) rhs;
	int							nLen;

	I_ASSERT(pszRhs != NULL);

	nLen = lstrlen(pszRhs);

	if ((nLen > 0) && MakeRoom(nLen + 1))
		{
		lstrcpy(m_pszPath, pszRhs);
		SetupNameExt();

		fAssigned = TRUE;
		}

	return fAssigned;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::BeginModify
//
// DESCR:		Returns a pointer to the path buffer that the client can
//					write to.  The client must call EndModify() to perform
//					necessary initialization of internal pointers.
//
////////////////////////////////////////////////////////////////

char*		SFilePath::BeginModify(
	void
	)
	{
	MakeRoom(MaxLength());

	return m_pszPath;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::EndModify
//
// DESCR:		Performs initialization after a client writes directly to
//					the path buffer.
//
////////////////////////////////////////////////////////////////

void		SFilePath::EndModify(
	void
	)
	{
	CleanName();
	Normalize();
	SetupNameExt();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::MakeFromCurrentDirectory
//
// DESCR:		Makes the directory of the path the current directory.  It
//					does not change the fullname.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::MakeFromCurrentDirectory(
	void
	)
	{
	char		szDir[MAX_PATH];

	GetCurrentDirectory(sizeof(szDir), szDir);
	return SetDirectory(szDir);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::AddToEnd
//
// DESCR:		Adds a new element to the end of the path.  There is no
//					distinction made between a subdirectory and a file.  TRUE
//					is returned if the path is not too long.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::AddToEnd(
	const char*		pszElement
	)
	{
	BOOL					fAdded	= TRUE;
	int						nLen		= lstrlen(m_pszPath);

	I_ASSERT(pszElement != NULL);

	if (MakeRoom(nLen + K_DIRSEPSIZE + lstrlen(pszElement) + 1))
		{
		if (nLen > 0)
			{
			m_pszPath[nLen] = K_DIRSEPCHAR;
			lstrcpy(m_pszPath + nLen + 1, pszElement);
			}
		else
			{
			lstrcpy(m_pszPath, pszElement);
			}

		CleanName();
		Normalize();
		SetupNameExt();
		fAdded = TRUE;
		}

	return fAdded;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::RemoveFromEnd
//
// DESCR:		Removes the last element from the path.  TRUE is returned if
//					there was an element to remove.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::RemoveFromEnd(
	void
	)
	{
	BOOL			fRemoved	= FALSE;

	if (m_pszFullName != NULL)
		{
		*m_pszFullName = K_NULLCHAR;

		CleanName();
		SetupNameExt();

		fRemoved = TRUE;
		}

	return fRemoved;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::Length
//
// DESCR:		Returns the length of the path in bytes.
//
////////////////////////////////////////////////////////////////

int		SFilePath::Length(
	void
	) const
	{
	return lstrlen(m_pszPath);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetPath
//
// DESCR:		Fills in the buffer with the path.
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetPath(
	char*		pBuf,
	int			nSize								// size of buffer--includes space for nul
	) const
	{
	int			nCopied;

	if (pBuf == NULL)
		{
		nCopied = lstrlen(m_pszPath);
		}
	else
		{
		lstrcpyn(pBuf, m_pszPath, nSize);
		RemoveDirSeparators(pBuf);
		nCopied = lstrlen(pBuf);
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetPathNoExt
//
// DESCR:		Fills in the buffer with the path but doesn't include any
//					extension.
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetPathNoExt(
	char*		pBuf,
	int			nSize								// size of buffer
	) const
	{
	int			nCopied;

	if (pBuf == NULL)
		{
		if (HasExtension())
			nCopied = (m_pszExtension - m_pszPath - 1);	// subtract the '.'
		else
			nCopied = lstrlen(m_pszPath);
		}
	else
		{
		if (!HasExtension())
			lstrcpyn(pBuf, m_pszPath, nSize);
		else
			CopyUpTo(pBuf, nSize, m_pszPath, m_pszExtension - 1);	// don't copy '.'

		RemoveDirSeparators(pBuf);

		nCopied = lstrlen(pBuf);
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetPathNoExt
//
// DESCR:		Fills in the path object passed in with the path (without ext).
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::GetPathNoExt(
	SFilePath&		dstPath
	) const
	{
	BOOL					fCopied		= FALSE;
	int						nNeeded		= GetPathNoExt(NULL, 0) + 1;

	if (dstPath.MakeRoom(nNeeded))
		{
		GetPathNoExt(dstPath.m_pszPath, nNeeded);
		dstPath.SetupNameExt();
		fCopied = TRUE;
		}

	return fCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::HasDirectory
//
// DESCR:		Returns TRUE if the path has a directory component.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::HasDirectory(
	void
	) const
	{
	return (m_pszPath < m_pszFullName);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetDirectory
//
// DESCR:		Copies the directory portion of the path into the buffer
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetDirectory(
	char*		pBuf,
	int			nSize								// size of buffer
	) const
	{
	int			nCopied;

	if (pBuf == NULL)
		{
		if (HasDirectory())
			nCopied = (m_pszFullName - m_pszPath);
		else
			nCopied = 0;
		}
	else
		{
		if (!HasDirectory())
			nCopied = GetPath(pBuf, nSize);
		else
			{
			CopyUpTo(pBuf, nSize, m_pszPath, m_pszFullName);
			RemoveDirSeparators(pBuf);

			nCopied = lstrlen(pBuf);
			}
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetDirectory
//
// DESCR:		Copies the directory portion of the path into the dst path object.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::GetDirectory(
	SFilePath&		dstPath
	) const
	{
	BOOL					fCopied		= FALSE;
	int						nNeeded		= GetDirectory(NULL, 0) + 1;

	if (dstPath.MakeRoom(nNeeded))
		{
		GetDirectory(dstPath.m_pszPath, nNeeded);
		dstPath.SetupNameExt();
		fCopied = TRUE;
		}

	return fCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SetDirectory
//
// DESCR:		Sets the directory portion of the path to the argument.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::SetDirectory(
	const char*			pszDir
	)
	{
	BOOL			fSet								= FALSE;
	char			szTmp[MAX_PATH];

	I_ASSERT(pszDir != NULL);

	GetFullName(szTmp, sizeof(szTmp));

	if (MakeRoom(lstrlen(pszDir) + lstrlen(szTmp) + K_DIRSEPSIZE + 1))
		{
		SetPath(pszDir);
		AddToEnd(szTmp);

		fSet = TRUE;
		}

	return fSet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetFullName
//
// DESCR:		Returns the fullname of the path.
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetFullName(
	char*		pBuf,
	int			nSize
	) const
	{
	int			nCopied;

	I_ASSERT(m_pszFullName != NULL);

	if (pBuf == NULL)
		{
		nCopied = lstrlen(m_pszFullName);
		}
	else
		{
		lstrcpyn(pBuf, m_pszFullName, nSize);
		nCopied = lstrlen(pBuf);
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetFullName
//
// DESCR:		Copies the fullname of the path into the dst path object.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::GetFullName(
	SFilePath&		dstPath
	) const
	{
	BOOL					fCopied		= FALSE;
	int						nNeeded		= GetFullName(NULL, 0) + 1;

	if (dstPath.MakeRoom(nNeeded))
		{
		GetFullName(dstPath.m_pszPath, nNeeded);
		dstPath.SetupNameExt();
		fCopied = TRUE;
		}

	return fCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SetFullName
//
// DESCR:		Sets the fullname of the path.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::SetFullName(
	const char*		pszFullName
	)
	{
	BOOL			fSet								= FALSE;
	char			szTmp[MAX_PATH];

	I_ASSERT(pszFullName != NULL);

	GetDirectory(szTmp, sizeof(szTmp));

	if (MakeRoom(lstrlen(szTmp) + lstrlen(pszFullName) + K_DIRSEPSIZE + 1))
		{
		SetPath(szTmp);
		AddToEnd(pszFullName);

		fSet = TRUE;
		}

	return fSet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetName
//
// DESCR:		Returns the name part of the path.
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetName(
	char*		pBuf,
	int			nSize
	) const
	{
	int			nCopied;

	I_ASSERT(m_pszFullName != NULL);

	if (pBuf == NULL)
		{
		if (HasExtension())
			nCopied = (m_pszExtension - m_pszFullName - 1);
		else
			nCopied = lstrlen(m_pszFullName);
		}
	else
		{
		if (!HasExtension())
			{
			lstrcpyn(pBuf, m_pszFullName, nSize);
			}
		else
			{
			CopyUpTo(pBuf, nSize, m_pszFullName, m_pszExtension - 1);
			}

		nCopied = lstrlen(pBuf);
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetName
//
// DESCR:		Copies the name portion of the path into the dst path object.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::GetName(
	SFilePath&		dstPath
	) const
	{
	BOOL					fCopied		= FALSE;
	int						nNeeded		= GetName(NULL, 0) + 1;

	if (dstPath.MakeRoom(nNeeded))
		{
		GetName(dstPath.m_pszPath, nNeeded);
		dstPath.SetupNameExt();
		fCopied = TRUE;
		}

	return fCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SetName
//
// DESCR:		Sets the name of the path.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::SetName(
	const char*		pszName
	)
	{
	BOOL			fSet								= FALSE;
	char			szTmp[MAX_PATH];
	char			szExt[MAX_PATH];

	I_ASSERT(pszName != NULL);

	GetDirectory(szTmp, sizeof(szTmp));
	GetExtension(szExt, sizeof(szExt));

	if (MakeRoom(lstrlen(szTmp) + lstrlen(szExt) + lstrlen(pszName) +
								K_DIRSEPSIZE + 1))
		{
		SetPath(szTmp);
		AddToEnd(pszName);
		CleanName();

		if (lstrlen(szExt) == 0)
			{
			int		nLen	= lstrlen(m_pszPath);

			if ((nLen > 0) && (*(m_pszPath + nLen - 1) == K_EXTCHAR))
				m_fNameHasPeriod = TRUE;
			else
				m_fNameHasPeriod = FALSE;
			}
		else
			SetExtension(szExt);

		fSet = TRUE;
		}

	return fSet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::HasExtension
//
// DESCR:		Returns TRUE if the path has an extension component.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::HasExtension(
	void
	) const
	{
	return (m_pszExtension != NULL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetExtension
//
// DESCR:		Copies the extension part of the path to the buffer.
//
////////////////////////////////////////////////////////////////

int		SFilePath::GetExtension(
	char*		pBuf,
	int			nSize
	) const
	{
	int			nCopied;

	if (pBuf == NULL)
		{
		if (HasExtension())
			nCopied = lstrlen(m_pszExtension);
		else
			nCopied = 0;
		}
	else
		{
		if (HasExtension())
			lstrcpyn(pBuf, m_pszExtension, nSize);
		else
			*pBuf = K_NULLCHAR;

		nCopied = lstrlen(pBuf);
		}

	return nCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::GetExtension
//
// DESCR:		Copies the extension part of the path to the dst path object.
//
////////////////////////////////////////////////////////////////

BOOL		SFilePath::GetExtension(
	SFilePath&		dstPath
	) const
	{
	BOOL					fCopied		= FALSE;
	int						nNeeded		= GetExtension(NULL, 0) + 1;

	if (dstPath.MakeRoom(nNeeded))
		{
		GetExtension(dstPath.m_pszPath, nNeeded);
		dstPath.SetupNameExt();
		fCopied = TRUE;
		}

	return fCopied;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SFilePath::SetExtension
//
// DESCR:		Sets the extension component of the path.
//
////////////////////////////////////////////////////////////////

BOOL	SFilePath::SetExtension(
	const char*		pszExtension
	)
	{
	BOOL			fSet								= FALSE;
	int				nExtLen;
	int				nPathLen;
	char			szTmp[MAX_PATH];

	I_ASSERT(pszExtension != NULL);

	nExtLen = lstrlen(pszExtension);

	GetPathNoExt(szTmp, sizeof(szTmp));		// won't leave trailing '.'

	if (MakeRoom(lstrlen(szTmp) + nExtLen + K_EXTCHARSIZE + 1))
		{
		SetPath(szTmp);

		if (nExtLen > 0)
			{
			nPathLen = lstrlen(m_pszPath);
			m_pszPath[nPathLen] = K_EXTCHAR;
			lstrcpy(m_pszPath + nPathLen + 1, pszExtension);

			CleanName();
			Normalize();
			SetupNameExt();
			}

		fSet = TRUE;
		}

	return fSet;
	}
