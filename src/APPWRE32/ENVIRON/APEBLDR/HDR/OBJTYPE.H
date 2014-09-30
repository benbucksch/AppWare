#if !defined( __OBJTYPE_H )
#define __OBJTYPE_H

#include "classdef.h"
#include "dbstring.h"
#include "serrec.h"
#include "basetype.h"
#include "sereclib.h"

class OBJTYPE:public SERIUSRECORD, public BASETYPE
	{

        friend long ALMAPI ATypeGetName(
		ATYPEID theType,
		LPSTR  theName,
		long	 nameSize);

        friend BOOL ALMAPI AEvtCheckForMethod (
		ATYPEID  theType,
		AEVENTID selector);

        friend BOOL ALMAPI AEvtCheckForMethod (
		ATYPEID  theType,
		AEVENTID selector);

        friend BOOL ALMAPI AEvtCallMethod (
		AOBJECTID sourceObject,
		AOBJECTID targetObject,
		ATYPEID   targetType,
		AEVENTID  selector,
		LONG     methodParam);

	private:
		DBSTRING 	typeName;
		LPOBJDESC pObjDesc;
		long			totalObjCount;  //count of real + alias objects
		//NOTE: the instCount in the BASETYPE class is used to hold the
		//			count of the real objects.
    
    long requiredCount;

	public:
		OBJTYPE(
			ATYPEID		typeID,
			LPOBJDESC pObjDef);

		OBJTYPE(void * thisPtr);

		~OBJTYPE(
			void);

		LPCSTR GetTypeName()
			{
			return typeName;
			}

		static OBJTYPE * GetObjectType(
			ATYPEID typeID);

		short PostReadInit();

		short CheckObjectVersion(
			SERUID  objectUID,
			long messages);

		long GetRealObjCount()
			{
			return GetTypeCount();
			}

		long GetTotalObjCount()
			{
			return totalObjCount;
			}

		//returns the total number of objects of the type.
		long IncObjCount(
			BOOL realObj)
			{
			totalObjCount++;
			if(realObj)
				IncrTypeCount();
			return totalObjCount;
			}

		//returns the total number of remaining objects of the type.
		long DecObjCount(
			BOOL realObj)
			{
			if(totalObjCount > 0)
      	{
				totalObjCount--;
				if(realObj)
					DecrTypeCount();
				}
			return totalObjCount;
			}

    LPOBJDESC GetObjDesc(void)
      {
      return pObjDesc;
      }
      
    void IncRequiredCount()
    	{
      requiredCount++;
      }
      
    void DecRequiredCount()
    	{
      if(requiredCount > 0)
      	requiredCount--;
      else
      	I_ASSERT(FALSE);
      }
      
    long GetRequiredCount()
    	{
      return requiredCount;
      }

	};


//type records help by the subject
class SUBTYPE:public SERIUSRECORD
	{
	public:
		SERUID objTypeUID;
		ATYPEID typeID;
		long objCount;

		SUBTYPE(
			ATYPEID theTypeID,
			SERUID realTypeUID);

		SUBTYPE(
			void * thisPtr)
			{
			}

		LPOBJTYPE GetObjTypePtr()
			{
			return (LPOBJTYPE)GetObject(objTypeUID);
			}

		long GetObjCount();

		//returns the incremented objCount;
		long IncObjCount(
			BOOL realObject)
			{
			objCount++;
			GetObjTypePtr()->IncObjCount(realObject);
			return objCount;
			}

		long DecObjCount(
			BOOL realObject);

		short PostReadInit();

		};


#endif
