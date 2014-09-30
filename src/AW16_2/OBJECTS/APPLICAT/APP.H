
//======================================================================
// app.h
//
// jon 15-Aug-92 [1.0] created.
//======================================================================

#ifndef APP_H
#define APP_H


#define OTYPE_APPLICATION								MAKETYPEID('S','y','s','m')



// ----- VERSION # ----------------

#define DLLVersion  1
#define DATAVersion 2   // changed on May 3, 1993 for signal Ids correction

#define CORRECTEDSIGNALVERSION 2

// OLD SIGNALS

	#define oldstartupSignalID      0  //   1
  #define oldsuspendedSignalID    2  //   2
  #define oldresumedSignalID      3  //   3
  #define oldmouseDownSignalID    4  //   4
	#define oldmouseUpSignalID      5  //   5
  // different from new signals
  #define olddoubleClickSignalID  6  //   6
  #define oldkeyDownSignalID      7  //   7
  #define oldkeyUpSignalID        8  //   8
  #define olddiskInSignalID       9  //   9
																	// ---
	#define oldnumSignalIDs                 9

// -- S I G N A L S --------------------------------------------------

// changed on May 31, 1993
	#define startupSignalID      0  //   1
  #define suspendedSignalID    2  //   2
  #define resumedSignalID      3  //   3
  #define mouseDownSignalID    4  //   4
	#define mouseUpSignalID      5  //   5
	#define keyDownSignalID      6  //   6
  #define keyUpSignalID        7  //   7
  #define diskInSignalID       8  //   8

  #define doubleClickSignalID  9  //   9
 																	// ---
	#define numSignalIDs                 9


// -- Temporary Type Datas

typedef enum {STARTUP, AFTER_STARTUP} DATA_0_TYPE;

typedef DATA_0_TYPE DATA0;

struct DATA {
	long version;
  };


////////////////////////////////////////////////////////////////


// Defines for fStatus

typedef struct AppRuntimeInfo_
	{
	WORD			fStatus;
	WORD			mouseInfo;
	WORD			lastMouse;
	HHOOK			hMouseHook;
	HHOOK			hKeyboardHook;
	}	AppRuntimeInfo, FAR* pAppRuntimeInfo;

// The mouseInfo field contains bits for each button, where if the bit
// is on, then the corresponding mouse button is down.  The following
// #define's give the bits.  DO NOT CHANGE THESE unless you also change
// the hack in appevt.cpp that assumes an ordering!!!

#define APP_LEFTBUTTON			(1 << 2)
#define APP_RIGHTBUTTON			(1 << 1)
#define APP_MIDDLEBUTTON		(1 << 0)
#define APP_CANCELMODE			(1 << 3) // SKJ
#endif	// !APP_H
