#ifndef __MSUPPORT_H_
#define __MSUPPORT_H_

#include "parse.h"

#ifdef __cplusplus
extern "C" {
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		*************** PROTOTYPES ********************
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
extern	void	UpperToken(char *token);
extern	void	CopyToken(MXGLOBALS *mxg,char *destinationString, char *token, char tokenNumber);
extern	int	SaveString(char *str,PSPACE *p);
extern	void	FreeString(int strID, PSPACE *p);
extern	void	SaveToken(char tokenNumber, PSPACE *p);
extern	void	SaveQuotedToken(PSPACE *p);
extern	void	SaveQuotedString(PSPACE *p);
extern	int	Evaluate(char *leftOp, enum RELTYPE rel, char *rightOp,int byValue);
extern	int	ParseLine(PARSEFLAG flag, char *line, PSPACE *p);
extern	WORD	GroupMember(char *groupName, PSPACE *p);
extern	void	ConcatStringTokens(int leftTokenID, int rightTokenID, PSPACE *p);
extern	void	ArithOperation(PSPACE *p);
extern	void	ShiftOperation(PSPACE *p);
extern	void	CheckMailbox(PSPACE *p);
extern	int	CheckNDSAttribute(PSPACE *p);
extern int AvailableDrive(int relativeDriveNumber);
extern void DosGetDateTime(MYDATETIME *now);
extern int  DaysUntilPasswordExpires(PSPACE *p);
extern int	IsAccessServerPresent(void);

/*  CAUTION!!!!  This same structure is defined in syscon.  If the structure
				 changes, then the change must be made here and in syscon.
*/
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
	DWORD maxDiskBlocks;
	WORD badLoginCount;
	DWORD nextResetTime;
	BYTE badStationAddress[12];
	WORD daysNotice;
} Restrictions;

#ifdef __cplusplus
}
#endif

#endif
