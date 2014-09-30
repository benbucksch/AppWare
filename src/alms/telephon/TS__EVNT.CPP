/*************************************************************************
**	File Name:
**		TS__EVNT.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**      The CSTA event handler and it's components for the telephony
**		service ALM.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#define STRICT
#define ASTRICT

#include <a_alm.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "tlinvisu.h"
#include "o_tdev.h"
#include "o_tlin.h"

//	Module static function prototypes

static void acsRequest(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void acsUnsolicited(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void acsConfirmation(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void cstaRequest(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void cstaUnsolicited(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void cstaConfirmation(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);
static void cstaEventReport(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType);

//	Module code

/*************************************************************************
**	Function:
**		CSTAEventHandler		(Windows & Telephony Services)
**
**	Description:
**		This is the window handler for all CSTA device events. This
**		handler is established with Windows via a call to CreateWindow
**		in the OpenStream function and established with the ACS library
**		with a call to cstaMonitorDevice in OpenStream (CSTAFUNC.CPP).
*/

LRESULT CALLBACK CSTAEventHandler(HWND hwnd, UINT msg, WPARAM wParam,
	LPARAM lParam) {

	CREATESTRUCT *	cs;
	OBJECTID			oiTDev;
	EventClass_t		eventClass;
	EventType_t 		eventType;
	ACS 				acsHandle;

	switch(msg) {
		case WM_CREATE: {
			RetCode_t	rc;

			cs = (CREATESTRUCT *)lParam;
			oiTDev = (OBJECTID)cs->lpCreateParams;
			SetWindowLong(hwnd, 0, (LONG)oiTDev);
			acsHandle = OTDevGetStreamHandle(oiTDev);
			if (!acsHandle)
				return 1;
			rc = acsEventNotify(acsHandle, hwnd, WM_ACSEVENT, TRUE);
			if (rc < 1)
				return 1;
			return 0;
		}
		case WM_ACSEVENT:
			acsHandle	= (ACSHandle_t)wParam;
			eventClass	= (EventClass_t)HIWORD(lParam);
			eventType	= (EventType_t)LOWORD(lParam);
			oiTDev		= (OBJECTID)GetWindowLong(hwnd, 0);

			switch (eventClass) {
				case ACSREQUEST:
					acsRequest(oiTDev, acsHandle, eventType);
					break;
				case ACSUNSOLICITED:
					acsUnsolicited(oiTDev, acsHandle, eventType);
					break;
				case ACSCONFIRMATION:
					acsConfirmation(oiTDev, acsHandle, eventType);
					break;
				case CSTAREQUEST:
					cstaRequest(oiTDev, acsHandle, eventType);
					break;
				case CSTAUNSOLICITED:
					cstaUnsolicited(oiTDev, acsHandle, eventType);
					break;
				case CSTACONFIRMATION:
					cstaConfirmation(oiTDev, acsHandle, eventType);
					break;
				case CSTAEVENTREPORT:
					cstaEventReport(oiTDev, acsHandle, eventType);
					break;
				default: {
					CSTAEvent_t 	eventBuffer;
					unsigned short	eventBufferSize = sizeof(eventBuffer);

					//	toss out NOT-HANDLED unknown event

					acsGetEventPoll(acsHandle, &eventBuffer,
						&eventBufferSize, NULL, NULL);
					break;
				}
			}
			return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*************************************************************************
**	Function:
**		acsRequest				(CSTAEventHandler)
**
**	Description:
**		Sub-handler for acs requests.
*/

#pragma argsused
static void acsRequest(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {
	CSTAEvent_t 	eventBuffer;
	unsigned short	eventBufferSize = sizeof(eventBuffer);

	//	toss NOT-HANDLED acs request event

	acsGetEventPoll(acsHandle, &eventBuffer, &eventBufferSize, NULL, NULL);
	return;
}

/*************************************************************************
**	Function:
**		acsUnsolicited			(CSTAEventHandler)
**
**	Description:
**		Sub-handler for acs unsolicited events.
*/

#pragma argsused
static void acsUnsolicited(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {
	switch(eventType) {
		case ACS_UNIVERSAL_FAILURE:
			break;

		default: {
			CSTAEvent_t 	eventBuffer;
			unsigned short	eventBufferSize = sizeof(eventBuffer);

			//	toss NOT-HANDLED ACS unsolicited event

			acsGetEventPoll(acsHandle, &eventBuffer,
				&eventBufferSize, NULL, NULL);
			break;
		}
	}
	return;
}

/*************************************************************************
**	Function:
**		acsConfirmation 		(CSTAEventHandler)
**
**	Description:
**		Sub-handler for acs confirmation events.
*/

#pragma argsused
static void acsConfirmation(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {

	switch(eventType) {

		//	allow all ACS confirmation events to pass to function poll

		case ACS_OPEN_STREAM_CONF:
		case ACS_CLOSE_STREAM_CONF:
		case ACS_UNIVERSAL_FAILURE_CONF:
			break;

		default: {
			CSTAEvent_t 	eventBuffer;
			unsigned short	eventBufferSize = sizeof(eventBuffer);

			//	toss NOT-HANDLED ACS confirmation event

			acsGetEventPoll(acsHandle, &eventBuffer,
				&eventBufferSize, NULL, NULL);
			break;
		}
	}
	return;
}

/*************************************************************************
**	Function:
**		cstaRequest 			(CSTAEventHandler)
**
**	Description:
**		Sub-handler for csta request confirmation events.
*/

#pragma argsused
static void cstaRequest(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {
	CSTAEvent_t 	eventBuffer;
	unsigned short	eventBufferSize = sizeof(eventBuffer);

	//	toss NOT-HANDLED CSTA request event

	acsGetEventPoll(acsHandle, &eventBuffer, &eventBufferSize, NULL, NULL);
	return;
}

static void cstaUnsolicited(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {

	CSTAEvent_t 	eventBuffer;
	unsigned short	eventBufferSize = sizeof(eventBuffer);

	//	retrieve ALL unsolicited CSTA events out of the queue buffer...

	acsGetEventPoll(acsHandle, &eventBuffer, &eventBufferSize, NULL, NULL);

	switch(eventType) {
		case CSTA_CALL_CLEARED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_CALLCLEAR);
			break;

		case CSTA_CONFERENCED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_CONFERENCED);
			break;

		case CSTA_CONNECTION_CLEARED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_CXTNCLEAR);
			break;

		case CSTA_DELIVERED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_ALERTING);
			break;

		case CSTA_DIVERTED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_DIVERTED);
			break;

		case CSTA_ESTABLISHED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_ESTABLISHED);
			break;

		case CSTA_FAILED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FAILED);
			break;

		case CSTA_HELD:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_HELD);
			break;

		case CSTA_NETWORK_REACHED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_NETREACHED);
			break;

		case CSTA_ORIGINATED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_ORIGINATED);
			break;

		case CSTA_QUEUED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_QUEUED);
			break;

		case CSTA_RETRIEVED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_RETRIEVED);
			break;

		case CSTA_SERVICE_INITIATED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_SERVICEINIT);
			break;

		case CSTA_TRANSFERRED:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_TRANSFERRED);
			break;

		case CSTA_CALL_INFORMATION:
			break;

		case CSTA_DO_NOT_DISTURB:
			AEvtPostSignalAtTail(oiTDev, TDEV_EVT_DND_OFF);
			break;

		case CSTA_FORWARDING: {
			CSTAForwardingEvent_t * fwd =
				&eventBuffer.event.cstaUnsolicited.u.forwarding;

			if (fwd->forwardingInformation.forwardingOn)
				switch(fwd->forwardingInformation.forwardingType) {
					case FWD_IMMEDIATE:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_IMM);
						break;

					case FWD_BUSY:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_BSY);
						break;

					case FWD_NO_ANS:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_NAN);
						break;

					case FWD_BUSY_INT:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_BIN);
						break;

					case FWD_BUSY_EXT:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_BEX);
						break;

					case FWD_NO_ANS_INT:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_NAI);
						break;

					case FWD_NO_ANS_EXT:
						AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_NAE);
						break;
				}
			else
				AEvtPostSignalAtTail(oiTDev, TDEV_EVT_FWD_OFF);
			break;
		}

		case CSTA_MESSAGE_WAITING: {
			CSTAMessageWaitingEvent_t *	msg =
				&eventBuffer.event.cstaUnsolicited.u.messageWaiting;

			if (msg->messageWaitingOn)
				AEvtPostSignalAtTail(oiTDev, TDEV_EVT_MSGWAITING_ON);
			else
				AEvtPostSignalAtTail(oiTDev, TDEV_EVT_MSGWAITING_OFF);
			break;
		}

		case CSTA_LOGGED_ON:
		case CSTA_LOGGED_OFF:
		case CSTA_NOT_READY:
		case CSTA_READY:
		case CSTA_WORK_NOT_READY:
		case CSTA_WORK_READY:
			break;

		case CSTA_BACK_IN_SERVICE:
		case CSTA_OUT_OF_SERVICE:
			break;
	}
	return;
}

/*************************************************************************
**	Function:
**		cstaConfirmation		(CSTAEventHandler)
**
**	Description:
**		Sub-handler for csta confirmation events.
*/

#pragma argsused
static void cstaConfirmation(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {

	switch(eventType) {

		//	allow all CSTA confirmation events to pass on to function poll
		case CSTA_ALTERNATE_CALL_CONF:
		case CSTA_ANSWER_CALL_CONF:
		case CSTA_CALL_COMPLETION_CONF:
		case CSTA_CLEAR_CALL_CONF:
		case CSTA_CLEAR_CONNECTION_CONF:
		case CSTA_CONFERENCE_CALL_CONF:
		case CSTA_CONSULTATION_CALL_CONF:
		case CSTA_DEFLECT_CALL_CONF:
		case CSTA_PICKUP_CALL_CONF:
		case CSTA_GROUP_PICKUP_CALL_CONF:
		case CSTA_HOLD_CALL_CONF:
		case CSTA_MAKE_CALL_CONF:
		case CSTA_MAKE_PREDICTIVE_CALL_CONF:
		case CSTA_QUERY_MWI_CONF:
		case CSTA_QUERY_DND_CONF:
		case CSTA_QUERY_FWD_CONF:
		case CSTA_QUERY_AGENT_STATE_CONF:
		case CSTA_QUERY_LAST_NUMBER_CONF:
		case CSTA_QUERY_DEVICE_INFO_CONF:
		case CSTA_RECONNECT_CALL_CONF:
		case CSTA_RETRIEVE_CALL_CONF:
		case CSTA_SET_MWI_CONF:
		case CSTA_SET_DND_CONF:
		case CSTA_SET_FWD_CONF:
		case CSTA_SET_AGENT_STATE_CONF:
		case CSTA_TRANSFER_CALL_CONF:
		case CSTA_SYS_STAT_REQ_CONF:
		case CSTA_SYS_STAT_START_CONF:
		case CSTA_SYS_STAT_STOP_CONF:
		case CSTA_CHANGE_SYS_STAT_FILTER_CONF:
		case CSTA_MONITOR_CONF:
		case CSTA_CHANGE_MONITOR_FILTER_CONF:
		case CSTA_MONITOR_STOP_CONF:
		case CSTA_UNIVERSAL_FAILURE_CONF:
			break;

		default: {
			CSTAEvent_t 	eventBuffer;
			unsigned short	eventBufferSize = sizeof(eventBuffer);

			//	toss NOT-HANDLED CSTA confirmation event

			acsGetEventPoll(acsHandle, &eventBuffer,
				&eventBufferSize, NULL, NULL);
			break;
		}
	}
	return;
}

/*************************************************************************
**	Function:
**		cstaEventReport 			(CSTAEventHandler)
**
**	Description:
**		Sub-handler for csta event reports.
*/

#pragma argsused
static void cstaEventReport(OBJECTID oiTDev, ACS acsHandle, EventType_t eventType) {
	CSTAEvent_t 	eventBuffer;
	unsigned short	eventBufferSize = sizeof(eventBuffer);

	//	toss NOT-HANDLED CSTA event report

	acsGetEventPoll(acsHandle, &eventBuffer, &eventBufferSize, NULL, NULL);
	return;
}
