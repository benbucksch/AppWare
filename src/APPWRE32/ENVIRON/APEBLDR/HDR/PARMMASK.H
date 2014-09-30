
#ifndef __PARMMASK_H
#define __PARMMASK_H

#include "seruid.h"
#include "dbstring.h"

#define PAR_SELECTED 				0x00000001
#define PAR_FROMOBJ  				0x00000002
#define PAR_FROMPARAM 			0x00000004
#define PAR_REQUIRED  			0x00000008
#define PAR_OPTIONAL  			0x00000010
#define PAR_NOTLINKED 			0x00000020
#define PAR_OUTPUT 					0x00000040
#define PAR_INPUT 					0x00000080
#define PAR_SINGLEQUOTED		0x00000100
#define PM_CONSTANT 				0x00000200
#define PM_SLOTFILLED	    	0x00000400
#define PAR_DELETED					0x00000800
#define PAR_CANBECONSTANT 	0x00001000
#define PAR_BADPARAM      	0x00002000
#define PAR_TEMPOBJ					0x00004000
#define PAR_TEXTLEGAL				0x00008000

#define PAR_TYPENEEDSADDING	0x00010000

class PARAMMASK
	{
	protected:
		LONG		 	typeID;
		DBSTRING	name;
		long			status;
		SERUID 		dataFrom;


	public:

		PARAMMASK(void * thisPtr):name(&name)
			{
			}

		PARAMMASK()
			{
			}

		LONG GetTypeID()
		{
		return typeID;
		}


		long GetStatus()
			{
			return status;
			}

		SERUID GetDataFromUID()
			{
			return dataFrom;
			}

		void SetDataFromUID(
			SERUID newUID)
			{
			dataFrom = newUID;
			}

		BOOL TestObjConnectFlag()
			{
				return (status &  PAR_FROMOBJ)?TRUE:FALSE;
			}

		BOOL TestConstantFlag()
			{
			return (status & PM_CONSTANT)?TRUE:FALSE;
			}

		BOOL TestParamConnectFlag()
			{
			return (status & PAR_FROMPARAM)?TRUE:FALSE;
			}

		BOOL TestLinkFlag()
			{
			return (status & PAR_NOTLINKED)?FALSE:TRUE;
			}

		BOOL TestDeletedFlag()
			{
			return (status & PAR_DELETED)?TRUE:FALSE;
			}

		BOOL TestRequiredFlag()
			{
			return (status & PAR_REQUIRED)?TRUE:FALSE;
			}

	};

#endif
