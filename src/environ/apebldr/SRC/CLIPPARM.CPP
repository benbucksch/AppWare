#include "almsys.h"

#include "clipparm.h"
#include "funcinst.h"
#include "progsys.h"
#include "clipfunc.h"
#include "objinst.h"
#include "clipobj.h"
#include "param.h"
#include "objgroup.h"

//note: CP_SOURCEINCLIP means either the source function or object is in
//			the clipboard.

CLIPPARAM::CLIPPARAM(
	PARAM * paramPtr,
	BOOL deleteAll)
	{
	LPFILEOBJECT holdContext;
	char buffer[256];
	FUNCINST * funcPtr;
	PARAM * linkParamPtr;
	OBJINST * linkObjPtr;

	clipStatus = 0L;
	status = paramPtr->GetStatus();
	ownerFuncUID = paramPtr->ownerFunc;
	holdContext = PROGSYSTEM::PopAndRegisterContext();
	dataFrom = paramPtr->GetDataFromUID();

	if(paramPtr->TestObjConnectFlag())
		{
		linkObjPtr = (OBJINST *)GetObject(dataFrom);
		if(linkObjPtr->TestSelectFlag() || linkObjPtr->GetGroupPtr()->IsSelected() ||
			 deleteAll)
			{
			dataFrom = linkObjPtr->copyUID;
			clipStatus|=CP_SOURCEINCLIP;
			}
		}
	else if(paramPtr->IsInput())
		{
		if(paramPtr->TestParamConnectFlag())
			{
			linkParamPtr = (PARAM *)GetObject(dataFrom);
			funcPtr=(FUNCINST *)GetObject(linkParamPtr->ownerFunc);
			if(funcPtr->IsSelected() || deleteAll)
				{
				linkOrdinal = linkParamPtr->GetOrdinal();
				linkFuncUID = funcPtr->copyUID;
				clipStatus|=CP_SOURCEINCLIP;
				}
			}
		}

	if(((FUNCINST *)GetObject(ownerFuncUID))->IsSelected() || deleteAll)
		{
		clipStatus|=CP_FUNCINCLIP;
		ownerFuncUID = ((FUNCINST *)GetObject(ownerFuncUID))->copyUID;
		}

	lstrcpy((LPSTR)buffer, (LPSTR)paramPtr->GetName());
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	name = (LPSTR)buffer;
	ordinal = paramPtr->GetOrdinal();
	}


void CLIPPARAM::UpdateEnvParam()
	{
	char buffer[256];
	FUNCINST * funcPtr;
	LPFILEOBJECT holdContext;
	PARAM * paramPtr;
	PARAM * linkParamPtr;
	LPCLIPFUNC clipFunc;
	LPCLIPOBJ clipObj;

	lstrcpy(buffer, name);
	holdContext = PROGSYSTEM::PopAndRegisterContext();
	if(clipStatus & CP_FUNCINCLIP)
		{
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		clipFunc = (LPCLIPFUNC)GetObject(ownerFuncUID);
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		funcPtr = (FUNCINST *)GetObject(clipFunc->copyUID);
		}
	else
		{
		funcPtr = (FUNCINST *)GetObject(ownerFuncUID);
		}
	paramPtr = funcPtr->GetParam(ordinal);
	paramPtr->SetName((LPSTR)buffer);
	if(status & PAR_FROMOBJ)
		{
		paramPtr->AddInfo(status);
		if(clipStatus & CP_SOURCEINCLIP)
			{
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			clipObj = (LPCLIPOBJ)GetObject(dataFrom);
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			paramPtr->AddLinkUID(clipObj->copyUID);
			}
		else
			paramPtr->AddLinkUID(dataFrom);
		}
	else if(status & PAR_INPUT)
		{
		paramPtr->AddInfo(status);
		if(status & PAR_FROMPARAM)
			{
			if(clipStatus & CP_SOURCEINCLIP)
				{
				PROGSYSTEM::PushAndRegisterContext(holdContext);
				clipFunc = (LPCLIPFUNC)GetObject(linkFuncUID);
				holdContext = PROGSYSTEM::PopAndRegisterContext();
				funcPtr = (FUNCINST *)GetObject(clipFunc->copyUID);
				linkParamPtr = funcPtr->GetParam(linkOrdinal);
				paramPtr->AddLinkUID(GetUID(linkParamPtr));
				}
			else
				{
				linkParamPtr = (PARAM *)GetObject(dataFrom);
				paramPtr->AddLinkUID(dataFrom);
				}
			linkParamPtr->AddInfo(PAR_FROMPARAM);
			linkParamPtr->SetName((LPSTR)buffer);
			}
		}
	else
		paramPtr->SetStatus(status);


	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}
