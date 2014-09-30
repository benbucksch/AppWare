////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"FileDlg.cpp"
//
// AUTHOR:
//
// DESCRIPTION:Open and Close File Dialog Boxes
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <dir.h>
#include "com_dlg.h"
#include "filetrns.h"
#include "filedlg.h"

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ FileOpenDlgProc (HWND, WORD, WORD, LONG) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ ChoosePathDlgProc (HWND, WORD, WORD, LONG) ;

int DoFileOpenDlg (char* name)
{

	HWND         hwnd = GetActiveWindow() ;
	int          iReturn ;

	lstrcpy (name, "INITIALIZE") ;
	iReturn = DialogBoxParam (hInstance, "CreateFile", hwnd, (DLGPROC) FileOpenDlgProc, (LPARAM)name) ; 
	return (iReturn) ;
}
void ShowDlgDirList (HWND hDlg, char *filename)
{
	WORD  wAttr1 = DDL_DRIVES|DDL_DIRECTORY|DDL_EXCLUSIVE ;
	WORD  wAttr2 = DDL_READWRITE ;
	char* name = (char*) GetWindowLong (hDlg, DWL_USER) ;
	*name = '\0' ; 
	DlgDirList (hDlg,	
		filename, 
		IDD_DRIVE,
		0,
		wAttr1) ;
	DlgDirList (
		hDlg,		
		filename,		
		IDD_FLIST,
		IDD_FPATH,
		wAttr2) ;	
}

BOOL ALMCALLBACK FileOpenDlgProc (HWND hDlg, WORD message,
                                 WORD wParam, LONG lParam)
{
	char  cLastChar ;
	char  filename[256]="*.*" ;
	char  fullname [256] ;
  char* name ;

	switch (message)
  	{
		case WM_INITDIALOG:
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			SetDlgItemText (hDlg, IDD_FNAME, filename) ;
			ShowDlgDirList (hDlg, filename) ;
      return TRUE ;

		case WM_COMMAND: //!!PORT!!
      switch (LOWORD(wParam))
				{
				case IDD_FNAME:
					if (HIWORD (wParam) ==EN_CHANGE)
					  {
						name = (char *)GetWindowLong (hDlg, DWL_USER) ;
						if (lstrcmp(name, "INITIALIZE"))
							lstrcpy (name, "IDD_FNAME") ;
					  }
					break;

				case IDD_DRIVE:
					switch (HIWORD (wParam))
					{
					case LBN_DBLCLK:
						if (DlgDirSelectEx (hDlg, fullname, strlen(fullname), IDD_DRIVE))
            	{
							GetDlgItemText (hDlg, IDD_FNAME, filename, 256) ;
							strcat (fullname, filename) ;
							ShowDlgDirList (hDlg, fullname) ;
              }
					}
          break ;
				case IDD_FLIST:
          switch (HIWORD (wParam))
           	{
           	case LBN_DBLCLK:
							DlgDirSelectEx (hDlg, filename, strlen(filename), IDD_FLIST) ;
							GetDlgItemText(hDlg, IDD_FPATH, fullname, 256) ;
							cLastChar = *AnsiPrev (fullname, fullname+lstrlen(fullname)) ;
							if (cLastChar!='\\') strcat (fullname,"\\") ;
							strcat (fullname,filename) ;  
							//SendDlgItemMessage (hDlg, IDD_SFNAME, LB_INSERTSTRING, -1, (long)fullname) ;
							ListBox_InsertString (GetDlgItem(hDlg, IDD_SFNAME), -1, fullname) ;
						}
						break ;
				case IDD_REMOVE:
          {
					//long selectCount = SendDlgItemMessage (hDlg, IDD_SFNAME, LB_GETSELCOUNT, 0,0L) ;
					long selectCount = ListBox_GetSelCount (GetDlgItem(hDlg, IDD_SFNAME)) ;
					int Index ;
	 				//if (selectCount!=0 &&(0!=SendDlgItemMessage (hDlg, IDD_SFNAME, LB_GETCOUNT, 0, 0)))
					if (selectCount!=0&&(0!=ListBox_GetCount(GetDlgItem(hDlg, IDD_SFNAME))))
	 				{
						while (ListBox_GetSelItems (GetDlgItem (hDlg, IDD_SFNAME), 1, &Index)!=0)
							{
							ListBox_DeleteString(GetDlgItem (hDlg, IDD_SFNAME), Index) ; 
							}
						ListBox_SetCaretIndex (GetDlgItem (hDlg, IDD_SFNAME), 0) ;
	 					//while(SendDlgItemMessage (hDlg, IDD_SFNAME, LB_GETSELITEMS, 1, (long)&Index) !=0)
						//{
						//SendDlgItemMessage (hDlg, IDD_SFNAME, LB_DELETESTRING, (WORD)Index, 0) ;
						//}	 
						//SendDlgItemMessage (hDlg, IDD_SFNAME, LB_SETCARETINDEX, 0, 0) ;
     			}
					break;
          }
				case IDD_ADD:
					{
					DlgDirSelectEx (hDlg, filename, strlen (filename), IDD_FLIST) ;
					GetDlgItemText(hDlg, IDD_FPATH, fullname, 256) ;		
					cLastChar = *AnsiPrev (fullname, fullname+lstrlen(fullname)) ;		
					if (cLastChar!='\\') strcat (fullname,"\\") ;		
					strcat (fullname,filename) ;  		
					ListBox_InsertString (GetDlgItem (hDlg, IDD_SFNAME), -1, fullname) ;
					//SendDlgItemMessage (hDlg, IDD_SFNAME, LB_INSERTSTRING, -1, (long)fullname) ;		
					}
					break;

				case IDOK:
					name = (char *)GetWindowLong(hDlg, DWL_USER) ;
					if (lstrcmp(name, "IDD_FNAME"))
					  {
						int count ;
 	       		int i ;
						//count = (int)SendDlgItemMessage(hDlg, IDD_SFNAME, LB_GETCOUNT, 0, 0) ;
						count = ListBox_GetCount (GetDlgItem (hDlg, IDD_SFNAME)) ;
						for (i=0; i<count; i++)
							{
							ListBox_GetText (GetDlgItem (hDlg, IDD_SFNAME), i, fullname) ;
							//SendDlgItemMessage(hDlg, IDD_SFNAME, LB_GETTEXT, (WORD)i, (long)fullname) ;
							int len = lstrlen(fullname) ;
							if (name+len+3!= NULL)
          			{
								CopyMemory (name, fullname, len+1) ;
								name+=len+1 ;
								}
							}
						EndDialog (hDlg, TRUE) ;
						}
					else
					  {
					  GetDlgItemText (hDlg, IDD_FNAME, filename, 80) ;
						ShowDlgDirList (hDlg, filename) ;
						*name ='\0' ;
					  }
          break ;

        case IDCANCEL:
          EndDialog (hDlg, FALSE) ;
          break ;
				}
    }
    return FALSE ;
}
int ChooseDirectoryDlg (char *directory)
{
     HWND         hwnd = GetActiveWindow() ;
		 int          iReturn ;

		 iReturn = DialogBoxParam (hInstance, "ChoosePath", hwnd, (DLGPROC)ChoosePathDlgProc, (LPARAM)directory) ;
     return iReturn ;
}
BOOL ALMCALLBACK ChoosePathDlgProc (HWND hDlg, WORD message,
                                   WORD wParam, LONG lParam)
{
		char * directory ;
		char   tmpDir[128] ;

		switch (message)
    {
				case WM_INITDIALOG:
					 directory = (char *)lParam ;
					 SetWindowLong (hDlg, DWL_USER, lParam) ;
					 DlgDirList (hDlg, "*.*", IDD_DRIVE, 0, DDL_DRIVES) ;
					 DlgDirList (hDlg, "*.*", IDD_PATH, IDD_FPATH, DDL_DIRECTORY|DDL_EXCLUSIVE) ;
					 GetDlgItemText (hDlg, IDD_FPATH, directory, 256) ;
					 return TRUE ;

				case WM_COMMAND: //!!PORT!!
					 directory = (char *) GetWindowLong(hDlg, DWL_USER) ;
           switch (LOWORD(wParam))
					 		{
              case IDD_PATH:
                 switch (HIWORD (wParam))
                    {
                    case LBN_DBLCLK:
											 if (DlgDirSelectEx (hDlg, tmpDir, strlen (tmpDir), IDD_PATH))
                       {
													DlgDirList (hDlg, tmpDir, IDD_PATH, IDD_FPATH, DDL_DIRECTORY|DDL_EXCLUSIVE) ;
                       }
                       return TRUE ;
                    }
								 break ;

							case IDD_DRIVE:
								 switch (HIWORD (wParam))
                    {
                    case LBN_DBLCLK:
											 if (DlgDirSelectEx (hDlg, tmpDir, strlen(tmpDir), IDD_DRIVE))
                       {
													DlgDirList (hDlg, tmpDir, IDD_DRIVE, IDD_FPATH, DDL_DRIVES) ;
													DlgDirList (hDlg, tmpDir, IDD_PATH, IDD_FPATH, DDL_DIRECTORY|DDL_EXCLUSIVE) ;
                       }
                       return TRUE ;
                    }
								 break ;
							case IDOK:
              	 GetDlgItemText (hDlg, IDD_FPATH, directory, 256) ;  	
								 EndDialog (hDlg, TRUE) ;
                 return TRUE ;

							case IDCANCEL:
                 EndDialog (hDlg, FALSE) ;
                 return TRUE ;
              }
        }
		 return FALSE ;
}
