////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  		Launcher			
//
// Filename:	"launch.cpp"
//
// Author:	Kevin Smart
//   Start:		August  5, 1992
//   Finish:	August  6, 1992
//
// Description:
//	Code for LAUNCH and OPENDLG class functions
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<string.h>
#include	<helpids.h>

#include	"launch.h"

////////////////////////////////////////////////////////////////////////////

extern GLOBAL	G;

////////////////////////////////////////////////////////////////////////////

LAUNCH::LAUNCH(
	OBJECTID	obj)
	:
		m_CmdLine(NULL),
		m_StartDir(NULL),
		m_RunMin(NULL),
		m_ver(NULL)

	{
	char*				pszTmp;
	BOOL*				pfTmp;
	int*				piTmp;

  theObject = obj;

	if(!theObject || AObjGetType(theObject)!=OTYPE_LAUNCHER) return;

  if(AObjGetDataCount(theObject)<4L)
    AObjSetDataCount(theObject, 4L);

	// Data 0
	if ((pszTmp = (char*) AObjLockData(theObject, 0L)) == NULL)
		{
		AObjResizeData(theObject, 0L, sizeof(char));
		pszTmp  = (char*) AObjLockData(theObject, 0L);
		if (pszTmp)
			*pszTmp = '\0';
		}
	if (pszTmp)
		AObjUnlockData(theObject, 0L);

	// Data 1
	if ((pszTmp = (char*) AObjLockData(theObject, 1L)) == NULL)
		{
		AObjResizeData(theObject, 1L, sizeof(char));
		pszTmp  = (char*) AObjLockData(theObject, 1L);
		if (pszTmp)
			*pszTmp = '\0';
		}
	if (pszTmp)
		AObjUnlockData(theObject, 1L);

	// Data 2
	if ((pfTmp = (BOOL*) AObjLockData(theObject, 2L)) == NULL)
		{
		AObjResizeData(theObject, 2L, sizeof(BOOL));
		pfTmp  = (BOOL*) AObjLockData(theObject, 2L);
		if (pfTmp)
			*pfTmp = '\0';
		}
	if (pfTmp)
		AObjUnlockData(theObject, 2L);


	// Data 3
	if ((piTmp = (int*) AObjLockData(theObject, 3L)) == NULL)
		{
		AObjResizeData(theObject, 3L, sizeof(int));
		piTmp  = (int*) AObjLockData(theObject, 3L);
		if (piTmp)
			*piTmp = VERSION_OBJECT;
		}
	if (piTmp)
		AObjUnlockData(theObject, 3L);

	// Leave all unlocked
	}

////////////////////////////////////////////////////////////////////////////

LAUNCH::~LAUNCH(
	VOID)
	{
	if (m_CmdLine)
		AObjUnlockData(theObject, 0L);
	if (m_StartDir)
		AObjUnlockData(theObject, 1L);
	if (m_RunMin)
		AObjUnlockData(theObject, 2L);
	if (m_ver)
		AObjUnlockData(theObject, 3L);
	}

////////////////////////////////////////////////////////////////////////////

BOOL far pascal _export LAUNCH::LaunchDlg(
	HWND	hDlg,
	WORD	message,
	WORD	wParam,
	long	lParam)

	{
  LAUNCH*	launch;
  char	szBuffer[OBJECTNAMESIZE];

	switch(message)
		{
    case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER, lParam);
			launch = (LAUNCH*)lParam;
			launch->InitDlgControls(hDlg);
			AUtlSetTitleFont(OTYPE_LAUNCHER, NULL, GetDlgItem(hDlg, IDD_OBJECT));

			if(launch->style==1)
				{
				AObjGetName(launch->theObject, szBuffer, OBJECTNAMESIZE);
				SetWindowText(hDlg, szBuffer);
				}

			PostMessage(GetDlgItem(hDlg, IDD_CMDLINE), EM_SETSEL, TRUE, MAKELONG(0, -1));
			return TRUE;

    case WM_COMMAND:
			launch = (LAUNCH*)GetWindowLong(hDlg, DWL_USER);

			switch(wParam)
				{
   			case IDOK:
					launch->UpdateData(hDlg);
          EndDialog(hDlg, TRUE);
					return TRUE;

			  case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;

			  case IDBROWSE:
					OPENDLG	od(hDlg);
          char		szBuffer[256];
					int			len = GetDlgItemText(hDlg, IDD_STARTDIR, szBuffer, 256);

					if(len>0)
						od.SetInitDir(szBuffer);

					if(od.FileDlg())
						{
					  SetDlgItemText(hDlg, IDD_CMDLINE, od.GetName());
						if(len==0)
							{
					    strcpy(szBuffer, od.GetName());
					    szBuffer[od.GetFileOffset()==3 ? 3 : od.GetFileOffset()-1] = 0;
					    SetDlgItemText(hDlg, IDD_STARTDIR, szBuffer);
					    }
            }
					return TRUE;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Appl);
					return TRUE;

				default:
					break;
  			}
	    break;
    }

  return FALSE;
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::InitDlgControls(
	HWND	hDlg)

	{
  SetDlgItemText(hDlg, IDD_CMDLINE,  GetCmdLine());
  SetDlgItemText(hDlg, IDD_STARTDIR, GetStartDir());

  CheckDlgButton(hDlg, IDD_RUNMIN,   GetRunMin());
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::UpdateData(
	HWND	hDlg)

	{
  char	szBuffer[256];

  GetDlgItemText(hDlg, IDD_CMDLINE, szBuffer, 256);
  SetCmdLine(szBuffer);

  GetDlgItemText(hDlg, IDD_STARTDIR, szBuffer, 256);
  SetStartDir(szBuffer);

  SetRunMin(IsDlgButtonChecked(hDlg, IDD_RUNMIN));
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::SetCmdLine(
	char*	cmdline)

	{
  int len = strlen(cmdline);

	if (m_CmdLine)
		{
		AObjUnlockData(theObject, 0L);
		m_CmdLine = NULL;
		}

  AObjSetData(theObject, 0L, (void*)cmdline, len+1);
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::SetStartDir(
	char*	startdir)

	{
  int len = strlen(startdir);

	if (m_StartDir)
		{
		AObjUnlockData(theObject, 1L);
		m_StartDir = NULL;
		}

  AObjSetData(theObject, 1L, (void*)startdir, len+1);
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::SetRunMin(
	BOOL		fRunMin)

	{
	if (m_RunMin)
		{
		AObjUnlockData(theObject, 2L);
		m_RunMin = NULL;
		}

  AObjSetData(theObject, 2L, (void*)&fRunMin, sizeof(BOOL));
	}

////////////////////////////////////////////////////////////////////////////

int	LAUNCH::Version(
	VOID
	)
	{
	if (m_ver == NULL)
		m_ver = (int*) AObjLockData(theObject, 3L);

	return ((m_ver) ? *m_ver : -1);
	}

////////////////////////////////////////////////////////////////////////////

char*		LAUNCH::GetCmdLine(
	VOID
	)
	{
	if (m_CmdLine == NULL)
		m_CmdLine = (char*) AObjLockData(theObject, 0L);

	return m_CmdLine;
	}

////////////////////////////////////////////////////////////////////////////

char*		LAUNCH::GetStartDir(
	VOID
	)
	{
	if (m_StartDir == NULL)
		m_StartDir = (char*) AObjLockData(theObject, 1L);

	return m_StartDir;
	}

////////////////////////////////////////////////////////////////////////////

BOOL		LAUNCH::GetRunMin(
	VOID
	)
	{
	if (m_RunMin == NULL)
		m_RunMin = (BOOL*) AObjLockData(theObject, 2L);

	return ((m_RunMin) ? *m_RunMin : FALSE);
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::ExportObject()

	{
	int		version			= Version();
	BOOL	runMin			= GetRunMin();
	char*	pszCmdLine	= GetCmdLine();
	char*	pszStartDir	= GetStartDir();

	// ignore errors--check on import
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &version, sizeof(int));
	AUpsfExportField("CMDLINE", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, pszCmdLine, strlen(pszCmdLine));
	AUpsfExportField("STRTUPDR", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, pszStartDir, strlen(pszStartDir));
	AUpsfExportField("MINIMIZD", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &runMin, sizeof(BOOL));
	}

////////////////////////////////////////////////////////////////////////////

void	LAUNCH::ImportObject()

	{
	int version;
	BOOL	runMin;

	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &version, sizeof(int))!=AUPSF_NOERROR)
		version = 0;

	if(version>VERSION_OBJECT)
		AUpsfLogComment("Possible version incomptibility for the Launcher object.");

	long	length;
  char*	string;
  long	ret;
	ret = AUpsfFieldInfo("CMDLINE", 0L, AUPSF_NATV_STRING, NULL, &length);
	if(ret == AUPSF_NOERROR)
		{
		string = new char[(int)length];
		if(string)
			{
			ret = AUpsfImportField("CMDLINE", 0L, AUPSF_NATV_STRING, string, length);
			if(ret == AUPSF_NOERROR)
				SetCmdLine(string);
			else
				AUpsfLogComment("Error importing the command line.");
			delete string;
			}
		}

	ret = AUpsfFieldInfo("STRTUPDR", 0L, AUPSF_NATV_STRING, NULL, &length);
	if(ret == AUPSF_NOERROR)
		{
		string = new char[(int)length];
		if(string)
			{
			ret = AUpsfImportField("STRTUPDR", 0L, AUPSF_NATV_STRING, string, length);
			if(ret == AUPSF_NOERROR)
				SetStartDir(string);
			else
				AUpsfLogComment("Error importing the startup directory.");
			delete string;
			}
		}

	if(AUpsfImportField("MINIMIZD", 0L, AUPSF_NATV_BOOL, &runMin, sizeof(BOOL))==AUPSF_NOERROR)
		SetRunMin(runMin);
	else
		AUpsfLogComment("Defaulting to a non-minimized application.");
	}

////////////////////////////////////////////////////////////////////////////

char* OPENDLG::filter = "Programs\0*.exe;*.pif;*.com;*.bat\0All Files (*.*)\0*.*\0";
char* OPENDLG::title  = "Browse";

////////////////////////////////////////////////////////////////////////////

OPENDLG::OPENDLG(
	HWND	hWnd)

	{
  filename[0]	= '\0';
	file_title[0]	= '\0';

  memset(&ofn, 0, sizeof(OPENFILENAME));

  ofn.lStructSize 		= sizeof(OPENFILENAME);
	ofn.hwndOwner 			= hWnd;
  ofn.lpstrFilter 		= filter;
  ofn.nFilterIndex 		= 1;
  ofn.lpstrFile 			= filename;
  ofn.nMaxFile 				= 256;
  ofn.lpstrTitle			= title;
  ofn.lpstrFileTitle	= file_title;
  ofn.nMaxFileTitle		= 13;
  ofn.Flags						= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	}

////////////////////////////////////////////////////////////////////////////

BOOL	OPENDLG::FileDlg(
	char*	p)

	{
	if (p != NULL)
		ofn.lpstrFilter = p;
  return GetOpenFileName((LPOPENFILENAME)&ofn);
	}

