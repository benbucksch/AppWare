#include <nwcaldef.h>
#include "mx.h"
#include "PARSEUT.H"

/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#include "features.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#undef BYTE
#undef WORD
#undef DWORD
#undef LONG
#include <dbcsmac.h>
#include "errors.h"
#include "msupport.h"
#include "cmds.h"
#include "cmdline.h"
#include "resource.h"

/*-----------------------------------------------------------------------------
 * start of M
 */
#pragma argsused
int ActionProcedure(int action,void *globals)
{
	int abort = 0, unmatch = 0;

	switch(action)
	{
		case   0:
		{	SaveToken(0,(PSPACE *)globals);
		}	break;
		case   1:
		{	/* VarName */ SaveToken(0,(PSPACE *)globals);
		}	break;
		case   2:
		{	CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedName, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)], 1);
		}	break;
		case   3:
		{	SaveQuotedToken((PSPACE *)globals);
		}	break;
		case   4:
		{	SaveQuotedString((PSPACE *)globals);
		}	break;
		case   5:
		{	SetMVal(&((PSPACE *)globals)->mxg,10 + TVal(&((PSPACE *)globals)->mxg,2) - '0');
		}	break;
		case   6:
		{	SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1) - '0');
		}	break;
		case   7:
		{	SetMVal(&((PSPACE *)globals)->mxg,AvailableDrive(10 + TVal(&((PSPACE *)globals)->mxg,3) - '0'));
		}	break;
		case   8:
		{	SetMVal(&((PSPACE *)globals)->mxg,AvailableDrive(20 + TVal(&((PSPACE *)globals)->mxg,3) - '0'));
		}	break;
		case   9:
		{	SetMVal(&((PSPACE *)globals)->mxg,AvailableDrive(TVal(&((PSPACE *)globals)->mxg,2) - '0'));
		}	break;
		case  10:
		{	SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  11:
		{	SetMVal(&((PSPACE *)globals)->mxg,250 + TVal(&((PSPACE *)globals)->mxg,3) - '0');					
		}	break;
		case  12:
		{	SetMVal(&((PSPACE *)globals)->mxg,200+
(TVal(&((PSPACE *)globals)->mxg,2)-'0')*10+TVal(&((PSPACE *)globals)->mxg,3)-'0');	
		}	break;
		case  13:
		{	SetMVal(&((PSPACE *)globals)->mxg,100+
(TVal(&((PSPACE *)globals)->mxg,2)-'0')*10+TVal(&((PSPACE *)globals)->mxg,3)-'0');	
		}	break;
		case  14:
		{	SetMVal(&((PSPACE *)globals)->mxg,(TVal(&((PSPACE *)globals)->mxg,1) - '0')*10 + TVal(&((PSPACE *)globals)->mxg,2)-'0');	
		}	break;
		case  15:
		{	SetMVal(&((PSPACE *)globals)->mxg,(TVal(&((PSPACE *)globals)->mxg,1) - '0'));							
		}	break;
		case  16:
		{	lstrcpy(((PSPACE *)globals)->parsedString, TChP(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  17:
		{	
										/* DSObject */
										lstrcpy(((PSPACE *)globals)->parsedUser, ((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,0)]);
		}	break;
		case  18:
		{	
												/* NameOfObject */
												if(((PSPACE *)globals)->NDSorBindery != BINDERY)
													((PSPACE *)globals)->NDSorBindery = ((PSPACE *)globals)->NDSPresent ? NDS : BINDERY;
												
		}	break;
		case  19:
		{	
												/* CheckUser */
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedUser, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,0)], 0);
										  			SaveCmdLineArg(((PSPACE *)globals)->parsedUser, 1,(PSPACE *)globals);
		}	break;
		case  20:
		{	
												/* Bindery Server */
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedServer, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,0)],0);
												SaveCmdLineArg(((PSPACE *)globals)->parsedServer, 0,(PSPACE *)globals);	
		}	break;
		case  21:
		{	((PSPACE *)globals)->ident[0] = 0;
							 DosGetDateTime(&((PSPACE *)globals)->now);

		}	break;
		case  22:
		{	SetMVal(&((PSPACE *)globals)->mxg,IsAccessServerPresent());
		lstrcpy(((PSPACE *)globals)->ident, IsAccessServerPresent() ? "1" : "0"); 
		}	break;
		case  23:
		{	DSALMGetMessage((((PSPACE *)globals)->now.hours<12?TIME_AM:TIME_PM),
((PSPACE *)globals)->ident,sizeof(((PSPACE *)globals)->ident));	
		}	break;
		case  24:
		{	DSALMGetMessage(((PSPACE *)globals)->dayNames[((PSPACE *)globals)->now.weekday],
((PSPACE *)globals)->ident,sizeof(((PSPACE *)globals)->ident));			
		}	break;
		case  25:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.day);							
		}	break;
		case  26:
		{	wsprintf(((PSPACE *)globals)->ident, "%u", ((PSPACE *)globals)->errorLevel);							
		}	break;
		case  27:
		{	lstrcpy(((PSPACE *)globals)->ident,((PSPACE *)globals)->serverName);          	             	
		}	break;
		case  28:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->fullName);										
		}	break;
		case  29:
		{	DSALMGetMessage((((PSPACE *)globals)->now.hours < 12? MORNING:
(((PSPACE *)globals)->now.hours < 18? AFTERNOON: EVENING)),((PSPACE *)globals)->ident,sizeof(((PSPACE *)globals)->ident));				
		}	break;
		case  30:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.hours);							
		}	break;
		case  31:
		{	wsprintf(((PSPACE *)globals)->ident, "%d", ((PSPACE *)globals)->now.hours == 0? 12:
								(((PSPACE *)globals)->now.hours > 12? ((PSPACE *)globals)->now.hours - 12: ((PSPACE *)globals)->now.hours));		
		}	break;
		case  32:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->lastName);										
		}	break;
		case  33:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->loginName);										
		}	break;
		case  34:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->aliasContext);										
		}	break;
		case  35:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->loginContext);									
		}	break;
		case  36:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->machine);										
		}	break;
		case  37:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.minutes);						
		}	break;
		case  38:
		{	DSALMGetMessage(((PSPACE *)globals)->monthNames[((PSPACE *)globals)->now.month-1],
((PSPACE *)globals)->ident,sizeof(((PSPACE *)globals)->ident));
		}	break;
		case  39:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.month);							
		}	break;
		case  40:
		{	wsprintf(((PSPACE *)globals)->ident, "%01d", ((PSPACE *)globals)->now.weekday+1);					
		}	break;
		case  41:
		{	wsprintf(((PSPACE *)globals)->ident, "%02.2X%02.2X%02.2X%02.2X",
								(int)((PSPACE *)globals)->netAddress[0], (int)((PSPACE *)globals)->netAddress[1],
								(int)((PSPACE *)globals)->netAddress[2], (int)((PSPACE *)globals)->netAddress[3]);				
		}	break;
		case  42:
		{	CheckMailbox((PSPACE *)globals);
					  		 lstrcpy(((PSPACE *)globals)->ident,((PSPACE *)globals)->mailStatus);										
		}	break;
		case  43:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->osVersion);										
		}	break;
		case  44:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->os);												
		}	break;
		case  45:
		{	((PSPACE *)globals)->daysLeft = DaysUntilPasswordExpires((PSPACE *)globals);
							 wsprintf(((PSPACE *)globals)->ident, "%d", ((PSPACE *)globals)->daysLeft);								
		}	break;
		case  46:
		{	wsprintf(((PSPACE *)globals)->ident, "%02.2X%02.2X%02.2X%02.2X%02.2X%02.2X",
								(int)((PSPACE *)globals)->netAddress[4 + 0],(int)((PSPACE *)globals)->netAddress[4 + 1],
								(int)((PSPACE *)globals)->netAddress[4 + 2],(int)((PSPACE *)globals)->netAddress[4 + 3],
								(int)((PSPACE *)globals)->netAddress[4 + 4],(int)((PSPACE *)globals)->netAddress[4 + 5]);		
		}	break;
		case  47:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->requesterVersion);							
		}	break;
		case  48:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->requesterContext);							
		}	break;
		case  49:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.seconds);						
		}	break;
		case  50:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->shellVersion);									
		}	break;
		case  51:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.year % 100);					
		}	break;
		case  52:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->sMachine);										
		}	break;
		case  53:
		{	wsprintf(((PSPACE *)globals)->ident, "%u", ((PSPACE *)globals)->station);								
		}	break;
		case  54:
		{	wsprintf(((PSPACE *)globals)->ident, "%lX", ((PSPACE *)globals)->userObjectID);						
		}	break;
		case  55:
		{	wsprintf(((PSPACE *)globals)->ident, "%02d", ((PSPACE *)globals)->now.year);							
		}	break;
		case  56:
		{	lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,0)]); 						
		}	break;
		case  57:
		{	/* ((PSPACE *)globals)->identifier */
										 NWLSTRUPR(((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,1)]);
										 ((PSPACE *)globals)->tmp = GetEnvironmentString(((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,1)]);
										 lstrcpy(((PSPACE *)globals)->ident, ((PSPACE *)globals)->tmp==NULL? "": ((PSPACE *)globals)->tmp);
										 FreeString(TVal(&((PSPACE *)globals)->mxg,1),(PSPACE *)globals);
										 SetMVal(&((PSPACE *)globals)->mxg,SaveString(((PSPACE *)globals)->ident,(PSPACE *)globals));			
		}	break;
		case  58:
		{	SetMVal(&((PSPACE *)globals)->mxg,SaveString(((PSPACE *)globals)->ident,(PSPACE *)globals));			
		}	break;
		case  59:
		{	if(CheckNDSAttribute((PSPACE *)globals))
															unmatch = 1;
		}	break;
		case  60:
		{	SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  61:
		{	ConcatStringTokens(1, 3,(PSPACE *)globals);
		}	break;
		case  62:
		{	ConcatStringTokens(0, 1,(PSPACE *)globals);
		}	break;
		case  63:
		{	ConcatStringTokens(0, 1,(PSPACE *)globals);
		}	break;
		case  64:
		{	((PSPACE *)globals)->operation = 0;

		}	break;
		case  65:
		{	((PSPACE *)globals)->operation = *TChP(&((PSPACE *)globals)->mxg,0);
									 	 SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  66:
		{	ArithOperation((PSPACE *)globals);
		}	break;
		case  67:
		{	((PSPACE *)globals)->operation = 0;

		}	break;
		case  68:
		{	((PSPACE *)globals)->operation = *TChP(&((PSPACE *)globals)->mxg,0);
									 	 SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  69:
		{	ArithOperation((PSPACE *)globals);
		}	break;
		case  70:
		{	((PSPACE *)globals)->operation = *TChP(&((PSPACE *)globals)->mxg,0);
									 	 SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case  71:
		{	ShiftOperation((PSPACE *)globals);
		}	break;
		case  72:
		{	((PSPACE *)globals)->parsedString[0] = 0;

		}	break;
		case  73:
		{	lstrcpy(((PSPACE *)globals)->parsedString, ((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,1)]);
										 FreeString(TVal(&((PSPACE *)globals)->mxg,1),(PSPACE *)globals);
		}	break;
		case  74:
		{	CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedString+lstrlen(((PSPACE *)globals)->parsedString),
&((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,0)],0);
		}	break;
		case  75:
		{	CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedString+lstrlen(((PSPACE *)globals)->parsedString),
&((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,0)],0);
							 		 strcat(((PSPACE *)globals)->parsedString, TVal(&((PSPACE *)globals)->mxg,2) == 0? "%":
((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,2)]);
							 		 FreeString(TVal(&((PSPACE *)globals)->mxg,2),(PSPACE *)globals);

		}	break;
		case  76:
		{	((PSPACE *)globals)->parsedString[0] = 0;
		}	break;
		case  77:
		{	SetMVal(&((PSPACE *)globals)->mxg,1);
		}	break;
		case  78:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_U);

		}	break;
		case  79:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_E);		
		}	break;
		case  80:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_E);		
		}	break;
		case  81:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  82:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  83:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  84:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_GE);		
		}	break;
		case  85:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_LE);		
		}	break;
		case  86:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_G);		
		}	break;
		case  87:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_L);		
		}	break;
		case  88:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_GE);	
		}	break;
		case  89:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_G);		
		}	break;
		case  90:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_LE);	
		}	break;
		case  91:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_L);		
		}	break;
		case  92:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  93:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  94:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_E);		
		}	break;
		case  95:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  96:
		{	SetMVal(&((PSPACE *)globals)->mxg,REL_NE);		
		}	break;
		case  97:
		{	SetMVal(&((PSPACE *)globals)->mxg,
GroupMember(((PSPACE *)globals)->parsedString,(PSPACE *)globals));		
		}	break;
		case  98:
		{	SetMVal(&((PSPACE *)globals)->mxg,
(!GroupMember(((PSPACE *)globals)->parsedString,(PSPACE *)globals)));		
		}	break;
		case  99:
		{	lstrcpy(((PSPACE *)globals)->leftStr, ((PSPACE *)globals)->parsedString);						

		}	break;
		case 100:
		{	SetMVal(&((PSPACE *)globals)->mxg,
Evaluate(((PSPACE *)globals)->leftStr, (RELTYPE)TVal(&((PSPACE *)globals)->mxg,
2), ((PSPACE *)globals)->parsedString,
 TVal(&((PSPACE *)globals)->mxg,0) || TVal(&((PSPACE *)globals)->mxg,3)));	
		}	break;
		case 101:
		{	SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));							
		}	break;
		case 102:
		{	SetMVal(&((PSPACE *)globals)->mxg,IsAccessServerPresent());	 
		}	break;
		case 103:
		{	SetMVal(&((PSPACE *)globals)->mxg,~IsAccessServerPresent()); 
		}	break;
		case 104:
		{	SetMVal(&((PSPACE *)globals)->mxg,!TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case 105:
		{	SetMVal(&((PSPACE *)globals)->mxg,
(TVal(&((PSPACE *)globals)->mxg,0) && !TVal(&((PSPACE *)globals)->mxg,1)));
		}	break;
		case 106:
		{	SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,1));
		}	break;
		case 107:
		{	
SetMVal(&((PSPACE *)globals)->mxg,(TVal(&((PSPACE *)globals)->mxg,0) ||
 TVal(&((PSPACE *)globals)->mxg,1)));
		}	break;
		case 108:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);
		}	break;
		case 109:
		{	((PSPACE *)globals)->parsedString[0] = 0xff;
												 ((PSPACE *)globals)->parsedString[1] = 0;

		}	break;
		case 110:
		{	
												/* server/user ; */
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedServer, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1);
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedUser, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,3)],3); 	
		}	break;
		case 111:
		{	
												/* server/user eol */
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedServer, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1);
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedUser, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,3)],3); 	
		}	break;
		case 112:
		{	
												/* server eol */
												CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedServer, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1);
												((PSPACE *)globals)->parsedUser[0] = 0; 				
		}	break;
		case 113:
		{	((PSPACE *)globals)->parsedServer[0] = 0;
												 ((PSPACE *)globals)->parsedUser[0] = 0;
												 ((PSPACE *)globals)->parsedTree[0] = 0;
												 ((PSPACE *)globals)->parsedProfile[0] = 0;				
		}	break;
		case 114:
		{	((PSPACE *)globals)->parsedServer[0] = 0;
												 ((PSPACE *)globals)->parsedUser[0] = 0;
												 ((PSPACE *)globals)->parsedTree[0] = 0;
												 ((PSPACE *)globals)->parsedProfile[0] = 0;				
		}	break;
		case 115:
		{	CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedName,&((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,0)], 0);
		}	break;
		case 116:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);
		}	break;
		case 117:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);
		}	break;
		case 118:
		{	((PSPACE *)globals)->parsedCondition = 1;
		}	break;
		case 119:
		{	((PSPACE *)globals)->parsedCondition = 0;
		}	break;
		case 120:
		{	/* SetSpec */
												 lstrcpy(((PSPACE *)globals)->parsedName, ((PSPACE *)globals)->tokenString[TVal(&((PSPACE *)globals)->mxg,1)]);
									 			 FreeString(TVal(&((PSPACE *)globals)->mxg,1),(PSPACE *)globals);

		}	break;
		case 121:
		{	if (TVal(&((PSPACE *)globals)->mxg,0) > MAX_SCRIPT_ARGS)
													unmatch = 1;
												else
													SetMVal(&((PSPACE *)globals)->mxg,TVal(&((PSPACE *)globals)->mxg,0));	
		}	break;
		case 122:
		{	((PSPACE *)globals)->parsedCondition = 1;

		}	break;
		case 123:
		{	((PSPACE *)globals)->parsedCondition = -TVal(&((PSPACE *)globals)->mxg,1);	
		}	break;
		case 124:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1); 	
		}	break;
		case 125:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1); 	
		}	break;
		case 126:
		{	CopyMemory(((PSPACE *)globals)->parsedString, TChP(&((PSPACE *)globals)->mxg,
1), TVal(&((PSPACE *)globals)->mxg,1));
												 ((PSPACE *)globals)->parsedString[TVal(&((PSPACE *)globals)->mxg,1)] = 0;
		}	break;
		case 127:
		{	CopyMemory(((PSPACE *)globals)->parsedString, TChP(&((PSPACE *)globals)->mxg,1), TVal(&((PSPACE *)globals)->mxg,1));
												 ((PSPACE *)globals)->parsedString[TVal(&((PSPACE *)globals)->mxg,1)] = 0;
		}	break;
		case 128:
		{	((PSPACE *)globals)->parsedCmdType = CMD_UNKNOWN;

		}	break;
		case 129:
		{	((PSPACE *)globals)->parsedCmdType = CMD_NOTHING;		
		}	break;
		case 130:
		{	((PSPACE *)globals)->parsedCmdType = CMD_NOTHING;		
		}	break;
		case 131:
		{	((PSPACE *)globals)->parsedCmdType = CMD_LABEL;		
		}	break;
		case 132:
		{	/* ATTACH AttachSpec */
													((PSPACE *)globals)->attachNDS = BINDERY;
													((PSPACE *)globals)->parsedCmdType = CMD_ATTACH;		
		}	break;
		case 133:
		{	/* ATTACHB AttachSpec */
													((PSPACE *)globals)->attachNDS = BINDERY;
													((PSPACE *)globals)->parsedCmdType = CMD_ATTACH;		
		}	break;
		case 134:
		{	((PSPACE *)globals)->parsedCmdType = CMD_BREAK;		
		}	break;
		case 135:
		{	((PSPACE *)globals)->parsedCmdType = CMD_COMSPEC;		
		}	break;
		case 136:
		{	((PSPACE *)globals)->parsedCmdType = CMD_CONTEXT;	
		}	break;
		case 137:
		{	((PSPACE *)globals)->parsedCmdType = CMD_CLEARSCREEN;
		}	break;
		case 138:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DISPLAY;
													 ((PSPACE *)globals)->parsedCondition = 1;				
		}	break;
		case 139:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DOSBREAK;	
		}	break;
		case 140:
		{	((PSPACE *)globals)->parsedCmdType = CMD_SET;
												 	 ((PSPACE *)globals)->parsedCondition = 1;				
		}	break;
		case 141:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DOSVERIFY;	
		}	break;
		case 142:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DRIVE;
												 	 ((PSPACE *)globals)->parsedString[0] = (char)TVal(&((PSPACE *)globals)->mxg,2);
		}	break;
		case 143:
		{	((PSPACE *)globals)->parsedCmdType = CMD_EXIT;		
		}	break;
		case 144:
		{	((PSPACE *)globals)->parsedCmdType = CMD_EXTERNAL;	
		}	break;
		case 145:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DISPLAY;
											 		 ((PSPACE *)globals)->parsedCondition = 0;				
		}	break;
		case 146:
		{	
											 	 	 ((PSPACE *)globals)->parsedCmdType = CMD_FIRE;
											 	 	 ((PSPACE *)globals)->parsedString[0] = (char)TVal(&((PSPACE *)globals)->mxg,4);
		}	break;
		case 147:
		{	((PSPACE *)globals)->parsedCmdType = CMD_GOTO;			
		}	break;
		case 148:
		{	((PSPACE *)globals)->parsedCmdType = CMD_IFDO;		
		}	break;
		case 149:
		{	((PSPACE *)globals)->parsedCmdType = CMD_IF;			
		}	break;
		case 150:
		{	((PSPACE *)globals)->parsedCmdType = CMD_END;			
		}	break;
		case 151:
		{	((PSPACE *)globals)->parsedCmdType = CMD_ELSE;			
		}	break;
		case 152:
		{	((PSPACE *)globals)->parsedCmdType = CMD_INCLUDE;	
		}	break;
		case 153:
		{	((PSPACE *)globals)->parsedCmdType = CMD_DISPLAY_LTIME;
													 ((PSPACE *)globals)->parsedCondition = 1;			
		}	break;
		case 154:
		{	((PSPACE *)globals)->parsedCmdType = CMD_SET;
											 		 ((PSPACE *)globals)->parsedCondition = 0;				
		}	break;
		case 155:
		{	
											 		 ((PSPACE *)globals)->parsedCmdType = CMD_MACHINE;		
		}	break;
		case 156:
		{	((PSPACE *)globals)->parsedCmdType = CMD_MAP;			
		}	break;
		case 157:
		{	((PSPACE *)globals)->parsedCmdType = CMD_NO_DEFAULT;	
		}	break;
		case 158:
		{	((PSPACE *)globals)->parsedCmdType = CMD_PAUSE;		
		}	break;
		case 159:
		{	((PSPACE *)globals)->parsedCmdType = CMD_COMPATIBLE; 
		}	break;
		case 160:
		{	((PSPACE *)globals)->parsedCmdType = CMD_SCRIPT_SERVER;	
		}	break;
		case 161:
		{	((PSPACE *)globals)->parsedCmdType = CMD_SET_TIME;
		}	break;
		case 162:
		{	((PSPACE *)globals)->parsedCmdType = CMD_SHIFT;		
		}	break;
		case 163:
		{	/* swap DOS ONLY */
													((PSPACE *)globals)->parsedCmdType = CMD_NOTHING;
													#ifdef DOS
														lstrcpy(swapPath, ((PSPACE *)globals)->parsedName);
														swap = TRUE;
													#endif
																							
		}	break;
		case 164:
		{	/* swap DOS ONLY */
													((PSPACE *)globals)->parsedCmdType = CMD_NOTHING;
													#ifdef DOS
														swap = TRUE;
													#endif
																								
		}	break;
		case 165:
		{	/* NOSWAP DOS ONLY */
													((PSPACE *)globals)->parsedCmdType = CMD_NOTHING;
													#ifdef DOS
														swap = FALSE;
														noSwap = TRUE;	/* never swap */
													#endif
																								
		}	break;
		case 166:
		{	((PSPACE *)globals)->parsedCmdType = CMD_WRITE;
													 lstrcpy(((PSPACE *)globals)->parsedString, "\r\n");		
		}	break;
		case 167:
		{	((PSPACE *)globals)->parsedCmdType = CMD_WRITE;
											 		 strcat(((PSPACE *)globals)->parsedString, "\r\n");		
		}	break;
		case 168:
		{	((PSPACE *)globals)->parsedCmdType = CMD_WRITE;		
		}	break;
		case 169:
		{	((PSPACE *)globals)->parsedName[0] = (char)TVal(&((PSPACE *)globals)->mxg,0);
													 ((PSPACE *)globals)->parsedName[1] = ':';
											 		 CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedName + 2, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1);	
		}	break;
		case 170:
		{	CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedName, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1);
		}	break;
		case 171:
		{	((PSPACE *)globals)->parsedName[0] = (char)TVal(&((PSPACE *)globals)->mxg,0);
							  	 					 ((PSPACE *)globals)->parsedName[1] = ':';
								 					 CopyToken(&((PSPACE *)globals)->mxg,
((PSPACE *)globals)->parsedName + 2, &((PSPACE *)globals)->lowerCommandLine[TLoc(&((PSPACE *)globals)->mxg,1)],1); 
		}	break;
		case 172:
		{	((PSPACE *)globals)->parsedName[0] = (char)TVal(&((PSPACE *)globals)->mxg,0);
							  	 					 ((PSPACE *)globals)->parsedName[1] = ':';
							  	 					 ((PSPACE *)globals)->parsedName[2] = 0;				
		}	break;
		case 173:
		{	((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,0);
		}	break;
		case 174:
		{	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);
		}	break;
		case 175:
		{	((PSPACE *)globals)->parsedRoot = 0;

		}	break;
		case 176:
		{	((PSPACE *)globals)->parsedRoot = 1;
		}	break;
		case 177:
		{	/* DISPLAY < > ON */
													((PSPACE *)globals)->parsedCmdType = MAP_DISPLAY_ON;	
		}	break;
		case 178:
		{	/* DISPLAY < > OFF */
													((PSPACE *)globals)->parsedCmdType = MAP_DISPLAY_OFF;	
		}	break;
		case 179:
		{	/* WARNINGS < > ON */
													((PSPACE *)globals)->parsedCmdType = MAP_ERRORS_ON;	
		}	break;
		case 180:
		{	/* WARNINGS < > OFF */
													((PSPACE *)globals)->parsedCmdType = MAP_ERRORS_OFF;
		}	break;
		case 181:
		{	
													/* INS (RootD) SDrive <=> (ADrive) TargetSearchPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 182:
		{	
													/* INS (RootD) SDrive <=> (ADrive) TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH;
													((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 183:
		{	
													/* INS (RootD) SDrive <=> (ADrive) */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
											 		((PSPACE *)globals)->parsedCmdType   = MAP_INS_SEARCH_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,2);			
		}	break;
		case 184:
		{	/* INS CHANGE Drive (TO) SDrive */
													((PSPACE *)globals)->parsedCmdType  = MAP_CHANGE_INS_DRIVE_TO_SDRIVE;
												 	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,4);
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 185:
		{	
													/* INS (RootD) Drive <=> ASDrive TargetSearchPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 186:
		{	
													/* INS (RootD) Drive <=> ASDrive TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH;
												 	((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 187:
		{	
													/* INS (RootD) Drive <=> ASDrive */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
											 		((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 188:
		{	
													/* RootD INS SDrive <=> (ADrive) TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH;
													((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,2);	
		}	break;
		case 189:
		{	
													/* RootD INS Drive <=> ASDrive TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_INS_SEARCH;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 190:
		{	/*  DEL [ ] Drive */
													((PSPACE *)globals)->parsedCmdType = MAP_DEL_DRIVE;
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 191:
		{	/* DEL SDrive */
													((PSPACE *)globals)->parsedCmdType = MAP_DEL_SEARCH;
												 	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 192:
		{	/* NEXT Space TargetPath */
													((PSPACE *)globals)->parsedCmdType   = MAP_NEXT_DRIVE;
												 	((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 193:
		{	/* CHANGE SDrive */
													((PSPACE *)globals)->parsedCmdType   = MAP_CHANGE_SEARCH;
											 		((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 194:
		{	/* CHANGE Drive (TO) SDrive */
													((PSPACE *)globals)->parsedCmdType   = MAP_CHANGE_DRIVE_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,3);		
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1);	
		}	break;
		case 195:
		{	/* CHANGE INS Drive (TO) SDrive */
													((PSPACE *)globals)->parsedCmdType  = MAP_CHANGE_INS_DRIVE_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,4);
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);		
		}	break;
		case 196:
		{	/* CHANGE Drive */
													((PSPACE *)globals)->parsedCmdType = MAP_CHANGE_DRIVE_TO_SDRIVE;
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1);
													((PSPACE *)globals)->parsedCondition = 0;		
		}	break;
		case 197:
		{	
													/* (RootD) SDrive <=> (ADrive) TargetSearchPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_SEARCH_TO_SDRIVE;
													((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 198:
		{	
													/* (RootD) SDrive <=> (ADrive) TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_SEARCH;
													((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 199:
		{	
													/* (RootD) Drive <=> ASDrive TargetSearchPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_SEARCH_TO_SDRIVE;
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1);		
		}	break;
		case 200:
		{	
													/* (RootD) Drive <=> ASDrive TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
													((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1); 
		}	break;
		case 201:
		{	/* Drive <=> ASDrive */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
											 		((PSPACE *)globals)->parsedCmdType   = MAP_ADD_SEARCH;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,0);
											 		((PSPACE *)globals)->parsedRoot = 0;				
		}	break;
		case 202:
		{	
													/* (RootD) Drive <=> TargetSearchPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE_TO_SDRIVE;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1); 
		}	break;
		case 203:
		{	/* (RootD) Drive <=> TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1);
		}	break;
		case 204:
		{	/* (RootD) Drive <=> */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
											 		((PSPACE *)globals)->parsedCmdType   = MAP_ADD_DRIVE;
											 		((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,1); 
		}	break;
		case 205:
		{	/* SDrive <=> (ADrive) */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
												 	((PSPACE *)globals)->parsedCmdType   = MAP_ADD_SEARCH;
												 	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);
												 	((PSPACE *)globals)->parsedRoot = 0;				
		}	break;
		case 206:
		{	/* Drive <=> TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
												   ((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,0);
												   ((PSPACE *)globals)->parsedRoot = 0;			
		}	break;
		case 207:
		{	/* ROOT TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
												   ((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);
												   ((PSPACE *)globals)->parsedRoot = 1;			
		}	break;
		case 208:
		{	/* TargetPath */
													((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
												   ((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);
												   ((PSPACE *)globals)->parsedRoot = 0;			
		}	break;
		case 209:
		{	/* ROOT [ ] Drive <=> */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
												   ((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
												   ((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,2);
												   ((PSPACE *)globals)->parsedRoot = 1;			
		}	break;
		case 210:
		{	/* Drive <=> */
													lstrcpy(((PSPACE *)globals)->parsedName, ".");
												   ((PSPACE *)globals)->parsedCmdType = MAP_ADD_DRIVE;
												   ((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,0);
												   ((PSPACE *)globals)->parsedRoot = 0;			
		}	break;
		case 211:
		{	/* SDrive */
													((PSPACE *)globals)->parsedCmdType = MAP_DISPLAY_SEARCH;
											   	((PSPACE *)globals)->parsedCondition = TVal(&((PSPACE *)globals)->mxg,0);		
		}	break;
		case 212:
		{	/* Drive */
													((PSPACE *)globals)->parsedCmdType = MAP_DISPLAY_DRIVE;
											   	((PSPACE *)globals)->parsedDrive = TVal(&((PSPACE *)globals)->mxg,0);		
		}	break;
		case 213:
		{	/* PreMapCommand: PhysicalVol (Remarks) */
													((PSPACE *)globals)->parsedPhysical = TRUE; 
								    	 		 	((PSPACE *)globals)->parsedCmdType = MAP_DISPLAY_ALL_DRIVES; 
		}	break;
		case 214:
		{	
													/* PhysicalVol MapCommand (Remarks) */
													((PSPACE *)globals)->parsedPhysical = TRUE; 
		}	break;
		case 215:
		{	/* MapCommand (Remarks) */
													((PSPACE *)globals)->parsedPhysical = FALSE;
		}	break;
	}
	return(abort? 2: (unmatch? 1: 0));
}

