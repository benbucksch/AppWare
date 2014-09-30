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
// FILE:				MAKELIST.CPP
//
// AUTHOR:			Scott McCarty
//
// DESCRIPTION:	
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

////////////////

typedef unsigned int CmdOptions;

const CmdOptions	HeaderLine					= 0x001;	// "-h":  
const CmdOptions	MacroLine						= 0x002;	// "-m":  
const CmdOptions	RedirectOutput			= 0x004;	// "-a":  
const CmdOptions	FileSuffix					= 0x008;	// "-s":  

typedef unsigned short				BOOL;
#define TRUE									((BOOL) 1)
#define FALSE									((BOOL) 0)

//////////////// Global Variables

static CmdOptions					g_options;
static char*							g_pszHeaderLine		= "OBJECTS=";
static char*							g_pszMacroLine;
static char*							g_pszOutput;
static char*							g_pszSuffix;

static char								g_szUsage[]		=
		"Usage:  makelist [options] ([files] | [@file]) ....                    8/24/93\n"
		"\n"
		"  Options:\n"
		"\t-a<f>\tUse file <f> as output file (appends)\n"
		"\t-h<h>\tPrint <h> as the first line printed\n"
		"\t-m<m>\tPrint <m> before file name (path is stripped)\n"
		"\t-s<s>\tChanges the suffix of the file to <s>\n"
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
// FUNCTION:	MakeLower
//
// DESCR:			Convert the input string to lowercase
//
////////////////////////////////////////////////////////////////

void		MakeLower(
	char*		pszStr
	)
	{
	for (; *pszStr != '\0'; pszStr++)
		*pszStr = tolower(*pszStr);
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
// FUNCTION:	OpenOutput
//
// DESCR:			Opens the appropriate output file to hold the dependency list.
//
////////////////////////////////////////////////////////////////

FILE*		OpenOutput(
	FILE*&			pFList
	)
	{
	FILE*				pFOut;

	// Open up the output for the dependency list
	if (g_options & RedirectOutput)
		{
		if ((pFList = fopen(g_pszOutput, "a+")) != NULL)
			pFOut = pFList;
		else
			{
			Output(stderr, "Unable to open file %s for list append\n",
							g_pszOutput);
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
	FILE*		pFList
	)
	{
	if ((g_options & RedirectOutput) && pFList)
		fclose(pFList);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ProcessHeader
//
// DESCR:			Prints the header if it hasn't been printed already.
//
////////////////////////////////////////////////////////////////

void	ProcessHeader(
	FILE*				pFOut
	)
	{
	static BOOL		s_fHeaderPrinted				= FALSE;

	if (!s_fHeaderPrinted)
		{
		if (g_pszHeaderLine)
			Output(pFOut, "%s ", g_pszHeaderLine);

		s_fHeaderPrinted = TRUE;
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ProcessFile
//
// DESCR:			Prints the macro and filename and suffix.
//
////////////////////////////////////////////////////////////////

void	ProcessFile(
	char*				pszFile,
	FILE*				pFOut
	)
	{
	char					szPath[_MAX_PATH];
	char*					pszScan;

	MakeLower(pszFile);

	// Print <tab>macro
	if (g_pszMacroLine == NULL)
		g_pszMacroLine = "";

	Output(pFOut, "\\\n\t%s", g_pszMacroLine);

	strcpy(szPath, pszFile);

	pszScan = GetExtension(szPath);

	if (pszScan == NULL)												// no extension
		{
		pszScan = szPath + strlen(szPath);
		*(pszScan++) = '.';
		}

	if (g_pszSuffix)
		strcpy(pszScan, g_pszSuffix);

	Output(pFOut, "%s ", GetFile(szPath));
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ProcessFiles
//
// DESCR:			Scans all files that match a given pattern.
//
////////////////////////////////////////////////////////////////

void	ProcessFiles(
	char*		pszFiles
	)
	{
	BOOL							fIndirect;
	struct _find_t		findInfo;
	char							szPath[_MAX_PATH];
	char*							pszEnd;
	FILE*							pFOut;
	FILE*							pFList;
	FILE*							pFIndirect;
	int								nLen;

	fIndirect = (pszFiles && *pszFiles == '@');

	pFOut = OpenOutput(pFList);

	ProcessHeader(pFOut);

	if (pszFiles == NULL)					// signifies end of processing
		{
		Output(pFOut, "\n\n\n");
		}

	else if (fIndirect)
		{
		if ((pFIndirect = fopen(pszFiles + 1, "r")) != NULL)
			{
			while (fgets(szPath, sizeof(szPath), pFIndirect) != NULL)
				{
				nLen = strlen(szPath);
				if (nLen > 0 && szPath[nLen - 1] == '\n')
					szPath[nLen - 1] = '\0';

				ProcessFile(szPath, pFOut);
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

			ProcessFile(szPath, pFOut);
			}
		while (_dos_findnext(&findInfo) == 0);
		}

	else
		{
		Output(stderr, "No files match the pattern %s\n", pszFiles);
		}

	CloseOutput(pFList);
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
	BOOL			fFirstFile;
	int				nArg;
	char			cOpt;

	if (argc < 2)
		{
		Output(stdout, g_szUsage);
		}
	else
		{
		fFirstFile = FALSE;

		for (nArg = 1; nArg < argc; nArg++)
			{
			if (*argv[nArg] == '-')									// option
				{
				if (fFirstFile)
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
							g_options |= RedirectOutput;
							g_pszOutput = argv[nArg] + 2;
							break;

						case 'H':
							g_options |= HeaderLine;
							g_pszHeaderLine = argv[nArg] + 2;
							break;

						case 'M':
							g_options |= MacroLine;
							g_pszMacroLine = argv[nArg] + 2;
							break;

						case 'S':
							g_options |= FileSuffix;
							g_pszSuffix = argv[nArg] + 2;
							break;

						default:
							Output(stdout, "Unknown option -%c\n", cOpt);
							break;
						}
					}
				}

			else
				{
				fFirstFile = TRUE;

				ProcessFiles(argv[nArg]);										// process possibly multiple files
				}
			}

		ProcessFiles(NULL);															// finishes printing
		}

	return 0;
	}
