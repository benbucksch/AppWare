#ifndef _DIALOGSHEADER_
#define _DIALOGSHEADER_

#include "stdio.h"
#include "stdlib.h"

#define RADIO_SETUP_DIALOG		108

#ifndef IDHELP

  #define IDHELP 998   // Thanks bulldog!

#endif // IDHELP


#define ST_OBJECTNAME				1001  //The name of the object type being edited
#define LB_ITEMS	          1113
#define	LB_OBJECTS          1114
#define PB_ADD              1116
#define PB_ATTACH           1117
#define PB_REMOVE           1118
#define PB_ENCLOSE          1131
#define PB_OK               1
#define PB_LAYOUT           1133
#define PB_CANCEL           2
#define PB_HELP							IDHELP
#define PB_EDIT							1136


inline void SetDialogPtr1(HWND hWnd, void  *ptr)
	{
	SetProp(hWnd, "HI1", (HANDLE)HIWORD( (LONG)ptr) );
	SetProp(hWnd, "LO1", (HANDLE)LOWORD( (LONG)ptr) );
	}

inline void  *GetDialogPtr1(HWND hWnd)
	{
	void  *ptr = (void  *) MAKELONG(
																GetProp(hWnd, "LO1"),
																GetProp(hWnd, "HI1"));
	return ptr;
	}

inline void RemoveDialogPtr1(HWND hWnd)
	{
	RemoveProp(hWnd, "HI1");
	RemoveProp(hWnd, "LO1");
	}

///
#endif

