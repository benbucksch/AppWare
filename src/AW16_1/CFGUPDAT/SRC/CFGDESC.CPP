#include "windows.h"
#include "cfgdesc.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "evntdesc.h"
#include "compuid.h"
#include "recblk.h"
#include "sereclib.h"
#include "rectypes.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CFGDDESC
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
CFGDESC::CFGDESC():
		funcGroupName()
	{
  char name[15];
  int i;

  lstrcpy(name, SYSTEM::GetFileObject()->GetName());

  for (i=0; i<15; i++)
    if (name[i] == '.')
      {
      name[i] = '\0';
      break;
      }

  funcGroupName = name;

	objectsBlk = ::GetUID(new RECORDBLOCK(sizeof(OBJDESC), RT_OBJDESC));

	functionsBlk = ::GetUID(new RECORDBLOCK(sizeof(FUNCDESC), RT_FUNCDESC));

	eventsBlk = ::GetUID(new RECORDBLOCK(sizeof(EVENTDESC), RT_EVENTDESC));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateObjectDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERUID CFGDESC::CreateObjectDesc(
	TSTRING		name,
	COMPUID		objectID)
	{
	return ::GetUID(new(objectsBlk) OBJDESC(objectID, name));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateFunctionDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERUID CFGDESC::CreateFunctionDesc(
	TSTRING		name,
	COMPUID		functionID)
	{

	return ::GetUID(new(functionsBlk) FUNCDESC(functionID, name));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateEventDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERUID CFGDESC::CreateEventDesc(
	TSTRING		name,
	COMPUID		eventID)
	{

	return ::GetUID(new(eventsBlk) EVENTDESC(eventID, name));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteObjectDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void CFGDESC::DeleteObjectDesc(
	SERUID	 objectUID)
	{
	delete (LPOBJDESC)GetObject(objectUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteFunctionDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void CFGDESC::DeleteFunctionDesc(
	SERUID	 functionUID)
	{
	delete (LPFUNCDESC)GetObject(functionUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteEventDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void CFGDESC::DeleteEventDesc(
	SERUID	 eventUID)
	{
	delete (LPEVENTDESC)GetObject(eventUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextObjectDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPOBJDESC CFGDESC::NextObjectDesc(
	LPOBJDESC	currentObject)
	{

	return (LPOBJDESC)((LPRECORDBLOCK)GetObject(objectsBlk))->
	 NextRec(currentObject);

	}

LPOBJDESC CFGDESC::pNextObjectDesc(
	LPOBJDESC	currentObject)
	{

	return (LPOBJDESC)((LPRECORDBLOCK)objectsBlk.ptr)->
	 pNextRec(currentObject);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextFunctionDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPFUNCDESC CFGDESC::NextFunctionDesc(
	LPFUNCDESC	currentFunction)
	{

	return (LPFUNCDESC)((LPRECORDBLOCK)GetObject(functionsBlk))->
		NextRec(currentFunction);

	}

LPFUNCDESC CFGDESC::pNextFunctionDesc(
	LPFUNCDESC	currentFunction)
	{

	return (LPFUNCDESC)((LPRECORDBLOCK)functionsBlk.ptr)->
		pNextRec(currentFunction);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextEventDesc
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPEVENTDESC CFGDESC::NextEventDesc(
	LPEVENTDESC	currentEvent)
	{

	return (LPEVENTDESC)((LPRECORDBLOCK)GetObject(eventsBlk))->
	 NextRec(currentEvent);

	}

LPEVENTDESC CFGDESC::pNextEventDesc(
	LPEVENTDESC	currentEvent)
	{

	return (LPEVENTDESC)((LPRECORDBLOCK)eventsBlk.ptr)->
	 pNextRec(currentEvent);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  CFGDESC
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void CFGDESC::ConvertToPointers(
	void)
	{
	int i;
	LPOBJDESC		pObjDesc;
	LPFUNCDESC  pFuncDesc;
	LPEVENTDESC	pEventDesc;

	i = 0;
	while (0 != (pObjDesc = (LPOBJDESC)GetObject(objectsBlk, i)))
		pObjDesc->ConvertToPointers();

	i = 0;
	while (0 != (pFuncDesc = (LPFUNCDESC)GetObject(functionsBlk, i)))
		pFuncDesc->ConvertToPointers();

	i = 0;
	while (0 != (pEventDesc = (LPEVENTDESC)GetObject(eventsBlk, i)))
		pEventDesc->ConvertToPointers();

	objectsBlk.ptr = GetObject(objectsBlk);
	functionsBlk.ptr = GetObject(functionsBlk);
	eventsBlk.ptr = GetObject(eventsBlk);

	((LPRECORDBLOCK)objectsBlk.ptr)->ConvertToPointers();
	((LPRECORDBLOCK)functionsBlk.ptr)->ConvertToPointers();
	((LPRECORDBLOCK)eventsBlk.ptr)->ConvertToPointers();

  funcGroupName.ConvertToPointers();

	}