//--------------------------------------------------------------------------
// File Name:  BindEdit.CPP
// Description:
//    The AppWare Bindery object's Object Definition Routine used to edit the
//    Bindery object's data.
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>    // defines OTYPE_TEXT
#include <o_number.h>  // defines OTYPE_NUMBER

#include "bindery.h"
#include "nwbindrc.h"


#define STR_NO_SELECTION   "[None]"


BOOL InitDialog(   HWND, OBJECTID );
BOOL UpdateObject( HWND );


//--------------------------------------------------------------------------
LONG EditObject( OBJECTID oiBindery )
//--------------------------------------------------------------------------
{
   long   retCode = A_OK;

   BOOL result = DialogBoxParam( hInstance, "OBJ_EDIT_2", GetActiveWindow(),
                                 (DLGPROC)EditObjectDialogProc, (LONG)oiBindery );

   if ( result == TRUE )
      retCode = A_MODIFIED;

   return retCode;

} // end of EditObject( )

//--------------------------------------------------------------------------
//  EditObjectDialogProc
//  Used to edit the data members of the Bindery object at design time
//--------------------------------------------------------------------------
BOOL FAR PASCAL _export EditObjectDialogProc ( HWND hDlg, UINT message,
                                               UINT wParam, LONG lParam )
{
   OBJECTID        oiBindery;

   oiBindery = (OBJECTID)lParam;

   switch (message)
   {
      case WM_INITDIALOG:
         return InitDialog( hDlg, oiBindery );

      case WM_COMMAND:
         switch( wParam )
         {
            case IDOK:
               return UpdateObject( hDlg );

            case IDCANCEL:
               EndDialog( hDlg, FALSE );
               return TRUE;

            case IDC_OBJ_EDIT_HELP:
               // execute Help system with the appropriate file and context #
               WinHelp( hDlg, "NWBIND.HLP", HELP_CONTEXT, (DWORD)IDH_OBJ_EDIT_HELP );
               return TRUE;
         }
   }
   return FALSE;
} // end of EditObjectDialogProc( )

//--------------------------------------------------------------------------
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the Bindery Object's edit dialog box.
//--------------------------------------------------------------------------
BOOL InitDialog ( HWND hDlg, OBJECTID oiBindery)
{
   BinderyData far *   pBinderyData;
   char                buffer[40];
   OBJECTID            oiToAdd;
   long                key, index;

   SetWindowLong( hDlg, DWL_USER, (LONG)oiBindery ); // save the object ID in the window extra bytes

   HWND hText = GetDlgItem( hDlg, IDC_DLG_TITLE );   // set the title text in the dialog box
   AUtlSetTitleFont( OTYPE_BINDERY, NULL, hText );   //   to the name of the object

   pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
   if ( pBinderyData == NULL )
      return TRUE;

   // initialize the combo box for the 'Server Name' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_SERVER_NAME, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) );
	SendDlgItemMessage (hDlg, IDC_SERVER_NAME, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0)
	{
		AObjGetName(oiToAdd, buffer, 40) ;
		index = SendDlgItemMessage(hDlg, IDC_SERVER_NAME, CB_ADDSTRING, 0, (long)buffer);
		SendDlgItemMessage (hDlg, IDC_SERVER_NAME, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
	}

   // initialize the combo box for the 'Object Name (Filter)' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_OBJ_NAME_FILTER, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) ) ;
	SendDlgItemMessage (hDlg, IDC_OBJ_NAME_FILTER, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0)
	{
		AObjGetName(oiToAdd, buffer, 40) ;
		index = SendDlgItemMessage(hDlg, IDC_OBJ_NAME_FILTER, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage (hDlg, IDC_OBJ_NAME_FILTER, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
	}

   // initialize the combo box for the 'Object Type (Filter)' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_OBJ_TYPE_FILTER, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) ) ;
   SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_FILTER, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
// while((oiToAdd = AObjGetNextObject(OTYPE_NUMBER, &key)) != 0)
   while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0)
   {
		AObjGetName(oiToAdd, buffer, 40);
		index = SendDlgItemMessage(hDlg, IDC_OBJ_TYPE_FILTER, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_FILTER, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
   }

   // initialize the combo box for the 'Object Name (Data)' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_OBJ_NAME_DATA, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) ) ;
	SendDlgItemMessage (hDlg, IDC_OBJ_NAME_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0)
	{
		AObjGetName(oiToAdd, buffer, 40) ;
		index = SendDlgItemMessage(hDlg, IDC_OBJ_NAME_DATA, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage (hDlg, IDC_OBJ_NAME_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
	}

   // initialize the combo box for the 'Object Type (Data)' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_OBJ_TYPE_DATA, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) ) ;
	SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
// while((oiToAdd = AObjGetNextObject(OTYPE_NUMBER, &key)) != 0)
   while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0)
	{
		AObjGetName(oiToAdd, buffer, 40) ;
		index = SendDlgItemMessage(hDlg, IDC_OBJ_TYPE_DATA, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
	}

   // initialize the combo box for the 'Object ID (Data)' object attribute
   key = AOBJ_GETFIRSTKEY;
   index = SendDlgItemMessage(hDlg, IDC_OBJ_ID_DATA, CB_ADDSTRING, 0, (long)(STR_NO_SELECTION) ) ;
	SendDlgItemMessage (hDlg, IDC_OBJ_ID_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)0));
	while((oiToAdd = AObjGetNextObject(OTYPE_NUMBER, &key)) != 0)
	{
		AObjGetName(oiToAdd, buffer, 40);
		index = SendDlgItemMessage(hDlg, IDC_OBJ_ID_DATA, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage (hDlg, IDC_OBJ_ID_DATA, CB_SETITEMDATA, index, (LPARAM)((LPSTR)oiToAdd));
	}


	if ( AObjGetName(pBinderyData->ServerName, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_SERVER_NAME, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if (index != CB_ERR)
		{
			SendDlgItemMessage (hDlg, IDC_SERVER_NAME, CB_SETCURSEL, index, 0);
      }
	}

	if ( AObjGetName(pBinderyData->ObjNameFilter, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_OBJ_NAME_FILTER, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if (index != CB_ERR)
		{
			SendDlgItemMessage (hDlg, IDC_OBJ_NAME_FILTER, CB_SETCURSEL, index, 0);
      }
	}

	if ( AObjGetName(pBinderyData->ObjTypeFilter, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_FILTER, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if (index != CB_ERR)
		{
			SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_FILTER, CB_SETCURSEL, index, 0);
      }
	}

	if ( AObjGetName(pBinderyData->ObjNameData, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_OBJ_NAME_DATA, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if ( index != CB_ERR )
		{
			SendDlgItemMessage( hDlg, IDC_OBJ_NAME_DATA, CB_SETCURSEL, index, 0 );
      }
	}

	if ( AObjGetName(pBinderyData->ObjTypeData, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_OBJ_TYPE_DATA, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if ( index != CB_ERR )
		{
			SendDlgItemMessage( hDlg, IDC_OBJ_TYPE_DATA, CB_SETCURSEL, index, 0 );
      }
	}

	if ( AObjGetName(pBinderyData->ObjIDData, buffer, 40) )
   {
		index = SendDlgItemMessage (hDlg, IDC_OBJ_ID_DATA, CB_FINDSTRINGEXACT,0,(LPARAM) ((LPSTR)buffer));
		if (index != CB_ERR)
		{
			SendDlgItemMessage (hDlg, IDC_OBJ_ID_DATA, CB_SETCURSEL, index, 0);
      }
	}

	AObjUnlockData( oiBindery, 0 );

   return TRUE;
} // end of InitDialog( )
    
//--------------------------------------------------------------------------
// FUNCTION:  UpdateObject
// DESCRIPTION:  Updates the Bindery object's data.
//--------------------------------------------------------------------------
BOOL  UpdateObject( HWND hDlg )
{
   OBJECTID            oiBindery;
// OBJECTID            oiObject;
   BinderyData far *   pBinderyData;
   int                 index;
// char                buffer[41], oibuffer[41];
// long                key;

   oiBindery = (OBJECTID)GetWindowLong( hDlg, DWL_USER );

   pBinderyData = (BinderyData far*) AObjLockData( oiBindery, 0 );
   if (pBinderyData == NULL)
   {
      EndDialog( hDlg, FALSE );
      return TRUE;
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_SERVER_NAME, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ServerName = 0;
   else
   {
      pBinderyData->ServerName = (OBJECTID)SendDlgItemMessage( hDlg, IDC_SERVER_NAME,
                                                               CB_GETITEMDATA, index, 0 );
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_OBJ_NAME_FILTER, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ObjNameFilter = 0;
   else
   {
      pBinderyData->ObjNameFilter = (OBJECTID)SendDlgItemMessage( hDlg, IDC_OBJ_NAME_FILTER, CB_GETITEMDATA, index, 0 );
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_OBJ_TYPE_FILTER, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ObjTypeFilter = 0;
   else
   {
      pBinderyData->ObjTypeFilter = (OBJECTID)SendDlgItemMessage( hDlg, IDC_OBJ_TYPE_FILTER,
                                                                  CB_GETITEMDATA, index, 0 );
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_OBJ_NAME_DATA, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ObjNameData = 0;
   else
   {
      pBinderyData->ObjNameData = (OBJECTID)SendDlgItemMessage( hDlg, IDC_OBJ_NAME_DATA,
                                                            CB_GETITEMDATA, index, 0 );
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_OBJ_TYPE_DATA, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ObjTypeData = 0;
   else
   {
      pBinderyData->ObjTypeData = (OBJECTID)SendDlgItemMessage( hDlg, IDC_OBJ_TYPE_DATA,
                                                                CB_GETITEMDATA, index, 0 );
   }

   index = (int)SendDlgItemMessage( hDlg, IDC_OBJ_ID_DATA, CB_GETCURSEL, 0, 0 );
   if ( index == CB_ERR )
      pBinderyData->ObjIDData = 0;
   else
   {
      pBinderyData->ObjIDData = (OBJECTID)SendDlgItemMessage( hDlg, IDC_OBJ_ID_DATA,
                                                              CB_GETITEMDATA, index, 0 );
   }

   AObjUnlockData( oiBindery, 0 );
   EndDialog( hDlg, TRUE );
   return TRUE;
} // end of UpdateObject( )
