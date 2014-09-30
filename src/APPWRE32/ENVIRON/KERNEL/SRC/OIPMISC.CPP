////////////////////////////////////////////////////////////////////////
// oipmisc.cpp -- miscellaneous OIP calls
//
//    oipGetLastInputInfo
//    AAppGetName
//    AAppGetPriorityMode
//    AEvtGetObjectChangedPriority
//    AAppConvertOSTime
//    AFuncReportResult
//    AAppSetName
//    AAppSetPriorityMode
//    AAppGetTime
//
//  jon 10-Jun-92 created
//  jon 16-Aug-92 added oipGetLast<Key,Mouse>Info
//  jon 17-Feb-93 added oip<Get,Set>AppName
////////////////////////////////////////////////////////////////////////

#include "almsys.h"

#include <stdlib.h>
#include "system.h"
#include "objinst.h"
#include "context.h"
#include "oiputils.h"
#include "sericon.h"
#include "objdesc.h"
#include "objtype.h"
#include "a_almutl.h"
#include "regdef.h"

extern HINSTANCE	hDLL;

//----------------------------------------------------------------------
//   AAppSave
//
// Returns TRUE if successfully saved, otherWise false is returned.
// If we are running in the environment we will return true even though
// persistants are never save when running in the environment.
//----------------------------------------------------------------------
extern "C"
BOOL ALMAPI	AAppSave()
	{
	LPAPPCONTEXT  currContext = pTheSystem->GetCurrentContext();

	if (currContext->IsMicroApp())
		return currContext->SavePersistents(FALSE); //FALSE means don't
	else                                          //display errors.
		return TRUE;

	}

//----------------------------------------------------------------------
//   AAppGetPriorityMode
//
// Returns the current priority mode for this app (function vs visual).
//----------------------------------------------------------------------
extern "C"
LONG ALMAPI	AAppGetPriorityMode()
  {
  return pTheSystem->GetCurrentContext()->GetAppPriorityMode();
  }


//----------------------------------------------------------------------
//   AAppSetPriorityMode
//
// Sets the current priority mode for this app (function vs visual).
// Returns the previous mode, or zero on error.
//----------------------------------------------------------------------
extern "C"
LONG ALMAPI	AAppSetPriorityMode(
  LONG theMode)
  {
  return pTheSystem->GetCurrentContext()->SetAppPriorityMode(theMode);
  }

//----------------------------------------------------------------------
//   AEvtGetObjectChangedPriority
//
// Returns the appropriate mangling of the 'preferred' how bits based
// on this application's current priority mode (visual vs. function).
//----------------------------------------------------------------------
extern "C"
LONG ALMAPI	AEvtGetObjectChangedPriority(
  LONG preferred)
	{
  LONG       howNow;
  LONG       out;

	pTheSystem->GetCurrentContext();

	howNow = AAppGetPriorityMode();

	if (howNow == AAPP_VISUALPRIORITY)
		out = (preferred & ~AEVT_ATTAIL) | AEVT_ATMARK;
	else if (howNow == AAPP_FUNCTIONPRIORITY)
		out = (preferred & ~AEVT_ATMARK) | AEVT_ATTAIL;
	else
		out = preferred;

	return out;
  }

//----------------------------------------------------------------------
//   AAppConvertOSTime
//
// convert a Windows timestamp to a serius timestamp.
//----------------------------------------------------------------------
extern "C"
LONG ALMAPI	AAppConvertOSTime(
  LONG osTime)
  {
  return (osTime * 60) / 1000;
  }

//----------------------------------------------------------------------
//   AAppGetTime
//
// Returns time since boot, in 60ths of a second.
//   ----------
// Note: for efficiency, we could return winTicks >> 4 (equiv to
// winTicks * 64 / 1024)
//----------------------------------------------------------------------
extern "C"
LONG ALMAPI	AAppGetTime()
	{
	return (GetTickCount() * 60) / 1000;
	}

//----------------------------------------------------------------------
//   AFuncReportResult
//
// Sets the specified flowIndex/errMessage for the current function.
//----------------------------------------------------------------------
extern "C"
BOOL ALMAPI	AFuncReportResult(
	pAObjMessage theSystem,
	LONG      flowIndex,
	LPCSTR    errMessage)
	{
	if (flowIndex)
		theSystem->message1 = flowIndex;
	if (errMessage)
		lstrcpyn(theSystem->errorMessage, errMessage, sizeof(theSystem->errorMessage));
	return TRUE;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AObjReportError
//
// DESC:			Reports a warning or an error to the user.  This should
//						be called during processing of a request message.  It brings
//						up a message box of some standard format.  If it is a warning,
//						then OK & Cancel are choices, otherwise just OK.
//
////////////////////////////////////////////////////////////////

extern "C"
LONG ALMAPI	AObjReportError(
	AOBJECTID			oid,									// object with the error
	LONG					lMessage,							// error occurred during this message
	LONG					lStatus,							// A_WARNING, A_ERROR
	LPCSTR				pszMessage,						// warning or error message
	LONG					lInfo									// extra info (per-message defined)
	)
	{
	BOOL					fCancelButton		= FALSE;
	int						iDlgRes;
	LPOBJINST			pObject;
	char					szMsg[256];
	char					szCap[256];
	char					szObjName[64];
	WORD					wStyle;
	LPSTR					pszMsg;
	BOOL					setHighWord = FALSE;
	WORD					hiWord;

  pTheSystem->GetCurrentContext();

	// Get name of object as best we can
	if (oid == 0)															// use generic message
		lstrcpy(szObjName, "an object");
	else if (pTheSystem->IsExeStructures())		// use object id
		wsprintf(szObjName, "object %ld", oid);
	else																			// use object name
		{
		pObject = (LPOBJINST) GetObjectRecord(oid);
		wsprintf(szObjName, "object \"%s\"", pObject->GetName());
		}

	switch (lMessage)
		{
		case AOBJ_EDITOBJECT:
			lstrcpy(szCap, "Warning");
			pszMsg = (LPSTR) pszMessage;
			break;

		case AOBJ_CHECKOBJECT:

			setHighWord = TRUE;
			if (lStatus == A_WARNING)
				{
				wsprintf(szCap, "Compile warning from %s", szObjName);
				fCancelButton = TRUE;
				lstrcpyn(szMsg, pszMessage, sizeof(szMsg) - 16);
				szMsg[sizeof(szMsg) - 16] = '\0';
				lstrcat(szMsg, "\r\nContinue?");
				pszMsg = szMsg;
				}
			else
				{
				wsprintf(szCap, "Critical compile error from %s", szObjName);
				pszMsg = (LPSTR) pszMessage;
				}
			break;

		case AOBJ_READ:
			{
			LPSTR					pszLocMsg		= new char[lstrlen(pszMessage) + 1];
			LPSTR					pszDllName;

			lstrcpy(pszLocMsg, pszMessage);
			pszDllName = strpbrk(pszLocMsg, "\t");

			if (pszDllName)
				{
				*pszDllName = '\0';
				pszDllName++;
				}
			else
				pszDllName = "<unknown>";

			if (lStatus == A_WARNING)
				{
				lstrcpy(szCap, "Warning");
				wsprintf(szMsg, "The %s %s is incompatible with\r"
												 "DLL %s "
												 "version %ld.\r"
												 "Continuing may cause loss of data.\r\r"
												 "Continue?",
												 pszMessage,
												 szObjName,
												 pszDllName,
												 lInfo);

				fCancelButton = TRUE;
				}

			else if (lStatus == A_ERROR)
				{
				lstrcpy(szCap, "Fatal Error");
				wsprintf(szMsg, "The %s %s is incompatible with\r"
												 "DLL %s "
												 "version %ld.\r"
												 "The application cannot be executed.",
												 pszMessage,
												 szObjName,
												 pszDllName,
												 lInfo);
				}
			pszMsg = szMsg;

			delete [] pszLocMsg;
			break;
			}

		case AOBJ_IMPORTOBJECT:
			wsprintf(szCap, "Error while importing %s", szObjName);
			pszMsg = (LPSTR) pszMessage;
			break;

		case AOBJ_EXPORTOBJECT:
			wsprintf(szCap, "Error while exporting %s", szObjName);
			pszMsg = (LPSTR) pszMessage;
			break;
		}

	if (fCancelButton)
		wStyle = MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL;
	else
		{
		wStyle = MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL;
		
		}

	iDlgRes = MessageBox(NULL, pszMsg, szCap, wStyle);

	
	
	// If an option of Continue was given (i.e., both OK and Cancel were possible),
	// change the return value back to A_OK if OK was chosen.
	if(setHighWord)
		{
		if(iDlgRes == IDOK)
			hiWord = AHIGH_CONTINUE;
		else if(iDlgRes == IDCANCEL)
			hiWord = AHIGH_CANCEL;
		else
			hiWord = 0;
			 
		lStatus = MAKELONG(LOWORD(lStatus), hiWord); 	
		}
	else
		{
		if (fCancelButton && iDlgRes == IDOK)
			lStatus = A_OK;
		}

	return lStatus;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ATypeGetIcon
//
// DESC:			Returns a windows handle for the types icon.
//
////////////////////////////////////////////////////////////////
extern "C"
HICON ALMAPI ATypeGetIcon(
  ATYPEID  theType)
  {
  LPBASETYPE    pType;
  LPSERIUSICON  pIcons;
  HDC           hdc;
  HICON         hIcon = 0;
  HBITMAP       hImageBitmap;
  HBITMAP       hMaskBitmap;
  void  *       pImageBits;
  void  *       pMaskBits;
  long          maskBufferSize;
  long          imageBufferSize;
  BITMAP        bitmap;

  if (pTheSystem->IsExeStructures())
    {
    }
  else if (0 != (pType = GetTypeRecord(theType)))
    {
    hdc = GetDC(0);
    pIcons = ((LPOBJTYPE)pType)->GetObjDesc()->GetIcons();
    if (pTheSystem->GetNumColors() > 2)
      {//color icon
      hImageBitmap = CreateCompatibleBitmap(hdc,
                        (int)pIcons->colorIcon.imageHdr.biWidth,
                        (int)pIcons->colorIcon.imageHdr.biHeight);
      if(hImageBitmap)
        {
        SetDIBits(hdc, hImageBitmap,
          0, (UINT)pIcons->colorIcon.imageHdr.biHeight,
          (LPSTR)pIcons->colorIcon.imageArray,
          (LPBITMAPINFO)&pIcons->colorIcon.imageHdr, DIB_RGB_COLORS);
        }

      hMaskBitmap = CreateBitmap((int)pIcons->colorIcon.maskHdr.biWidth,
                        (int)pIcons->colorIcon.maskHdr.biHeight,
                        1, 1, NULL);
      if(hMaskBitmap)
        {
        SetDIBits(hdc, hMaskBitmap,
          0, (UINT)pIcons->colorIcon.maskHdr.biHeight,
          (LPSTR)pIcons->colorIcon.maskArray,
          (LPBITMAPINFO)&pIcons->colorIcon.maskHdr, DIB_RGB_COLORS);
        }

      if (hMaskBitmap && hImageBitmap)
        {
        GetObject(hMaskBitmap, sizeof(bitmap), &bitmap);
        maskBufferSize =
         bitmap.bmWidthBytes * bitmap.bmHeight * bitmap.bmPlanes;

        GetObject(hImageBitmap, sizeof(bitmap), &bitmap);
        imageBufferSize =
         bitmap.bmWidthBytes * bitmap.bmHeight * bitmap.bmPlanes;

        pMaskBits = (void*) GlobalAlloc(GPTR, maskBufferSize);
        pImageBits = (void*) GlobalAlloc(GPTR, imageBufferSize);

        if (NULL != pMaskBits)
          {
          if (NULL != pImageBits)
            {
            if (GetBitmapBits(hMaskBitmap, maskBufferSize, pMaskBits) &&
                GetBitmapBits(hImageBitmap, imageBufferSize, pImageBits))
              {
              hIcon = CreateIcon(hDLL,
               (int)bitmap.bmWidth,
               (int)bitmap.bmHeight,
               (BYTE)bitmap.bmPlanes,
               (BYTE)bitmap.bmBitsPixel,
               (CONST BYTE*) pMaskBits, (CONST BYTE*) pImageBits);
              }
            GlobalFree((HGLOBAL) pImageBits);
            }
          GlobalFree((HGLOBAL) pMaskBits);
          }
        }
      if (hImageBitmap)
        DeleteObject(hImageBitmap);
      if (hMaskBitmap)
        DeleteObject(hMaskBitmap);
      }
    else
      {//black and White icon

      hImageBitmap = CreateDIBitmap(hdc, &pIcons->bwIcon.imageHdr,
       CBM_INIT, &pIcons->bwIcon.imageArray,
       (LPBITMAPINFO)&pIcons->bwIcon.imageHdr,  DIB_RGB_COLORS);

      hMaskBitmap = CreateDIBitmap(hdc, &pIcons->bwIcon.maskHdr,
       CBM_INIT, &pIcons->bwIcon.maskArray,
       (LPBITMAPINFO)&pIcons->bwIcon.maskHdr,  DIB_RGB_COLORS);

      if (hMaskBitmap && hImageBitmap)
        {
        GetObject(hMaskBitmap, sizeof(bitmap), &bitmap);
        maskBufferSize =
         bitmap.bmWidthBytes * bitmap.bmHeight * bitmap.bmPlanes;

        GetObject(hImageBitmap, sizeof(bitmap), &bitmap);
        imageBufferSize =
         bitmap.bmWidthBytes * bitmap.bmHeight * bitmap.bmPlanes;

        pMaskBits = (void*) GlobalAlloc(GPTR, maskBufferSize);
        pImageBits = (void*) GlobalAlloc(GPTR, imageBufferSize);

        if (NULL != pMaskBits)
          {
          if (NULL != pImageBits)
            {
            if (GetBitmapBits(hMaskBitmap, maskBufferSize, pMaskBits) &&
                GetBitmapBits(hImageBitmap, imageBufferSize, pImageBits))
              {
              hIcon = CreateIcon(hDLL,
               (int)pIcons->bwIcon.imageHdr.biWidth,
               (int)pIcons->bwIcon.imageHdr.biHeight,
               (BYTE)pIcons->bwIcon.imageHdr.biPlanes,
               (BYTE)pIcons->bwIcon.imageHdr.biBitCount,
               (CONST BYTE*) pMaskBits, (CONST BYTE*) pImageBits);

              }
            GlobalFree((HGLOBAL) pImageBits);
            }
          GlobalFree((HGLOBAL) pMaskBits);
          }
        }
      if (hImageBitmap)
        DeleteObject(hImageBitmap);
      if (hMaskBitmap)
        DeleteObject(hMaskBitmap);
      }
    ReleaseDC(NULL, hdc);
    }

  return hIcon;

  }

//----------------------------------------------------------------------
//   AAppIsDesignTime
//
// Returns TRUE if we are in design time.
//----------------------------------------------------------------------
extern "C"
BOOL ALMAPI	AAppIsDesignTime(
  void)
  {

  return (!pTheSystem->GetCurrentContext()->IsRuntime());

  }


//----------------------------------------------------------------------
//   AAppGetExecutionState
//
//	returns the current state of the system.
//----------------------------------------------------------------------
extern "C"
WORD ALMAPI	AAppGetExecutionState(
  void)
  {
	WORD rVal;

	if(pTheSystem->GetCurrentContext()->IsMicroApp())
		rVal = AAPP_EXECUTABLETIME;
	else if(pTheSystem->GetCurrentContext()->IsRuntime())
		rVal = AAPP_PROJECTRUNTIME;	
  else
		rVal = AAPP_DESIGNTIME;

	return rVal;
  }

//----------------------------------------------------------------------
//   AAppGetCmdLine
//
// Copies the command line to the given buffer.
//----------------------------------------------------------------------
extern "C"
void	ALMAPI	AAppGetCmdLine(
	LPSTR					pszBuf,
	short					cbBuf							// size of buffer (including space for \0)
	)
	{
	ADBG_VALIDATE(pszBuf, return, "NULL buffer passed to AAppGetCmdLine");
	
	if (cbBuf > 0)
		{
		EXEINFOAUX	myAuxInfo;
		EXEINFO			myExeInfo(&myAuxInfo);
		LPAPPCONTEXT		context;

		context = pTheSystem->GetCurrentContext();
		context->GetCopyOfExeInfo(&myExeInfo);

		if(myAuxInfo.lpExeCmdLine)
			lstrcpyn(pszBuf, myAuxInfo.lpExeCmdLine, cbBuf - 1);
		else
    	pszBuf[0] = '\0';
      
		pszBuf[cbBuf - 1] = '\0';
		}
  else
  	{
    ADBG_PRINT_E("Character buffer too small in AAppGetCmdLine.");
    }
	}


//----------------------------------------------------------------------
//   AAppGetName
//
// stub code
//----------------------------------------------------------------------
extern "C"
BOOL  ALMAPI  AAppGetName(AMEMBLOCKID theBlock)
  {
	I_UNUSED(theBlock);

  return FALSE;

  }

//----------------------------------------------------------------------
//   AAppSetName
//
// stub code
//----------------------------------------------------------------------
extern "C"
BOOL  ALMAPI  AAppSetName(AMEMBLOCKID theBlock)
  {
	I_UNUSED(theBlock);
  return FALSE;
 	}


//----------------------------------------------------------------------
//   AAppShowAppBuilder
//
// 
//----------------------------------------------------------------------
extern "C"
BOOL  ALMAPI  AAppShowAppBuilder(BOOL show)
	{
  if(pTheSystem->GetCurrentContext()->GetShowVABProc())
  	return pTheSystem->GetCurrentContext()->GetShowVABProc()(show);
	else
		ADBG_PRINT_W("Invalid call to AAppShowAppBuilder.\r\nIt can only be called at design time.");
	
	return FALSE;
	}	

//////////////////////////////////////////////////////////////////////
// the registery structure should be the following:
//
// SYSTEMKEY:
// 	Software:
//    Novell:
//    	AppWare:
//      	SubKey:
//    	
// The key to the subkey is what is returned. 						

static char KEY_SOFTWARE[] = 		{"Software"};
static char KEY_NOVELL[] = 			{"Novell"};
static char KEY_PRODUCT[] = 		{REG_PRODUCT};

static char* KEY_SUBKEY[] = 		{REG_APPBUILDER,
                                 REG_ALMBUILDER,
                                 REG_ALMS,
                                 REG_DIRECTORIES,
                                 REG_APPSETTINGS,
                                 REG_FONTTRANS,
                                 REG_DEBUG};
                                 
static char KEY_MAXSUBKEYS = sizeof(KEY_SUBKEY) / sizeof(KEY_SUBKEY[0]);

extern "C"
HKEY ALMAPI AUtlOpenProductSubKey(HKEY sysKey, AUTL_SUBKEY_TYPE subKeyType)
	{
  LONG  result;
  HKEY softKey;
  HKEY novKey;
  HKEY prodKey;
  HKEY subKey = NULL;	 //subKey is the return value
  DWORD action;
   
  if(sysKey == HKEY_LOCAL_MACHINE || sysKey == HKEY_CURRENT_USER)
  	{
    if(subKeyType >= 0 || subKeyType < KEY_MAXSUBKEYS)
    	{
 	    //first open the software key
	    result = RegOpenKeyEx(sysKey, KEY_SOFTWARE, 0, KEY_ALL_ACCESS, &softKey);
	    	
	    if(result == ERROR_SUCCESS)
	    	{
	    	result = RegCreateKeyEx(softKey, KEY_NOVELL, 0, NULL,
	      	REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &novKey, &action);
	      
	      if(result == ERROR_SUCCESS)
	      	{		
	        if((result = RegCreateKeyEx(novKey, KEY_PRODUCT, 0, NULL,
	        	REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &prodKey, 
	        	&action)) == ERROR_SUCCESS)
	          {
            if(RegCreateKeyEx(prodKey, KEY_SUBKEY[subKeyType], 0, NULL,
            	REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &subKey, 
            	&action) != ERROR_SUCCESS)
              {
              I_ASSERT(FALSE);
              subKey = NULL; //just incase set to some invalid value
              }
            
            RegCloseKey(prodKey);
	          }
          else
          	{
            I_ASSERT(FALSE);
            }
	        
	        RegCloseKey(novKey);
	        }
	      else
	      	{
	        I_ASSERT(FALSE);
	        }
	        
	    	RegCloseKey(softKey);
	    	}
	   	else
	   		{
	   		I_ASSERT(FALSE);
	   		}
	   	}
	  else
	  	{
	  	ADBG_PRINT_E("Invalid sub key type passed in to AAppOpenProductSubKey.");
	  	} 
      
    }
  else
  	{
    ADBG_PRINT_E("Invalid system key passed in to AAppOpenProductSubKey.");
    }
  
  return subKey;
  }

//////////////////////////////////////////////////////////////////////
extern "C"
LONG				ALMAPI AUtlCloseProductSubKey(HKEY hKey)
	{
  I_ASSERT(hKey);
  
  if(hKey)
  	return RegCloseKey(hKey);
  else
  	return 0;
  }

///////////////////////////////////////////////////////////////////////
extern "C"
BOOL ALMAPI AUtlWriteKeyLong(
	HKEY subKey,
  LPCSTR name,
  long value)
  {
  char buffer[32];
  
  //write as a string to preserve the sign
  itoa(value, buffer, 10);
  
  return AUtlWriteKeyString(subKey, name, buffer);
  }

////////////////////////////////////////////////////////////////////
extern "C"
long ALMAPI AUtlGetKeyLong(
	HKEY subKey, 
	LPCSTR name, 
	long defValue)
	{
  long rVal = defValue;
  char buffer[32];
	char defBuf[32];
  
  //get as a string to preserve the sign
 	itoa(defValue, defBuf, 10);
  
 	if(AUtlGetKeyString(subKey, name, buffer, sizeof(buffer), defBuf))
 		{
 		rVal = atoi(buffer);
 		}
 		 
 	return rVal;
  }

///////////////////////////////////////////////////////////////////////
extern "C"
BOOL ALMAPI  AUtlWriteKeyString(
	HKEY subKey,
  LPCSTR name,
  LPCSTR data)
	{
	BOOL rVal = FALSE;
  if(subKey && (RegSetValueEx(subKey, name, 0, REG_SZ, (PBYTE)data, 
  	lstrlen(data) + 1) == ERROR_SUCCESS))
  	{
  	rVal = TRUE;
  	}
  	   
	return rVal;
	}	

///////////////////////////////////////////////////////////////////////
extern "C"
int ALMAPI AUtlGetKeyString(
	HKEY subKey,
  LPCSTR name,
  LPSTR buffer,
  WORD bufSize,
  LPCSTR defValue)
  {
  DWORD type;
  DWORD dataSize = bufSize;
  
  buffer[0] = '\0'; //initialize to NULL string
  
	// Const Bogusness:  lpszValueName parameter is not modified (according
	// to the helpfile), but it is not declared as LPCTSTR.
  if(subKey && (RegQueryValueEx(subKey, (LPSTR) name, NULL, &type, 
  	(LPBYTE)buffer, &dataSize)	== ERROR_SUCCESS))
    {
    if((type != REG_SZ))
    	{
      I_ASSERT(FALSE);
      lstrcpyn(buffer, defValue, bufSize-1);
      }
    }
  else
  	lstrcpyn(buffer, defValue, bufSize-1);
  
  buffer[bufSize-1] = '\0';
  
  return lstrlen(buffer);
  }
