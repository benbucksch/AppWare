#include <windows.h>
#include <custcntl.h>
#include "resizer.h"

////////////////////////////////////////////////////////////////////////////////
// ResizerInfo                                                                //
// Provides basic information about the control to the caller (Dialog Editor) //
// The information is contained in a global memory object of type             //
// CTLINFO.  This memory object is freed by the caller. The memory must       //
// be allocated with GMEM_DDESHARE!                                           //
////////////////////////////////////////////////////////////////////////////////
extern "C" _export HANDLE FAR PASCAL ResizerInfo(void)
    {
    HANDLE      					hMem;
    LPCTLINFO   					pCtlInfo;

    if ( (hMem = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, sizeof(CTLINFO))) != NULL)
			{
	    if ( (pCtlInfo = (LPCTLINFO)GlobalLock(hMem)) != NULL)
				{
		    //Set the overall control info.
		    pCtlInfo->wVersion = 0100;
		    pCtlInfo->wCtlTypes = 1;
		    lstrcpy(pCtlInfo->szClass, g_szClassName);
		    lstrcpy(pCtlInfo->szTitle, g_szCopyright);

		    //Set the types--there is only one in this control type.
		    pCtlInfo->Type[0].wType   = 0;
		    pCtlInfo->Type[0].wWidth  = 20;
		    pCtlInfo->Type[0].wHeight = 20;
		    pCtlInfo->Type[0].dwStyle = WS_CHILD;
		    lstrcpy(pCtlInfo->Type[0].szDescr, g_szClassName);

		    //Give the memory to the Dialog Editor.
		    GlobalUnlock(hMem);
				}
			else //pCtlInfo==NULL
				{
        GlobalFree(hMem);
				hMem = NULL;
				}
			}

    return hMem;
    }

////////////////////////////////////////////////////////////////////////////
// ResizerCtlStyleDlg                                                     //
// Brings up a dialog when the resizer is edited.  It just                //
// says that there is nothing to edit.  ("Thank you please drive though") //
// Returns FALSE cause nothing gets changed                               //
////////////////////////////////////////////////////////////////////////////
extern "C" _export BOOL FAR PASCAL ResizerCtlStyleDlg(HWND , HANDLE , LPFNSTRTOID , LPFNIDTOSTR )
    {
		MessageBox(GetActiveWindow(), "There is nothing to edit.", "AWResizer", MB_OK);
		return FALSE;
    }


////////////////////////////////////////////////////////////////////////////
// WTranslateCtlFlags                                                     //
// Translates the class style flags into a text string to be used in the  //
// output of the Dialog Editor in a CONTROL statement of a dialog script. //
// But, there are none, so it does nothing.                               //
// Return the number of character put into the buffer -- 0                //
////////////////////////////////////////////////////////////////////////////
extern "C" _export WORD FAR PASCAL ResizerTranslateCtlFlags(WORD , LPSTR , WORD )
    {
		return 0;
    }




//////////////////////////////////////////////////////////////////////
// ResizerStyleDlgProc                                              //
// This is a dummy dialog proc.  It should never be called, but is  //
// necessary in the def file.                                       //
//////////////////////////////////////////////////////////////////////
extern "C" _export BOOL FAR PASCAL ResizerStyleDlgProc(HWND , WORD , WORD , LONG )
    {
		return 0;
		}
