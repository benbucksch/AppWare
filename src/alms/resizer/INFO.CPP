#include "resizer.h"

////////////////////////////////////////////////////////////////////////////////
UINT CALLBACK CustomControlInfoA(
	LPCCINFOA 							acci)
    {
    if ( acci != NULL)
			{
		  acci[0].flOptions         = CCF_NOTEXT;
		  acci[0].cxDefault         = 20;      // default width  (dialog units)
		  acci[0].cyDefault         = 20;      // default height (dialog units)
		  acci[0].flStyleDefault    = WS_CHILD;
		  acci[0].flExtStyleDefault = 0;
		  acci[0].flCtrlTypeMask    = 0;
		  acci[0].cStyleFlags       = 0;
		  acci[0].aStyleFlags       = NULL;
		  acci[0].lpfnStyle         = ResizerCtlStyleDlg;
		  acci[0].lpfnSizeToText    = ResizerSizeToText;
		  acci[0].dwReserved1       = 0;
		  acci[0].dwReserved2       = 0;

	    lstrcpy(acci[0].szClass, g_szClassName);
	    lstrcpy(acci[0].szDesc, g_szCopyright);
    	acci[0].szTextDefault[0] = 0;

			}

    return 1; //the number of custom controls
    }

////////////////////////////////////////////////////////////////////////////
// ResizerCtlStyleDlg                                                     //
// Brings up a dialog when the resizer is edited.  It just                //
// says that there is nothing to edit.  ("Thank you please drive though") //
// Returns FALSE cause nothing gets changed                               //
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ResizerCtlStyleDlg (HWND hWndParent, LPCCSTYLE )
    {
		MessageBox(hWndParent, "There is nothing to edit.", "AWResizer", MB_OK);
		return FALSE;
    }


//this does nothing either
int CALLBACK ResizerSizeToText (DWORD , DWORD , HFONT , LPSTR )
{
  return -1;
}
