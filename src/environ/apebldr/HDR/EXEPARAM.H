#ifndef __exePARAM_H
#define __exePARAM_H

#include "parmmask.h"
#include "exserrec.h"
#include "seruid.h"
#include "exstring.h"

class EXEPARAM:public EXESERIUSRECORD, public PARAMMASK
	{
	private:
		EXESTRING exeName;
		SERUID    tempObjectUID;

	public:

		EXEPARAM(
			long newStatus,
			LPCSTR newName,
			LONG newTypeID,
			SERUID newDataFrom)
			{
			status = newStatus;
			typeID = newTypeID;
			dataFrom = newDataFrom;
			exeName = newName;
			tempObjectUID = NULLSERUID;

			if (TestObjConnectFlag())
				SetSlotFilledFlag();

			// Ensure that EXEFUNCINST::GetParameter will work properly
			if (this->TestConstantFlag())
				dataFrom.id = 0;
			}

		EXEPARAM():PARAMMASK()
			{
			}

		BOOL TestSlotFilledFlag()
			{
			return (status & PM_SLOTFILLED)?TRUE:FALSE;
			}

		void SetSlotFilledFlag()
			{
			status|=PM_SLOTFILLED;
			}

		LPCSTR GetName()
			{
			return exeName;
			}

		SERUID GetTempObjectUID()
			{
			return tempObjectUID;
			}

		void SetTempObjectUID(SERUID theUID)
			{
			tempObjectUID = theUID;
			return;
			}

	};



#endif
