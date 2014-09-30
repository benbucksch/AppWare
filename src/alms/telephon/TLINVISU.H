/*************************************************************************
**	File Name:
**		TLINVISU.H			Copyright 1994 Novell Inc.
**
**	Description:
**		The Visual Instance class for the Telephony Services TLINE
**		Window Object.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#ifndef TLINE_VI_H
#define TLINE_VI_H

#include <op_wnd.h> 				//	Window Item protocol
#include <o_wnd.h>					//	Window Object API

#ifdef __cplusplus
extern "C" {
#endif

OpWndItemD * ALMAPI TLineCreate(OBJECTID oiItem, OBJECTID oiParent,
	RECT * rcItem, long status, long special);

#ifdef __cplusplus
}
#endif

#define TLINE_CLASSNAME 	"TLineClass"
#define TLINE_LB_CLASSNAME	"TLineLBClass"

//	Visual instance changed flags (for details field of AEvtInfo struct)

#define TLINE_REDLED 		0x00000001
#define TLINE_GREENLED		0x00000002
#define TLINE_CALLID 		0x00000004
#define TLINE_DEVICEID		0x00000008
#define TLINE_LINELETTER 	0x00000010

enum TLSIG {
	s_TLineActivated,
	s_TLineDeactivated,
	s_TLineRedLedON,
	s_TLineRedLedOFF,
	s_TLineGreenLedON,
	s_TLineGreenLedOFF,
};

//	Define the visual instance implementation class

class TLineVI : public OpHwndWndItem {
  private:
	~TLineVI(void) {};

  public:
	BOOL	fDropped;
	BOOL	fButtonPressed;
	int 	nListHeight;
	HWND	hwndList;
	HBITMAP hBmpLgts;
	HBITMAP hBmpBtns;

	TLineVI(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);

	void ALMAPI Destroy(void);			// MUST override Destroy!

	void ALMAPI DataChanged(long details, long modifiers, long variation);
	long ALMAPI Activate(int causedBy, BOOL mustKeep);
	void ALMAPI Deactivated(void);

	void Draw();

	void ALMAPI ParentOpened();
	void ALMAPI ParentClosing();
};


#endif	// !TLINE_VI_H
