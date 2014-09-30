#include "almsys.h"

#include "flowmgr.h"
#include "flow.h"
#include "list.h"
#include "objinst.h"
#include "fwinobj.h"
#include "recblk.h"
#include "rectypes.h"
#include "exeflow.h"
#include "exeflwmg.h"
#include "funcinst.h"
#include "progsys.h"
#include "clipflow.h"
#include "sjtwin.h"
#include "uidlist.h"
#include "subject.h"
#include "project.h"
#include "flowstak.h"
#include "upsfname.h"
#include "intdebug.h"
#include "apbutil.h"
#include "mdiclass.h"
#include "objdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FLOWMGR
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOWMGR::FLOWMGR()
	{
	flowList = GetUID(new RECORDBLOCK(sizeof(FLOW), RT_FLOW));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FLOWMGR
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
#pragma argsused
FLOWMGR::FLOWMGR(
	void * thisPtr)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~FLOWMGR
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOWMGR::~FLOWMGR()
	{

	((RECORDBLOCK *)GetObject(flowList))->PurgeBlock();

	delete (RECORDBLOCK *)(GetObject(flowList));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddFlow
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOW * FLOWMGR::AddFlow(
	SERUID destinationUID,
	SERUID sourceUID,
	long   status,
	LONG newSignal,
	int order,
	WORD constructorType)
	{
	FLOW * flowPtr;
	FLOW * tempPtr;
	int found = 0;
	int i = 0;
	int highOrder = 0;

	flowPtr = (FLOW *)new(flowList) FLOW(destinationUID, sourceUID, status,
		newSignal, constructorType);

	flowPtr->SetDraw(FALSE);
	while(0 != (tempPtr = (FLOW *)GetObject(flowList,i)))
		{
		if(flowPtr != tempPtr)
			{
			if(destinationUID == tempPtr->GetDestinationUID())
				{
				found++;
				if(order == NO_ORDER)
					{
					tempPtr->ShiftSignalRect(-1);
					if(tempPtr->GetOrder() > highOrder)
						highOrder = tempPtr->GetOrder();
					}
				else
					{
					if(tempPtr->GetOrder()<order)
						{
						tempPtr->ShiftSignalRect(-1);
						flowPtr->ShiftSignalRect(1);
						}
					else
						{
						if(order == 0 && found==1)
							{
							tempPtr->SetDraw(FALSE);
							flowPtr->SetDraw(TRUE);
							}
						tempPtr->ShiftSignalRect(1);
						flowPtr->ShiftSignalRect(-1);
						}

					}
				}
			}
		}
	if(order == NO_ORDER)
		{
		if(found)
			{
			flowPtr->ShiftSignalRect(found);
			flowPtr->SetDraw(FALSE);
			}
		else
			{
			flowPtr->SetDraw(TRUE);
			}
		flowPtr->SetOrder(highOrder+1);
		}
	else
		{
		flowPtr->SetOrder(order);
		if(found == 0)
			flowPtr->SetDraw(TRUE);
		}
	return flowPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  RemoveFlow
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::RemoveFlow(
	FLOW * flowPtr)
	{
	FLOW * tempPtr;
	int i = 0;
	SERUID destUID = flowPtr->GetDestinationUID();
	FLOW * drawFlow=NULL;

	while(0 != (tempPtr = (FLOW *)GetObject(flowList,i)))
		{
		if(flowPtr != tempPtr)
			{
			if(destUID == tempPtr->GetDestinationUID())
				{
				//drawFlow will be the signal which now draws its flow
				//if we haven't set it to anything yet, set it to the first
				//posssible flow, else see if the temp flow has a lower order
				//than the draw flow and if it does the temp flow becomes the
				//new draw flow.
				if(!drawFlow)
					drawFlow = tempPtr;
				else if (drawFlow->GetOrder() > tempPtr->GetOrder())
					drawFlow = tempPtr;

				if(flowPtr->GetOrder() < tempPtr->GetOrder())
					{
					tempPtr->ShiftSignalRect(-1);
					}
				else
					{
					tempPtr->ShiftSignalRect(1);
					}
				}
			}
		}
	if(drawFlow)
		drawFlow->SetDraw(TRUE);

	delete flowPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  RecalulateFlowPaths
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::RecalculateFlowPaths(
	LPRECT unionBounds)
	{
	FLOW * tempFlow;
	int i=0;
	RECT flowBounds;

	SetRectEmpty(unionBounds);
	while(0 != (tempFlow = (FLOW *)GetObject(flowList, i)))
			{
			tempFlow->GetFlowBounds(&flowBounds);
			UnionRect(unionBounds, unionBounds, &flowBounds);
			tempFlow->CalculatePath();
			tempFlow->GetFlowBounds(&flowBounds);
			UnionRect(unionBounds, unionBounds, &flowBounds);
			}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  RecalulateFlowPaths
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::RecalculateFlowPaths()
	{
	FLOW * tempFlow;
	int i=0;

	while(0 != (tempFlow = (FLOW *)GetObject(flowList, i)))
		{
		tempFlow->CalculatePath();
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawFlows
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::DrawFlows(
	HDC hdc,
	LPRECT refreshRect,
	LPRECT drawBounds)
	{
	FLOW * temp;
	int i=0;

	while(0 != (temp = (FLOW *)GetObject(flowList, i)))
		{
		if(temp->FlowInRect(refreshRect))
			temp->Draw(hdc,drawBounds);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawSelectedSignals
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::DrawSelectedSignals(
	HDC hdc,
	LPRECT refreshRect)
	{
	FLOW * temp;
	int i=0;

	while(0 != (temp = (FLOW *)GetObject(flowList, i)))
		{
		if(temp->SignalInRect(refreshRect) && temp->IsSelected())
			temp->DrawSignalRect(hdc);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawUnSelectedSignals
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::DrawUnSelectedSignals(
	HDC hdc,
	LPRECT refreshRect,
	LPRECT screenRect,
	LPZLIST zlistPtr)
	{
	FLOW * temp;
	int i=0;

	while(0 != (temp = (FLOW *)GetObject(flowList, i)))
		{
		if(!temp->IsSelected() && temp->AddFlowToZlist(screenRect, zlistPtr))
			if(temp->SignalInRect(refreshRect))
				temp->DrawSignalRect(hdc);
		}
	}


/////////////////////////////////////////////////////////////
void FLOWMGR::AddUnSelectedToZlist(
	LPRECT screenRect,
	LPZLIST zlistPtr)
	{
	FLOW * temp;
	int i=0;

	while(0 != (temp = (FLOW *)GetObject(flowList, i)))
		{
		if(!temp->IsSelected())
			temp->AddFlowToZlist(screenRect, zlistPtr);
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FlowFromPoint
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOW * FLOWMGR::FlowFromPoint(
	CONST POINT &pt)
	{
	FLOW * flowPtr;
	int i=0;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->PointInFlow(pt))
			break;
		}
	return flowPtr;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddFlowsInRectToList
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:  returns TRUE if any flows are added
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::AddFlowsInRectToList(
	HDC hdc,
	LPRECT rect,
	LIST * list)
	{
	LPUIDLIST temp;
	FLOW * flowPtr;
	int i=0;
  BOOL flowsAdded = FALSE;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if((flowPtr->SignalInRect(rect)) && (!flowPtr->IsSelected()))
			{
      flowsAdded = TRUE;
			flowPtr->SetSelected(TRUE);
			if(hdc)
				flowPtr->DrawSignalRect(hdc);
			temp = new UIDLIST(GetUID(flowPtr));
			list->Add(temp);
			}
		}

	return flowsAdded;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddSelectedFlowsToList
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::AddSelectedFlowsToList(
	LIST * list)
	{
	FLOW * flowPtr;
	int i=0;
	LPUIDLIST temp;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->IsSelected())
			{
			temp = new UIDLIST(GetUID(flowPtr));
			list->Add(temp);
			}
		}
	}


/////////////////////////////////////////////////////////////
void FLOWMGR::AddAllFlowsToList(
	LIST * list)
	{
	FLOW * flowPtr;
	int i=0;
	LPUIDLIST temp;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		temp = new UIDLIST(GetUID(flowPtr));
		list->Add(temp);
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CompileFlows
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::CompileFlows(
	RUNTIMEUID exeFlowList)
	{
	FLOW *   flowPtr;
	RUNTIMEUID  flowList;
	int         i = 0;

	flowList = GetFlowList();

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		flowPtr->copyUID =
     GetUID(new(exeFlowList) EXEFLOW(flowPtr->GetDestinationUID(),
                                     flowPtr->GetSignal(),
                                     flowPtr->GetStatus()));
		
		flowPtr->SetLinkToExeFlag();
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateUIDs
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::UpdateUIDs()
	{
	FLOW *   flowPtr;
	RUNTIMEUID  flowList;
	int         i=0;

	flowList = GetFlowList();

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
    flowPtr->UpdateUIDs();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ClearFlows
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::ClearFlows(
	RUNTIMEUID clipFlowBlock,
	BOOL       sourceSelected,
	BOOL       clearAll)
	{
	FLOW *      flowPtr;
	FUNCINST *  linkFunc;
	LPFILEOBJECT   holdContext;
	int            i=0;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		linkFunc = (FUNCINST *)GetObject(flowPtr->GetDestinationUID());
		if(flowPtr->IsSelected() || sourceSelected || linkFunc->IsSelected() ||
			clearAll)
			{
			if(!flowPtr->TestSrcObjFlag())
				{
				((FUNCINST *)GetObject(flowPtr->GetSourceUID()))->
					DecFlowCount(FLOW_OUT);
				}
			linkFunc->DecFlowCount(FLOW_IN);
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			new(clipFlowBlock) CLIPFLOW(flowPtr, sourceSelected, clearAll);
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			RemoveFlow(flowPtr);
			i--;
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CopyFlows
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::CopyFlows(
	RUNTIMEUID clipFlowBlock,
	BOOL       sourceSelected,
	BOOL       clearAll)
	{
	FLOW *      flowPtr;
	FUNCINST *  linkFunc;
	LPFILEOBJECT   holdContext;
	int            i = 0;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		linkFunc = (FUNCINST *)GetObject(flowPtr->GetDestinationUID());
		if(clearAll || (linkFunc->IsSelected() && sourceSelected &&
			flowPtr->IsSelected()))
			{
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			new(clipFlowBlock) CLIPFLOW(flowPtr, sourceSelected, clearAll);
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ChangeSignalDisplay
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::ChangeSignalDisplay(
	BOOL showSignalNames)
	{
	FLOW * flowPtr;
	int i=0;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		flowPtr->ChangeSignalDisplay(showSignalNames);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ToggleStops
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::ToggleStops(
	LPRECT bounds)
	{
	FLOW * flowPtr;
	int i=0;
	BOOL rVal = FALSE;

	while(NULL != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->IsSelected())
			{
			flowPtr->ToggleStop(bounds);
			rVal = TRUE;
			}
		}

	return rVal;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ClearAllStops
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::ClearAllStops(
	LPRECT bounds)
	{
	FLOW * flowPtr;
	int i=0;
	BOOL result = FALSE;

	while(NULL != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->TestStopFlag())
			{
			flowPtr->ClearStop(bounds);
			result = TRUE;
			}
		}
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  EnableStops
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::EnableStops(
	HDC hdc)
	{
	FLOW * flowPtr;
	int i=0;
	BOOL result = FALSE;

	while(NULL != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->TestStopFlag())
			{
			flowPtr->EnableStop();
			flowPtr->DrawSignalRect(hdc, PROGSYSTEM::TestBufferedRedrawPref());
			result = TRUE;
			}
		}
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DisableStops
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::DisableStops(
	HDC hdc)
	{
	FLOW * flowPtr;
	int i=0;
	BOOL result = FALSE;

	while(NULL != (flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->TestStopFlag())
			{
			flowPtr->DisableStop();

			flowPtr->DrawSignalRect(hdc, PROGSYSTEM::TestBufferedRedrawPref());
			}
		}
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  IsStopSet
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FLOWMGR::IsStopSet()
	{
	FLOW * flowPtr;
	int i=0;
	BOOL result = FALSE;

	while(0 != (flowPtr = (FLOW *)GetObject(flowList, i)))
		if(flowPtr->TestStopFlag())
			{
			result=TRUE;
			break;
			}

	return result;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetDesignFlow
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOW * FLOWMGR::GetDesignFlow(
	LPEXEFLOW exeFlow)
	{
	FLOW * flowPtr=NULL;
	FLOW * temp;
	int i=0;

	while(0 != (temp = (FLOW *)GetObject(flowList, i)))
		if(temp->copyUID == GetUID(exeFlow))
			{
			flowPtr = temp;
			break;
			}

	return flowPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetFlowCount
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
int FLOWMGR::GetFlowCount()
	{
	RECORDBLOCK * blockPtr = (RECORDBLOCK *)GetObject(flowList);

	return (int)blockPtr->GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UnselectFlows
//
// CLASS.......:  FLOWMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FLOWMGR::UnselectFlows()
	{
	FLOW * flowPtr;
	int i=0;

	while(0!=(flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		if(flowPtr->IsSelected())
			flowPtr->SetSelected(FALSE);
		}
	}


WORD FLOWMGR::FlowExportUPSF(
	LPSTR fieldName,
	int * numExported)
	{
	int i=0;
	FLOW * flowPtr;
	WORD returnValue = EXPORT_NOERROR;
	long result;

	//initialize to signal no flows exported
	*numExported = 0;

	while(!(returnValue & EXPORT_OUTOFDISK) &&
		0!=(flowPtr = (FLOW *)GetObject(flowList, i)))
		{
		result = AUpsfCreateScope(fieldName, (long)(i-1));
		
		I_ASSERT(result == AUPSF_NOERROR);

		if(result == AUPSF_NOERROR)
			{
			(*numExported)++;
			returnValue |= flowPtr->ExportUPSF();
			AUpsfCloseScope();
			}
		else if(result == AUPSF_OUTOFDISKSPACE)
			returnValue |= EXPORT_OUTOFDISK;
		else
			returnValue |= EXPORT_GENERALERROR;
		}
	return returnValue;
	}

BOOL FLOWMGR::FlowImportUPSF(
	LPSTR fieldName,
	LPSTR fieldCount,
	SERUID srcUID,
	BOOL srcIsFunc)
	{
	int flowCount;
	long result;
	int i;
	BOOL badFunc;
	SERUID dstUID;
	int tempInt;
	LPSUBJECT subjectPtr;

	I_ALWAYSASSERT((result = AUpsfImportField(fieldCount, 0L, AUPSF_NATV_INT,
		(LPVOID)&flowCount, (long)sizeof(int))) != AUPSF_MEMORYERROR, 
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		if(PROJECT::UpsfPreAppwareImportVersion())
			flowCount++;

		if(flowCount > 0)
			{
			for(i=0;i<flowCount;i++)
				{
				I_ALWAYSASSERT((result = AUpsfOpenScope(fieldName, (long)i)) !=
					AUPSF_FILEERROR, return FALSE);
								
				if(result == AUPSF_NOERROR)
					{
					PROJECT::PushScopeStack(ST_FULL);
					
					badFunc = FALSE;

					if(PROJECT::UpsfPreAppwareImportVersion())
						{
						//we're bring in a workshop version
						I_ALWAYSASSERT((result = AUpsfOpenScope((LPSTR)"FlwParm", 0L))
							!= AUPSF_FILEERROR, return FALSE);
					
						if(result == AUPSF_NOERROR)
							{
							PROJECT::PushScopeStack(ST_FULL);
							
							I_ALWAYSASSERT((result=AUpsfImportField((LPSTR)"PrmIndex", 0L, AUPSF_NATV_INT,
								(LPVOID)&tempInt, (long)sizeof(int))) != AUPSF_MEMORYERROR,
								return FALSE);
								
							PROJECT::PopScopeStack();
							AUpsfCloseScope(); //FlwParm scope
							}
						else
							apb_LogUpsfError(result, "flow parm scope", APB_UPSFOPEN);

						}	 //test appware version
					else
						{
						//we're in an appware version
						I_ALWAYSASSERT((result = AUpsfImportField(FLOW_CONNECTINDEX,
							0L, AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) !=
							AUPSF_MEMORYERROR, return FALSE);
						}
					
					//if there is an error to here and we're importing a Workshop
					//file, the parm scope must have been previously closed.

					if(result == AUPSF_NOERROR)
						{
						//we successfully read in the index
						subjectPtr = (srcIsFunc)?((LPFUNCINST)GetObject(srcUID))->GetSubjectPtr():
							((LPOBJINST)GetObject(srcUID))->GetSubjectPtr();
								
						dstUID = subjectPtr->UPSFIndexToFunc(tempInt);
						
						if(dstUID == NULLSERUID)
							{
							//no destination function could be found
							AUpsfLogComment((LPSTR)"Invalid function index - could not"
								" link flow.");
							
							badFunc = TRUE;
							}
						}
					else
						{
						apb_LogUpsfError(result ,"function's index");						
						badFunc = TRUE;
						}

					if((!badFunc) && (!ImportUpsfFlowData(srcUID, dstUID, srcIsFunc)))
							return FALSE;

					PROJECT::PopScopeStack();
					AUpsfCloseScope(); // flow scope

					if(badFunc)
						continue;	//try to import the next flow.

					}	//test of open scope
				else 
					apb_LogUpsfError(result, "flow scope", APB_UPSFOPEN);

				}	//for loop
			
			} //flowCount > 0
		
		}	//test on reading in flow count
	else
		apb_LogUpsfError(result, "flow count");

	return TRUE;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			FLOWMGR::ImportUpsfFlowData
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL FLOWMGR::ImportUpsfFlowData(
	SERUID srcUID,
	SERUID dstUID,
	BOOL srcIsFunc)
	{
	long result;
	long newStatus;
	SUBJECT * subjectPtr;
	FUNCINST * funcPtr;
	OBJINST * objPtr;
	long newSignal;
	FLOW * flowPtr;

	if(srcIsFunc)
		{
		funcPtr = (FUNCINST *)GetObject(srcUID);
		
		I_ASSERT(funcPtr);
		
		subjectPtr = funcPtr->GetSubjectPtr();
		}
	else
		{
		objPtr = (LPOBJINST)GetObject(srcUID);
		
		I_ASSERT(objPtr);

		subjectPtr = objPtr->GetSubjectPtr();
		}

	I_ASSERT(subjectPtr);

	//read in the signal
	I_ALWAYSASSERT((result = AUpsfImportField(FLOW_SIGNALID, 0L, AUPSF_NATV_LONG,
		(LPVOID)&newSignal, (long)sizeof(long))) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result != AUPSF_NOERROR)
		{
		newSignal = DEFAULT_SIGNAL;
		apb_LogUpsfError(result, "signal. Using default signal id");
		}
	else if (srcIsFunc)
		{
		// UPSF flow IDs are zero-based, so make it one-based
		newSignal++;
		}

	//get bounding rectangle fromPoint of source and create the flow.
	if(srcIsFunc)
		{
		newStatus = FL_FUNCSOURCE;
		funcPtr->IncFlowCount(FLOW_OUT);
		}
	else
		{
		newStatus = FL_OBJSOURCE;
		}

	if(subjectPtr->TestSignalNamesFlag())
		newStatus|=FL_SIGNALNAME;

	flowPtr = AddFlow(dstUID, srcUID, newStatus, newSignal, NO_ORDER,
		FLOW_UPSFCONSTRUCTOR);

	//now increment the destination function flow count
	funcPtr = (FUNCINST *)GetObject(dstUID);
	
	I_ASSERT(funcPtr);

	funcPtr->IncFlowCount(FLOW_IN);

	return flowPtr->ImportUPSF();
	
	} 
																			 

/////////////////////////////////////////////////////////////
BOOL FLOWMGR::FlowPostUPSFImport()
	{
	int i;
	FLOW * flowPtr;
	int offset;
	FLOW * tempFlow;
	int j;

	//we need to figure each signal rect offset
	i=0;
	while(0!=(flowPtr = (FLOW *)GetObject(flowList, i)))
		if(!flowPtr->PostUPSFImport())
			return FALSE;
		else
			{
			offset = 0;
			j=0;
			while(0 != (tempFlow = (LPFLOW)GetObject(flowList, j)))
				{
				if((tempFlow != flowPtr) && 
					(tempFlow->GetDestinationUID() == flowPtr->GetDestinationUID()))
					{
					//the orders have already been calculated and should not be equal
					I_ASSERT(flowPtr->GetOrder() != tempFlow->GetOrder());

					if(flowPtr->GetOrder() < tempFlow->GetOrder())
						offset--;
					else
						offset++; 
					}
				}

			if(offset)
				{
				flowPtr->CalculatePath();  	//this will happen later incase the
																		//log bounds change but we need to
																		//calculate it here as well or shifting
																		//the signalRect won't work.

				flowPtr->ShiftSignalRect(offset);
				}
			}
	return TRUE;
	}

/////////////////////////////////////////////////////////////
void FLOWMGR::PostPaste()
	{
	int i;
	FLOW * flowPtr;

	i=0;
	while(0!=(flowPtr = (FLOW *)GetObject(flowList, i)))
		flowPtr->PostPaste();
	}

///////////////////////////////////////////////////////////////
// Verify Signal Names
//
// Causes the flow to match a name if possible to the signal ID
// it's currently holding onto.
void FLOWMGR::VerifySignalNames()
	{
	int i=0;
	LPFLOW flowPtr;

	while(0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
		flowPtr->UpdateSignalIntegrity();
  }

/////////////////////////////////////////////////////////////
short FLOWMGR::ValidateFlows(
	BOOL srcIsFunc)
	{
	int i;
	FLOW * flowPtr;
	short result = PRJ_READNOERROR;
	short tempResult;
	LPOBJINST objPtr = NULL;
	LPFUNCINST funcPtr = NULL;
	char srcType[16];
	char srcName[48];
	char additionalInfo[64];
	char message[256];
	char lostMsg[16];
	 
	static char pattern[] = {"%s '%s'%s no longer has %s so all outgoing"
		" flows have been deleted."};

	if(((LPRECORDBLOCK)GetObject(flowList))->GetCount() > 0)
		{
		if(srcIsFunc)
			funcPtr = (LPFUNCINST)this;
		else
			objPtr = (LPOBJINST)this;

		if((objPtr && !objPtr->HasFlows()) ||
			(funcPtr && !funcPtr->SupportsOutFlows()))
			{
			result |= PRJ_READWARNING;
			
			if(srcIsFunc)
				{
				lstrcpy(srcType, "Function");
				lstrcpy(srcName, funcPtr->GetDescPtr()->pGetName());
				additionalInfo[0] = '\0';
				lstrcpy(lostMsg, "output flows");
				}
			else
				{
				lstrcpy(srcType, "Object");
				lstrcpy(srcName, objPtr->GetName());
				lstrcpy(additionalInfo, " of type \'");
				lstrcat(additionalInfo, objPtr->objDesc->pGetName());
				lstrcat(additionalInfo, "\' "); 
				lstrcpy(lostMsg, "signals");
				}

			wsprintf(message, pattern, srcType, srcName, additionalInfo,
				lostMsg);
								
			MDI::AddMessage(message, TRUE);

			while(i=0, 0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
				{
				if(srcIsFunc)
					funcPtr->DecFlowCount(FLOW_OUT);

				if(GetObject(flowPtr->GetDestinationUID()) &&
					(GetUIDType(flowPtr->GetDestinationUID()) == RT_FUNCINST))
					{
					((LPFUNCINST)GetObject(flowPtr->GetDestinationUID()))->DecFlowCount(FLOW_IN);
					}

				delete flowPtr;
				}
			}
		else
			{

			//the flows ValiateFlow routine should output any necessary messages.
			i=0;
			while(!(result & PRJ_READERROR) &&
				0!=(flowPtr = (FLOW *)GetObject(flowList, i)))
				{
				tempResult = flowPtr->ValidateFlow();
				if(tempResult & PRJ_READBADFLOW)
					{
					RemoveFlow(flowPtr);
					i--;
					}
				result |= tempResult;
				}
			}
		}

	return result;
	}

/////////////////////////////////////////////////////////////
void FLOWMGR::PushFlows(
	LPFLOWSTACK flowStack)
	{
	int i=0;
	LPFLOW flowPtr;

	while(0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
		flowStack->Push(flowPtr);
  }


/////////////////////////////////////////////////////////////
// FUNCTION:			FLOWMGR::HandleBadFlows
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void FLOWMGR::HandleBadFlows()
	{
	int i=0;
	LPFLOW flowPtr;
	LPFUNCINST funcPtr;
	SERUID funcUID;

	while(0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
		{
		funcUID = flowPtr->GetDestinationUID();
		funcPtr = (LPFUNCINST)GetObject(funcUID);

		if(funcPtr && (GetUIDType(funcUID) == RT_FUNCINST))
			funcPtr->DecFlowCount(FLOW_IN);
		}
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			FLOWMGR::GetFlowBounds
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void FLOWMGR::GetUnionFlowBounds(
	LPRECT bounds)
	{
	int i=0;
	LPFLOW flowPtr;
	RECT flowRect;

	I_ASSERT(bounds);

	while(0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
		{
		flowPtr->GetFlowBounds(&flowRect);
		UnionRect(bounds, bounds, &flowRect);
		}	
	} 

																			 																			 
/////////////////////////////////////////////////////////////
void FLOWMGR::GetBoundsOfIntersectingFlows(
	LPRECT bounds, 
	SERUID uid)
	{
	I_ASSERT(bounds);

	int i=0;
	LPFLOW flowPtr;
	RECT flowRect;

	while(0 != (flowPtr = (LPFLOW)GetObject(flowList, i)))
		{
		if(flowPtr->GetDestinationUID() == uid)
			{
			flowPtr->GetFlowBounds(&flowRect);
			UnionRect(bounds, bounds, &flowRect);
			}
		}	
	}
