#include "almsys.h"
#include <windowsx.h>

#include "palette.h"
#include "objinst.h"
#include "objdesc.h"
#include "cfglibp.h"
#include "memblk.h"
#include "pflwstak.h"
#include "funcinst.h"
#include "errors.h"
#include "progsys.h"
#include "mdiclass.h"
#include "subject.h"
#include "project.h"
#include "objtype.h"
#include "printer.h"
#include "flow.h"
#include "clipobj.h"
#include "sjtwin.h"
#include "prtlabel.h"
#include "flowstak.h"
#include "uidlist.h"
#include "list.h"
#include "objgroup.h"
#include "objdesc.h"
#include "sericon.h"
#include "intdebug.h"
#include "oiputils.h"
#include "upsfname.h"
#include "upsf_env.h"
#include "apbutil.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  OBJINST
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  Constructor
//
/////////////////////////////////////////////////////////////////////////////
OBJINST::OBJINST (
	LPOBJDESC	pObjDef,
	SERUID creatorUID,
	SERUID typeUID):
		FLOWMGR(),
		OBJECTRECORD(pObjDef->GetUID(),
								 pTheSystem->GetFileObject()->GetSeriusBlocks(),
								 typeUID),
		objectName(pObjDef->pGetName())
	{

	status=0;
	ClearPersistentsFlag();
	ClearShareFlag();
	ClearAliasFlag();
	pIcon = pObjDef->GetIcons();
	subjectUID = creatorUID;
	objDesc = pObjDef;
  OBJECTRECORD::objectStatus |= OBJ_DESIGNTIME;

  SetSharedObjectUID(0);
  groupUID = NULLSERUID;

	}

OBJINST::OBJINST(
	ATYPEID theID,
	LPCSTR theName,
	long  theObjectStatus,
	SERUID theSubjectUID,
	SERUID typeUID):
		FLOWMGR(),
		OBJECTRECORD(theID,
								 pTheSystem->GetFileObject()->GetSeriusBlocks(),
								 typeUID),
    objectName(theName)
	{
	objectStatus = theObjectStatus;
	status = 0;
	objDesc = cfgGetObjectDesc(theID);
	pIcon = objDesc->GetIcons();
	subjectUID = theSubjectUID;
	SetSharedObjectUID(0);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  OBJINST
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  Constructor for file I/O
//
/////////////////////////////////////////////////////////////////////////////
OBJINST::OBJINST(
	void * thisPtr):
    objectName(&objectName),
    FLOWMGR(thisPtr),
    OBJECTRECORD(pTheSystem->GetFileObject()->GetSeriusBlocks())
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PostReadInit
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short OBJINST::PostReadInit()
	{
	char buffer[256];
	char idBuffer[6];
	short result;
	LPOBJTYPE typePtr;

	//if there is not config or there is no type pointer or the type pointer
	//is bad, then the object is bad.

	objDesc = cfgGetObjectDesc(typeID);

	if(!objDesc || 
		(0 == (typePtr=GetSubjectPtr()->GetParentProject()->GetTypePtr(typeID))) ||
		(typePtr->TestBadTypeFlag()))
		{
		CopyMemory((void *)idBuffer, (void *)&typeID, sizeof(COMPUID));

		//swap the bytes
		idBuffer[0]^=idBuffer[3]^=idBuffer[0]^=idBuffer[3];
		idBuffer[1]^=idBuffer[2]^=idBuffer[1]^=idBuffer[2];
		idBuffer[4] = '\0';

		wsprintf((LPSTR)buffer, (LPSTR)"Object named %s with typeID %s in "
			"subject %s could not be found in the configuration information or"
			" the ALM cound not be loaded.\r\n",
			(LPCSTR)objectName, idBuffer, GetSubjectPtr()->GetName());
		MDI::AddMessage(buffer, TRUE);
		result = PRJ_READBADOBJECT|PRJ_READWARNING;
		}
	else
		{
		pIcon = objDesc->GetIcons();
		result = PRJ_READNOERROR;
		}

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~OBJINST
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  Destructor
//
/////////////////////////////////////////////////////////////////////////////
OBJINST::~OBJINST(
	void)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetSignals
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL OBJINST::GetSignals(
	RUNTIMEUID		memBlock,
	long int	    signalStyle)
	{
	LPOBJTYPE			pType;
	AObjMessage  systemRec;
	BOOL					result;
	SERUID				objectID;

	if (0 != (pType = GetSubjectPtr()->GetTypePtr(typeID)))
		{
		//if aliase linked get uid of real object
		if(TestAliasFlag() && (sharedObjectUID != NULLSERUID))
				objectID = sharedObjectUID;
		else
    	objectID = GetUID(this);

		//if objectID points to an object then get the signals
		if(objectID != NULLSERUID)
			{
			systemRec.message1 = signalStyle;
			systemRec.message2 = memBlock.id;
			systemRec.message3 = 0;
			systemRec.message4 = 0;
			(*(pType->GetAdmnProc()))(objectID.id, &systemRec);
			}
		else
			result = FALSE;
		}
	else
	 result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateUIDs
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  Updates the object ids held in object datas after a
//                compilation.  Also invokes FLOWMGR's UpdateUIDs function
//                since FLOWMGR is a parent class.
//
//                Assumes that all alias objects have been properly connected
//                to a shared object.  This means this function should only
//                be called after a successful check of the object.
//
//
/////////////////////////////////////////////////////////////////////////////
void OBJINST::UpdateUIDs()
  {
  LPOBJTYPE     pType;
  AObjMessage  systemRec;

  //do if we're not an alias or we're a dummy alias
	if ((!TestAliasFlag()) || (sharedObjectUID == 0))
    {
	  if (0 != (pType = GetSubjectPtr()->GetTypePtr(typeID)))
		  {
			systemRec.message1 = AOBJ_VALIDATEOBJECTIDS;
			systemRec.message2 = 0;
			systemRec.message3 = 0;
			systemRec.message4 = validateCompileTime;

			(*(pType->GetAdmnProc()))(this->copyUID.id, &systemRec);		  
		  }
    }

	FLOWMGR::UpdateUIDs();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ValidateUIDs
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  Updates the object ids held in object datas after a
//                paste.
//
/////////////////////////////////////////////////////////////////////////////
void OBJINST::ValidateAllUIDs(
  RUNTIMEUID theObjects)
	{
	LPOBJTYPE     pType;
	LPOBJINST     pObjInst;
	AObjMessage   systemRec;
	int           i;

	systemRec.message1 = AOBJ_VALIDATEOBJECTIDS;
	systemRec.message4 = validateDesignTime;

	i = 0;
	while (0 != (pObjInst = (LPOBJINST)GetObject(theObjects, i)))
		{
		//if pObjInst is not an alias or we're a dummy alias,
		//then tell it to validate its datas
		if ((!pObjInst->TestAliasFlag()) || (sharedObjectUID == 0))
			{
			pType = GetSubjectPtr()->GetTypePtr(pObjInst->GetTypeID());
			if (pType != 0)
				{
				systemRec.message2 = oldUID.id;  // old uid
				systemRec.message3 = GetUID(this).id;   // new uid
				(*(pType->GetAdmnProc()))(GetUID(pObjInst).id, &systemRec);
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////
//Only validate objects on the clipboard against objects on the clipboard
//It is assumed that we will be in a clipboard context when this function
//is entered.

void OBJINST::ValidatePastedUIDs(
  RUNTIMEUID clipObjects)
	{
	LPOBJTYPE     pType;
	LPOBJINST     pObjInst;
	AObjMessage  systemRec;
	int           i;
	LPFILEOBJECT  holdContext;
	LPCLIPOBJ			clipObj;

	systemRec.message1 = AOBJ_VALIDATEOBJECTIDS;
	systemRec.message4 = validateDesignTime;

	i = 0;
	while (0 != (clipObj = (LPCLIPOBJ)GetObject(clipObjects, i)))
		{
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		pObjInst = (LPOBJINST)GetObject(clipObj->copyUID);
		//if pObjInst is not an alias or its a dummy alias,
		//then tell it to validate its datas
		if ((!pObjInst->TestAliasFlag()) || (sharedObjectUID == 0))
			{
			pType = GetSubjectPtr()->GetTypePtr(pObjInst->GetTypeID());
			if (pType != 0)
				{
				systemRec.message2 = oldUID.id;  // old uid
				systemRec.message3 = GetUID(this).id;   // new uid
				(*(pType->GetAdmnProc()))(GetUID(pObjInst).id, &systemRec);
				}
			}
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PostValidateSubjectUIDs
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  This routine should be called for objects which were
//								pasted into a subject so if its holding onto other
//								object id's, the object can check if those other object
//								id's are valid.  They will only be valid if the other
//								objects were also pasted or if the object was pasted into
//								the same subject from which it was cut.
//
/////////////////////////////////////////////////////////////////////////////
void OBJINST::PostValidateSubjectUIDs()
	{
	LPOBJTYPE     pType;
	AObjMessage  systemRec;

	//do if not an alias or a dummy alias
	if((!TestAliasFlag()) || (sharedObjectUID == 0))
		{
		systemRec.message1 = AOBJ_VALIDATEOBJECTIDS;
		systemRec.message2 = subjectUID.id;
		systemRec.message3 = 0;
		systemRec.message4 = validateSubject;

		pType = GetSubjectPtr()->GetTypePtr(GetTypeID());
		if (pType != 0)
			{
			(*(pType->GetAdmnProc()))(GetUID(this).id, &systemRec);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PostValidateProjectUIDs
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:  This routine should be called for objects which were
//								pasted into different project than the project they
//								were cut from.  This allows the objects to validate its
//								ids against the other clipboard objects.  If the id does
//								not exist on the clipboard then it will be zeroed out.
/////////////////////////////////////////////////////////////////////////////
void OBJINST::PreValidateProjectUIDs(
	LPFILEOBJECT clipboardContext,
	RUNTIMEUID   clipObjs)
	{
	LPOBJTYPE     pType;
	AObjMessage  systemRec;

	if((!TestAliasFlag()) || (sharedObjectUID == 0))
		{
		systemRec.message1 = AOBJ_VALIDATEOBJECTIDS;
		systemRec.message2 = clipObjs.id;
		systemRec.message3 = (long)clipboardContext;
		systemRec.message4 = validateProject;

		pType = GetSubjectPtr()->GetTypePtr(GetTypeID());
		if (pType != 0)
			{
			(*(pType->GetAdmnProc()))(GetUID(this).id, &systemRec);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  HasFlows
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
BOOL OBJINST::HasFlows()
	{
	LPMEMBLOCK signalBlock;
	LONG signalSize;

	signalBlock = new MEMBLOCK();
	GetSignals(GetUID(signalBlock), AOBJ_BUILDSIGNALS);
	signalSize = AMemGetSize(GetUID(signalBlock).id);
	delete signalBlock;
	return (0 < signalSize / sizeof(AObjSignalRecord))?TRUE:FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DoPrint
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJINST::DoPrint(
	LPPRINTER printPtr,
	int indent)
	{
	LPOBJTYPE objTypePtr;
	HANDLE hBuffer;
	LPSTR buffer;
	PRINTFLOWSTACK * flowStackPtr;
	FLOW * flowPtr;
	int newIndent;

	objTypePtr = GetSubjectPtr()->GetTypePtr(GetTypeID());
	hBuffer = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, objectName.strlen() +
		lstrlen(objTypePtr->GetTypeName()) + 20);
	buffer = (LPSTR) GlobalLock(hBuffer);
	wsprintf(buffer, "%s [%s object]", (LPCSTR)objectName,
		objTypePtr->GetTypeName());

	PROJECT::SetLabelIndent(indent);
	printPtr->SingleLine(indent);
	printPtr->PrintLine(indent, buffer);
	printPtr->PrintLine(indent+1, "{");
	if(GetFlowCount() == 0)
		printPtr->PrintLine(indent+1, "-- no signals");
	else
		{
		flowStackPtr = new PRINTFLOWSTACK();
		//Determine print labels
		PushSignals(flowStackPtr, 0);
		while(!flowStackPtr->IsListEmpty())
			{
			flowPtr = flowStackPtr->PopFlow(&newIndent);
			if(!(flowPtr == (FLOW *)FN_DRAWBRACKET))
				flowPtr->BuildLabels(flowStackPtr);
			}

		//Do printint
		PushSignals(flowStackPtr, indent);
		while(!flowStackPtr->IsListEmpty())
			{
			flowPtr = flowStackPtr->PopFlow(&newIndent);
			if(flowPtr == (FLOW *)FN_DRAWBRACKET)
				FUNCINST::PrintBracket(printPtr, newIndent);
			else
				flowPtr->DoPrint(printPtr, flowStackPtr, newIndent);
			}

		// now clear printing flags
		GetSubjectPtr()->CleanFuncsOfPrint();
		}

	printPtr->PrintLine(indent+1, "}");
	PROJECT::GetPrintLabelPtr()->PurgeList();
	GlobalUnlock(hBuffer);
	GlobalFree(hBuffer);

	}


BOOL OBJINST::MoveFunctionChainRelative(
	POINT newNipple,
	POINT oldNipple)
	{
	BOOL result = TRUE;
	POINT offset;
	LPFLOWSTACK flowStack;
	LPFLOW flowPtr;

	offset.x = newNipple.x - oldNipple.x;
	offset.y = newNipple.y - oldNipple.y;

	flowStack = new FLOWSTACK();
	PushFlows(flowStack);

	while(NULL != (flowPtr = flowStack->Pop()))
		{
		if(!flowPtr->GetDestinationFunc()->
			MoveIfSelected(offset, flowStack))
			{
			result = FALSE;
			break;
      }
		}

  delete flowStack;

  return result;
  }


void OBJINST::SelectSignalChain(
	HDC hdc,
	LIST * selectedFuncs,
	LIST * selectedFlows)
	{
	LPFLOWSTACK flowStack;
	LPFLOW flowPtr;
  LPUIDLIST member;

	flowStack = new FLOWSTACK();

	PushFlows(flowStack);
	while(NULL != (flowPtr = flowStack->Pop()))
		{
		if(!flowPtr->IsSelected())
			{
			member = new UIDLIST(GetUID(flowPtr));
			selectedFlows->Add(member);
			flowPtr->SetSelected(TRUE);
			flowPtr->DrawSignalRect(hdc);
			}

		flowPtr->GetDestinationFunc()->
			PushAndSelectFuncChain(hdc, selectedFuncs, selectedFlows, flowStack);
		}

  delete flowStack;
	}

//Pushes signals onto the print flow stack
void OBJINST::PushSignals(
	PRINTFLOWSTACK * stackPtr,
	int indent)
	{
	FLOW *  flowPtr;
	RUNTIMEUID flowList = GetFlowList();
	int        numsigs = GetFlowCount();
	int        i=0;

	// push signals twice so on second time, closing bracket will be printed
	for(i=numsigs-1;i>-1;i--)
		{
		if((flowPtr = (FLOW *)GetObject(flowList, i))!=NULL)
			{
			//make sure status bits are clean -- they should not be dirty but clean
			//them just in case.
			flowPtr->ClearPrintingFlags();
			stackPtr->PushFlow(indent + 1, flowPtr);
			stackPtr->PushFlow(indent + 1, flowPtr);
			flowPtr->SetPrintFlag();
			}
		i--; //needed to balance the fact that i is incremented in call
				 //to GetObject;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckObject
//
// CLASS.......:  OBJINST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short OBJINST::CheckObject(
  void)
  {
	LPOBJTYPE         pType;
	AObjMessage      systemRec;
	AObjInfo  infoRec;
	char              cap[80];
	int               response;
	LPMEMBLOCK 				signalBlock;
	AObjSignalRecord * signals;
	AObjSignalRecord * hugeSignals;   // huge SKJ
	LONG 							signalSize;
	short             result = CHECK_NOERRORS;
	FLOW *					flowPtr;
	RUNTIMEUID				flowBlock;
	int								i,j;
	BOOL							flowLinked;
	RECT							flowRect;
	char							message[208];
	LPSTR							funcName;
	FUNCINST *			funcPtr;
	int								maxSignals;
	HDC								hdc;
	HWND							funcWin;
	LONG							lStatus;
	WORD							lowStatus;
	WORD							hiStatus;

	//we only want the object to check itself if it's not an alias or if
	//its a dummy alias
	if(!TestAliasFlag() || (sharedObjectUID == NULLSERUID))
		{
		if (0 != (pType = GetSubjectPtr()->GetTypePtr(typeID)))
			{
			infoRec.errorMessage[0] = '\0';
			systemRec.message1 = AOBJ_CHECKOBJECT;
			systemRec.message2 = (LONG)(pAObjInfo)&infoRec;
			systemRec.message3 = 0;
			systemRec.message3 = 0;
			lStatus = (*(pType->GetAdmnProc()))(::GetUID(this).id, &systemRec);

			lowStatus = LOWORD(lStatus);
			if ((lowStatus != A_OK) && (lowStatus != A_NOTHANDLED))
				{
				hiStatus = HIWORD(lStatus);

				if(hiStatus == AHIGH_CONTINUE)
					result |= CHECK_CONTINUE;
				else if (hiStatus == AHIGH_CANCEL)
					result|=CHECK_CANCEL;

				if(lStatus == A_WARNING)
					result|=CHECK_WARNING;
				else if (lStatus == A_ERROR)
				  result|=CHECK_ERROR;							

				}
			}
		}

	//Check the flows only if there weren't any errors in the object from
	//which the user wanted to cancel - this check is mainly for dynamic
	//signals which may have changed.
	if(!(result & CHECK_CANCEL))
		{
		//fill the signal block with all possible signals
		signalBlock = new MEMBLOCK();
		GetSignals(GetUID(signalBlock), AOBJ_BUILDSIGNALS);
		signals = (AObjSignalRecord *)signalBlock->GetData();
		signalSize = AMemGetSize(GetUID(signalBlock).id);
		maxSignals = (int)(signalSize / sizeof(AObjSignalRecord));
		flowBlock = GetFlowList();
		//go through each flow in the object
		j=0;
		while(0!=(flowPtr = (FLOW *)GetObject(flowBlock, j)))
			{
			//we'll assume that a flow is initially bad
			flowLinked = FALSE;
			//If the flow is already marked as not linked we can skip the next
			//section.
			if(!flowPtr->TestSignalNotLinkedFlag())
				{
				//dupliate code for efficiancy if huge pointers are not needed
				if(signalSize > 0xFFFF)
					{
					hugeSignals = signals;

					for(i=0;i<maxSignals;i++)
						{
						//A flow is good only if the signal matches.
						if(hugeSignals[i].theID == flowPtr->GetSignal())
							{
							//see if the name has changed and if so, update the signal name
							MaybeChangeSignalName(flowPtr, 
								hugeSignals[i].theName, flowPtr->GetName());
			 							//mark the flow as good and break from the for loop.
							flowLinked = TRUE;
							break;
							}
						}
					}
				else
					{
					for(i=0;i<maxSignals;i++)
						{
						//A flow is good only if the signal matches.
						if(signals[i].theID == flowPtr->GetSignal())
							{
							//see if the name has changed and if so, update the signal name
							MaybeChangeSignalName(flowPtr, (LPSTR)signals[i].theName, 
								flowPtr->GetName());
							//mark the flow as good and break from the for loop.
							flowLinked = TRUE;
							break;
							}
						}
					}
				if(!flowLinked)
					{
					//get the old signal rectangle before we change the signal to a
					//question mark
					flowPtr->GetSignalRect((LPRECT)&flowRect);

					//mark the flow as not linked
					flowPtr->SetSignalNotLinkedFlag();

					//cause the signal text to change;
					flowPtr->ChangeSignal(flowPtr->GetSignal());

					//invalidate the signal Rect if necessary
					if(GetSubjectPtr()->GetWindowPtr())
						{
						//get the window handle
            funcWin = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
						hdc = GetDC(funcWin);
						//convert to client coordinates
						LPtoDP(hdc, (LPPOINT)&flowRect, 2);
						ReleaseDC(funcWin, hdc);
						//invalidate the area under the old flow rectangle so it will
						//be erased.
						InvalidateRect(funcWin,	(LPRECT)&flowRect, TRUE);
						}
					}
				}
			//If the flow is invalid, display message box.
			if(!flowLinked)
				{
				LPCSTR	pName = this->objectName;

				//Make sure object name plus text does not excede buffer length
				wsprintf(cap, "Compile Warning for object %0.48s", pName);

				//Make sure we can get the function's name
				funcPtr = ((FUNCINST *)GetObject(flowPtr->GetDestinationUID()));
				if(funcPtr)
					funcName = funcPtr->GetDescPtr()->pGetName();
				else
					funcName = (LPSTR)"?";

				//Build the message
				wsprintf(message, "Invalid signal between object: %0.40s and "
					"function: %0.40s in Subject: %0.40s.\r \tContinue?",
					pName,funcName,GetSubjectPtr()->GetName());

				//Display the messagebox
				response = MessageBox(GetActiveWindow(), message, cap,
				 MB_ICONEXCLAMATION|MB_OKCANCEL|MB_TASKMODAL);
				if(response == IDCANCEL)
					{
					result |= CHECK_CANCEL;
					break;
					}
				else
					result |= CHECK_WARNING;
				}
			}
		
		delete signalBlock;
		}
	return result;
	}

BOOL OBJINST::MaybeChangeSignalName(
	LPFLOW flowPtr,
	LPCSTR newName,
	LPCSTR oldName)
	{
	RECT unionRect;
	RECT flowRect;
	HWND funcWin;
	HDC hdc;
	BOOL rVal;

	//see if the name has changed and if so, update the signal name
	if(lstrcmp(newName, oldName))
		{
		//get the window handle
		flowPtr->GetSignalRect((LPRECT)&unionRect);

		//cause the name to change
		flowPtr->ChangeSignal(flowPtr->GetSignal());

		//Get the new rect
		flowPtr->GetSignalRect((LPRECT)&flowRect);
		//form the union of the old and the new
		UnionRect((LPRECT)&unionRect, (LPRECT)&unionRect,
			(LPRECT)&flowRect);

		//see if the subject window is open and if so, invalidate
		//the rectangle.
		if(GetSubjectPtr()->GetWindowPtr())
			{
      funcWin = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
			hdc = GetDC(funcWin);
			//Convert to client coordinates
			LPtoDP(hdc, (LPPOINT)&unionRect, 2);
			ReleaseDC(funcWin, hdc);
			//invalidate the area under the old flow rectangle so it will
			//be erased.
			InvalidateRect(funcWin,	(LPRECT)&unionRect, TRUE);
			}
		rVal = TRUE;
		}
	else
		rVal = FALSE;
	
	return rVal;
	}


BOOL OBJINST::ImportUPSF()
	{
	long result;
	BOOL tempBool;
	char nameBuffer[64];
	char msgBuffer[256];

	I_ALWAYSASSERT((result = AUpsfImportField(OBJECT_NAME, 0L, AUPSF_NATV_STRING,
		(LPVOID)&nameBuffer, (long)sizeof(nameBuffer))) != AUPSF_MEMORYERROR,
		return FALSE);

	if(UPSF_BADSTRINGRESULT(result))
		{
		//we need to use the default name

		wsprintf(msgBuffer, "name for object of type: %s so "
			"using default name", objDesc->pGetName());
		
		apb_LogUpsfError(result, msgBuffer);
		}
	else
		UpdateName(nameBuffer);

	I_ALWAYSASSERT((result = AUpsfImportField(OBJECT_PERSISTENT, 0L, AUPSF_NATV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR, 
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		if(tempBool)
			SetPersistentsFlag();
		else
			ClearPersistentsFlag();
		}
	else
		{
		ClearPersistentsFlag();
		apb_LogUpsfError(result, "persistent flag");
 		}

	//FALSE means not a function
	if(!FlowImportUPSF(OBJECT_FLOWDATA, OBJECT_FLOWCOUNT, GetUID(this), FALSE))
		return FALSE;

	//import obj data
	if(!TestAliasFlag())
		{
		I_ALWAYSASSERT((result = AUpsfOpenScope(OBJECT_DATAS, 0L)) !=
			AUPSF_FILEERROR, return FALSE);

		if(result == AUPSF_NOERROR)
			{
			PROJECT::PushScopeStack(ST_FULL);

			wsprintf(msgBuffer, "%s%s Importing datas for object named: '%s'"
				" of type: %s", 
				UPSF_IMPORTPREFIXSTRING, UPSF_IMPORTPREFIXSTRING, GetName(),
				objDesc->pGetName());
			AUpsfLogComment(msgBuffer);

			if(!AObjImport(GetUID(this).id))
				{
				wsprintf(msgBuffer, "Could not import datas for object: %s so"
					" using default values.", GetName());
				AUpsfLogComment(msgBuffer);
				}

			#ifdef ADBG_DEBUG
				if(upsfValidateScope(OBJECT_DATAS, 0L) <= 0)
					{
					wsprintf(msgBuffer, "Fatal Error - object named '%s' of type %s "
						"opened or closed too many scopes while importing its datas.",
						GetName(), objDesc->pGetName());
					
					AUpsfLogComment(msgBuffer);
					
					ADBG_PRINT_E(msgBuffer);

					return FALSE;
					}
			#endif

			wsprintf(msgBuffer, "%s%s Ending datas import for object named: '%s'"
				" of type: %s",
				UPSF_IMPORTPREFIXSTRING, UPSF_IMPORTPREFIXSTRING, GetName(),
				objDesc->pGetName());                                             
			AUpsfLogComment(msgBuffer);

			PROJECT::PopScopeStack();
			AUpsfCloseScope();
			}
		else
			apb_LogUpsfError(result, "object's datas scope", APB_UPSFOPEN);
		}

	return TRUE;
	}

//check for out of disk space when exporting the flows, the last field
//of the object info and extensively when exporting the datas.
WORD OBJINST::ExportUPSF()
	{
	int tempBool;
	int tempInt;
	long tempID;
	long result;
	WORD returnValue = EXPORT_NOERROR;
	
	I_ALWAYSASSERT(AUpsfExportField(OBJECT_NAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPCSTR)objectName, (long)objectName.strlen()) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	I_ALWAYSASSERT(AUpsfExportField(OBJECT_TYPE, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&typeID, (long)sizeof(COMPUID)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//export the old objectID;
	tempID = GetUID(this).id;
	I_ALWAYSASSERT(AUpsfExportField(OBJECT_ID, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempID, (long)sizeof(long)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if(TestShareFlag())
		tempInt = 1;
	else if(TestAliasFlag())
		tempInt = 2;
	else
		tempInt = 0;

	I_ALWAYSASSERT(AUpsfExportField(OBJECT_ALIAS, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempInt, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if(TestPersistentsFlag())
		tempBool = 1;
	else
		tempBool = 0;

	I_ALWAYSASSERT(AUpsfExportField(OBJECT_PERSISTENT, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//make call to export flows
	returnValue |= FlowExportUPSF(OBJECT_FLOWDATA, (int *)&tempInt);

	//make a check to make sure we're not out of disk space
	//Do check here since the object could be an alias  and therefore we won't
	//execute any of the code underneath where datas are exported.
	result = AUpsfExportField(OBJECT_FLOWCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempInt, (long)sizeof(int));
	
	I_ASSERT(result == AUPSF_NOERROR);
	
	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	//Export the object Datas
	if(!TestAliasFlag())
		{
		//set value of result
		result = AUpsfCreateScope(OBJECT_DATAS, 0L);
		
		I_ASSERT(result == AUPSF_NOERROR);

		if(result == AUPSF_NOERROR)
			{
			if(!AObjExport(GetUID(this).id))
				returnValue|=EXPORT_GENERALERROR;
			//At this point, 'result' could only be AUPSF_NOERROR so reset the value to
			//be the result of closing the scope.  We need to check if we'er out
			//of disk space here because it's the only way to find out if we ran
			//out while exporting the datas.
			result = AUpsfCloseScope();
			}

		//check to see if we're out of disk space
		if(result == AUPSF_OUTOFDISKSPACE)
			returnValue |= EXPORT_OUTOFDISK;
		else if(result != AUPSF_NOERROR)
			returnValue|=EXPORT_GENERALERROR;
		}

	PROJECT::IncStatusBar();
	
	return returnValue;
	}


//if we ever need to drag the icon image, use this operation.
//#define BITBLT_NOTXOR 0x00990066 	//index 99 and code DSxn in MS Programmer's Reference
																	//Volume 2 11-10
void OBJINST::DrawDrag(
	HDC hdc,
	LPRECT bounds)
	{	HPEN oldPen;
	HBRUSH oldBrush;
	int oldMode;

	oldMode = SetROP2(hdc, R2_NOTXORPEN);
	oldPen = SelectPen(hdc, PROGSYSTEM::dottedDragPen);
	oldBrush = SelectBrush(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, bounds->left, bounds->top, bounds->right, bounds->bottom);
	SelectPen(hdc, oldPen);
	SelectBrush(hdc, oldBrush);
	SetROP2(hdc, oldMode);
	}

//////////////////////////////////////////////////////////////////////////////////////
//	VerifyAliasDatasStatus
//
//If the object is changing to a dummy alias, datas are created.
//If the object is changing to a linked alias, datas are destroyed.
//Also sets the shared UID field
void OBJINST::VerifyAliasDatasStatus(
	SERUID sharedUID)
	{
	long mes2=0;
	long mes3=0;
	long mes4=0;

	I_ASSERT(TestAliasFlag());
	//only go through the other stuff if the alias is somehow changing links.
	if(TestAliasFlag() && (sharedUID != GetSharedObjectUID()))
		{
		if(sharedUID == NULLSERUID)
			{
			//we're going to be a dummy alias

			//we need to create some datas for the dummy since we're changing from
			//a linked to a dummy alias.

			//First NULL out the sharedObjectUID or if the datas are created, it will
			//try to create them for the shared object.
			SetSharedObjectUID(sharedUID);

			SendMessageToObject(AOBJ_CREATED, mes2, mes3, mes4, GetUID(this).id);
			}
		else
			{
			//we're going to be a linked alias
			if(GetSharedObjectUID() == 0)
				{
				//we need to destroy the datas since we're changing from a dummy
				//to a linked alias.
				SendMessageToObject(AOBJ_DESTROYED, mes2, mes3, mes4, GetUID(this).id);
				SetDatasCount(0);
				}

			//Dont set the sharedObjectUID until after the datas have been destroyed
			//or the datas will be destroyed for the real object.
			SetSharedObjectUID(sharedUID);
			}
		//signals we're holding onto will need to be verified to make sure they exist.
    VerifySignalNames();
		}
	}

BOOL OBJINST::PostUPSFImport()
	{
	return FlowPostUPSFImport();
	}
