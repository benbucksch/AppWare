#include "almsys.h"

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
// CLASS.......:  ACFGDESC
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
ACFGDESC::ACFGDESC():
		funcGroupName()
	{
	SFilePath			fileName;

	// GetName() doesn't include the path (directory) portion
	fileName = pTheSystem->GetFileObject()->GetName();
	fileName.SetExtension("");

  funcGroupName = (const char*) fileName;

	objectsBlk = ::GetUID(new RECORDBLOCK(sizeof(AOBJDESC), RT_OBJDESC));

	functionsBlk = ::GetUID(new RECORDBLOCK(sizeof(AFUNCDESC), RT_FUNCDESC));

	eventsBlk = ::GetUID(new RECORDBLOCK(sizeof(AEVENTDESC), RT_EVENTDESC));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateObjectDesc
//
// CLASS.......:  ACFGDESC
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
SERUID ACFGDESC::CreateObjectDesc(
	TSTRING		name,
	COMPUID		objectID)
	{
	return ::GetUID(new(objectsBlk) AOBJDESC(objectID, name));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateFunctionDesc
//
// CLASS.......:  ACFGDESC
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
SERUID ACFGDESC::CreateFunctionDesc(
	TSTRING		name,
	COMPUID		functionID)
	{

	return ::GetUID(new(functionsBlk) AFUNCDESC(functionID, name));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateEventDesc
//
// CLASS.......:  ACFGDESC
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
SERUID ACFGDESC::CreateEventDesc(
	TSTRING		name,
	COMPUID		eventID)
	{

	return ::GetUID(new(eventsBlk) AEVENTDESC(eventID, name));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteObjectDesc
//
// CLASS.......:  ACFGDESC
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
void ACFGDESC::DeleteObjectDesc(
	SERUID	 objectUID)
	{
	delete (LPOBJDESC)GetObject(objectUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteFunctionDesc
//
// CLASS.......:  ACFGDESC
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
void ACFGDESC::DeleteFunctionDesc(
	SERUID	 functionUID)
	{
	delete (LPAFUNCDESC)GetObject(functionUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteEventDesc
//
// CLASS.......:  ACFGDESC
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
void ACFGDESC::DeleteEventDesc(
	SERUID	 eventUID)
	{
	delete (LPEVENTDESC)GetObject(eventUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextObjectDesc
//
// CLASS.......:  ACFGDESC
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
LPOBJDESC ACFGDESC::NextObjectDesc(
	LPOBJDESC	currentObject)
	{

	return (LPOBJDESC)((LPRECORDBLOCK)GetObject(objectsBlk))->
	 NextRec(currentObject);

	}

LPOBJDESC ACFGDESC::pNextObjectDesc(
	LPOBJDESC	currentObject)
	{

	return (LPOBJDESC)((LPRECORDBLOCK)objectsBlk.ptr)->
	 pNextRec(currentObject);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextFunctionDesc
//
// CLASS.......:  ACFGDESC
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
LPAFUNCDESC ACFGDESC::NextFunctionDesc(
	LPAFUNCDESC	currentFunction)
	{

	return (LPAFUNCDESC)((LPRECORDBLOCK)GetObject(functionsBlk))->
		NextRec(currentFunction);

	}

LPAFUNCDESC ACFGDESC::pNextFunctionDesc(
	LPAFUNCDESC	currentFunction)
	{

	return (LPAFUNCDESC)((LPRECORDBLOCK)functionsBlk.ptr)->
		pNextRec(currentFunction);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextEventDesc
//
// CLASS.......:  ACFGDESC
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
LPEVENTDESC ACFGDESC::NextEventDesc(
	LPEVENTDESC	currentEvent)
	{

	return (LPEVENTDESC)((LPRECORDBLOCK)GetObject(eventsBlk))->
	 NextRec(currentEvent);

	}

LPEVENTDESC ACFGDESC::pNextEventDesc(
	LPEVENTDESC	currentEvent)
	{

	return (LPEVENTDESC)((LPRECORDBLOCK)eventsBlk.ptr)->
	 pNextRec(currentEvent);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  ACFGDESC
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
void ACFGDESC::ConvertToPointers(
	void)
	{
	int i;
	LPOBJDESC		pObjDesc;
	LPAFUNCDESC  pFuncDesc;
	LPEVENTDESC	pEventDesc;

	i = 0;
	while (0 != (pObjDesc = (LPOBJDESC)GetObject(objectsBlk, i)))
		pObjDesc->ConvertToPointers();

	i = 0;
	while (0 != (pFuncDesc = (LPAFUNCDESC)GetObject(functionsBlk, i)))
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
