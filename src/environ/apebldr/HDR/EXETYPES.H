#ifndef __EXETYPES_H
#define __EXETYPES_H

#include "exserrec.h"
#include "basetype.h"
#include "system.h"

class EXEOBJTYPE:public BASETYPE, public EXESERIUSRECORD
	{
	private:
		RUNTIMEUID	exeObjInstList;

    EXEOBJTYPE();

	public:

		EXEOBJTYPE(
			LPBASETYPE	srcTypePtr,
			long		instCount);

    EXEOBJTYPE(
	    void *  thisPtr):
		    BASETYPE(thisPtr, pTheSystem->GetFileObject()->GetExeSeriusBlocks())
	    {
	    }

		~EXEOBJTYPE();

		RUNTIMEUID GetExeObjInstList()
			{
			return exeObjInstList;
			}

		static EXEOBJTYPE * GetObjectType(
			ATYPEID typeID);

		BOOL StorePersistentObjects(
      HANDLE hFile);

		BOOL CheckObjectVersions(
			void);

	};

#endif
