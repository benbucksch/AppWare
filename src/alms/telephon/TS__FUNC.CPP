/*************************************************************************
**	File Name:
**		TS__FUNC.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**		The AppWare Telephony Module's ALM functions.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "o_tdev.h"
#include "o_tlin.h"

//	AppWare AppBuilder Routines

#ifdef __cplusplus
extern "C" {
#endif

void ALMCALLBACK EnumServerNames(pAEvtInfo theEvent, pAObjMessage theSystem);

void ALMCALLBACK OpenStream(pAEvtInfo theEvent, pAObjMessage theSystem);

void ALMCALLBACK AlternateCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK AnswerCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK Camp(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK CallBack(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK Intrude(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK ClearCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK ClearConnection(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK ConferenceCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK ConsultationCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK DeflectCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK HoldCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK MakeCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK ReconnectCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK RetrieveCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK TransferCall(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK SetMsgWaitingInd(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK SetDoNotDisturb(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK SetForwarding(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK QueryMsgWaitingInd(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK QueryDoNotDisturb(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK QueryForwarding(pAEvtInfo theEvent, pAObjMessage theSystem);

void ALMCALLBACK CloseStream(pAEvtInfo theEvent, pAObjMessage theSystem);

Boolean CALLBACK EnumerationFunction(char *serverName, unsigned long lParam);

#ifdef __cplusplus
}
#endif

//	Module (STATIC) routines

static RetCode_t WaitForReply(ACSHandle_t, CSTAEvent_t *, unsigned short *, EventClass_t);
static LONG CallCompletion(Feature_t);
static LONG ReadDeviceContext(OBJECTID);
static LONG AddCallToDevice(OBJECTID, ConnectionID_t *);

/*************************************************************************
**	Function:
**		EnumServerNames
**
**	Description:
**		This function enumerates all currently sapping Telephony servers
**		and places them in a list object passed as the only parameter.
**
**	 Input 1: AppWare List Object.
**
**	 Outputs: NONE.
**
**	  Flow 1: After Enumeration
**	  Flow 2: Object Error
**	  Flow 3: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK EnumServerNames(pAEvtInfo theEvent, pAObjMessage theSystem) {
	EnumServerNamesCB	esnProc;
	OBJECTID			oiServerNameList;
	MEMBLOCKID			memBlock;
	char  *			esnBuffer;
	RetCode_t			rc;

	theSystem->message1 = 2;				//	Default to (2) object error

//	Get parameter from the AppWare bus
	if (!(oiServerNameList = AFuncGetTypedParameter(1, OTYPE_LIST)))
		return;

//	Allocate memory for server names, lock, initialize, and unlock
	if (!(memBlock = AMemAllocate(ESN_BLOCKSIZE)))
		return;
	if (!(esnBuffer = (char  *)AMemLock(memBlock)))
		return;
	*esnBuffer = 0;
	AMemUnlock(memBlock);

//	Call TSAPI for enum server names in CSTA library
	esnProc = (EnumServerNamesCB)MakeProcInstance((FARPROC)EnumerationFunction, hInstance);
	rc = acsEnumServerNames(ST_CSTA, esnProc, (unsigned long)memBlock);
	FreeProcInstance((FARPROC)esnProc);
	if (rc != ACSPOSITIVE_ACK) {
		theSystem->message1 = 3;			//	TSAPI (3) error
		return;
	}

//	Convert from text memory block to AppWare List object, then free block
	if (!AObjConvertFromText(oiServerNameList, memBlock))
		return;
	if (!AMemFree(memBlock))
		return;
	theSystem->message1 = 1;				//	OK! (1) 
	return;
}

/*************************************************************************
**	Function:
**		OpenStream
**
**	Description:
**		Opens an ACS Stream using the CSTA object attached to obtain the
**		Server ID, Login ID, and Password.	Then stores the connection
**		handle for the newly opened stream in this CSTA object.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: NONE.
**
**	  Flow 1: After Opening
**	  Flow 2: Object Error
**	  Flow 3: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK OpenStream(pAEvtInfo theEvent, pAObjMessage theSystem) {
	OBJECTID			oiTDevice;
	OBJECTID			oiPhone;
	CSTAEvent_t 		eventBuf;
	unsigned short		eventBufSize = sizeof(CSTAEvent_t);
	ACSHandle_t 		acsHandle;
	unsigned short		sendQSize;
	unsigned short		recvQSize;
	ServerID_t			szServerID;
	LoginID_t			szLoginID;
	Passwd_t			szPassword;
	AppName_t			szAppName;
	Version_t			szCurAPIVer;
	DeviceID_t			szExtension;
	CSTAMonitorFilter_t filter;
	RetCode_t			rc;

	theSystem->message1 = 2;				//	Default to (2) object error

//	Get parameters from the AppWare Bus
	if (!(oiTDevice = AFuncGetTypedParameter(1, OTYPE_TDEV)))
		return;

//	Get relevant TSAPI info from the TDevice and Text Objects
	sendQSize = OTDevGetSendQSize(oiTDevice);
	recvQSize = OTDevGetRecvQSize(oiTDevice);
	OTDevGetServerID(oiTDevice, szServerID, sizeof(szServerID));
	OTDevGetLoginID(oiTDevice, szLoginID, sizeof(szLoginID));
	OTDevGetPassword(oiTDevice, szPassword, sizeof(szPassword));
	OTDevGetMonitorDevice(oiTDevice, szExtension, sizeof(DeviceID_t));
	lstrcpy(szAppName, APP_NAME);
	lstrcpy(szCurAPIVer, CURRENT_APIVER);

	theSystem->message1 = 3;			//	Default to (3) TSAPI error

//	Make TSAPI acsOpenStream function call to CSTA library
	rc = acsOpenStream(&acsHandle, LIB_GEN_ID, INVOKE_ID, ST_CSTA,
		&szServerID, &szLoginID, &szPassword, &szAppName, (Level_t)0,
		&szCurAPIVer, sendQSize, 0, recvQSize, 0, PD_NONE);
	if (rc < 1)
		return;
	rc = WaitForReply(acsHandle, &eventBuf, &eventBufSize, ACSCONFIRMATION);
	if (rc != ACSPOSITIVE_ACK)
		return;
	if (eventBuf.eventHeader.eventType != ACS_OPEN_STREAM_CONF)
		return;

	theSystem->message1 = 2;				//	Default to (2) object error

//	Set the stream handle in the TDev Object
	if (!OTDevSetStreamHandle(oiTDevice, acsHandle))
		return;

//	Create the event handler window (calls acsEventNotify in WM_CREATE)
	CreateWindow("CSTAEventClass", NULL, 0, 0, 0, 0, 0,
		HWND_DESKTOP, NULL, hInstance, (void *)oiTDevice);

	theSystem->message1 = 3;				//	Default to (3) TSAPI error

//	Setup the monitor event filter, and then activate device monitor
	FillMemory(&filter, 0, sizeof(CSTAMonitorFilter_t));
	filter.call = 0xFFFF;				// no filter for these events
	filter.feature = FF_DO_NOT_DISTURB | FF_FORWARDING | FF_MESSAGE_WAITING;
	rc = cstaMonitorDevice(acsHandle, INVOKE_ID, &szExtension, &filter, PD_NONE);
	if (rc < 1) 
		return;
	rc = WaitForReply(acsHandle, &eventBuf, &eventBufSize, CSTACONFIRMATION);
	if (rc != ACSPOSITIVE_ACK)
		return;
	if (eventBuf.eventHeader.eventType != CSTA_MONITOR_CONF)
		return;

//	Read into this application the current device context
	theSystem->message1 = ReadDeviceContext(oiTDevice);

	return;
}

/*************************************************************************
**	Function:
**		AlternateCall
**
**	Description:
**		Combines the effects of placing a call on hold and answering or
**		picking up a second call.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The call string for the device on hold to be picked up.
**
**	Output	: NONE.
**
**	  Flow 1: After Alternating
*/

#pragma argsused
void ALMCALLBACK AlternateCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
    theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		AnswerCall
**
**	Description:
**		Picks up an alerting call.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: The Call String for the newly established call (TEXT). (Optional)
**
**	  Flow 1: After Answering
*/

#pragma argsused
void ALMCALLBACK AnswerCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		Camp
**
**	Description:
**		Camps on a busy line. Actually calls the CallCompletion function
**		with FT_CAMP_ON as its feature flag.
**
**	INPUTS OUTPUTS and FLOWS:
**		See CallCompletion.
*/

#pragma argsused
void ALMCALLBACK Camp(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = CallCompletion(FT_CAMP_ON);
	return;
}

/*************************************************************************
**	Function:
**		CallBack
**
**	Description:
**		Instructs the server to call both parties when the currently busy
**		party returns to an idle state. Actually calls the CallCompletion
**		function with FT_CALL_BACK as its feature flag.
**
**	INPUTS OUTPUTS and FLOWS:
**		See CallCompletion.
*/

#pragma argsused
void ALMCALLBACK CallBack(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = CallCompletion(FT_CALL_BACK);
	return;
}

/*************************************************************************
**	Function:
**		Intrude
**
**	Description:
**		Instructs the server to intrude on the busy party's current call.
**		Actually calls the CallCompletion function with FT_INTRUDE as its
**		feature flag.
**
**	INPUTS OUTPUTS and FLOWS:
**		See CallCompletion.
*/

#pragma argsused
void ALMCALLBACK Intrude(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = CallCompletion(FT_INTRUDE);
	return;
}

/*************************************************************************
**	Function:
**		ClearCall
**
**	Description:
**		Clears a call from an open stream and an open connection.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: NONE.
**
**	  Flow 1: After Hanging Up
**	  Flow 2: Stream Not Open
**	  Flow 3: Connection Not Active
**	  Flow 4: Object Error
**	  Flow 5: TSAPI Error
*/

#define CALL_MAXCS	(CALL_MAXCMNT + 20)

#pragma argsused
void ALMCALLBACK ClearCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	OBJECTID		oiTDevice;
	OBJECTID		oiTLine;
	ACSHandle_t		acsHandle;
	CSTAEvent_t		eventBuf;
    ConnectionID_t	call;
	unsigned short	eventBufSize = sizeof(CSTAEvent_t);
	RetCode_t		rc;

	theSystem->message1 = 4;			//	Default to (4) object error

//	Get parameters from the AppWare Bus
	if (!(oiTDevice = AFuncGetTypedParameter(1, OTYPE_TDEV)))
		return;

	theSystem->message1 = 2;			//	Default to (2) no stream

	if (!(acsHandle = OTDevGetStreamHandle(oiTDevice)))
    	return;

    theSystem->message1 = 3;			//	Default to (3) no active cxtn

	if (!(oiTLine = OTDevGetActiveLine(oiTDevice)))
		return;

	OTLinGetDeviceID(oiTLine, call.deviceID, sizeof(DID));
	OTLinGetCallID(oiTLine, &call.callID);

	theSystem->message1 = 4;			//	Default to (4) TSAPI error
	
//	Clear call in TS Object using cstaClearCall() in CSTA library.
	rc = cstaClearCall(acsHandle, INVOKE_ID, &call, PD_NONE);
	if (rc < 1)
		return;
	rc = WaitForReply(acsHandle, &eventBuf, &eventBufSize, CSTACONFIRMATION);
	if (rc != ACSPOSITIVE_ACK)
		return;
	if (eventBuf.eventHeader.eventType != CSTA_CLEAR_CALL_CONF)
		return;

	OTLinSetCallInEffect(oiTLine, FALSE);
	OTLinSetCallID(oiTLine, 0);

	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		ClearConnection
**
**	Description:
**		Clears a connection on a call in a CSTA object.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The Call string.
**	 Input 3: The Connection string.
**
**	Output	: NONE.
**
**	  Flow 1: After Clearing
*/

#pragma argsused
void ALMCALLBACK ClearConnection(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		ConferenceCall
**
**	Description:
**		Conferences two calls into one.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The primary call string.
**	 Input 3: The secondary call string.
**
**	Output	: The new call string. (Optional)
**
**	  Flow 1: After Conferencing
*/

#pragma argsused
void ALMCALLBACK ConferenceCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		ConsultationCall
**
**	Description:
**		Creates a connection to the CSTA server based on information
**		in the CSTA object.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: Device to call for consultation (TEXT)
**
**	Output	: The Call String (TEXT). (Optional)
**
**	  Flow 1: After Calling
*/

#pragma argsused
void ALMCALLBACK ConsultationCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		DeflectCall
**
**	Description:
**		Deflects an alerting call to another device on the switch.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: Call string for the alerting call.
**	 Input 3: Device to deflect the call to (TEXT)
**
**	Output	: NONE.
**
**	  Flow 1: After Diverting
*/

#pragma argsused
void ALMCALLBACK DeflectCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		HoldCall
**
**	Description:
**		Places an active call on hold.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: NUMBER object indicating the call ID of the call placed on hold.
**
**	  Flow 1: After Placing Call on Hold
**	  Flow 2: Stream Not Open
**	  Flow 3: Connection Not Active
**	  Flow 4: Object Error
**	  Flow 5: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK HoldCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		MakeCall
**
**	Description:
**		Creates a connection to the CSTA server based on information
**		in the CSTA object.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The Target Device's PBX Extension (TEXT).
**	 Input 3: OPTIONAL: Source device's PBX Extension (TEXT).
**
**	Output	: NONE.
**
**	  Flow 1: After Calling
**	  Flow 2: Stream Not Open
**	  Flow 3: Object Error
**	  Flow 4: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK MakeCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	OBJECTID		oiTDevice, oiDstExt, oiSrcExt;
	ACSHandle_t		acsHandle;
	DeviceID_t		szDstExt, szSrcExt;
	CSTAEvent_t		eventBuf;
	unsigned short	eventBufSize = sizeof(CSTAEvent_t);
	RetCode_t		rc;

	theSystem->message1 = 3;			//	Default to (3) object error

//	Get parameters from the AppWare Bus
	if (!(oiTDevice = AFuncGetTypedParameter(1, OTYPE_TDEV)))
		return;
	if (!(oiDstExt = AFuncGetTypedParameter(2, OTYPE_TEXT)))
		return;

	if (!(oiSrcExt = AFuncGetTypedParameter(3, OTYPE_TEXT)))
		OTDevGetMonitorDevice(oiTDevice, szSrcExt, sizeof(DeviceID_t));
	else
		OTextGetToBuffer(oiSrcExt, szSrcExt, sizeof(DeviceID_t));

	OTextGetToBuffer(oiDstExt, szDstExt, sizeof(DeviceID_t));

	theSystem->message1 = 2;			//	Default to (2) no stream

	if (!(acsHandle = OTDevGetStreamHandle(oiTDevice)))
    	return;

	theSystem->message1 = 4;			//	Default to (4) TSAPI error
	
//	Make call to cstaMakeCall() in CSTA library.
	rc = cstaMakeCall(acsHandle, INVOKE_ID, &szSrcExt, &szDstExt, PD_NONE);
	if (rc < 1)
		return;
	rc = WaitForReply(acsHandle, &eventBuf, &eventBufSize, CSTACONFIRMATION);
	if (rc != ACSPOSITIVE_ACK)
		return;
	if (eventBuf.eventHeader.eventType != CSTA_MAKE_CALL_CONF)
		return;

// 	Add call to device and return
	theSystem->message1 = AddCallToDevice(oiTDevice,
		&eventBuf.event.cstaConfirmation.u.makeCall.newCall);
	return;
}

/*************************************************************************
**	Function:
**		ReconnectCall
**
**	Description:
**		Combined effect of hanging up and retrieving a held call.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The call string for the call to reconnect to (optional).
**
**	Output	: NONE.
**
**	  Flow 1: After Reconnecting
*/

#pragma argsused
void ALMCALLBACK ReconnectCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		RetrieveCall
**
**	Description:
**		Retrieves a previously held call from Hold status.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: NUMBER object indicating the call ID of the call to retrieve
**
**	Output	: NONE.
**
**	  Flow 1: After Retrieving Call
**	  Flow 2: Stream Not Open
**	  Flow 3: Connection Not Active
**	  Flow 4: Object Error
**	  Flow 5: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK RetrieveCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		TransferCall
**
**	Description:
**		Transfers a call from this device to another device on the switch.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: Call String for the primary call.
** 	 Input 3: Call String for the secondary call.
**
**	Output	: NONE.
**
**	  Flow 1: After Transferring
*/

#pragma argsused
void ALMCALLBACK TransferCall(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		SetMsgWaitingInd
**
**	Description:
**		Sets the message waiting indicator on this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: On or Off (FLAG).
**
**	Output	: NONE.
**
**	  Flow 1: After Setting
*/

#pragma argsused
void ALMCALLBACK SetMsgWaitingInd(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		SetDoNotDisturb
**
**	Description:
**		Sets the do not disturb option on this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: On or Off (FLAG).
**
**	Output	: NONE.
**
**	  Flow 1: After Setting
*/

#pragma argsused
void ALMCALLBACK SetDoNotDisturb(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		SetForwarding
**
**	Description:
**		Sets the forwarding status on this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**	 Input 2: The type of forwarding (0 - 6) (NUMBER)
**
**	Output	: NONE.
**
**	  Flow 1: After Setting
*/

#pragma argsused
void ALMCALLBACK SetForwarding(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		QueryMsgWaitingInd
**
**	Description:
**		Queries the message waiting indicator on this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: On/Off flag.
**
**	  Flow 1: After Querying
*/

#pragma argsused
void ALMCALLBACK QueryMsgWaitingInd(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		QueryDoNotDisturb
**
**	Description:
**		Queries the do not disturb status of this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: On/Off flag.
**
**	  Flow 1: After Querying
*/
                                                    
#pragma argsused
void ALMCALLBACK QueryDoNotDisturb(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		QueryForwarding
**
**	Description:
**		Queries the forwarding status of this device.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: Type (NUMBER)
**
**	  Flow 1: After Querying
*/

#pragma argsused
void ALMCALLBACK QueryForwarding(pAEvtInfo theEvent, pAObjMessage theSystem) {
	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**	Function:
**		CloseStream
**
**	Description:
**		Closes an ACS Stream previously opened with a call to OpenStream.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: NONE.
**
**	  Flow 1: After Closing
**	  Flow 2: Stream Not Open
**	  Flow 3: TSAPI Error
*/

#pragma argsused
void ALMCALLBACK CloseStream(pAEvtInfo theEvent, pAObjMessage theSystem) {
	OBJECTID		oiTDevice;
	CSTAEvent_t 	eventBuf;
	unsigned short	eventBufSize = sizeof(CSTAEvent_t);
	ACSHandle_t 	acsHandle;
	RetCode_t		rc;

	theSystem->message1 = 2;			//	Default to (2) object error

//	Get parameters from the AppWare Bus
	if (!(oiTDevice = AFuncGetTypedParameter(1, OTYPE_TDEV)))
    	return;

//	Get ACS handle from the parameter objects
	if (!(acsHandle = OTDevGetStreamHandle(oiTDevice)))
		return;

	theSystem->message1 = 3;			//	Default to (3) TSAPI error

//	Make call to acsCloseStream in CSTA library
	rc = acsCloseStream(acsHandle, INVOKE_ID, PD_NONE);
	if (rc < 1) 
		return;
	rc = WaitForReply(acsHandle, &eventBuf, &eventBufSize, ACSCONFIRMATION);
	if (rc != ACSPOSITIVE_ACK)
		return;
	if (eventBuf.eventHeader.eventType != ACS_CLOSE_STREAM_CONF)
		return;

	theSystem->message1 = 1;			//	OK! (1)
	return;
}

/*************************************************************************
**
**							INTERNAL ONLY FUNCTIONS
**
**	----------------------------------------------------------------------
**
**	Function:
**		WaitForReply
**
**	Description:
**		This is a windows version of acsGetEventPoll. It uses Yield
**		internally so as not to put the Windows VDM on hold while waiting
**		for a reply from the Telephony Server. It also searches the queue
**		for an event of a class specified by the eventClass parameter.
*/

static RetCode_t WaitForReply(ACSHandle_t acsHandle, CSTAEvent_t *eventBuf,
	unsigned short *eventBufSize, EventClass_t eventClass) {
	RetCode_t rc;

	eventBuf->eventHeader.eventClass = -1;
	rc = acsGetEventPoll(acsHandle, eventBuf, eventBufSize, NULL, NULL);
	while (rc == ACSERR_NOMESSAGE &&
		eventBuf->eventHeader.eventClass != eventClass) {
		Yield();
		rc = acsGetEventPoll(acsHandle, eventBuf, eventBufSize, NULL, NULL);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		CallCompletion
**
**	Description:
**		This is a private function called by Camp, CallBack, and Intrude.
**		Since this function is used with different feature flags for each
**		of the above operations.
**
**	 Input 1: The Telephony Stream (CSTA) Object.
**
**	Output	: NONE.
**
**	  Flow 1: After Call Completion (Camp, CallBack, or Intrude)
**	  Flow 2: Stream Not Open
**	  Flow 3: Connection Not Active
**	  Flow 4: Object Error
**	  Flow 5: TSAPI Error
*/

#pragma argsused
static LONG CallCompletion(Feature_t feature) {
	return 1;							//	OK! (1)
}

/*************************************************************************
**	Function:
**		EnumerationFunction
**
**	Description:
**		Callback procedure for the EnumServerNames function.
*/

#pragma argsused
Boolean CALLBACK EnumerationFunction(char *serverName, unsigned long lParam) {
	MEMBLOCKID		memBlock;
	unsigned long	blockSize;
	char  *		esnBuffer;
	unsigned long	textLength;

	if (!(memBlock = (MEMBLOCKID)lParam))
		return FALSE;
	if (!(blockSize = AMemGetSize(memBlock)))
		return FALSE;
	if (!(esnBuffer = (char  *)AMemLock(memBlock)))
		return FALSE;               	
	textLength = _fstrlen(esnBuffer) + lstrlen(serverName) + 2;
	if (textLength > blockSize) {
		if (!AMemUnlock(memBlock))
			return FALSE;
		if (!AMemSetSize(memBlock, blockSize + ESN_BLOCKSIZE))
			return FALSE;
		esnBuffer = (char  *)AMemLock(memBlock);
		if (!esnBuffer)
			return FALSE;
	}
	_fstrcat(esnBuffer, (char  *)serverName);
	_fstrcat(esnBuffer, (char  *)"\r\n");
	if (!AMemUnlock(memBlock))
		return FALSE;
	return TRUE;
}

/*************************************************************************
**	Function:
**		ReadDeviceContext
**
**	Description:
**		Reads the device context into the application so current call
**		information is correctly established on initialization of the
**		application.
**
**	Return:
**		1 = OK, 2 = Object Error, 3 = TSAPI error.
*/

static LONG ReadDeviceContext(OBJECTID oiTDevice) {
	ACSHandle_t	acsHandle;
	LONG		lKey = TDEVC_FIRSTLINE;

//	Retrieve the acsHandle to be used in the cstaDevSnapshotReq call
	if (!(acsHandle = OTDevGetStreamHandle(oiTDevice)))
		return 2;

//	Set the first line as the active line
	OTLinSetActiveLine(OTDevGetNextLine(oiTDevice, &lKey));

//							<<< NOT DONE >>>

//	Call cstaSnapshotDeviceReq() to determine the current device and line
//	status information.  Then use this information to fill in the line
//	connection lists and device line lists information.

	(void *)acsHandle;		// temporary to alleviate warning

//	In the mean time, just return OK!

	return 1;						    //	OK! (1)
}

/*************************************************************************
**	Function:
**		AddCallToDevice
**
**	Description:
**		Adds a call and it's associated information to a device context.
**
**	Return:
**		1 = OK, 2 = Object Error.
*/

#pragma argsused
static LONG AddCallToDevice(OBJECTID oiTDevice, ConnectionID_t * newCall) {
	OBJECTID	oiTLine = OTDevGetActiveLine(oiTDevice);
	MEMBLOCKID	mTCxtn = OTCxnCreate();

	if (oiTLine && mTCxtn) {
		OTCxnSetDeviceID(mTCxtn, newCall->deviceID);
		OTCxnSetState(mTCxtn, CS_ALERTING);
		OTCxnSetComment(mTCxtn, "Testing");
		OTLinAddCxtn(oiTLine, mTCxtn);
		OTLinSetCallID(oiTLine, newCall->callID);
		OTLinSetLocalState(oiTLine, CS_CONNECT);
		OTLinSetCallInEffect(oiTLine, TRUE);
		return 1;						//	OK! (1)
    }
	return 2;						    //	Object Error (2)
}
