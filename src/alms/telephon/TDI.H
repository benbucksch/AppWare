/***********************************************************/
/* Copyright (C) 1993 AT&T Bell Labs.  All rights reserved.*/
/***********************************************************/
/*
 *  tdi.h    Tserver-Driver Interface
 *
 *  Header File Dependencies
 *      acs.h
 *      csta.h
 *
 */

#ifndef TDI__H
#define TDI__H



     /*
      * DEFINES
      */

#define TDI_SUCCESS             1       /* TDI Function call completed OK */

#define TDI_MAX_SERVICE_NAME    10      /* Max length of the service name */
#define TDI_MAX_DRIVER_NAME     10      /* Max length of the driver name  */
#define TDI_MAX_VENDOR_NAME     6       /* Max length of the vendor name  */
#define TDI_MAX_SERVICE_TYPE    5       /* Max length of the service type */

#define TDI_MAX_REGISTRATIONS   16      /* Max driver registerations per
					 * Tserver */
#define TDI_MAX_NAME_SPACE      48      /* Max size of char arrays stored by
					 * TDI */
#define TDI_MAX_BUFFER_SIZE     (4096-12) /* Max size of buffers sent across
					 * the TDI */


/* TDI Default buffer descriptor values */

#define TDI_MAX_BYTES_ALLOCATED 0x100000 /* Default max bytes allocated per
					  * driver: 1 meg  */
#define TDI_BUFFER_HI_WATER_MARK (TDI_MAX_BYTES_ALLOCATED * 90 / 100)
					/* Default high water mark for
					 * allocated buffers */

/* TDI Buffer allocation condition flags */

#define TDI_EXCEED_HIWATER_MARK 0x01    /* Buffers allocated exceed high water
					 * mark */
#define TDI_EXCEED_MAX_BYTES    0x02    /* Buffers allocated exceed max bytes
					 * allowed */ 

/* TDI Security Checks per client request 
 * All levels of security verify that the
 * client has a valid NetWare login and password
 */

#define TDI_CSTA_SECURITY       0       /* Security check per client req. */
#define TDI_LOGIN_SECURITY      1       /* Security check on login */
#define TDI_NO_SECURITY         -1      /* Verifies NetWare login only */


/* TDI Message Priority Class */

#define TDI_NORMAL_MESSAGE      0       /* Send message normal class */
#define TDI_PRIORITY_MESSAGE    1       /* Send message priority class */

/* TDI Error Codes  */

#define TDI_ERR_ESYS            -1      /* System Error occurred */
#define TDI_ERR_BAD_DRVRID      -2      /* Unregistered driverID */
#define TDI_ERR_DUP_DRVR        -3      /* Service name already registered */
#define TDI_ERR_NO_MEM          -4      /* Unable to allocate memory */
#define TDI_ERR_MAX_DRVR        -5      /* Max  number of drivers exceeded */
#define TDI_ERR_EINVAL          -6      /* Invalid parameter to TDI call */
#define TDI_ERR_NO_BUFFERS      -7      /* No  buffers available to allocate */
#define TDI_ERR_BADLENGTH       -8      /* Requested length > max buf size */
#define TDI_ERR_BAD_BUF         -9      /* Buffer supplied is invalid */

     /*
      * TYPEDEFS
      */

typedef int     TDIHandle_t;            /* Identifies Tserver-Driver intf. */
typedef int     TDIReturn_t;            /* Return type for TDI functions */
typedef int     TDISecurity_t;          /* Security requested/client req. */
typedef int     TDIBuf_flag_t;          /* Current buffer allocation info */
typedef int     TDIPriority_t;          /*  Message priority */

typedef unsigned short	SessionID_t;	/* SPX Connection ID */

/* Buffer Descriptor */

typedef struct
{
	unsigned long   max_bytes;      /* Maximum number of bytes to allocate
					 * for this interface */
	unsigned long   hiwater_mark;   /* High water mark for buffer allocation
					 * on this interface */
} TDIBuf_info_t;

/* Queue Descriptor */

typedef struct
{
	int     queued_to_driver;       /* Number of bufs queued to driver */
	int     queued_to_tserver;      /* Number of bufs queued to tserver */
	int     allocd_by_driver;       /* Number of bufs allocated to driver */
	int     allocd_by_tserver;      /* Number of bufs allocated to tserver*/
} TDIQueue_info_t;

/* Mem Descriptor */

typedef struct
{
	unsigned long   bytes_queued_to_driver; /* Number of bytes in message
						 * buffers queued to driver */
	unsigned long   bytes_queued_to_tserver; /* Number of bytes in message
						 * buffers queued to tserver */
	unsigned long   bytes_allocd_by_driver; /* Number of bytes in message
						 * buffers allocated to driver*/
	unsigned long   bytes_allocd_by_tserver; /* Number of bytes in message
						 * buffers alloc'd to tserver */
} TDIMemAlloc_info_t;


#ifdef __cplusplus
extern "C" {
#endif
     /*
      * FUNCTION PROTOTYPES
      */

TDIHandle_t tdiDriverRegister(
	const char	*service_name,          /* INPUT  */
	const char	*driver_name,           /* INPUT  */
	int		service_type,           /* INPUT  */
	int		channel_number,         /* INPUT  */
	const char	*vendor_name,           /* INPUT  */
	const char	*version,               /* INPUT  */
	TDISecurity_t	driver_security,	/* INPUT  */
	const TDIBuf_info_t	*buffer_descriptor); /* INPUT  */

TDIReturn_t tdiDriverUnregister (
	TDIHandle_t     driverID);     /* INPUT */

TDIReturn_t tdiAllocBuffer (
	TDIHandle_t     driverID,	/* INPUT   */
	char            **bufptr,	/* OUTPUT */
	unsigned int    length,		/* INPUT   */
	TDIBuf_flag_t   *buf_flag);	/* OUTPUT */

TDIReturn_t tdiFreeBuffer (
	TDIHandle_t     driverID,      /* INPUT   */
	char            *bufptr);       /* INPUT   */

TDIReturn_t tdiReceiveFromTserver(
	TDIHandle_t     driverID,      	/* INPUT  */
	char            **bufptr);      /* OUTPUT */

TDIReturn_t tdiSendToTserver(
	TDIHandle_t     driverID,      	/* INPUT  */
	char            *bufptr,        /* INPUT  */
	TDIPriority_t   priority);      /* INPUT  */

TDIReturn_t tdiQueueSize (
	TDIHandle_t     driverID,      	/* INPUT  */
	TDIQueue_info_t *queue_descriptor); /* OUTPUT */

TDIReturn_t tdiMemAllocSize (
	TDIHandle_t     driverID,      	/* INPUT  */
	TDIMemAlloc_info_t *mem_descriptor); /* OUTPUT */

TDIReturn_t tdiDriverSanity (
	TDIHandle_t     driverID);     	/* INPUT  */

#ifdef __cplusplus
}
#endif

/* Tserver and Vendor advertised names */
#define TSRV_NM         "TSERVER"
#define NOVELL_NM       "NOVELL"
#define	SIM_NM		"SIMULATOR"
#define CSTA_NM		"CSTA"
#define OAM_NM		"OAM"
#define OTHER_NM	"OTHER"
#define TSRVOAM_NM	"TSRV_OAM"

/* Driver Service Type - passed in tdiDriverRegister() */
#define TDI_ST_OAM      1       
#define TDI_ST_CSTA     2
#define TDI_ST_SIM      3

#define CS_HOME_WORKTOP 1
#define CS_DEVICE_DEVICE 2
#define CS_DEVICE_CALL  3
#define CS_CALL_CALL    4
#define CS_CONTROL      5
#define CS_ROUTE        6

/* Driver Control Block Structure */
typedef struct {
    unsigned short              messageOffset;
    unsigned short              messageLength;
    unsigned short              privateOffset;
    unsigned short              privateLength;
    InvokeID_t                  invokeID;
    CSTAMonitorCrossRefID_t     monitorCrossRefID;
    SessionID_t              	sessionID;
    EventClass_t                messageClass;
    EventType_t                 messageType;
    short                       class_of_service;
} TDIDriverControlBlock_t;

/* 
 * tdi.h - public defintions for the Tserver-Driver Interface.
 * This header file defines the public information for using the TDI (actually,
 * just the tdiLogError() and related functions.
 */

/*
 * Error Levels used by tdiLogError
 */

typedef enum error_level {
	TRACE = 0x1,		/* trace message for debugging */
	CAUTION = 0x2,		/* unexpected software error (non-fatal) */
	AUDIT_TRAIL = 0x4,	/* important (normal) event */
	WARNING = 0x8,		/* non-service-affecting condition */
	ERROR = 0x10,		/* non-fatal, service-affecting condition */
	FATAL = 0x20		/* FATAL condition (calling module aborting) */
} ElogLevel_t;

typedef enum error_level ElogLevelMask_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Public (and exported out of the NLM) Function Prototypes
 */

void tdiLogError(
		const char      *module,        /* I - module name */
		ElogLevel_t     level,          /* I - TRACE, ERROR, FATAL,...*/
		int		location_code,  /* I - unique location number */
		int		error_code,     /* I - error code */
		const char      *format,        /* I - printf style format */
		...				/* I - optional arguments 
						 * (0 or more)   */
		);

#ifdef __cplusplus
}
#endif

#endif
