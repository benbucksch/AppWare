/*
===============================================================================

MkLnkPar.c: SequeLink Client Connection Definitions
  Copyright(c) Gnosis NV 1993.  All rights reserved
  Version 1.00.00

Description
  This file is part of the published source files for the SequeLink Client.
  It contains all the necessary connection parameter calls to be used when
  developing SequeLink Client applications as well as Environment Modules.

  This file should be included in the Client application
  or Environment Module.

===============================================================================
*/

#ifdef _WINDOWS
#include "windows.h"
#endif

#if defined (macintosh) || defined (_WINDOWS)
#include <memory.h>
#endif

#include <string.h>
#include "SequeLnk.h"

/*------------------------------------------------------------*
	MakeLinkParams the LinkParam String to be used in the 
	SQConnect call. The string is copied in connectString
	which has to be allocated (& deallocated) by the caller
	of this function.
	The parameters are :
		SSPNetDriver netWorkLink			I: Type of Network Link
		SSPName 		host_node 				I: Network address of the host on 
																		which the SequeLink service runs
    SSPName			partner_lu_name		I: Connection Parameters
		SSPName			service						I: Name of the SequeLink service
		SSPName			APPC_mode					I: Mode for APPC
		SSPName			host_logon				I: Used Id on the host
		SSPName			host_password			I: Password on the host
		SSPString		connectString			0: Connect string 
 *------------------------------------------------------------*/
void MakeLinkParams(network, 
										host_node, 
										partner_lu_name, 
										service,
										APPC_mode, 
										host_logon, 
										host_password,
										connectString)
SSPNetDriver	network;
SSPName				host_node;
SSPName 			partner_lu_name;
SSPName 			service;
SSPName 			APPC_mode;
SSPName 			host_logon;
SSPName 			host_password;
SSPString 		connectString;
{
	SSPString 	temp_LP = connectString;
	SSPName 	params[6];
	SSPCount 	nrParams;

	switch(network)
		{
#ifdef VAX
			case DECnet:
				params[0] = host_node;
				params[1] = service;
				params[2] = host_logon;
				params[3] = host_password;
				nrParams = 4;
				break;
#else 
			case DECnet:
				params[0] = service;
				params[1] = host_node;
				params[2] = host_logon;
				params[3] = host_password;
				nrParams = 4;
				break;
#endif

			case AppleTalk:
			case ADSP:
				params[0] = service;
				params[1] = host_node;
				params[2] = host_logon;
				params[3] = host_password;
				nrParams = 4;
				break;

			case TCP:
			case PCS:
			case SPX:
				params[0] = host_node;
				params[1] = service;
				params[2] = host_logon;
				params[3] = host_password;
				nrParams = 4;
				break;

			case CPIC:
			case NetBIOS:
				params[0] = host_node;
				params[1] = host_logon;
				params[2] = host_password;
				params[3] = service;
				nrParams = 4;
				break;

			case APPC:
				params[0] = host_node;
				params[1] = partner_lu_name;
				params[2] = service;
				params[3] = APPC_mode;
				params[4] = host_logon;
				params[5] = host_password;
				nrParams = 6;
				break;

			default:
				params[0] = service;
				params[1] = host_node;
				params[2] = host_logon;
				params[3] = host_password;
				nrParams = 4;
				break;
		}
	ConcatLinkParams(params,nrParams,connectString);
}


/*------------------------------------------------------------*
	ConcatLinkParams concatenates the array if strings in Params in the
	format of a SequeLink connect string. 
	The parameters are :
		SSPName 		Params[]			I: Array of poiters to strings  
											   				that have to be concatenated
    SSPCount		nrParams 			I: Number of parameters
		SSPString		connectString	0: Connect string 
 *------------------------------------------------------------*/
void ConcatLinkParams(Params,nrParams,connectString)
SSPName 	Params[];
SSPCount 	nrParams;
SSPString 	connectString;
{
	SSPString	temp_LP = connectString;
	short			index;

	for (index = 0; index < nrParams; index++)
		{
			temp_LP[0] = (char)strlen(Params[index]);
			strcpy(&temp_LP[1], Params[index]);
			temp_LP += temp_LP[0]+1;
		}
	temp_LP[0] = '\0';
}