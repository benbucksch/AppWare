/***********************************************************/
/* Copyright (C) 1993 AT&T Bell Labs.  All rights reserved.*/
/***********************************************************/
/*
 *  csta.h    CSTA Events Defines
 *
 *  Header File Dependencies
 *   	cstadefs.h, acs.h
 *
 */

#ifndef	CSTA_H
#define	CSTA_H

#include "cstadefs.h"

// defines for CSTA event classes

#define	CSTAREQUEST		3
#define	CSTAUNSOLICITED		4
#define	CSTACONFIRMATION	5
#define	CSTAEVENTREPORT		6

typedef struct 
{
	InvokeID_t	invokeID;
	union 
	{
		CSTARouteRequestEvent_t		routeRequest;
		CSTAReRouteRequest_t		reRouteRequest;
		CSTAEscapeSvcReqEvent_t		escapeSvcReqeust;
		CSTASysStatReqEvent_t		sysStatRequest;
	} u;
} CSTARequestEvent;

typedef struct
{
	union
	{
		CSTARouteRegisterAbortEvent_t   registerAbort;
		CSTARouteUsedEvent_t		routeUsed;
		CSTARouteEndEvent_t		routeEnd;
		CSTAPrivateEvent_t		privateEvent;
		CSTASysStatEvent_t		sysStat;
		CSTASysStatEndedEvent_t		sysStatEnded;
	}u;
} CSTAEventReport;

typedef struct 
{
	CSTAMonitorCrossRefID_t		monitorCrossRefId;
	union 
	{
		CSTACallClearedEvent_t		callCleared;
		CSTAConferencedEvent_t		conferenced;
		CSTAConnectionClearedEvent_t	connectionCleared;
		CSTADeliveredEvent_t		delivered;
		CSTADivertedEvent_t		diverted;
		CSTAEstablishedEvent_t		established;
		CSTAFailedEvent_t		failed;
		CSTAHeldEvent_t			held;
		CSTANetworkReachedEvent_t	networkReached;
		CSTAOriginatedEvent_t		originated;
		CSTAQueuedEvent_t		queued;
		CSTARetrievedEvent_t		retrieved;
		CSTAServiceInitiatedEvent_t	serviceInitiated;
		CSTATransferredEvent_t		transferred;
		CSTACallInformationEvent_t	callInformation;
		CSTADoNotDisturbEvent_t		doNotDisturb;
		CSTAForwardingEvent_t		forwarding;
		CSTAMessageWaitingEvent_t	messageWaiting;
		CSTALoggedOnEvent_t		loggedOn;
		CSTALoggedOffEvent_t		loggedOff;
		CSTANotReadyEvent_t		notReady;
		CSTAReadyEvent_t		ready;
		CSTAWorkNotReadyEvent_t		workNotReady;
		CSTAWorkReadyEvent_t		workReady;
		CSTABackInServiceEvent_t	backInService;
		CSTAOutOfServiceEvent_t		outOfService;
		CSTAPrivateStatusEvent_t	privateStatus;
		CSTAMonitorEndedEvent_t  	monitorEnded;
	} u;

} CSTAUnsolicitedEvent;


typedef struct 
{
	InvokeID_t	invokeID;
	union 
	{
		CSTAAlternateCallConfEvent_t		alternateCall;
		CSTAAnswerCallConfEvent_t		answerCall;
		CSTACallCompletionConfEvent_t		callCompletion;
		CSTAClearCallConfEvent_t		clearCall;
		CSTAClearConnectionConfEvent_t    	clearConnection;
		CSTAConferenceCallConfEvent_t		conferenceCall;
		CSTAConsultationCallConfEvent_t		consultationCall;
		CSTADeflectCallConfEvent_t		deflectCall;
		CSTAPickupCallConfEvent_t		pickupCall;
		CSTAGroupPickupCallConfEvent_t		groupPickupCall;
		CSTAHoldCallConfEvent_t			holdCall;
		CSTAMakeCallConfEvent_t			makeCall;
		CSTAMakePredictiveCallConfEvent_t 	makePredictiveCall;
		CSTAQueryMwiConfEvent_t			queryMwi;
		CSTAQueryDndConfEvent_t			queryDnd;
		CSTAQueryFwdConfEvent_t			queryFwd;
		CSTAQueryAgentStateConfEvent_t		queryAgentState;
		CSTAQueryLastNumberConfEvent_t		queryLastNumber;
		CSTAQueryDeviceInfoConfEvent_t		queryDeviceInfo;
		CSTAReconnectCallConfEvent_t		reconnectCall;
		CSTARetrieveCallConfEvent_t		retrieveCall;
		CSTASetMwiConfEvent_t			setMwi;
		CSTASetDndConfEvent_t			setDnd;
		CSTASetFwdConfEvent_t			setFwd;
		CSTASetAgentStateConfEvent_t		setAgentState;
		CSTATransferCallConfEvent_t		transferCall;
		CSTAUniversalFailureConfEvent_t		universalFailure;
		CSTAMonitorConfEvent_t			monitorStart;
		CSTAChangeMonitorFilterConfEvent_t	changeMonitorFilter;
		CSTAMonitorStopConfEvent_t		monitorStop;
		CSTASnapshotDeviceConfEvent_t		snapshotDevice;
		CSTASnapshotCallConfEvent_t		snapshotCall;
		CSTARouteRegisterReqConfEvent_t		routeRegister;
		CSTARouteRegisterCancelConfEvent_t	routeCancel;
		CSTAEscapeSvcConfEvent_t		escapeService;
		CSTASysStatReqConfEvent_t		sysStatReq;
		CSTASysStatStartConfEvent_t		sysStatStart;
		CSTASysStatStopConfEvent_t		sysStatStop;
		CSTAChangeSysStatFilterConfEvent_t	changeSysStatFilter;
		CSTAGetAPICapsConfEvent_t		getAPICaps;
		CSTAGetDeviceListConfEvent_t	getDeviceList;
		CSTAQueryCallMonitorConfEvent_t	queryCallMonitor;
		
	} u;

} CSTAConfirmationEvent;


#define CSTA_MAX_HEAP	1024

typedef struct 
{
	ACSEventHeader_t	eventHeader;
	union 
	{
		ACSUnsolicitedEvent	acsUnsolicited;
		ACSConfirmationEvent	acsConfirmation;
		CSTARequestEvent	cstaRequest;
		CSTAUnsolicitedEvent	cstaUnsolicited;
		CSTAConfirmationEvent	cstaConfirmation;
		CSTAEventReport		cstaEventReport;
	} event;
	char	heap[CSTA_MAX_HEAP];
} CSTAEvent_t;

// function prototypes

#ifdef __cplusplus
extern "C"
{
#endif  

// Basic Call Control Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaAlternateCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*activeCall,
					ConnectionID_t		*otherCall,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaAnswerCall (ACSHandle_t			acsHandle,
				InvokeID_t			invokeID,
				ConnectionID_t		*alertingCall,
				PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaCallCompletion (ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					Feature_t			feature,
					ConnectionID_t		*call,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaClearCall (	ACSHandle_t			acsHandle,
				InvokeID_t			invokeID,
				ConnectionID_t		*call,
				PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaClearConnection (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						ConnectionID_t		*call,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaConferenceCall (ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*heldCall,
					ConnectionID_t		*activeCall,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaConsultationCall (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						ConnectionID_t		*activeCall,
						DeviceID_t			*calledDevice,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaDeflectCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*deflectCall,
					DeviceID_t			*calledDevice,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaGroupPickupCall (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*group,
						DeviceID_t			*pickupDevice,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaHoldCall (	ACSHandle_t			acsHandle,
				InvokeID_t			invokeID,
				ConnectionID_t		*activeCall,
				Boolean				reservation,
				PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMakeCall (	ACSHandle_t			acsHandle,
				InvokeID_t			invokeID,
				DeviceID_t			*callingDevice,
				DeviceID_t			*calledDevice,
				PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMakePredictiveCall (ACSHandle_t				acsHandle,
						InvokeID_t				invokeID,
						DeviceID_t				*callingDevice,
						DeviceID_t				*calledDevice,
						AllocationState_t		allocationState,
						PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaPickupCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*deflectCall,
					DeviceID_t			*calledDevice,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaReconnectCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*activeCall,
					ConnectionID_t		*heldCall,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaRetrieveCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*heldCall,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaTransferCall (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					ConnectionID_t		*heldCall,
					ConnectionID_t		*activeCall,
					PrivateData_t		*privateData);


// Telephony Supplementary Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSetMsgWaitingInd (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						Boolean				messages,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSetDoNotDisturb (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						Boolean				doNotDisturb,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSetForwarding (	ACSHandle_t				acsHandle,
					InvokeID_t				invokeID,
					DeviceID_t				*device,
					ForwardingType_t		forwardingType,
					Boolean					forwardingOn,
					DeviceID_t				*forwardingDestination,
					PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSetAgentState (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					DeviceID_t			*device,
					AgentMode_t			agentMode,
					AgentID_t			*agentID,
					AgentGroup_t		*agentGroup,
					AgentPassword_t		*agentPassword,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryMsgWaitingInd (ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryDoNotDisturb (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryForwarding (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryAgentState (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryLastNumber (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryDeviceInfo (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*device,
						PrivateData_t		*privateData);


// Monitor Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMonitorDevice (	ACSHandle_t				acsHandle,
					InvokeID_t				invokeID,
					DeviceID_t				*deviceID,
					CSTAMonitorFilter_t		*monitorFilter,
					PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMonitorCall (	ACSHandle_t				acsHandle,
					InvokeID_t				invokeID,
					ConnectionID_t			*call,
					CSTAMonitorFilter_t		*monitorFilter,
					PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMonitorCallsViaDevice (	ACSHandle_t				acsHandle,
							InvokeID_t				invokeID,
							DeviceID_t				*deviceID,
							CSTAMonitorFilter_t		*monitorFilter,
							PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaChangeMonitorFilter (	ACSHandle_t					acsHandle,
							InvokeID_t					invokeID,
							CSTAMonitorCrossRefID_t		monitorCrossRefID,
							CSTAMonitorFilter_t			*filterlist,
							PrivateData_t				*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaMonitorStop (	ACSHandle_t					acsHandle,
					InvokeID_t					invokeID,
					CSTAMonitorCrossRefID_t		monitorCrossRefID,
					PrivateData_t				*privateData);

// Snapshot Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSnapshotCallReq (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						ConnectionID_t		*snapshotObj,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSnapshotDeviceReq (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*snapshotObj,
						PrivateData_t		*privateData);


// Routing Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaRouteRegisterReq (	ACSHandle_t			acsHandle,
						InvokeID_t			invokeID,
						DeviceID_t			*routingDevice,
						PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaRouteRegisterCancel (	ACSHandle_t				acsHandle,
							InvokeID_t				invokeID,
							RouteRegisterReqID_t	routeRegisterReqID,
							PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaRouteSelect (	ACSHandle_t				acsHandle,
					RouteRegisterReqID_t 	routeRegisterReqID,
					RoutingCrossRefID_t		routingCrossRefID,
					DeviceID_t				routeSelected,
					RetryValue_t			remainRetry,
					SetUpValues_t		 *setupInformation,
					Boolean					routeUsedReq,
					PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaRouteEnd (	ACSHandle_t					acsHandle,
				RouteRegisterReqID_t		routeRegisterReqID,
				RoutingCrossRefID_t			routingCrossRefID,
				CSTAUniversalFailure_t		errorValue,
				PrivateData_t				*privateData);


// Escape Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaEscapeService (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaEscapeServiceConf (	ACSHandle_t					acsHandle,
						InvokeID_t					invokeID,
						CSTAUniversalFailure_t		error,
						PrivateData_t				*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSendPrivateEvent (	ACSHandle_t			acsHandle,
						PrivateData_t		*privateData);


// Maintenance Services

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSysStatReq (ACSHandle_t			acsHandle,
				InvokeID_t			invokeID,
				PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSysStatStart (	ACSHandle_t					acsHandle,
					InvokeID_t					invokeID,
					SystemStatusFilter_t		statusFilter,
					PrivateData_t				*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSysStatStop (	ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaChangeSysStatFilter (	ACSHandle_t				acsHandle,
							InvokeID_t				invokeID,
							SystemStatusFilter_t	statusFilter,
							PrivateData_t			*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSysStatReqConf (ACSHandle_t			acsHandle,
					InvokeID_t			invokeID,
					SystemStatus_t		systemStatus,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaSysStatEvent (	ACSHandle_t			acsHandle,
					SystemStatus_t		systemStatus,
					PrivateData_t		*privateData);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaGetAPICaps (ACSHandle_t acsHandle, InvokeID_t invokeID);
 
 
RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaGetDeviceList (	ACSHandle_t	acsHandle,
					InvokeID_t	invokeID,
				    long		index,
				    CSTALevel_t	level);

					
RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
cstaQueryCallMonitor (ACSHandle_t acsHandle, InvokeID_t invokeID);


#ifdef __cplusplus
}
#endif  

#endif

