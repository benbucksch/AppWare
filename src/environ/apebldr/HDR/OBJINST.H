#if !defined( __OBJINST_H )
#define __OBJINST_H

#include "classdef.h"
#include "object.h"
#include "flowmgr.h"
#include "serrec.h"
#include "dbstring.h"

#define	OBJ_SELECTFLAG		0x00000001
#define OBJ_COMPILEDFLAG  0x00000004
#define OBJ_EDITFLAG			0x00000008

#define DEFAULT_ALIAS_NAME ("Dummy Alias")

class OBJINST:public SERIUSRECORD, public OBJECTRECORD, public FLOWMGR
	{

  friend BOOL ALMAPI AObjValidateID(
	  LPOBJECTID  pObjectID,
	  pAObjMessage    pSystemRecord);

  friend LPOBJECTRECORD GetObjectRecord(   //from oipdatas.cpp
    AOBJECTID  theObject);

        friend long ALMAPI AObjGetName(
		AOBJECTID theObject,
		LPSTR    theName,
		long     nameSize);

        friend BOOL ALMAPI AObjValidateID(
		LPOBJECTID  pObjectID,
		pAObjMessage    pSystemRecord);
private:

	OBJINST();

protected:
	DBSTRING      objectName;
	long					status;
	LPSERIUSICON	pIcon;
	int	    		  itemIndex;
	int						oldItemIndex;
	SERUID				subjectUID;
	SERUID        oldUID;
  SERUID        groupUID;
  SERUID        sharedObjectUID;


	void PushSignals(
		PRINTFLOWSTACK * stackPtr,
		int indent);

	BOOL MaybeChangeSignalName(
		LPFLOW flowPtr,
		LPCSTR newName,
		LPCSTR oldName);


public:

	LPOBJDESC objDesc;

	OBJINST(
		LPOBJDESC	pObjDef,
		SERUID 		creatorUID,
		SERUID    typeUID);

	OBJINST(
		ATYPEID theID,
		LPCSTR theName,
		long  theObjectStatus,
		SERUID theSubjectUID,
		SERUID typeUID);

	OBJINST(
		void * thisPtr);


	~OBJINST(
		void);

	void UpdateName(
		LPCSTR           newName)
		{
		objectName = newName;
		}

	LPCSTR GetName()
		{
		return objectName;
		}

	SUBJECT * GetSubjectPtr()
		{
		return (SUBJECT *)GetObject(subjectUID);
		}

	LPSERIUSICON GetIcon()
		{
		return pIcon;
		}

	void SetIndex(
		int itemIndex)
		{
		this->itemIndex = itemIndex;
		}

	int GetIndex()
		{
		return this->itemIndex;
		}

	int GetOldIndex()
		{
		return oldItemIndex;
		}

	void SetOldIndex(
		int oldIndex)
		{
		oldItemIndex = oldIndex;
		}

	void SetSelectFlag()
		{
		status |= OBJ_SELECTFLAG;
		}

	void ClearSelectFlag()
		{
		status &= ~OBJ_SELECTFLAG;
		}

	BOOL TestSelectFlag()
		{
		if (status & OBJ_SELECTFLAG)
			return TRUE;
		else
			return FALSE;
		}

	void SetEditFlag()
		{
		status |= OBJ_EDITFLAG;
		}

	void ClearEditFlag()
		{
		status &= ~OBJ_EDITFLAG;
		}

	BOOL TestEditFlag()
		{
		return (status & OBJ_EDITFLAG)?TRUE:FALSE;
		}

	void SetCompiledFlag()
		{
		status |= OBJ_COMPILEDFLAG;
		}

	void ClearCompiledFlag()
		{
		status &= ~OBJ_COMPILEDFLAG;
		}

	BOOL TestCompiledFlag()
		{
		if (status & OBJ_COMPILEDFLAG)
			return TRUE;
		else
			return FALSE;
		}

	BOOL GetSignals(
		RUNTIMEUID	memBlock,
		long int	  whichSignals);

	long	GetStatus()
		{
		return status;
		}

	BOOL HasFlows();

	void UpdateUIDs();

	SERUID GetOldUID()
		{
		return oldUID;
		}

	void SetOldUID(
		SERUID uid)
		{
		oldUID = uid;
		}

	void ValidateAllUIDs(
		RUNTIMEUID theObjects);

	void ValidatePastedUIDs(
		RUNTIMEUID clipObjects);

	void DoPrint(
		LPPRINTER printPtr,
		int indent);

  short CheckObject(
		void);

	void PostValidateSubjectUIDs();

	void PreValidateProjectUIDs(
		LPFILEOBJECT clipboardContext,
		RUNTIMEUID   clipObjs);

	WORD ExportUPSF();

	BOOL ImportUPSF();

	short PostReadInit();

  void SetGroupUID(
    SERUID theGroup)
    {
    groupUID = theGroup;
    }

  SERUID GetGroupUID()
    {
    return groupUID;
    }

  OBJGROUP * GetGroupPtr()
    {
    return (OBJGROUP *)GetObject(groupUID);
    }

  SERUID GetSharedObjectUID(
    void)
    {
    return sharedObjectUID;
    }

  BOOL SetSharedObjectUID(
    SERUID  newSharedObjectUID)
    {
    this->sharedObjectUID = newSharedObjectUID;
    return TRUE;
    }


	void SelectSignalChain(
		HDC hdc,
		LIST * selectedFuncs,
		LIST * selectedFlows);

	void DrawDrag(
		HDC hdc,
		LPRECT bounds);

	BOOL MoveFunctionChainRelative(
		POINT newNipple,
		POINT oldNipple);

	void VerifyAliasDatasStatus(
		SERUID sharedUID);

	BOOL PostUPSFImport();

	};

#endif
