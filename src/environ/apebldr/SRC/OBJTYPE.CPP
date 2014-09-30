#include "almsys.h"
#include "intalm.h"

#include "objtype.h"
#include "objdesc.h"
#include "progsys.h"
#include "subject.h"
#include "cfglibp.h"
#include "objinst.h"
#include "mdiclass.h"
#include "project.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	OBJTYPE
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  constructor
//
/////////////////////////////////////////////////////////////////////////////
OBJTYPE::OBJTYPE(
	ATYPEID		typeID,
	LPOBJDESC pObjDef):
		BASETYPE(typeID, 0, pTheSystem->GetFileObject()->GetSeriusBlocks()),
		typeName(pObjDef->pGetName())
	{

	this->typeID=typeID;
	this->typeName = pObjDef->pGetName();
	totalObjCount=0;
	this->pObjDesc = pObjDef;
	SetAdmnProc(NULL);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	OBJTYPE
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  constructor for record read from disk
//
/////////////////////////////////////////////////////////////////////////////
OBJTYPE::OBJTYPE(
	void *  thisPtr):
		BASETYPE(thisPtr, pTheSystem->GetFileObject()->GetSeriusBlocks()),
		typeName(&typeName)
	{
	SetAdmnProc(NULL);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PostReadInit
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  constructor for record read from disk
//
/////////////////////////////////////////////////////////////////////////////
short OBJTYPE::PostReadInit()
	{
	char buffer[256];

	static char message[]={"Could not find object type %s "
													"in the loaded configuration files.\r\n"};

	BOOL usedType =  (totalObjCount > 0 || GetDatasCount() > 0 ||
		GetRequiredCount() > 0);

	pObjDesc = cfgGetObjectDesc(this->typeID);
	if(!pObjDesc ||  (usedType && (GetAdmnProc() == NULL)))
		{
		//check again for usedType incase pObjDesc == NULL
		if(usedType)
			{
			//set just in case GetAdmnProc wasn't called
			SetAdmnProc(TYPE_BADTYPE);
			wsprintf(buffer, message, (LPCSTR)typeName);
			MDI::AddMessage(buffer, TRUE);
			//could not find object description in config info
			return PRJ_READWARNING | PRJ_READBADTYPE;
			}
		}

	return PRJ_READNOERROR;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~OBJTYPE
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  destructor
//
/////////////////////////////////////////////////////////////////////////////
OBJTYPE::~OBJTYPE()
	{
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetObjectType
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  destructor
//
/////////////////////////////////////////////////////////////////////////////
OBJTYPE * OBJTYPE::GetObjectType(
	ATYPEID typeID)
	{

	return
	 PROGSYSTEM::GetCurrentProject()->GetActiveSubject()->GetTypePtr(typeID);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CheckObjectVersion
//
// CLASS.......:  OBJTYPE
//
// DESCRIPTION.:  Sends the object a AOBJ_READ and then a AOBJ_AWAKEN
//								message.
//
/////////////////////////////////////////////////////////////////////////////
short OBJTYPE::CheckObjectVersion(
	SERUID  objectUID,
	long messages)
  {
	AObjMessage sysRec;
	short         result = PRJ_READNOERROR;
	LPOBJINST    pObject;
	LONG				 lStatus;

	pObject = (LPOBJINST)GetObject(objectUID);

  //don't need to check the versions of linked alias objects.
	if (!pObject->TestAliasFlag() || (pObject->GetSharedObjectUID() == 0))
		{
    
		if(messages & SUB_SEND_READ)
			{
		
			sysRec.message1 = AOBJ_READ;
			sysRec.message2 = 0;
			sysRec.message3 = 0;
			sysRec.message4 = 0;
			sysRec.errorMessage[0] = '\0';

			lStatus = this->GetAdmnProc()(::GetUID(pObject).id, &sysRec);

			if (lStatus != A_OK)
				{
				if (lStatus == A_WARNING)
					{
					result = PRJ_READWARNING;
					}
				else if (lStatus == A_ERROR)
					{
					result  = PRJ_READERROR;
					}
				ADBG_EVAL(
					else if (lStatus == A_NOTHANDLED)
						ADbgPrint(ADBG_WARNING, 0, "Object type: '%s' should handle AOBJ_READ message",
							(LPCSTR)typeName);
					);
				}
			}

		//now send the awaken message if no error occured.
		if((messages & SUB_SEND_AWAKENED) && (result != PRJ_READERROR))
			{
			sysRec.message1 = AOBJ_AWAKENED;
			sysRec.message2 = 0;
			sysRec.message3 = 0;
			sysRec.message4 = 0;
			sysRec.errorMessage[0] = '\0';
			
			lStatus = this->GetAdmnProc()(::GetUID(pObject).id, &sysRec);
			if(lStatus == A_ERROR)
				result = PRJ_READERROR;
			else if(lStatus == A_WARNING)
				result = PRJ_READWARNING;
			}
		}

	return result;
	}


////////////////////////////////////////////////////////////////
//Constructor for the subtypes
SUBTYPE::SUBTYPE(
	ATYPEID theTypeID,
	SERUID realTypeUID):typeID(theTypeID), objTypeUID(realTypeUID),objCount(0)
	{
	}

long SUBTYPE::DecObjCount(
	BOOL realObject)
	{
	LPOBJTYPE typePtr;

	objCount--;
	typePtr = GetObjTypePtr();
	typePtr->DecObjCount(realObject);

	return objCount;
	}

short SUBTYPE::PostReadInit()
	{
	short result = PRJ_READNOERROR;

	if((!GetObjTypePtr()) || GetObjTypePtr()->TestBadTypeFlag())
		result= PRJ_READWARNING|PRJ_READBADTYPE;

	return result;
  }
