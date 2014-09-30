#include "almsys.h"

#include "clipflow.h"
#include "progsys.h"
#include "fileobj.h"
#include "clipfunc.h"
#include "clipobj.h"
#include "objinst.h"
#include "flow.h"
#include "funcinst.h"
#include "sjtwin.h"

//used for clipboard flows

#define CF_SOURCEINCLIP 			0x00000001L
#define CF_DESTINATIONINCLIP 	0x00000002L


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CLIPFLOW constructor
//
// CLASS.......:  CLIPFLOW
//
// DESCRIPTION.:  initializes a clipboard flow with data from a design flow
//								pointed to by flowPtr.
//
//								deleteAll means that all functions and objects in the
//								subject are being copied to the clipboard.
//
//
/////////////////////////////////////////////////////////////////////////////
CLIPFLOW::CLIPFLOW(
	FLOW * flowPtr,
  BOOL sourceSelected,
	BOOL deleteAll)
	{
	FUNCINST * linkFunc;
	FUNCINST * sourceFunc=NULL;
	OBJINST * sourceObj=NULL;
	LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	//get out of clipbaord context

	status = flowPtr->GetStatus();
	
	clipStatus = 0L;

	//preserve the possible link to the exeflow incase of an undo operation.
	copyUID = flowPtr->copyUID;

	signal = flowPtr->GetSignal();
	order = flowPtr->GetOrder();
	linkFunc = (FUNCINST *)GetObject(flowPtr->GetDestinationUID());
	if(status & FL_OBJSOURCE)
		sourceObj = (OBJINST *)GetObject(flowPtr->GetSourceUID());
	else
		sourceFunc = (FUNCINST *)GetObject(flowPtr->GetSourceUID());

	if(linkFunc->IsSelected() || deleteAll)
	{
		clipStatus|=CF_DESTINATIONINCLIP;
		flowTo = linkFunc->copyUID;
		}
	else
		{
		flowTo = flowPtr->GetDestinationUID();
		clipStatus &= ~CF_DESTINATIONINCLIP;
		}

	if(status & FL_OBJSOURCE)
		{
		if(sourceSelected || deleteAll)
			{
			clipStatus |= CF_SOURCEINCLIP;
			flowFrom = sourceObj->copyUID;
			}
		else
			{
			flowFrom = flowPtr->GetSourceUID();
			clipStatus &= ~CF_SOURCEINCLIP;
			}
		}
	else
		{
		if(sourceSelected || deleteAll)
			{
			clipStatus |= CF_SOURCEINCLIP;
			flowFrom = sourceFunc->copyUID;
			}
		else
			{
			flowFrom = flowPtr->GetSourceUID();
			clipStatus &= ~CF_SOURCEINCLIP;
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateEnvFlow
//
// CLASS.......:  CLIPFLOW
//
// DESCRIPTION.:  Adds a flow to the design subject pointed to by
//								envSjtPtr and fills it with the clipboard flow info.
//
/////////////////////////////////////////////////////////////////////////////
void CLIPFLOW::UpdateEnvFlow(
	SUBJECT * /* envSjtPtr */)
	{
	FUNCINST * srcFuncPtr;
	FUNCINST * destFuncPtr;
	OBJINST * srcObjPtr;
	LPCLIPFUNC clipFunc;
	LPCLIPOBJ	clipObj;
	SERUID source;
	SERUID dest;
	LPFILEOBJECT holdContext;
	long newStatus;
	LPFLOW flowPtr;

	if(clipStatus & CF_SOURCEINCLIP)
		{
		if(status & FL_OBJSOURCE)
			{
			clipObj = (LPCLIPOBJ)GetObject(flowFrom);
			source = clipObj->copyUID;
			}
		else
			{
			clipFunc = (LPCLIPFUNC)GetObject(flowFrom);
			source = clipFunc->copyUID;
			}
		}
	else
		source = flowFrom;

	if(clipStatus & CF_DESTINATIONINCLIP)
		{
		clipFunc = (LPCLIPFUNC)GetObject(flowTo);
		dest = clipFunc->copyUID;
		}
	else
		dest = flowTo;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	
	newStatus = status;
	//The flow may have been cut or copied during a debugging stop and
	//therefore have the stopped-at-flow flag set and we don't want to
	//pass this attribute on.	Also, if the previous case holds, we need
	//to break our link to the exeFlow.
	if(PROGSYSTEM::TestPasteOpFlag())
		{
		//we're begin pasted and not undone.
		newStatus&= ~(FL_STOPPED | FL_LINKTOEXEFLOW);
		}

	destFuncPtr = (FUNCINST *)GetObject(dest);
	if(status & FL_OBJSOURCE)
		{
		srcObjPtr = (OBJINST *)GetObject(source);
		flowPtr = srcObjPtr->AddFlow(dest, source, newStatus,signal, order, FLOW_PASTECONSTRUCTOR);
		destFuncPtr->IncFlowCount(FLOW_IN);
		}
	else
		{
		srcFuncPtr = (FUNCINST *)GetObject(source);
		srcFuncPtr->IncFlowCount(FLOW_OUT);
		destFuncPtr->IncFlowCount(FLOW_IN);
		flowPtr = srcFuncPtr->AddFlow(dest, source,newStatus,signal,order, FLOW_PASTECONSTRUCTOR);
		}

	if(!PROGSYSTEM::TestPasteOpFlag())
		{
		//we're being undone so restore the possbile link to the
		//EXE flow
		flowPtr->copyUID = copyUID;
		}

	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}
