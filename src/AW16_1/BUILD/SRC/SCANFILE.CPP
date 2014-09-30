/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:				SCANFILE.CPP
//
// AUTHOR:			Scott McCarty
//
// DESCRIPTION:	Implements a file-scanning utility that looks for
//							file references within source code.  The main use of
//							this is to generate dependencies for makefiles.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define _DOS

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dos.h>
#include <afx.h>
#include <afxcoll.h>

#include "scanfile.h"

////////////////

typedef unsigned int CmdOptions;

const CmdOptions	DescendIntoSystem				= 0x001;	// "-y":  scan <> includes
const CmdOptions	PrintSystemIncludes			= 0x002;	// "-s":  print <> includes
const CmdOptions	IncludeNotFoundFiles		= 0x004;	// "-n":  print files unable to be found
const CmdOptions	WarnOnNonRelative				= 0x008;	// "-w":  warn on non-relative non-system includes
const CmdOptions	IncludeSourceAsDep			= 0x010;	// "-i":  include .c/.cpp as dependent
const CmdOptions	IncludePathOnDep				= 0x020;	// "-p":  include path on dependent
const CmdOptions	PrefixMacroToTarget			= 0x040;	// "-m":  print macro before target
const CmdOptions	ForceCppScan						= 0x080;	// "-c":  force C++ scan mode
const CmdOptions	ForceRcScan							= 0x100;	// "-r":  force RC scan mode
const CmdOptions	DepFileDirectory				= 0x200;	// "-d":  directory to upt .dep files in
const CmdOptions	AppendAllDepsToFile			= 0x400;	// "-a":  append all dependencies to one file

//////////////// Global Variables

static CMapStringToPtr*		g_pKnownFiles;
static CmdOptions					g_options;
static char*							g_pszMacro;
static char*							g_pszDepDir;
static char*							g_pszDepFile;

static char								g_szUsage[]		=
		"Usage:  scanfile [options] ([files] | [@file]) ....                    8/24/93\n"
		"\n"
		"  Options:\n"
		"\t-a<f>\tAppend all dependencies to file <f>\n"
		"\t-d<d>\tCreate and put .DEP file in it\n"
		"\t-m<s>\tPrint <s> before target filename (path is stripped)\n"
		"\n"
		"\t-c\tForce C/C++ scan of files\n"
		"\t-i\tInclude source file as dependent\n"
		"\t-n\tPrint files that can't be located using INCLUDE env variable\n"
		"\t-p\tInclude path of dependent when listing dependents\n"
		"\t-r\tForce RC scan of files\n"
		"\t-s\tPrint system includes\n"
		"\t-w\tWarn if non-relative, non-system include is found\n"
		"\t-y\tScan system includes\n"
		"\n"
		"  [files] can contain wildcard characters\n"
		"  [@file] contains a list of files (no wildcard characters)\n";


////////////////////////////////////////////////////////////////
//
// FUNCTION:	Output
//
// DESCR:			Prints the makefile target to the file.
//
////////////////////////////////////////////////////////////////

int		Output(
	FILE*		pFOut,
	char*		pszFmt,
	...
	)
	{
	int				nPrinted;
	va_list		va;

	va_start(va, pszFmt);

	nPrinted = vfprintf(pFOut, pszFmt, va);

	va_end(va);

	return nPrinted;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SkipWhite
//
// DESCR:			Skips white space and positions the pointer to the
//						first non-white character (or \0 if none).
//
////////////////////////////////////////////////////////////////

void	SkipWhite(
	char*&		pszText
	)
	{
	char*			pszTmp		= pszText;

	while (*pszTmp != '\0' && (*pszTmp == ' ' || *pszTmp == '\t'))
		pszTmp++;

	pszText = pszTmp;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SkipToWhite
//
// DESCR:			Skips all non-white character and positions the pointer
//						to the first ' ' or '\t' (or \0 if none).
//
////////////////////////////////////////////////////////////////

void	SkipToWhite(
	char*&		pszText
	)
	{
	char*			pszTmp		= pszText;

	while (*pszTmp != '\0'        &&
					(*pszTmp != ' '       &&
					 *pszTmp != '\t'      &&
					 *pszTmp != '\r'      &&
					 *pszTmp != '\n'))
		pszTmp++;

	pszText = pszTmp;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	GetWord
//
// DESCR:			Scans the line for the next word and returns in in the
//						'cWord' parameter.  A word is defined as a sequence of
//						characters surrounded by white space.
//
////////////////////////////////////////////////////////////////

void	GetWord(
	char*&		pszLine,
	CString&	cWord
	)
	{
	char*			pszTmp;
	BOOL			fEnd;

	SkipWhite(pszLine);
	pszTmp = pszLine;
	SkipToWhite(pszTmp);

	if (*pszTmp == '\0')
		fEnd = TRUE;
	else
		{
		*pszTmp = '\0';
		fEnd = FALSE;
		}

	cWord = pszLine;

	pszLine = pszTmp + ((fEnd) ? 0 : 1);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	IsAbsolute
//
// DESCR:			Returns TRUE if the path starts with <drive>:
//
////////////////////////////////////////////////////////////////

BOOL		IsAbsolute(
	CString&		cPath
	)
	{
	return (cPath[0] == '\\' || cPath[0] == '/'  ||
					(cPath.GetLength() > 1 &&
					 cPath[1] == ':' &&
					 isalpha(cPath[0])));
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	IsRelative
//
// DESCR:			Returns TRUE if the path starts with a "."
//
////////////////////////////////////////////////////////////////

BOOL		IsRelative(
	CString&		cPath
	)
	{
	return (cPath[0] == '.');
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	GetExtension
//
// DESCR:			Searches from the end of the string for a "." and returns
//						a pointer to the character after it.  NULL is returned
//						if there is no ".".
//
////////////////////////////////////////////////////////////////

char*		GetExtension(
	char*		pszPath
	)
	{
	char*		pszScan;

	for (pszScan = pszPath + strlen(pszPath) - 1;
				pszScan >= pszPath;
				pszScan--)
		{
		if (*pszScan == '.')
			{
			pszScan++;
			break;
			}
		else if (*pszScan == '/' || *pszScan == '\\')
			{
			pszScan = pszPath - 1;			// force a NULL return value
			break;
			}
		}

	return ((pszScan > pszPath) ? pszScan : NULL);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	GetFile
//
// DESCR:			Searches from the end of the string for the beginning of
//						a filename, or beginning of string, whichever comes first.
//
////////////////////////////////////////////////////////////////

char*		GetFile(
	char*		pszPath
	)
	{
	char*		pszScan;

	for (pszScan = pszPath + strlen(pszPath) - 1;
				pszScan >= pszPath;
				pszScan--)
		{
		if (*pszScan == '/' || *pszScan == '\\')
			break;
		}

	pszScan++;

	return pszScan;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	Search
//
// DESCR:			Uses the given environment variable's value to search
//						for the given file.  The environment variable's value
//						is assumed to be a ";" separated list of directories.
//
////////////////////////////////////////////////////////////////

CString		Search(
	const char*			pszFile,
	char*						pszEnvVarName,
	BOOL						fLocal
	)
	{
	CString					realFile						= "";
	char						szFile[_MAX_DIR];
	char						szTmp[_MAX_PATH];
	char*						pszEnvVal;
	char*						pszScan;
	struct _find_t	findInfo;
	int							nLen;
	BOOL						fEnd;

	pszEnvVal = getenv(pszEnvVarName);

	if (pszEnvVal)
		{
		while (*pszEnvVal != '\0' || fLocal)
			{
			if (fLocal)											// check local file first
				{
				strcpy(szFile, ".");
				}
			else
				{
				for (pszScan = pszEnvVal;
							*pszScan != '\0' && *pszScan != ';';
							pszScan++)
					;

				fEnd = (*pszScan == '\0');
				nLen = (size_t) (pszScan - pszEnvVal);
				strncpy(szFile, pszEnvVal, nLen);
				szFile[nLen] = '\0';
				}

			sprintf(szTmp, "%s\\%s", szFile, pszFile);

			if (_dos_findfirst(szTmp, _A_NORMAL, &findInfo) == 0)
				{
				realFile = szTmp;
				break;
				}

			if (fLocal)
				fLocal = FALSE;
			else
				pszEnvVal = pszScan + ((fEnd) ? 0 : 1);
			}
		}

	return realFile;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	GetKnownName
//
// DESCR:			Converts a filename and a scanmode into a unique string
//						used for mapping.
//
////////////////////////////////////////////////////////////////

void		GetKnownName(
	CString&	cFile,
	ScanMode	scanMode,
	CString&	cMappedName
	)
	{
	char*					pszMode	= NULL;

	switch (scanMode)
		{
		case cppScan:
			pszMode = "C++";
			break;

		case rcScan:
			pszMode = "RC";
			break;

		default:
			break;
		}

	if (pszMode)
		{
		cMappedName = cFile + ":" + pszMode;
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AddKnownFile
//
// DESCR:			Adds a FileNode to the list of known files.  They
//						are remembered by both their name and their scan
//						mode.  Scan mode is important because a different
//						set of tokens is looked for (and therefore a different
//						set of file references could be found).
//
////////////////////////////////////////////////////////////////

void		AddKnownFile(
	FileNode*		pFile
	)
	{
	CString			cName;

	if (g_pKnownFiles == NULL)
		g_pKnownFiles = new CMapStringToPtr();

	if (g_pKnownFiles)
		{
		GetKnownName(pFile->Name(), pFile->Mode(), cName);
		g_pKnownFiles->SetAt(cName, (void*) pFile);
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	KnownFile
//
// DESCR:			Looks up the file (and uses the scan mode) to see if
//						it's already been processed before.
//
////////////////////////////////////////////////////////////////

FileNode*		KnownFile(
	CString&		cFile,
	ScanMode		scanMode
	)
	{
	FileNode*		pFile		= NULL;
	CString			cName;
	void*				pVoid;

	if (g_pKnownFiles)
		{
		GetKnownName(cFile, scanMode, cName);
		if (g_pKnownFiles->Lookup(cName,  pVoid))
			pFile = (FileNode*) pVoid;
		}

	return pFile;
	}

//**************************************************************
//												 FileNode methods

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode constructor
//
// DESCR:		Initializes a new FileNode.
//
////////////////////////////////////////////////////////////////

FileNode::FileNode(
	const char*		pszFile,
	int						nType,
	ScanMode			scanMode
	) :
		m_status(0),
		m_ScanMode(scanMode),
		m_nType(nType),
		m_fTerminal(FALSE)
	{
	m_Name = (CString) pszFile;
	m_Name.MakeLower();

	// Now store this FileNode as a known file
	AddKnownFile(this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode destructor
//
// DESCR:		Cleans up a dying FileNode.
//
////////////////////////////////////////////////////////////////

FileNode::~FileNode(
	void
	)
	{
	m_Files.RemoveAll();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode::FindPath
//
// DESCR:		The full path of the file is derived using the name and
//					the INCLUDE environment variable.
//
////////////////////////////////////////////////////////////////

void	FileNode::FindPath(
	void
	)
	{
	struct	_find_t		findInfo;

	if (!(m_status & wasSearched))
		{
		if (IsAbsolute(m_Name) || IsRelative(m_Name))
			{
			if (_dos_findfirst(m_Name, _A_NORMAL, &findInfo) == 0)
				{
				m_Path = m_Name;
				m_status |= validPath;			// success
				}
			}

		else
			{
			m_Path = Search(m_Name, "INCLUDE", (m_nType != stdinclude));
			if (m_Path != "")
				{
				m_Path.MakeLower();
				m_status |= validPath;

				if ((g_options & WarnOnNonRelative) &&
						(m_nType != stdinclude)         &&
						IsAbsolute(m_Path))
					Output(stderr, "Warning:  Non-relative, non-system include:  %s\n",
										(const char*) m_Path);
				}
			}

		m_status |= wasSearched;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode::ValidPath
//
// DESCR:		Returns TRUE if the path is valid.  It will search if
//					necessary.
//
////////////////////////////////////////////////////////////////

BOOL	FileNode::ValidPath(
	void
	)
	{
	if (!(m_status & wasSearched))
		FindPath();

	return ((m_status & validPath) != 0);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	CheckInclude
//
// DESCR:		Returns TRUE if the given word is a valid include construct.
//					It also returns the type (local or standard) along with the
//					file name.
//
////////////////////////////////////////////////////////////////

BOOL	CheckInclude(
	CString&		cWord,
	int&				nType,
	CString&		cFile
	)
	{
	BOOL			fIsNode			= TRUE;
	char			c0;
	char			cl;
	int				nLen;

	nLen = cWord.GetLength();

	c0 = cWord[0];
	if (nLen > 0)
		cl = cWord[nLen - 1];

	if (c0 == '"' && cl == '"')
		{
		nType = FileNode::locinclude;
		cFile = cWord.Mid(1, nLen - 2);
		}
	else if (c0 == '<' && cl == '>')
		{
		nType = FileNode::stdinclude;
		cFile = cWord.Mid(1, nLen - 2);
		}
	else
		fIsNode = FALSE;

	return fIsNode;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode::IsNode
//
// DESCR:		Returns TRUE if the input line contains a reference to a
//					file.  If it does, then the file name and the type of
//					reference is returned.
//
////////////////////////////////////////////////////////////////

BOOL	FileNode::IsNode(
	long			lLine,
	char*			pszLine,										// line to scan
	int&			nType,											// type of file
	CString&	cFile												// filename
	)
	{
	BOOL			fIsNode			= FALSE;
	CString		cWord;

	switch (m_ScanMode)
		{
		case cppScan:
			GetWord(pszLine, cWord);
			cWord.MakeUpper();

			if (cWord == "#INCLUDE")
				{
				GetWord(pszLine, cWord);			// get word after #include

				if (!CheckInclude(cWord, nType, cFile))
					Output(stderr, "Invalid #include directive in file %s at line %ld\n",
									(const char*) Path(), lLine);
				else
					fIsNode = TRUE;
				}
			break;

		case rcScan:
			GetWord(pszLine, cWord);
			cWord.MakeUpper();

			if (cWord == "RCINCLUDE")
				{
				GetWord(pszLine, cFile);
				nType   = locinclude;
				fIsNode = TRUE;
				}
			else if (cWord == "#INCLUDE")
				{
				GetWord(pszLine, cWord);

				if (!CheckInclude(cWord, nType, cFile))
					Output(stderr, "Invalid #include directive in file %s at line %ld\n",
									(const char*) Path(), lLine);
				else
					fIsNode = TRUE;
				}
			else													// check all second-word constructs
				{
				GetWord(pszLine, cWord);
				cWord.MakeUpper();

				if (cWord == "BITMAP" ||
						cWord == "CURSOR" ||
						cWord == "FONT"   ||
						cWord == "ICON")				// assume no options between keyword and filename
					{
					GetWord(pszLine, cFile);

					nType   = rcterminal;
					fIsNode = TRUE;
					}
				}
			break;

		default:
			break;
		}

	return fIsNode;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode::Scan
//
// DESCR:		Scans the file looking for references to files.  All references
//					to files are kept in a list.  If the file has been seen before,
//					a pointer to its FileNode is kept, else a new FileNode is
//					created and stored.
//
////////////////////////////////////////////////////////////////

void	FileNode::Scan(
	void
	)
	{
	long				lLine						= 0;
	BOOL				fNotAll;
	FileNode*		pFileNode;
	FILE*				pFile;
	char				szLine[256];
	int					nLen;
	int					nType;
	CString			cFile;

	if (!(m_status & scanned) &&
			(ValidPath()) &&
			((pFile = fopen(Path(), "r")) != NULL))
		{
		while (fgets(szLine, sizeof(szLine), pFile) != NULL)
			{
			lLine++;

			nLen    = strlen(szLine);
			fNotAll = (nLen > 0 && szLine[nLen - 1] != '\n');

			if (IsNode(lLine, szLine, nType, cFile))
				{
				if ((pFileNode = KnownFile(cFile, m_ScanMode)) == NULL)
					pFileNode = new FileNode(cFile, nType, m_ScanMode);

				m_Files.AddHead(pFileNode);
				}

			// if we didn't hit \n, then advance to it
			if (fNotAll)
				{
				while (fgets(szLine, sizeof(szLine), pFile) != NULL)
					{
					if (strlen(szLine) != sizeof(szLine) - 1)
						break;
					}
				}
			}

		fclose(pFile);
		}

	m_status |= scanned;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FileNode::AddToQueue
//
// DESCR:		Adds all non-terminal FileNode* objects to the queue
//					that are already in the queue or in the dependency list.
//
////////////////////////////////////////////////////////////////

void	FileNode::AddToQueue(
	FileDeps&		deps,
	FileQueue&	queue
	)
	{
	POSITION		pos		= m_Files.GetHeadPosition();
	void*				pVoid;

	while (pos != NULL)
		{
		pVoid = m_Files.GetNext(pos);
		if (deps.Find(pVoid) == NULL && !queue.InQueue(pVoid))
			queue.Push(pVoid);
		}
	}

//**************************************************************
//														Functions

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OpenOutput
//
// DESCR:			Opens the appropriate output file to hold the dependency list.
//
////////////////////////////////////////////////////////////////

FILE*		OpenOutput(
	char*				pszFile,
	FILE*&			pFDeps
	)
	{
	char				szDep[_MAX_PATH];
	FILE*				pFOut;
	int					nLen;
	char*				pszScan;

	// Open up the output for the dependency list
	if (g_options & AppendAllDepsToFile)
		{
		if ((pFDeps = fopen(g_pszDepFile, "a+")) != NULL)
			pFOut = pFDeps;
		else
			{
			Output(stderr, "Unable to open file %s for dependency append\n",
							g_pszDepFile);
			pFOut = stdout;
			}
		}

	else if (g_options & DepFileDirectory)
		{
		nLen = strlen(g_pszDepDir);

		if (nLen > 0 &&
				(g_pszDepDir[nLen - 1] != '\\' &&
				 g_pszDepDir[nLen - 1] != '/'))
			strcat(g_pszDepDir, "/");

		strcpy(szDep, g_pszDepDir);
		strcat(szDep, pszFile);
		pszScan = GetExtension(szDep);
		if (pszScan == NULL)
			{
			pszScan = szDep + strlen(szDep);
			*(pszScan++) = '.';
			}

		strcpy(pszScan, "DEP");

		if ((pFDeps = fopen(szDep, "w")) != NULL)
			pFOut = pFDeps;
		else
			{
			Output(stderr, "Unable to open .DEP file %s\n", szDep);
			pFOut = stdout;
			}
		}
	else
		pFOut = stdout;

	return pFOut;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	CloseOutput
//
// DESCR:			Closes the output file that was opened in OpenOutput.
//
////////////////////////////////////////////////////////////////

void		CloseOutput(
	FILE*		pFDeps
	)
	{
	if ((g_options & (DepFileDirectory | AppendAllDepsToFile)) && pFDeps)
		fclose(pFDeps);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OutputTarget
//
// DESCR:			Prints the makefile target to the output.
//
////////////////////////////////////////////////////////////////

void	OutputTarget(
	char*				pszFile,
	FILE*				pFOut,
	ScanMode		scanMode,
	int&				nCol														// next column to print at
	)
	{
	char				szPath[_MAX_PATH];
	char*				pszScan;

	strcpy(szPath, pszFile);

	pszScan = GetExtension(szPath);

	if (pszScan == NULL)												// no extension
		{
		pszScan = szPath + strlen(szPath);
		*(pszScan++) = '.';
		}

	if (scanMode == cppScan)
		strcpy(pszScan, "obj");
	else
		strcpy(pszScan, "res");

	if (g_pszMacro == NULL)
		nCol = Output(pFOut, "%s: ", szPath);
	else
		{
		nCol = Output(pFOut, "%s", g_pszMacro);

		pszScan = GetFile(szPath);
		nCol += Output(pFOut, "%s: ", pszScan);
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ScanFile
//
// DESCR:			Scan one file looking for references to other files
//						within it.
//
////////////////////////////////////////////////////////////////

void	ScanFile(
	char*				pszFile,
	FILE*				pFOut
	)
	{
	BOOL					fFirst	= TRUE;
	FileNode*			pFile;
	FileDeps			deps;
	FileQueue			queue;
	POSITION			iter;
	int						nCol;
	int						nStartDepCol;
	char					szDep[_MAX_PATH];
	char*					pszExt;
	ScanMode			scanMode;

	if (g_options & ForceCppScan)
		scanMode = cppScan;
	else if (g_options & ForceRcScan)
		scanMode = rcScan;
	else
		{
		// Determine from extension
		pszExt = GetExtension(pszFile);
		if (pszExt == NULL)
			Output(stderr, "No file extension on file %s\n", pszFile);
		else if (stricmp(pszExt, "RC") == 0)
			scanMode = rcScan;
		else if (stricmp(pszExt, "C") == 0 || stricmp(pszExt, "CPP") == 0)
			scanMode = cppScan;
		else
			{
			scanMode = cppScan;
			Output(stderr, "Unknown file extension:  %s\n", pszExt);
			}
		}

	pFile	= new FileNode(pszFile, FileNode::csource, scanMode);

	queue.Push(pFile);

	while (!queue.Empty())
		{
		pFile = (FileNode*) queue.Pop();

		deps.Add(pFile);

		if ((g_options & DescendIntoSystem) ||
				(pFile->Type() != FileNode::stdinclude &&
				 (pFile->Type() != FileNode::rcterminal)))
			pFile->Scan();

		pFile->AddToQueue(deps, queue);					// should detect duplicates
		}

	OutputTarget(pszFile, pFOut, scanMode, nCol);
	nStartDepCol = nCol;

	for (iter = deps.GetHeadPosition(); iter != NULL;)
		{
		pFile = (FileNode*) deps.GetNext(iter);

		if (pFile->ValidPath())
			{
			if (fFirst && !(g_options & IncludeSourceAsDep))
				;																		// do nothing

			else if ((g_options & PrintSystemIncludes) ||
					(pFile->Type() != FileNode::stdinclude))
				{
				strcpy(szDep, ((g_options & IncludePathOnDep) ?
												(const char*) pFile->Path() :
												(const char*) pFile->Name()));

				// make new line if wrapping will occur
				if (nCol + strlen(szDep) > 76)
					{
					Output(pFOut, " \\\n");
					for (nCol = 0; nCol < nStartDepCol; nCol++)
						Output(pFOut, " ");
					}

				nCol += Output(pFOut, " %s", szDep);
				}
			}
		else if (g_options & IncludeNotFoundFiles)
			Output(stderr, "\tUnable to find %s\n", (const char*) pFile->Name());

		fFirst = FALSE;
		}

	Output(pFOut, "\n\n");
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ScanFiles
//
// DESCR:			Scans all files that match a given pattern.
//
////////////////////////////////////////////////////////////////

void	ScanFiles(
	char*		pszFiles
	)
	{
	BOOL							fIndirect;
	struct _find_t		findInfo;
	char							szPath[_MAX_PATH];
	char*							pszEnd;
	FILE*							pFOut;
	FILE*							pFDeps;
	FILE*							pFIndirect;
	int								nLen;

	fIndirect = (pszFiles && *pszFiles == '@');

	if (fIndirect)
		{
		if ((pFIndirect = fopen(pszFiles + 1, "r")) != NULL)
			{
			while (fgets(szPath, sizeof(szPath), pFIndirect) != NULL)
				{
				nLen = strlen(szPath);
				if (nLen > 0 && szPath[nLen - 1] == '\n')
					szPath[nLen - 1] = '\0';

				pFOut = OpenOutput(GetFile(szPath), pFDeps);
				ScanFile(szPath, pFDeps);
				CloseOutput(pFDeps);
				}

			fclose(pFIndirect);
			}
		else
			Output(stderr, "Unable to open filelist %s\n", pszFiles + 1);
		}

	else if (pszFiles != NULL && *pszFiles != '\0' &&
						_dos_findfirst(pszFiles, _A_NORMAL, &findInfo) == 0)
		{
		strcpy(szPath, pszFiles);
		pszEnd = GetFile(szPath);

		// Scan every file that matches the pattern
		do
			{
			strcpy(pszEnd, findInfo.name);

			pFOut = OpenOutput(findInfo.name, pFDeps);
			ScanFile(szPath, pFOut);
			CloseOutput(pFDeps);
			}
		while (_dos_findnext(&findInfo) == 0);
		}

	else
		{
		Output(stderr, "No files match the pattern %s\n", pszFiles);
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	main
//
// DESCR:			Determines options, and calls ScanFiles() to scan
//						given set of files for file references.
//
////////////////////////////////////////////////////////////////

int		main(
	int				argc,
	char**		argv
	)
	{
	BOOL			fFoundFile;
	int				nArg;
	char			cOpt;

	if (argc < 2)
		{
		Output(stdout, g_szUsage);
		}
	else
		{
		fFoundFile = FALSE;

		for (nArg = 1; nArg < argc; nArg++)
			{
			if (*argv[nArg] == '-')									// option
				{
				if (fFoundFile)
					{
					Output(stdout, g_szUsage);
					break;
					}

				if (strlen(argv[nArg]) >= 2)
					{
					cOpt = toupper(*(argv[nArg] + 1));
					switch (cOpt)
						{
						case 'A':
							g_options |= AppendAllDepsToFile;
							g_pszDepFile = argv[nArg] + 2;
							break;

						case 'C':
							g_options |= ForceCppScan;
							break;

						case 'D':
							g_options |= DepFileDirectory;
							g_pszDepDir = argv[nArg] + 2;
							break;

						case 'I':
							g_options |= IncludeSourceAsDep;
							break;

						case 'M':
							g_options |= PrefixMacroToTarget;
							g_pszMacro = argv[nArg] + 2;
							break;

						case 'N':
							g_options |= IncludeNotFoundFiles;
							break;

						case 'P':
							g_options |= IncludePathOnDep;
							break;

						case 'R':
							g_options |= ForceRcScan;
							break;

						case 'S':
							g_options |= PrintSystemIncludes;
							break;

						case 'W':
							g_options |= WarnOnNonRelative;
							break;

						case 'Y':
							g_options |= DescendIntoSystem;
							break;

						default:
							Output(stdout, "Unknown option -%c\n", cOpt);
							break;
						}
					}
				}

			else
				{
				fFoundFile = TRUE;
				ScanFiles(argv[nArg]);												// scan possibly multiple files
				}
			}
		}

	return 0;
	}
