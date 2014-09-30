/*-----------------------
   FILEDLG.H header file
  -----------------------*/

typedef struct defChooseFile
{
		char     *szFileName ;
		int       filecounts ;
		char      szFileSpec[16] ;
		char      szFileDefExt[5] ;
		WORD      wFileAttr ;
		BOOL      bFilter ;

}  CHOOSEFILE ;
typedef CHOOSEFILE  * LPCHOOSEFILE ;

int DoFileOpenDlg(void) ;
int ChooseDirectoryDlg (char *directory) ;
LPSTR lstrchr  (LPSTR str, char ch) ;
LPSTR lstrrchr (LPSTR str, char ch) ;
