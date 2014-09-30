/*************************************************************************
**	File Name:
**		ALM_CSTA.H			Copyright 1994 Novell Inc.
**
**	Description:
**		The main header file for the Telephony Services ALM Objects.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#ifndef ALM_CSTA_H
#define ALM_CSTA_H

// Object types

#define OTYPE_LIST MAKETYPEID('L','i','s','t')	//	define the LIST type ID
#define OTYPE_TDEV MAKETYPEID('t','d','e','v')  //  define the TDEV type ID
#define OTYPE_TLIN MAKETYPEID('t','l','i','n')  //  define the TLIN type ID

// IDs for the TDEV definition (edit) dialog boxes

#define IDC_OBJECT			120
#define IDC_SERVERID		121
#define IDC_LOGINID 		122
#define IDC_PASSWORD		123
#define IDC_MONDEV			124
#define IDC_SENDQSIZE		125
#define IDC_RECVQSIZE		126
#define IDC_DEVICEID		127
#define IDC_AVAILLIST		128
#define IDC_OWNEDLIST		129
#define IDC_ADDLINES		130
#define IDC_REMOVELINES 	131
#define IDC_COMMENT 		132
#define IDC_TCXNLIST		133

#define IDC_PBXEXTENSION	220
#define IDC_LINECHAR		221
#define IDC_GREENLED		222
#define IDC_REDLED			223
#define IDC_FONTS			224
#define IDC_TITLE			225

//	CSTA Event handler Window Message

#define WM_ACSEVENT 		(WM_USER + 10)

//	CSTA Event Signal Definitions

#define TDEV_EVT_STARTUP			0
#define TDEV_EVT_CALLCLEAR			1
#define TDEV_EVT_CONFERENCED		2
#define TDEV_EVT_CXTNCLEAR			3
#define TDEV_EVT_ALERTING			4
#define TDEV_EVT_DIVERTED			5
#define TDEV_EVT_ESTABLISHED		6
#define TDEV_EVT_FAILED 			7
#define TDEV_EVT_HELD				8
#define TDEV_EVT_NETREACHED 		9
#define TDEV_EVT_ORIGINATED 		10
#define TDEV_EVT_QUEUED 			11
#define TDEV_EVT_RETRIEVED			12
#define TDEV_EVT_SERVICEINIT		13
#define TDEV_EVT_TRANSFERRED		14
#define TDEV_EVT_DND_ON 			15
#define TDEV_EVT_DND_OFF			16
#define TDEV_EVT_FWD_IMM			17
#define TDEV_EVT_FWD_BSY			18
#define TDEV_EVT_FWD_NAN			19
#define TDEV_EVT_FWD_BIN			20
#define TDEV_EVT_FWD_BEX			21
#define TDEV_EVT_FWD_NAI			22
#define TDEV_EVT_FWD_NAE			23
#define TDEV_EVT_FWD_OFF			24
#define TDEV_EVT_MSGWAITING_ON		25
#define TDEV_EVT_MSGWAITING_OFF 	26

//	Other definitions

#define ACS_LEVEL			0
#define APP_NAME			"AppWare"
#define CURRENT_APIVER		"V1.6   "
#define ACS_MAX_VERSION 	21
#define INVOKE_ID			0
#define PD_NONE 			((PrivateData_t  *)0L)
#define ESN_BLOCKSIZE		128

#define TDEVC_MAXLINES		50
#define TLINE_MAXCXTNS		50
#define TLINE_MAXCMNT		128
#define TCXTN_MAXCMNT		128
#define TCXTN_MAXCS 		(15+TCXN_MAXCMNT+sizeof(DID))
#define TDEVC_MAINBLK		0
#define TLINE_MAINBLK		0

#define TDEVC_FIRSTLINE		-1L
#define TLINE_FIRSTCXTN		-1L

#define CSTA_EVENTCLASSNAME	"CSTAEventClass"

//	External (GLOBAL) variables

extern HINSTANCE hInstance;

//	Externed un-mangled function prototypes

#ifdef __cplusplus
extern "C" {
#endif

LONG CALLBACK TLineWndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
LONG CALLBACK TLineLBWndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
LRESULT CALLBACK CSTAEventHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

//	Type definitions

typedef ConnectionID_t			CID;	//	short version of connection ID
typedef DeviceID_t				DID;	//	short version of device ID
typedef LocalConnectionState_t	LCS;	//	short version of connection state
typedef ACSHandle_t 			ACS;	//	short version of ACS Handle

typedef struct TDEVICE {				//	Telephony Device Object's data block
	ACS 		acsHandle;				//	Stream Handle
	LONG		lNumLines;				//	Number of Lines in this App
	OBJECTID	oiSendQSize;			//	Send Queue Size
	OBJECTID	oiRecvQSize;			//	Receive Queue Size
	OBJECTID	oiServerID; 			//	Telephony Server Name
	OBJECTID	oiLoginID;				//	Telephony User Name
	OBJECTID	oiPassword; 			//	Telephony User Password
	OBJECTID	oiMonDev;				//	Monitor Device (& default local)
	OBJECTID	oiLines[TDEVC_MAXLINES];//	Array of line objects
} TDEVICE,  * LPTDEVICE;

typedef struct TLINE {
	OBJECTID	oiPhone;				//	PBX Extension Object
    OBJECTID	oiOwner;				//	Owning TDevice Object
	char		szLineChar[2];			//	line character
	LONG		lCallID;				//	CID number
	LCS 		lConnState; 			//	local connection state
	LONG		lNumCxtns;				//	number of connection on line
	BOOL		fRedLEDActv;			//	Use Red LED display?
	BOOL		fGreenLEDActv;			//	Use Green LED display?
	BOOL		fActiveLine;			//	Active Line flag
	BOOL		fCallInEffect;			//	Call in effect flag
	MEMBLOCKID	mTCxtns[TLINE_MAXCXTNS];//	Array of connections
} TLINE,  * LPTLINE;

typedef struct TCXTN {					//	Connection data block
	OBJECTID	oiOwningLine;			//	Line back link
	DID 		didDeviceID;			//	Remote Cxtn phone number (DID)
	LCS 		lConnState; 			//	Local Connection state
	char		szComment[TCXTN_MAXCMNT];// Optional connection comment
} TCXTN,  * LPTCXTN;

#endif // !ALM_CSTA_H
