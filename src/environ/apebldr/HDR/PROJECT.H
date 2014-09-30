#if(!defined(__project_h))
#define __project_h

#include "classdef.h"
#include "wrksheet.h"
#include "seruid.h"
#include "sericon.h"
#include "rtuid.h"
#include "listmbr.h"
#include "memclass.h"
#include "unitree.h"
#include "progsys.h"
#include "objgroup.h"

//include scopstak.h since it has some #defines that a lot of files which
//include project.h will need.
#include "scopstak.h"

#define COMPILE_POINTS 100
#define PRJ_UPSF_CONSTRUCTOR		((char)0x01)
#define PRJ_NORMAL_CONSTRUCTOR 	((char)0x02)

//IMPORTANT:
//if the value for PRJ_READNOERROR is ever changed, then changes must be
//made to the postreadinit routines defined in memio because there is a zero
//value hard coded there to signal no error.
#define PRJ_READNOERROR 		0x0000
#define PRJ_READWARNING 		0x0001
#define PRJ_READERROR				0x0002
#define PRJ_READBADOBJECT 	0x0004
#define PRJ_READBADFUNC			0x0010
#define PRJ_READBADTYPE			0x0020
#define PRJ_READBADFLOW			0x0040
#define PRJ_READBADPARAM		0x0080

#define PRJ_NEWGROUPMAXNAME 16


class TYPELISTMEMBER:public LISTMEMBER, public MEMORY
	{
	friend PROJECT;

	private:

		long typeID;

		TYPELISTMEMBER(
			long type):typeID(type)
			{
			}
	};

typedef TYPELISTMEMBER * LPTYPELISTMEMBER;

struct UPSFTYPE:public UNITREEMEMBER
	{
	ATYPEID typeID;
	TSTRING typeName;

	UPSFTYPE(ATYPEID id, LPCSTR name):typeID(id), typeName(name)
		{
		}

	int Compare(
		LPBITREEMEMBER node)
		{
		if(typeID < ((UPSFTYPE *)node)->typeID)
			return -1;
		else if(typeID > ((UPSFTYPE *)node)->typeID)
			return 1;
		else
			return 0;
		}

	int MatchCriteria(
		long criteria)
		{
		if(typeID < (ATYPEID)criteria)
			return -1;
		else if(typeID > (ATYPEID)criteria)
			return 1;
		else
			return 0;
		}


	};

typedef UPSFTYPE * LPUPSFTYPE;

class PROJECT:public WORKSHEET
	{

	friend SUBJECT * GetSubjectPtr();

  friend LPBASETYPE GetTypeRecord(     //from oipdatas.cpp
    ATYPEID  theTypeID);

        friend BOOL ALMAPI AEvtCheckForMethod (
		ATYPEID  theType,
		AEVENTID selector);

        friend BOOL ALMAPI AEvtCallMethod (
		AOBJECTID sourceObject,
		AOBJECTID targetObject,
		ATYPEID   targetType,
		AEVENTID  selector,
		LONG     methodParam);

	friend BOOL OBJGROUP::EditObject(
		LPOBJINST objPtr);



	private:
		static SCOPESTACK *		scopeStackPtr;
		static unsigned int       projectIndex;
		static PRINTLABEL *    labelPtr;
		static int                labelIndent;
    static int projectCount;
		static LPUNIQUETREE				upsfObjTypeTree;
		static LPUNIQUETREE				upsfFuncTypeTree;
		static int								upsfImportVersion;

		RUNTIMEUID      subjectBlock;
		RUNTIMEUID		  typeBlock;
		POINT           lastSubjectPosition;
		unsigned int    subjectCount;
		SERIUSICON      theIcon;
		BOOL            notUsed_1;
		POINT           originOffset;
		POINT           logBounds;
		int             newGroupIndex;
		int             nObjectsCount;
		SUBJECT *    		activeSubject;

		void ShowAllWindows();

		void MakeClean();

		BOOL SaveToDisk(HANDLE fileHandle);

		void OnlyUnselectSubjects(
			HDC hdc,
			LPRECT refreshRect);

		BOOL OnlySnapSelected(
			HDC hdc,
			LPRECT refreshRect);

		BOOL Paste();

		void ClearSubjectSelections();

		void DoPartialDelete();

		void DoCompleteDelete();

		void DoPartialCopy();

		void DoCompleteCopy();

		void DoPartialCut();

		void DoCompleteCut();

    unsigned long WriteShellExe(
			HANDLE  hAppFile,
			HANDLE  hShellFile);

		BOOL  ReplaceIconInEXE(LPCSTR pszExeFile);

		void SetFlowStops(
			BOOL enable);

		void SetCancelCompileFlag()
			{
			status|=PRJ_CANCELCOMPILE;
			}

		void ClearCancelCompileFlag()
			{
			status&= ~PRJ_CANCELCOMPILE;
			}

		short DoChecking(
			BOOL updateStatus);

		BOOL DoPrivateColorSet();

		long GetExportTime();

		static LPUPSFTYPE GetNextUpsfObjType(
			LPUPSFTYPE node);

		static LPUPSFTYPE GetNextUpsfFuncType(
			LPUPSFTYPE node);

		static void UpsfObjTypePrepareForGetting();

		static void UpsfFuncTypePrepareForGetting();
	
		static void CreateUpsfFuncTypeTree()
			{
			if(PROJECT::upsfFuncTypeTree == NULL)
				PROJECT::upsfFuncTypeTree = new UNIQUETREE();
			}

		static void DestroyUpsfFuncTypeTree()
			{
			if(PROJECT::upsfFuncTypeTree != NULL)
				{
				delete PROJECT::upsfFuncTypeTree;
				PROJECT::upsfFuncTypeTree = NULL;
				}
			}

		static void CreateUpsfObjTypeTree()
			{
			if(PROJECT::upsfObjTypeTree == NULL)
				PROJECT::upsfObjTypeTree = new UNIQUETREE();
			}

		static void DestroyUpsfObjTypeTree()
			{
			if(PROJECT::upsfObjTypeTree != NULL)
				{
				delete PROJECT::upsfObjTypeTree;
				PROJECT::upsfObjTypeTree = NULL;
				}
			}

		BOOL ImportUpsfObjTypeNames();

		BOOL ImportUpsfFuncTypeNames();

		LPOBJTYPE GetGoodOrBadTypePtr(
			ATYPEID typeID);

		void PurgeBadUnusedTypes();

		BOOL MakeBackUp(
			LPCSTR oldFile);
      
    void SetUpsfImportFlag()
    	{
      status |= PRJ_IMPORTING;
      }
      
    void ClearUpsfImportFlag()
    	{
      status &= ~PRJ_IMPORTING;
      }

		BOOL AddExeObjType(
			LPOBJTYPE pObjType,
			LPEXEHEAD pExeHead);

		BOOL BuildExeTypeList();

	public:
		static COMPILESTATUS *       compStatPtr;
		static	COLORREF textColor;
		static COLORREF highlightColor;
		static  HPEN dragPen;
		static COLORREF windowBkgrndColor;
		LPFILEOBJECT            fileObjPtr;

		PROJECT(
			char constructorType);

		PROJECT(
			void *  thisPtr);

		short PostReadInit();

		~PROJECT();

		BOOL ShowObjectWindow();

		void GetClientBounds(LPRECT winBounds);

		BOOL    ChangeSelection(
			HDC                     hdc,
			SERUID  newSelection,
			WPARAM            wKeyStates);

		void OnlyChangeSelection(
			SERUID  newSelection);

		void UnSelectSubjects(
			HDC hdc);

		void AdjustLastSubjectPosition();

		void    CalculateSubjectPosition(
			SERUID  theSubject);

		void  CalculateSelectedSubjectOffset(
			POINT mouse);

		void AdjustLogBounds();

		void ArrangeSubjects();

		unsigned int SnapLogBounds(
			unsigned int howToSnap);

		BOOL Save();

		BOOL SaveAs();

    BOOL SaveAsExecutable();

		void OrderWindows();

		void MakeDirty();

		BOOL ProjectIsDirty();

		RUNTIMEUID GetSubjectBlock()				{return subjectBlock;}

		POINT GetLastSubjectPosition();

		void SetPath(
			LPCSTR thePath);

		LPCSTR GetPath();

		LPSERIUSICON GetIconBlock();

		BOOL GridIsOn();

		void SetGrid(
			BOOL set);

		void SetOriginOffset(
			POINT newOffset);

		POINT GetOriginOffset();

		void SetLogBounds(
			POINT newBounds);

		POINT GetLogBounds();

		void IncrementSubjectCount();

		void DecrementSubjectCount();

		unsigned int GetSubjectCount();

		static void MakeProjectName(
			LPSTR buffer,
			unsigned int index);

		static unsigned int GetIndex();

		static void IncIndex()
			{
			projectIndex++;
			}

		static BOOL  VerifyFile(
			HANDLE fileHandle,
			LPCSTR fileName);

		LPCSTR GetName();

		void SetName(
			LPCSTR newName);

		BOOL Compile(
			long   timePoints,
			double timeRatio,
      LPCSTR  masterPath);

		SUBJECT * GetActiveSubject()
			{
			return activeSubject;
			}

		void SetActiveSubject(
			SUBJECT * newActiveSubject)
			{
			activeSubject = newActiveSubject;
			}

		void DecObjectsCount()
			{
			nObjectsCount--;
			}

		void DoUndo();

		void DoPaste();

		void CleanCompile();

		void DoCut();

		void DoDelete();

		void DoClear();

		void DoCopy();

		BOOL SubjectsSelected();

		void IncObjectsCount()
			{
			nObjectsCount++;
			}

		BOOL FlowStopsSet();

		FLOW * GetDesignFlow(
			LPEXEFLOW exeFlow);

		void DoPrint();

    BOOL CheckProject(
			BOOL updateStatus);

		static PRINTLABEL * GetPrintLabelPtr()
			{
			return labelPtr;
			}

		static int GetLabelIndent()
			{
			return labelIndent;
			}

		static void SetLabelIndent(
			int newIndent)
			{
			labelIndent = newIndent;
			}

		BOOL DeleteSharedObjects();

		LPOBJTYPE GetTypePtr(
			long typeID);

		BOOL TestStopsDisabledFlag()
			{
			return (status & PRJ_STOPSDISABLED)?TRUE:FALSE;
			}

		RUNTIMEUID	GetTypeBlock()
			{
			return typeBlock;
			}

		void SetStopsDisabledFlag()
			{
			status|=PRJ_STOPSDISABLED;
			}

		void ClearStopsDisabledFlag()
			{
			status&= ~PRJ_STOPSDISABLED;
			}

		void ClearAllStops();

		void DisableAllStops()
			{
			SetFlowStops(FALSE);
			SetStopsDisabledFlag();
			}

		void EnableAllStops()
			{
			SetFlowStops(TRUE);
			ClearStopsDisabledFlag();
			}

		BOOL TestCancelCompileFlag()
			{
			return (status & PRJ_CANCELCOMPILE)?TRUE:FALSE;
			}

		void CheckProjectForErrors();

		void CenterSelection();

		void SelectAll();

		BOOL ImportUPSF();

		WORD ExportUPSF();

		void UpdateObjCountBoxes();

		static void PushScopeStack(
			double percent);

		static void PopScopeStack();

		static void IncStatusBar(
			long amount = 1L);

		long GetCompileTime();

    BOOL RenameAliases(
      LPOBJINST  theMaster,
			LPCSTR     newName);

    BOOL IsAliased(
      LPOBJINST  theMaster);

    int GetObjectCount()
    	{
      return nObjectsCount;
      }

		void UnselectEverything();

		void RedrawLinkedAliases(
			LPOBJINST sharedObjPtr);

		void RedrawAllLinkedAliases(
			LPSUBJECT subjectPtr);

		BOOL DuplicateSubjectName(
			LPCSTR name);

		short LinkAllAliases(
			BOOL warnings);

		BOOL SetPrivateColors();

		static BOOL SetSysColors();

		void RedrawAllFunctions();

		void ResetLogicalBounds();

		SERUID CreateObjType(
			long      typeID,
			ADMNPROC	admnProc);

		LPOBJTYPE ObtainTypePtr(
			ATYPEID typeID);

		SERUID NewType(
			long typeID,
			ADMNPROC admnProc);

		BOOL BuildTypeBlock();

		BOOL UpdateTypeBlock();

		void RestoreUselessTypeList(
			LPLIST list);

		LPLIST BuildUselessTypeList();

		AOBJECTID CreateDynamicObject(
			ATYPEID typeID,
			APROTOCOLID protocolID);

		BOOL DestroyObject(
			AOBJECTID objID);

		BOOL RenameObject(
			AOBJECTID objID,
			LPCSTR newName);

		void GetNewGroupName(
			LPSTR nameBuffer);

		static BOOL AddUpsfObjType(
			ATYPEID typeID,
			LPCSTR typeName);

		static BOOL AddUpsfFuncType(
			ATYPEID typeID,
			LPCSTR typeName);

		static BOOL GetUpsfFuncTypeName(
			long typeID, 
			LPSTR buffer, 
			int bufSize);

		static BOOL GetUpsfObjTypeName(
			long typeID, 
			LPSTR buffer, 
			int bufSize);


		static BOOL UpsfPreAppwareImportVersion()
			{
			return (PROJECT::upsfImportVersion > UPSF_WORKSHOP_VERSION)?FALSE:TRUE;
			}

	  void ImportSplashScreen();

		BOOL TestUpsfImportFlag()
    	{
      return (status & PRJ_IMPORTING)?TRUE:FALSE;
      }

		BOOL MarkRequiredType(
			ATYPEID typeID);

		BOOL ClearRequiredType(
			ATYPEID typeID);
	
	};

#endif
