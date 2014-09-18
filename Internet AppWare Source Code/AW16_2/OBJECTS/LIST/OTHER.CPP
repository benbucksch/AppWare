#include <windows.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <op_wnd.h>
#include "attribut.h"
#include "dialogs.h"
#include "list.h"

// from WIN_UTL.CPP
void PostObjectChanged(OBJECTID oiList, LONG modifiers)
	{
	AEvtInfo   event;

	event.sourceObject = oiList;
	event.targetObject = oiList;
	event.when         = AAppGetTime();
	event.how          = AEVT_KILLDUPLICATES | AEVT_KILLMODIFIERS ;
	event.what         = AEVENT_OBJECTCHANGED;
	event.details      = NULL;
	event.modifiers    = modifiers;

	AEvtPost(&event);
	return;
	}

// from WIN_UTL.CPP
BOOL PtInWindow(
  HWND    hWnd,
  POINT   pt) //pt is in screen coords.
  {
  RECT    rect;

  GetWindowRect(hWnd, &rect);
  return PtInRect(&rect, pt);
  }

// from WIN_UTL.CPP
void RemapWindowPoints(
  HWND        hWndFrom,
	HWND        hWndTo,
  LPPOINT     points,
  int         numPoints)
  {
  int  i;

  for(i=0; i<numPoints; i++, points++)
	{
	ClientToScreen(hWndFrom, points);
	ScreenToClient(hWndTo, points);
	}
  }


// from WIN_FUN.cpp
BOOL GetValidData(
	LPVOID far			*ptr,
	OBJECTID				theObject,
	LONG						dataIdx,
	LPSTR						str,
	pAObjMessage				theSystem)
	{
	*ptr = AObjLockData(theObject, dataIdx);
	if (!*ptr)
		{
		lstrcpy(theSystem->errorMessage, str);
		return FALSE;
		}
	return TRUE;
	}

// from WIN_FUN.cpp
BOOL GetValidParameter(
	LPOBJECTID			idPtr,
	LONG						parameterIdx,
	TYPEID					theType,
	LPSTR						str,
	pAObjMessage				theSystem)
	{
	*idPtr = AFuncGetTypedParameter(parameterIdx, theType);
	if (!*idPtr)
		{
		if (str) lstrcpy(theSystem->errorMessage, str);
		return FALSE;
		}
	return TRUE;
	}

// from TXT_FUN.cpp
LPSTR GetFieldOrLine(LPSTR str, LPLONG  startPos,  //This is a one based index!!!
		BOOL    justLines) //Defaults to FALSE
	{
	LPSTR retPos = _fstrchr(&str[(int)(*startPos)-1], 0x0d);
	LPSTR tabPos = justLines ? NULL : _fstrchr(&str[(int)(*startPos)-1], 0x09);
	LPSTR pos;
	int len;
	LPSTR out = NULL;

  // boundary problems.
	if (*startPos <= 0) // if the starting position is less than 1
		return NULL;
	if (!str || !*str) // if the pointer is NULL or the first character is 0
		return NULL;

	if (justLines)
		{
		while(str[(int)(*startPos)-1] == '\r' ||
				str[(int)(*startPos)-1] == '\n')
			{ //skip over blank lines
			(*startPos)++;
			}
		}
	else
		{
		while(str[(int)(*startPos)-1] == '\r' ||
				str[(int)(*startPos)-1] == '\n' ||
				str[(int)(*startPos)-1] == '\t')
			{ //skip over blank lines and tabs
			(*startPos)++;
			}
		}
	retPos = _fstrchr(&str[(int)(*startPos)-1], 0x0d);
	tabPos = justLines ? NULL : _fstrchr(&str[(int)(*startPos)-1], 0x09);

	if (tabPos && retPos)
		{
		if (tabPos < retPos)
			pos = tabPos ;
		else
			pos = retPos ;
//	pos = min(tabPos, retPos);
		}
	else if (tabPos)
		pos = tabPos;
	else if (retPos)
		pos = retPos;
	else
		pos = &str[(int)(*startPos)-1] + lstrlen(&str[(int)(*startPos)-1]);

	len = (int)(pos - &str[(int)(*startPos)-1]);
	if (!len)
		return NULL;

	out = (LPSTR) farmalloc(len+1);

	_fstrncpy(out, &str[(int)(*startPos)-1], len);      // We have got a field.
	out[len] = 0x0;        // terminate the string.
	*startPos += len;
	if (pos == tabPos)
		*startPos += 1;
	else if (pos == retPos)
		*startPos += 2;

	return out;
	}
/* from win_info.cpp
long WITEM_INFO::SendMsg()
	{
	if (!messageDispatched && msg.message)
		{
		messageDispatched = TRUE;
		return DispatchMessage(&msg);
		}
	return FALSE;
	}
*/
// from WIN_FUN.cpp
BOOL CreateValidOutput(
	LPOBJECTID			idPtr,
	TYPEID					theType,
	LPSTR						str,
	pAObjMessage				theSystem)
	{
	*idPtr = AObjCreate(theType);
	if (!*idPtr)
		{
		if (str) lstrcpy(theSystem->errorMessage, str);
		return FALSE;
		}
	return TRUE;
	}

// from WIN_FUN.cpp
BOOL CreateValidData(
	LPVOID far			*ptr,
	OBJECTID				theObject,
	LONG						dataIdx,
	LONG						dataSize,
	LPSTR						str,
	pAObjMessage				theSystem)
	{
	AObjResizeData(theObject, dataIdx, dataSize);
	*ptr = AObjLockData(theObject, dataIdx);
	if (!*ptr)
		{
		lstrcpy(theSystem->errorMessage, str);
		AObjUnlockData(theObject, dataIdx);
		return FALSE;
		}
	AObjUnlockData(theObject, dataIdx);
	return TRUE;
	}

