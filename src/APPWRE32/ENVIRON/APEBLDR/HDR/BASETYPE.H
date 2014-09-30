#if !defined( __BASETYPE_H )
#define __BASETYPE_H

#include "seruid.h"
#include "compuid.h"
#include "datasmgr.h"
#include "system.h"
#include "mtypelst.h"
#include "intdebug.h"

#define TYPE_BADTYPE ((ADMNPROC)-1)


class BASETYPE:public DATASMGR
	{
        friend long ALMAPI AObjGetObjectCount(
		ATYPEID typeID);

        friend LONG ALMAPI AObjEdit(
		AOBJECTID theObject);

        friend BOOL ALMAPI AObjExport(
		AOBJECTID theObject);

        friend BOOL ALMAPI AObjImport(
		AOBJECTID theObject);

        friend ATYPEID ALMAPI ATypeGetNext(
		LPLONG theGetKey);

private:

  BASETYPE(void);
 
protected:
	ATYPEID	 typeID;
	long		 instCount;

private:	
	ADMNPROC admnProc;

protected:
  long     runTimeData;

public:

	BASETYPE(
		LPBASETYPE        srcTypePtr,
		long              theInstCount,
		LPRUNTIMERECLIST  pDatasRecList):
			DATASMGR(pDatasRecList, srcTypePtr)
		{
		typeID = srcTypePtr->typeID;
		instCount = theInstCount;
		admnProc = 0;
    runTimeData = 0;
		}

	BASETYPE(
		COMPUID           theTypeID,
		long              theInstCount,
		LPRUNTIMERECLIST  pDatasRecList):
			DATASMGR(pDatasRecList)
		{
		typeID = theTypeID;
		instCount = theInstCount;
		admnProc = 0;
    runTimeData = 0;
		}

	BASETYPE(
		void  *  objptr,
    LPRUNTIMERECLIST   pDatasRecList):
			DATASMGR(objptr, pDatasRecList)
		{
		admnProc = (ADMNPROC)0;
    runTimeData = 0;
		}


		~BASETYPE(
			void)
			{
			}

		ATYPEID GetTypeID()
			{
			return typeID;
			}

		long GetTypeCount()
			{
			return instCount;
			}

    void SetAdmnProc(ADMNPROC theProc)
      {
      admnProc = theProc;
      }

    ADMNPROC GetAdmnProc()
      {
			//admnProc will always be valid during runtime
			if(!admnProc)
				{
				I_ASSERT(pTheSystem->GetMasterTypeList());
				admnProc = pTheSystem->GetMasterTypeList()->GetAdmnProc(typeID);
				if(!admnProc)
					admnProc = TYPE_BADTYPE;
				}
      return (admnProc == TYPE_BADTYPE)?NULL:admnProc;
      }

    long GetRunTimeData(void)
      {
      return this->runTimeData;
			}

    BOOL SetRunTimeData(
      long newData)
      {
      this->runTimeData = newData;
      return TRUE;
      }

		long IncrTypeCount()
			{
			instCount++;
			return instCount;
			}

		long DecrTypeCount()
			{
      if(instCount > 0)
				instCount--;
			return instCount;
			}


		BOOL TestBadTypeFlag()
			{
			return (admnProc == TYPE_BADTYPE)?TRUE:FALSE;
			}
	};

#endif
