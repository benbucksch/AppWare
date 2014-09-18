/*========================================================================
  ==
  ==	Program Name:	System configuration utility.
  ==
  ==	Filename:	NWSYSCON.H
  ==
  ==	Date:		6/04/85
  ==
  ==	Version:	1.00
  ==
  ==	Programmers:	Kevin Kingdon
  ==
  ==	Modifications:
  ==
  ==	Comments:	This include file contains the various structure,
  ==			constant, and global variable definitions needed by
  ==			the syscon utility.
  ==
  ===========================================================================
  ==  (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.      ==
  ==                                                                       ==
  ==  No part of this file may be duplicated, revised, translated,         ==
  ==  localized or modified in any manner or compiled, linked or uploaded  ==
  ==  or downloaded to or from any computer system without the prior       ==
  ==  written consent of Novell, Inc.                                      ==
  ===========================================================================
  ==
  ========================================================================*/

#if !defined NWSYSCON_H

#define NWSYSCON_H


/* #define WORK_GROUP_MANAGERS */

#if !defined LONG
#define LONG unsigned long
#endif  /** LONG **/

#if !defined ERROR_INVALID_DRIVE
#define ERROR_INVALID_DRIVE 15
#endif  /** ERROR_INVALID_DRIVE **/


#define MAX_DRIVES      (BYTE)27
#define MAX_TEMP_DRIVES (BYTE)6
#define TEMP_DRIVE1     (BYTE)27
#define TEMP_DRIVE2     (BYTE)28
#define TEMP_DRIVE3     (BYTE)29
#define TEMP_DRIVE4     (BYTE)30
#define TEMP_DRIVE5     (BYTE)31
#define TEMP_DRIVE6     (BYTE)32


#if !defined NO_BASE_DRIVE
#define NO_BASE_DRIVE   ((BYTE)0xFF)
#define TEMPORARY_DRIVE ((BYTE)0x00)
#define PERMANENT_DRIVE ((BYTE)0x01)
#define DRIVE_NETWORK   ((BYTE)0x03)
#endif  /** NO_BASE_DRIVE **/

/* User object LOGIN_CONTROL restriction flags */
#define LC_SUPERVISOR_PASSWORD_ONLY		((BYTE)0x01)
#define LC_FORCE_UNIQUE_PASSWORD		((BYTE)0x02)

/* Default values used when no previous value has been specified */
#define DEFAULT_PASSWORD_INTERVAL	40
#define DEFAULT_PASSWORD_LENGTH		5
#define DEFAULT_GRACE_LOGINS		6
#define DEFAULT_ACCOUNT_BALANCE		0L
#define DEFAULT_CREDIT_LIMIT		0L
#define DEFAULT_CONCURRENT_CONNS	1
#define DEFAULT_DISK_SPACE_IN_K		0L
#define INTRUDER_DETECT_DAYS		0
#define INTRUDER_DETECT_HOURS		0
#define INTRUDER_DETECT_MINUTES		30
#define INTRUDER_LOCKOUT_DAYS		0
#define INTRUDER_LOCKOUT_HOURS		0
#define INTRUDER_LOCKOUT_MINUTES	15
#define INTRUDER_MAX_ATTEMPTS		7

/* Defines to be used by the accounting charge rate editing routines */
#define MARK_OFF		0x00
#define MARK_ON			0x01 /* Set if mark is currently on */
#define MARK_IS_ABOVE	0x02 /* Set if cursor is below marked half hour */
#define MARK_IS_BELOW	0x04 /* Set if cursor is above marked half hour */
#define MARK_IS_RIGHT	0x08 /* Set if cursor is left of marked day */
#define MARK_IS_LEFT	0x10 /* Set if cursor is right of marked day */

#define USE_FILE_SYSTEM     32323

#define VERSION_2_15 0x020f

#define ACCOUNTING_VERSION		1
#define SECURITY_VERSION		1
#define VIRTUAL_CONSOLE_VERSION	1

#define LOGIN_SCRIPT_SIZE	(16 * 1024)
#define AUTOEXEC_FILE_SIZE	(16 * 1024)
/* mpinegar, 08/02/90, added for edit of server.cfg file on 286 */
#define SERVER_CFG_FILE_SIZE	(16 * 1024)

/* Property types, used for routines that specify privileged users */
#define PROPERTY_SECURITY_EQUALS	1
#define PROPERTY_OPERATORS			2
#define PROPERTY_OBJ_SUPERVISORS	3
#define PROPERTY_MANAGERS			4

/** Flags whether an object is directly or indirectly managed **/
#define DIRECTLY_MANAGED            (BYTE)0x01
#define INDIRECTLY_MANAGED          (BYTE)0x02

typedef struct
{
	BYTE accountExpiresYear;
	BYTE accountExpiresMonth;
	BYTE accountExpiresDay;
	BYTE accountExpired;
	BYTE passwordExpiresYear;
	BYTE passwordExpiresMonth;
	BYTE passwordExpiresDay;
	BYTE passwordGraceLogins;
	WORD expirationInterval;
	BYTE graceReset;
	BYTE minimumPasswordLength;
	WORD maxConcurrentConnections;
	BYTE timeBitMap[42];
	BYTE lastLoginDate[6];
	BYTE restrictionFlags;
	BYTE filler;
	LONG maxDiskBlocks;
	WORD badLoginCount;
	LONG nextResetTime;
	BYTE badStationAddress[12];
} Restrictions;

typedef struct
{
	BYTE accountExpiresYear;
	BYTE accountExpiresMonth;
	BYTE accountExpiresDay;
	BYTE restrictionFlags;
	WORD expirationInterval;
	BYTE graceReset;
	BYTE minimumPasswordLength;
	WORD maxConcurrentConnections;
	BYTE timeBitMap[42];
	long balance;
	long creditLimit;
	LONG maxDiskBlocks;
	BYTE createHomeDir;
	BYTE filler1;
} DefaultRestrictions;

typedef struct
{
	long balance;						/* Account balance */
	long creditLimit;					/* Can't go below this amount */
} ACCT_BALANCE;

typedef struct
{
	WORD multiplier;
	WORD divisor;
} CHARGE_RATE;

typedef struct
{
	BYTE	daysMask;
	BYTE	halfHour;
	WORD	multiplier;
	WORD	divisor;
} CHARGE_CHANGE_ENTRY;

typedef struct
{
	LONG	changeTime;
	WORD	currentMultiplier;
	WORD	currentDivisor;
	CHARGE_CHANGE_ENTRY chargeChange[20];
} CHARGE_RATE_STRUCT;

typedef struct
{
	WORD allowedAttempts;
	WORD resetMinutes;
	WORD lockoutMinutes;
} LOCKOUT_THRESHOLD;

#endif /** NWSYSCON_H **/

