
/***********************************************************/
/* Copyright (C) 1993 AT&T Bell Labs.  All rights reserved.*/
/***********************************************************/
/*
 *	acs.h	 ACS Events Defines
 *
 *	Header File Dependencies
 *		acsdefs.h
 *
 */

#ifndef ACS_H
#define ACS_H

#include "acsdefs.h"

/* SAP advertising for CSTA streams from the Tserver */
#define TSERV_SAP_CSTA			0x0559	/* For CSTA T-Servers */

/* SAP advertising number client uses to find the Tserver is the same number
 * which has fisrt been byte swapped.
 */
#define CLIENT_SAP_CSTA 		0x5905	/* For CSTA Clients */

typedef int RetCode_t;

#define ACSPOSITIVE_ACK 		0	/* The function was successful */

/* Error Codes */

#define ACSERR_APIVERDENIED 	-1	/* This return indicates that the
									 * API Version requested is invalid
									 * and not supported by the
									 * existing API Client Library.
									 */

#define ACSERR_BADPARAMETER 	-2	/* One or more of the parameters is
									 * invalid
									 */

#define ACSERR_DUPSTREAM		-3	/* This return indicates that an
									 * ACS Stream is already established
									 * with the requested Server.
									 */

#define ACSERR_NODRIVER 		-4	/* This error return value indicates
									 * that no API Client Library Driver
									 * was found or installed on the system.
									 */

#define ACSERR_NOSERVER 		-5	/* This indicates that the requested
									 * Server is not present in the network.
									 */

#define ACSERR_NORESOURCE		-6	/* This return value indicates that
									 * there are insufficient resources
									 * to open a ACS Stream.
									 */

#define ACSERR_UBUFSMALL		-7	/* The user buffer size was smaller
									 * than the size of the next available
									 * event.
									 */

#define ACSERR_NOMESSAGE		-8	/* There were no messages available to
									 * return to the application.
									 */

#define ACSERR_UNKNOWN			-9	/* The ACS Stream has encountered
									 * an unspecified error.
									 */

#define ACSERR_BADHDL			-10 /* The ACS Handle is invalid */

#define ACSERR_STREAM_FAILED	-11 /* The ACS Stream has failed
									 * due to network problems.
									 * No further operations are
									 * possible on this stream.
									 */

#define ACSERR_NOBUFFERS		-12 /* There were not enough buffers
									 * available to place an outgoing
									 * message on the send queue.
									 * No message has been sent.
									 */

#define ACSERR_QUEUE_FULL		-13 /* The send queue is full.
									 * No message has been sent.
									 */


typedef unsigned long	InvokeID_t;

typedef enum {
	APP_GEN_ID, 	// application will provide invokeIDs; any 4-byte value is legal
	LIB_GEN_ID		// library will generate invokeIDs in the range 1-32767
} InvokeIDType_t;


#if defined _MSDOS || defined __MSDOS__

typedef unsigned short	ACSHandle_t;

#else

typedef unsigned long	ACSHandle_t;

#endif


typedef unsigned short	EventClass_t;

// defines for ACS event classes

#define ACSREQUEST			0
#define ACSUNSOLICITED		1
#define ACSCONFIRMATION 	2

typedef unsigned short	EventType_t;	// event types are defined in acs.h and csta.h


typedef struct
{
	ACSHandle_t 	acsHandle;
	EventClass_t	eventClass;
	EventType_t 	eventType;

} ACSEventHeader_t;


typedef struct
{
	union
	{
		ACSUniversalFailureEvent_t	failureEvent;

		// future types to be defined here
	} u;

} ACSUnsolicitedEvent;


typedef struct
{
	InvokeID_t	invokeID;
	union
	{
		ACSOpenStreamConfEvent_t		acsopen;
		ACSCloseStreamConfEvent_t		acsclose;
		ACSUniversalFailureConfEvent_t	failureEvent;
	} u;

} ACSConfirmationEvent;


#define ACS_MAX_HEAP	1024

typedef struct
{
	ACSEventHeader_t	eventHeader;
	union
	{
		ACSUnsolicitedEvent 	acsUnsolicited;
		ACSConfirmationEvent	acsConfirmation;
	} event;
	char	heap[ACS_MAX_HEAP];

} ACSEvent_t;


typedef char Passwd_t[49];

typedef struct PrivateData_t {
	char			vendor[32];
	unsigned short	length;
	char			data[1];	// actual length determined by application
} PrivateData_t;


// function prototypes

#ifdef __cplusplus
extern "C"
{
#endif

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsOpenStream ( ACSHandle_t 	 *acsHandle,
				InvokeIDType_t		invokeIDType,
				InvokeID_t			invokeID,
				StreamType_t		streamType,
				ServerID_t		 *serverID,
				LoginID_t		 *loginID,
				Passwd_t		 *passwd,
				AppName_t		 *applicationName,
				Level_t 			acsLevelReq,
				Version_t		 *apiVer,
				unsigned short		sendQSize,
				unsigned short		sendExtraBufs,
				unsigned short		recvQSize,
				unsigned short		recvExtraBufs,
				PrivateData_t	 *priv);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsCloseStream (ACSHandle_t 		acsHandle,
				InvokeID_t			invokeID,
				PrivateData_t	 *priv);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsAbortStream (ACSHandle_t 		acsHandle,
				PrivateData_t	 *priv);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsFlushEventQueue (ACSHandle_t 	acsHandle);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsGetEventPoll (	ACSHandle_t 		acsHandle,
					void			 *eventBuf,
					unsigned short	 *eventBufSize,
					PrivateData_t	 *privData,
					unsigned short	 *numEvents);


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsGetEventBlock (	ACSHandle_t 		acsHandle,
					void			 *eventBuf,
					unsigned short	 *eventBufSize,
					PrivateData_t	 *privData,
					unsigned short	 *numEvents);

#if defined WINDOWS || defined _WINDOWS || defined _Windows

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsEventNotify (ACSHandle_t acsHandle, HWND hwnd, UINT msg, Boolean notifyAll);

#endif


RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsSetESR ( ACSHandle_t 	acsHandle,
			void			(CALLBACK /*!!PORT!! was _far_ _pascal_*/ *esr)(unsigned short esrParam),
			unsigned short	esrParam,
			Boolean 		notifyAll);


typedef Boolean (CALLBACK /*!!PORT!! was _far_ _pascal_*/ *EnumServerNamesCB) (	char *serverName,
													unsigned long lParam);

RetCode_t CALLBACK /*!!PORT!! was _far_ _pascal_*/
acsEnumServerNames (StreamType_t		streamType,
					EnumServerNamesCB	userCB,
					unsigned long		lParam);


#ifdef __cplusplus
}
#endif

#endif

