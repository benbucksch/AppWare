#include <windows.h>
#include <dir.h>
#include <string.h>
#include "openfile.h"


BOOL FAR PASCAL _export OpenFileDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	);

BOOL FAR PASCAL _export SaveAsDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	);

void AddExtension(
	LPSTR str);

int HasExtension(
	LPSTR str);

LPSTR lstrrchr(
	LPSTR str,
	char	ch);

LPSTR lstrchr(
	LPSTR str,
	char	ch);

void ParseFileName(
	LPSTR str);

#define FILESLISTING	(0X0000)
#define DIRECTORIESLISTING (0X0010|0x4000|0x8000)

#define SLE_FILENAME		107
#define ST_DIRNAME			110
#define LB_FILES				101
#define LB_DIRECTORIES  102
#define CREATE_OPTIONS		0

static int 		FILE_HANDLE;
static int		OPEN_OPTIONS;
static char		DIR_PATH[100];
static char		FILE_NAME[13];
static char		DEF_QUALIFIER[13];
static char 	SAVE_TYPE[10];
CHECKPROC CHECKPROCEDURE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SaveAsDlgBox
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int far SaveAsDlgBox(
	HANDLE	hAppInstance,
	HANDLE	hWndParent,
	LPSTR		fileNameIn,
	LPSTR		directoryIn,
	LPSTR		qualifierIn,
	int			openOptions,
	LPSTR		saveType,
	LPSTR		directoryOut,
	LPSTR		fileNameOut)
	{
	FARPROC			lpProc;

	if(fileNameIn)
		lstrcpy(FILE_NAME, fileNameIn);
	else
		fileNameIn[0] = '\0';

	lstrcpy((LPSTR)SAVE_TYPE, saveType);
	lstrcpy(DEF_QUALIFIER, qualifierIn);
	lstrcpy((LPSTR)DIR_PATH, directoryIn);
	OPEN_OPTIONS = openOptions;
	FILE_HANDLE = -1;

	lpProc = MakeProcInstance((FARPROC)SaveAsDlgProc, (HINSTANCE)hAppInstance);
	DialogBox((HINSTANCE)hAppInstance, "OPEN_FILE_DLG", (HWND)hWndParent, (DLGPROC)lpProc);
	FreeProcInstance(lpProc);
	lstrcpy(directoryOut, (LPSTR)DIR_PATH);
	lstrcpy(fileNameOut, (LPSTR)FILE_NAME);
	return FILE_HANDLE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SaveAsDlgProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export SaveAsDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	)
	{
	BOOL	msgHandled = TRUE;
	char	tmpStr1[26];
	char 	tmpStr2[26];
	LPSTR chrPtr;
	char	tmpPath[126];
	switch (message)
		{
		case WM_INITDIALOG:
			wsprintf((LPSTR)tmpStr1, (LPSTR)"Save %s As", (LPSTR)SAVE_TYPE);
			SetWindowText(hDlg, (LPSTR)tmpStr1);
			SendDlgItemMessage(hDlg, SLE_FILENAME, EM_LIMITTEXT, 80, 0L);
			SendDlgItemMessage(hDlg, SLE_FILENAME, EM_SETSEL, 0, 0L);
			lstrcpy(tmpPath, DIR_PATH);
			lstrcat(tmpPath, "\\");
			lstrcat(tmpPath, DEF_QUALIFIER);
			if(DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING))
				DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
			else
				{
				getcwd(DIR_PATH, 100);
				lstrcpy(tmpPath, DIR_PATH);
				lstrcat(tmpPath, "\\");
				lstrcat(tmpPath, DEF_QUALIFIER);
				DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING);
				DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
				}
			SetDlgItemText(hDlg, SLE_FILENAME, FILE_NAME);
			SendDlgItemMessage(hDlg, SLE_FILENAME,EM_SETSEL, NULL,
				MAKELONG(0, 32767));
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					FILE_HANDLE = -1;
					EndDialog(hDlg, NULL);
					break;

				case IDOK:
					GetDlgItemText(hDlg, SLE_FILENAME, tmpPath, 125);
					if (lstrchr(tmpPath, '*') || lstrchr(tmpPath, '?'))
						{ //path has wild card character so can only do directory listing
						if(DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING))
							{
							DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
							if ( ((chrPtr = (LPSTR)lstrrchr(tmpPath, '\\')) != NULL) ||
									((chrPtr = (LPSTR)lstrrchr(tmpPath, ':')) != NULL ))
								SetDlgItemText(hDlg, SLE_FILENAME, ++chrPtr);
							}
						else
							{//bad path
							MessageBeep(0);
							MessageBox(hDlg,"Invalid Directory Path!", "FILE ERROR",
							 MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
							}
						}
					else
						{
						/* must be some file name so add file extension if it is not
							already on the filename */
						if(!HasExtension((LPSTR)tmpPath))
							AddExtension(tmpPath);
						if (-1 != (FILE_HANDLE = _lopen(tmpPath, OPEN_OPTIONS)))
							{
							MessageBeep(0);
							if( IDCANCEL == MessageBox(hDlg, "Overwrite existing file?",
									"FILE EXISTS",
									MB_OKCANCEL|MB_SYSTEMMODAL|MB_ICONEXCLAMATION))
								{
								_lclose(FILE_HANDLE);
								FILE_HANDLE = -1;
								}
							}
						else
							FILE_HANDLE = _lcreat(tmpPath, CREATE_OPTIONS);

						if(-1 != FILE_HANDLE)
							{
							ParseFileName((LPSTR)tmpPath);
							EndDialog(hDlg, NULL);
							}

						}
					break;

				case LB_FILES:
					switch (HIWORD(lParam))
						{
						case LBN_SELCHANGE:
							DlgDirSelect(hDlg, FILE_NAME, LB_FILES);
							SetDlgItemText(hDlg, SLE_FILENAME, FILE_NAME);
							break;
						case LBN_DBLCLK:
							SendMessage(hDlg, WM_COMMAND, IDOK, 0L);
							msgHandled = FALSE;
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				case LB_DIRECTORIES:
					switch (HIWORD(lParam))
						{
						case LBN_DBLCLK:
							GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 25);
							DlgDirSelect(hDlg, tmpStr2, LB_DIRECTORIES);
							if (lstrchr(tmpStr1, '*') || lstrchr(tmpStr1, '?'))
								{
								if (((chrPtr = (LPSTR)lstrrchr(tmpStr1, '\\')) != NULL) ||
										((chrPtr = (LPSTR)lstrrchr(tmpStr1, ':')) != NULL))
									chrPtr++;
								else
									chrPtr = tmpStr1;
								lstrcat(tmpStr2, chrPtr);
								SetDlgItemText(hDlg, SLE_FILENAME, chrPtr);
								}
							else
								{
								lstrcat(tmpStr2, DEF_QUALIFIER);
								SetDlgItemText(hDlg, SLE_FILENAME, DEF_QUALIFIER);
								}
							DlgDirList(hDlg, tmpStr2, LB_FILES, ST_DIRNAME, FILESLISTING);
							DlgDirList(hDlg, tmpStr2, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
							break;

						case LBN_SELCHANGE:
							GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 25);
							DlgDirSelect(hDlg, tmpStr2, LB_DIRECTORIES);
							if (lstrchr(tmpStr1, '*') || lstrchr(tmpStr1, '?'))
								{
								if (((chrPtr = (LPSTR)lstrrchr(tmpStr1, '\\')) != NULL) ||
										((chrPtr = (LPSTR)lstrrchr(tmpStr1, ':')) != NULL))
									chrPtr++;
								else
									chrPtr = tmpStr1;
								lstrcat(tmpStr2, chrPtr);
								}
							else
								{
								lstrcat(tmpStr2, DEF_QUALIFIER);
								}
							SetDlgItemText(hDlg, SLE_FILENAME, tmpStr2);
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				case SLE_FILENAME:
					switch (HIWORD(lParam))
						{
						case EN_CHANGE:
							if (0 == GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 2))
								EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
							else
								EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				default:
					msgHandled = FALSE;
					break;
					}
			break;

		default:
			msgHandled = FALSE;
			break;
		}

	return msgHandled;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OpenFileDlgBox
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int far OpenFileDlgBox(
	HANDLE	hAppInstance,
	HANDLE	hWndParent,
	LPSTR		dirPathIn,
	LPSTR		qualifierIn,
	int			openOptions,
	LPSTR		dirPathOut,
	LPSTR		fileNameOut,
	CHECKPROC checkProc)

	{
	FARPROC			lpProc;

	lstrcpy(DIR_PATH, dirPathIn);
	lstrcpy(DEF_QUALIFIER, qualifierIn);
	OPEN_OPTIONS = openOptions;

	FILE_HANDLE = -1;
	CHECKPROCEDURE = checkProc;
	lpProc = MakeProcInstance((FARPROC)OpenFileDlgProc,(HINSTANCE) hAppInstance);
	DialogBox((HINSTANCE)hAppInstance, "OPEN_FILE_DLG", (HWND)hWndParent, (DLGPROC)lpProc);
	FreeProcInstance(lpProc);
	lstrcpy((LPSTR)dirPathOut, (LPSTR)DIR_PATH);
	lstrcpy((LPSTR)fileNameOut, (LPSTR)FILE_NAME);
	return FILE_HANDLE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OpenFileDlgProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export OpenFileDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	)
	{
	BOOL	msgHandled = TRUE;
	char	tmpStr1[26];
	char 	tmpStr2[26];
	LPSTR chrPtr;
	char	tmpPath[126];

	switch (message)
		{
		case WM_INITDIALOG:
			SendDlgItemMessage(hDlg, SLE_FILENAME, EM_LIMITTEXT, 80, 0L);
			SendDlgItemMessage(hDlg, SLE_FILENAME, EM_SETSEL, 0, 0L);
			lstrcpy(tmpPath, DIR_PATH);
			lstrcat(tmpPath, "\\");
			lstrcat(tmpPath, DEF_QUALIFIER);
			if(DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING))
				DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
			else
				{
				getcwd(DIR_PATH, 100);
				lstrcpy(tmpPath, DIR_PATH);
				lstrcat(tmpPath, "\\");
				lstrcat(tmpPath, DEF_QUALIFIER);
				DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING);
				DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
				}
			SetDlgItemText(hDlg, SLE_FILENAME, DEF_QUALIFIER);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					FILE_HANDLE = -1;
					EndDialog(hDlg, NULL);
					break;

				case IDOK:
					GetDlgItemText(hDlg, SLE_FILENAME, tmpPath, 125);
					if (lstrchr(tmpPath, '*') || lstrchr(tmpPath, '?'))
						{ //path has wild card character so can only do directory listing
						if(DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING))
							{
							DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
							if (((chrPtr = lstrrchr(tmpPath, '\\')) != NULL) ||
									((chrPtr = lstrrchr(tmpPath, ':')) != NULL))
								SetDlgItemText(hDlg, SLE_FILENAME, ++chrPtr);
							}
						else
							{//bad path
							MessageBeep(0);
							MessageBox(hDlg,"Invalid Directory Path!", "Open File Error",
							 MB_OK|MB_SYSTEMMODAL);
							}
						}
					 else
						{
						if (-1 == (FILE_HANDLE = _lopen(tmpPath, OPEN_OPTIONS)))
							{
							if (((((chrPtr=lstrrchr(tmpPath, '\\')) != NULL)&&(!lstrrchr(chrPtr, '.')))
										 ||
										 (!(lstrrchr(tmpPath, '.')))) &&
										 ((chrPtr=lstrrchr(DEF_QUALIFIER, '.')) != NULL))
								{ //add qualifier to path
								lstrcat(tmpPath, chrPtr);
								}
							if (-1 == (FILE_HANDLE = _lopen(tmpPath, OPEN_OPTIONS)))
								{
								GetDlgItemText(hDlg, SLE_FILENAME, tmpPath, 125);
								lstrcat(tmpPath, "\\");
								lstrcat(tmpPath, DEF_QUALIFIER);
								if(DlgDirList(hDlg, tmpPath, LB_FILES, ST_DIRNAME, FILESLISTING))
									{
									DlgDirList(hDlg, tmpPath, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
									SetDlgItemText(hDlg, SLE_FILENAME, DEF_QUALIFIER);
									}
								else
									{//bad path
									MessageBeep(1);
									MessageBox(hDlg,"Invalid file name!", "FILE ERROR",
									MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
									}
								}
							}
						if(FILE_HANDLE != -1)
							{
							if((*CHECKPROCEDURE)(FILE_HANDLE))
								{
								ParseFileName((LPSTR)tmpPath);
								EndDialog(hDlg, NULL);
								}
							else
								{
								_lclose(FILE_HANDLE);
								}
							}
						}
					break;

				case LB_FILES:
					switch (HIWORD(lParam))
						{
						case LBN_SELCHANGE:
							DlgDirSelect(hDlg, FILE_NAME, LB_FILES);
							SetDlgItemText(hDlg, SLE_FILENAME, FILE_NAME);
							break;
						case LBN_DBLCLK:
							SendMessage(hDlg, WM_COMMAND, IDOK, 0L);
							msgHandled = FALSE;
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				case LB_DIRECTORIES:
					switch (HIWORD(lParam))
						{
						case LBN_DBLCLK:
							GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 25);
							DlgDirSelect(hDlg, tmpStr2, LB_DIRECTORIES);
							if (lstrchr(tmpStr1, '*') || lstrchr(tmpStr1, '?'))
								{
								if (((chrPtr = lstrrchr(tmpStr1, '\\')) != NULL) ||
										((chrPtr = lstrrchr(tmpStr1, ':')) != NULL))
									chrPtr++;
								else
									chrPtr = tmpStr1;
								lstrcat(tmpStr2, chrPtr);
								SetDlgItemText(hDlg, SLE_FILENAME, chrPtr);
								}
							else
								{
								lstrcat(tmpStr2, DEF_QUALIFIER);
								SetDlgItemText(hDlg, SLE_FILENAME, DEF_QUALIFIER);
								}
							DlgDirList(hDlg, tmpStr2, LB_FILES, ST_DIRNAME, FILESLISTING);
							DlgDirList(hDlg, tmpStr2, LB_DIRECTORIES, 0, DIRECTORIESLISTING);
							break;
						case LBN_SELCHANGE:
							GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 25);
							DlgDirSelect(hDlg, tmpStr2, LB_DIRECTORIES);
							if (lstrchr(tmpStr1, '*') || lstrchr(tmpStr1, '?'))
								{
								if (((chrPtr = lstrrchr(tmpStr1, '\\')) != NULL)||
										((chrPtr = lstrrchr(tmpStr1, ':')) != NULL))
									chrPtr++;
								else
									chrPtr = tmpStr1;
								lstrcat(tmpStr2, chrPtr);
								}
							else
								{
								lstrcat(tmpStr2, DEF_QUALIFIER);
								}
							SetDlgItemText(hDlg, SLE_FILENAME, tmpStr2);
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				case SLE_FILENAME:
					switch (HIWORD(lParam))
						{
						case EN_CHANGE:
							if (0 == GetDlgItemText(hDlg, SLE_FILENAME, tmpStr1, 2))
								EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
							else
								EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					break;

				default:
					msgHandled = FALSE;
					break;
					}
			break;

		default:
			msgHandled = FALSE;
			break;
		}

	return msgHandled;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: lstrrchr
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPSTR lstrrchr(
	LPSTR str,
	char	ch)
	{
	LPSTR strl = str + lstrlen(str);

	do
		{
		if (ch == *strl)
			return strl;
		strl = AnsiPrev(str, strl);
		}
	while (strl>str);

	return NULL;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: lstrchr
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPSTR lstrchr(
	LPSTR str,
	char	ch)
	{

	while (*str)
		{
		if (ch == *str)
			return str;
		str = AnsiNext(str);
		}

	return NULL;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  HasExtension
//
// CLASS.......:
//
// DESCRIPTION.:  returns true if the str has an extension (.XXX)
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int HasExtension(LPSTR str)
	{
	LPSTR sPtr = str + lstrlen(str);
	do
		{
		sPtr--;
		if(*sPtr == '.')
			{
			if(sPtr > str)
				{
				sPtr--;
				if(*sPtr != '\\' && *sPtr != '.')
					return 1;
				else
					return 0;
				}
			}
		}
	while(sPtr > str);
	return 0;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddExtension
//
// CLASS.......:
//
// DESCRIPTION.:  adds the Default qualifier to the string
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void AddExtension(LPSTR str)
	{
	LPSTR sPtr = lstrchr(DEF_QUALIFIER, '.');

	lstrcat(str, sPtr);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ParseFileName
//
// CLASS.......:
//
// DESCRIPTION.:  separates the complete path from the filename.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void ParseFileName(LPSTR str)
	{
	int  drive;
	char driveChar;
	int  pathComplete = 0;
	LPSTR tempPtr;
	char workingDir[120];
	int doAppend = 0;

	if(lstrlen(str) > 2)
		{
		if(*(str + 1) == ':')/* path containes drive spedifier */
			{
			if(*(str + 2) == '\\') /* full path specified */
				{
				pathComplete = 1;
				lstrcpy((LPSTR)workingDir, str);
				}
			else  /* get working directory of specified drive */
				{
				driveChar = *str;
				if(driveChar <= 0x5a)
					drive = driveChar - 0x44;
				else
					drive = driveChar - 0x60;
				getcurdir(drive, DIR_PATH);
				wsprintf((LPSTR)workingDir,(LPSTR)"%c:\\%s",driveChar,
					(LPSTR)DIR_PATH);
				str+=2;
				}
			}
		else
			{
			getcwd(workingDir, 120);
			}
		}
	else
		{
		getcwd(workingDir, 120);
		}
	if(!pathComplete)
		{
		if(lstrlen(str) > 2)
			{
			while(*str == '.')
				{
				if(*(str + 1) == '\\')
					{
					str++;
					doAppend = 1;
					}
				else if(*(str + 1) == '.')
					{
						tempPtr = workingDir + lstrlen(workingDir)  - 1;
						while(*tempPtr != '\\' && tempPtr > workingDir)
							tempPtr--;
						*tempPtr = '\0';
						str+=2;
						doAppend = 1;
					}
				str++;
				}
			if(doAppend)
				{
				str--;
				}
			else
				{
				lstrcat((LPSTR)workingDir, (LPSTR)"\\");
				}
			lstrcat((LPSTR)workingDir, str);
			}
		else
			{
				lstrcat((LPSTR)workingDir, (LPSTR)"\\");
				lstrcat((LPSTR)workingDir, str);
			}
		}
	tempPtr = workingDir + lstrlen((LPSTR)workingDir) -1;
	while(*tempPtr != '\\')
		tempPtr --;
	tempPtr++;
	lstrcpy((LPSTR)FILE_NAME,(LPSTR)tempPtr);
	*tempPtr= '\0';
	lstrcpy((LPSTR)DIR_PATH, (LPSTR)workingDir);
	_fstrupr(DIR_PATH);
	_fstrupr(FILE_NAME);
	}
