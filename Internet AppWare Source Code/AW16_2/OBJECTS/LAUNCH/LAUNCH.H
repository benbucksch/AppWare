////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  		Launcher			
//
// Filename:	"launch.h"
//
// Author:	Kevin Smart
//   Start:		August  5, 1992
//   Finish:	August  5, 1992
//
// Description:
//	General header for the launch object
//
////////////////////////////////////////////////////////////////////////////
#ifndef __COMMDLG_H
#include	<commdlg.h>
#endif

///////////////////////////Global Container Class///////////////////////////

class GLOBAL {
  public:
    HANDLE	hInst;
  };

//////////////////////////LAUNCH class definition///////////////////////////

class LAUNCH {
  private:
    char*		m_CmdLine;	// data 0
    char*		m_StartDir;	// data 1
    BOOL*		m_RunMin;		// data 2
    int*		m_ver;			// data 3

  public:
    OBJECTID	theObject;
    int				style;

		LAUNCH(OBJECTID);
	 ~LAUNCH(VOID);

    int			Version();

    static BOOL far pascal _export	LaunchDlg(HWND, WORD, WORD, long);
    void		InitDlgControls(HWND);
    void		UpdateData(HWND);
          	
    char*		GetCmdLine();
    void		SetCmdLine(char*);
    char*		GetStartDir();
    void		SetStartDir(char*);
		BOOL		GetRunMin();
		void		SetRunMin(BOOL min);
    WORD		GetCmdShow()						{return ((GetRunMin()) ? SW_MINIMIZE : SW_SHOW);}

		void		ExportObject();
		void		ImportObject();
  };

/////////////////////////OPENDLG class definition///////////////////////////

class OPENDLG {
  private:
    static char*	filter;
    static char*	title;	  
    char					filename[256];
    char					file_title[13];
    OPENFILENAME	ofn;

  public:
    OPENDLG(HWND = NULL);

    int		GetFileOffset()				{return ofn.nFileOffset;}
    void	SetFlags(DWORD Flags)	{ofn.Flags = Flags;}
    char*	GetName()							{return filename;}
    void	SetInitDir(char* p)		{ofn.lpstrInitialDir = p;}
    BOOL	FileDlg(char* = NULL);
  };

////////////////////////////Resource Constants//////////////////////////////

#define	IDD_CMDLINE			101
#define	IDD_STARTDIR		102
#define	IDD_RUNMIN			103
#define	IDD_OBJECT			201

#define IDBROWSE				110
#define	IDHELP					998

#define	OBJECTNAMESIZE	32

#define OTYPE_LAUNCHER									MAKEID('A','p','p','l')


// version constants

#define VERSION_DLL			1L
#define VERSION_OBJECT	0

///////////////////////Exported Function Prototypes/////////////////////////

extern "C" {
  // functions in "launc_ao.cpp"
	long ALMCALLBACK LaunchAdmn				(OBJECTID, pAObjMessage);
	long ALMCALLBACK GetSerVersion		(TYPEID);

  // functions in "launc_fn.cpp"
  void ALMCALLBACK LaunchApplication(pAEvtInfo, pAObjMessage);
  void ALMCALLBACK SetApplication		(pAEvtInfo, pAObjMessage);
  }

////////////////////////////////////////////////////////////////////////////
